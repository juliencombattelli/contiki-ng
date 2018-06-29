# AT86RF2XX radio driver for Contiki-NG
This driver is based on a generic driver. An adaptation layer for Contiki-NG has been added, defining the `radio_driver` structure declared in `os/dev/radio.h`
To use this driver, include the makefile `Makefile.contiki-at86rf2xx`. Two variables are defined:
- `CONTIKI_AT86RF2XX_SOURCEFILES`: Lists the source files to compile
- `CONTIKI_AT86RF2XX_DIRS`: Lists the subdirectories in which the source files can be found
