# AT86RF2XX generic driver
This is a C driver for AT86RF2XX radio family. It support the following radio modules:
 - AT86RF212B (partial, not tested)
 - AT86RF231  (not tested)
 - AT86RF232  (not tested)
 - AT86RF233  (tested)

## Platform support
The driver is divided into two parts, a generic one in `driver/` folder, and a platform-specific one in `platform/` folder.
It actually supports stm32l1 mcu familly.

Port to other microcontrolers can be achievied by defining `at86rf2xx_hal` struct and related functions to use specific GPIO and SPI driver.

The platform folder must be organized as follow:
```
platform
└── <mcu vendor>
    └── <mcu family or model>
        ├── include
        │   └── <mcu>-platform.h
        └── src 
            └── <mcu>-platform.c
```
It is recommended to define the `struct at86rf2xx_hal` in the file `<mcu>-platform.h` and `at86rf2xx_hal_*` functions in the file `<mcu>-platform.c`. Feel free to inline these functions if they contain few instructions.

It is the responsability of the user of this driver to properly initialize the peripherals in the `at86rf2xx_hal` struct. These initializations must be done before the call of `at86rf2xx_init()` function

## Build
To build this driver, a C99 compliant compiler is needed.
You can directly use the makefile `Makefile.at86rf2xx` or include it in a larger project.
Before use it, you must define the `AT86RF2XX_PLATFORM` variable to `<mcu vendor>/<mcu>` (it is in fact the path to the platform code inside the platform folder).
The makefile defines three variables you can use:
- `AT86RF2XX_SOURCEFILES`: lists the source files to compile
- `AT86RF2XX_DIRS`: lists the subdirectories in which the source files can be found
- `CFLAGS` : add the include directories with `-I` option
