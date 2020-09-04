/**
 * @file disp_spi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define TAG "ILI93413wire"
#include <iostream>
#include <bitset>
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <array>
#include <vector>
#include <algorithm>

#include "lvgl/lvgl.h"

#include "disp_spi.h"
#include "disp_driver.h"

#include "../lvgl_driver.h"

/*********************
 *      DEFINES
 *********************/
 #if CONFIG_LVGL_TFT_DISPLAY_SPI_HSPI == 1
 #define TFT_SPI_HOST HSPI_HOST
 #else
 #define TFT_SPI_HOST VSPI_HOST
 #endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void IRAM_ATTR spi_ready (spi_transaction_t *trans);

/**********************
 *  STATIC VARIABLES
 **********************/
static spi_device_handle_t spi;
static volatile bool spi_trans_in_progress;
static volatile bool spi_color_sent;
static transaction_cb_t chained_post_cb;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void disp_spi_add_device_config(spi_host_device_t host, spi_device_interface_config_t *devcfg)
{
//    chained_post_cb=devcfg->post_cb;
//    devcfg->post_cb=spi_ready;
    esp_err_t ret=spi_bus_add_device(host, devcfg, &spi);
    assert(ret==ESP_OK);
}

void disp_spi_add_device(spi_host_device_t host)
{
    spi_device_interface_config_t devcfg={
            .command_bits = 0,
            .address_bits = 0,
            .dummy_bits = 0,
#if defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_ST7789
            .mode=2,                                // SPI mode 2
#else
            .mode=0,				                // SPI mode 0
#endif

#if defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_ST7789
        .clock_speed_hz=24*1000*1000,           // Clock out at 24 MHz
#elif defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_HX8357
        .clock_speed_hz=26*1000*1000,           // Clock out at 26 MHz
#elif defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_SH1107
        .clock_speed_hz=8*1000*1000,            // Clock out at 8 MHz
#elif defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_ILI9486
        .clock_speed_hz=24*1000*1000,           //Clock out at 24 MHz
#else
        .clock_speed_hz=40*1000*1000,           // Clock out at 40 MHz
#endif



            .spics_io_num=DISP_SPI_CS,              // CS pin

        .flags = ( SPI_DEVICE_3WIRE | SPI_DEVICE_NO_DUMMY),
         .queue_size=1,
            .pre_cb=NULL,
            .post_cb=NULL,    };

    disp_spi_add_device_config(host, &devcfg);
    spi_device_acquire_bus(spi, portMAX_DELAY);
}

void disp_spi_init(void)
{

    esp_err_t ret;

    spi_bus_config_t buscfg={
        .mosi_io_num=DISP_SPI_MOSI,
        .miso_io_num=-1,
        .sclk_io_num=DISP_SPI_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
#if defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_ILI9341
        .max_transfer_sz = DISP_BUF_SIZE * 2,
#elif defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_ST7789
        .max_transfer_sz = DISP_BUF_SIZE * 2,
#elif defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_ILI9488
        .max_transfer_sz = DISP_BUF_SIZE * 3,
#elif defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_HX8357
        .max_transfer_sz = DISP_BUF_SIZE * 2
#elif defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_ILI9486
            .max_transfer_sz = DISP_BUF_SIZE * 2,
#elif defined CONFIG_LVGL_TFT_DISPLAY_CONTROLLER_SH1107
		.max_transfer_sz = DISP_BUF_SIZE * 2
#endif
    };

    //Initialize the SPI bus
    ret=spi_bus_initialize(TFT_SPI_HOST, &buscfg, 1);
    assert(ret==ESP_OK);

    //Attach the LCD to the SPI bus
    disp_spi_add_device(TFT_SPI_HOST);

    // spi_device_acquire_bus(spi, portMAX_DELAY);
}

//#define SPI_SHIFT_DATA_16(data, len) __builtin_bswap16(data<<(16-len))

void send_bits(void * data, unsigned int length_in_bits) {
    while (spi_trans_in_progress);

    spi_trans_in_progress = true;

    spi_transaction_t t = {
            .length = length_in_bits,
            .tx_buffer = data,
            .rx_buffer = nullptr,
    };

    esp_err_t res = spi_device_polling_transmit(spi, &t);
    spi_trans_in_progress = false;
}

void send_bytes(bool is_data, const uint8_t * bytes, uint8_t num_bytes) {
    std::vector<uint8_t> buffer(num_bytes + 1);

    for (auto i = 0; i < num_bytes; ++i) {
        auto byte_offset = i * 9 / 8;
        auto bit_offset = i * 9 % 8;

        uint16_t val = (bytes[i] + (0x100 * is_data));
        uint8_t head = val >> (1 + bit_offset);
        uint8_t tail = val << (7 - bit_offset) & 0xFF;

        buffer[byte_offset] += head;
        buffer[byte_offset + 1] += tail;
    }

//    std::reverse(buffer.begin(), buffer.end());
    send_bits(&buffer[0], num_bytes * 9);

//    return buffer;
}

void send_byte(bool is_data, uint8_t * data) {
    uint16_t buffer = SPI_SWAP_DATA_TX(*data + 256 * is_data, 9);
    send_bits(&buffer, 9);
}

void send_command_byte(uint8_t * data) {
    send_byte(false, data);
}

void send_data_byte(uint8_t * data) {
    send_byte(true, data);
}

void shift_right(uint8_t * data, uint16_t data_length) {
    uint8_t carry = 0;
    for (uint8_t i = 0; i < data_length; ++i) {
        uint8_t next_carry = (data[i] & 1) << 7;
        data[i] >>= 1;
        data[i] += carry;
        carry = next_carry;
    }
}

void disp_spi_send_data(uint8_t * data, uint16_t length) {
    if (length == 0) return;      //no need to send anything

    auto chunk_size = 12;
    for (int data_i = 0; data_i < length; data_i += chunk_size) {
        auto num_bytes = std::min(chunk_size, length - data_i);
        send_bytes(true, &data[data_i], num_bytes);
    }

    spi_color_sent = false;             //Mark the "lv_flush_ready" NOT needs to be called in "spi_ready"
}

void disp_spi_send_colors(uint8_t * data, uint16_t length)
{
    disp_spi_send_data(data, length);
    spi_color_sent = true;
    if(spi_color_sent) lv_disp_flush_ready(&lv_refr_get_disp_refreshing()->driver);//Mark the "lv_flush_ready" needs to be called in "spi_ready"
}


bool disp_spi_is_busy(void)
{
    return spi_trans_in_progress;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void IRAM_ATTR spi_ready (spi_transaction_t *trans)
{
    return;
    spi_trans_in_progress = false;

    lv_disp_t * disp = lv_refr_get_disp_refreshing();
    if(spi_color_sent) lv_disp_flush_ready(&disp->driver);
    if(chained_post_cb) chained_post_cb(trans);
}
