CONTIKI_PROJECT = hello-world
all: $(CONTIKI_PROJECT)

TARGET=stm32ldisco-rf230

CONTIKI = ../..
include $(CONTIKI)/Makefile.include

CFLAGS += -g3 -O0 

OPENOCD=openocd
OPENOCD_DIR=/usr/share/openocd/scripts
#set(OPENOCD_CFG ${CMAKE_SOURCE_DIR}/at86rf2xx-radio-driver-debug.cfg)
OPENOCD_CFG=board/stm32ldiscovery.cfg

flash: $(CONTIKI_PROJECT).bin
	$(OPENOCD) -s $(OPENOCD_DIR) -f $(OPENOCD_CFG) -c "init;reset halt;flash write_image erase $(CONTIKI_PROJECT).bin 0x08000000;reset;shutdown"
