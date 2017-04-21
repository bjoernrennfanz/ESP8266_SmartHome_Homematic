# base directory of the ESP8266 Open SDK package, absolute
ESP8266_RTOS_SDK_ROOT ?= /opt/esp-open-sdk
ESP8266_SDK_ROOT ?= /opt/esp-open-sdk
	
ifeq ($(OS),Windows_NT)
	ESP8266_RTOS_SDK_PATH = $(shell cygpath.exe -C ANSI -u ${ESP8266_RTOS_SDK_ROOT})
	ESP8266_SDK_PATH = $(shell cygpath.exe -C ANSI -u ${ESP8266_SDK_ROOT})
else	
	ESP8266_RTOS_SDK_PATH = $(ESP8266_RTOS_SDK_ROOT)
	ESP8266_SDK_PATH = $(ESP8266_SDK_ROOT)
endif

# base directory for the compiler
XTENSA_TOOLS_ROOT ?= $(ESP8266_SDK_PATH)/xtensa-lx106-elf/bin

# select which tools to use as compiler, librarian and linker
AR  := $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-ar
CC  := $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-gcc
CXX := $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-gcc
LD  := $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-gcc

# program name
PROGRAM = esp8266-smarthome-bridge

# additional components
EXTRA_COMPONENTS = \
	drivers \
	extras/dht

include $(ESP8266_RTOS_SDK_PATH)/common.mk