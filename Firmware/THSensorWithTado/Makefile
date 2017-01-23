# base directory of the ESP8266 Open SDK package, absolute
ESP8266_SDK_ROOT ?= /opt/esp-open-sdk

# base directory for the compiler
XTENSA_TOOLS_ROOT ?= $(ESP8266_SDK_ROOT)/xtensa-lx106-elf/bin

# select which tools to use as compiler, librarian and linker
AR  := $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-ar
CC  := $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-gcc
CXX := $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-gcc
LD  := $(XTENSA_TOOLS_ROOT)/xtensa-lx106-elf-gcc

# compiler flags using during compilation of source files
CFLAGS = -I. -I../Libraries/ContikiCore -mlongcalls
LDLIBS = -nostdlib -L../Libraries/ContikiCore
LDLIBS += -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -Wl,--end-group
LDLIBS += -lcontikicore -lgcc -lc
LDFLAGS = -Teagle.app.v6.ld

# library name
PROG = thsensorwithtado

# list of source code files
OBJ = main.o

%.o: %.c
	@echo "CC	$<"
	@$(CC) -c $(CFLAGS) $< -o $@
 
%.o: %.cpp
	@echo "CXX	$<"
	@$(CC) -c $(CFLAGS) $< -o $@
	    
$(PROG) : $(OBJ)
	@echo "LD	$@"
	@$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

all: $(PROG)

clean:
	rm -rf $(OBJ) 
	rm -rf $(PROG)