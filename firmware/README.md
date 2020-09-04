# firmware-ui

[[_TOC_]]

## Getting Started

1.  Download submodules
  
    ```git submodule update --init --recursive```
  
2.  Build the project -- you'll need to do this before VS Code or CLion will be 
    able to perform introspection.

    ```pio run```
    
## Helpful commands

-   Open ESP-IDF configuration tool

    ```pio run --target menuconfig```

## NodeMCU-32S

This is the device Chancey is currently working with.

### Components

-   [NodeMCU-32S](https://www.shenzhen2u.com/NodeMCU-32S)
-   [Z320IT008 display](https://www.aliexpress.com/i/33013630307.html)

### Pin/GPIO mapping

|LCD PIN    |LABEL      |ESP32 PIN  |GPIO       |
|---        |---        |---        |---        |
|9          |CS/SS      |30         |18         |
|10         |CLK        |23         |0          |
|11         |DC         |31         |19         |
|13         |MOSI       |28         |17         |
|14         |MISO*      |27         |16         |
|15         |RESET      |10         |27         |

\* Connected but not actually used.

This mapping matches the schematic in the repo.

Mapping can be changed in the menuconfig tool under Component Config, LittlevGL 
display controller, Display pin assignments.

## Documentation/Bookmarks

* [PIO documentation for Espressif 32 platform](https://docs.platformio.org/en/latest/platforms/espressif32.html)
* [PIO documentation for ESP-IDF framework](https://docs.platformio.org/en/latest/frameworks/espidf.html)
* [LVGL](https://docs.lvgl.io/v7/en/html/index.html)
* [lv_port_esp32](https://github.com/lvgl/lv_port_esp32/)
* [NodeMCU-32S](https://docs.zerynth.com/latest/official/board.zerynth.nodemcu_esp32/docs/index.html#)
