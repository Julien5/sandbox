ifndef SRCSDIR
$(error SRCSDIR not set)
endif
OBJSDIR := /tmp/build/esp8266/$(NAME)

# todo: remove duplicates.
INCLUDE += -I$(SRCSDIR) $(XINCLUDE)

SRCS := $(shell find $(SRCSDIR) -maxdepth 1 \( \( -name "*.cpp" -and -not -name "*_*" \) -or  -name "*_esp8266.cpp" \))
OBJS := $(patsubst $(SRCSDIR)/%.cpp,$(OBJSDIR)/%.o,$(SRCS))

.PHONY: dir lib exe flash

dir:
	mkdir -p $(OBJSDIR) 

lib: dir $(OBJS) 
	$(AR) rcs $(OBJSDIR)/lib$(NAME).a $(OBJS)

exe: dir $(OBJS)
	$(CXX) $(OBJS) $(XLIBS) $(LIBS) $(LDFLAGS) -o $(OBJSDIR)/$(NAME).esp8266
	esptool.py elf2image --version=2 -fs 16m -fm qio -ff 40m $(OBJSDIR)/$(NAME).esp8266 -o $(NAME).esp8266

flash: exe
	esptool.py -p /dev/ttyUSB0 --baud 115200 write_flash -fs 16m -fm qio -ff 40m \
	0x0 /opt/esp8266/esp8266-toolchain/esp-open-rtos/bootloader/firmware_prebuilt/rboot.bin \
	0x1000 /opt/esp8266/esp8266-toolchain/esp-open-rtos/bootloader/firmware_prebuilt/blank_config.bin \
	0x2000 $(NAME).esp8266

monitor:
	screen -L /dev/ttyUSB0 115200

$(OBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	$(CXX) $(DEFINES) $(CXXFLAGS) $(XINCLUDE) $(INCLUDE) -c -o $@ $<

clean: dir
	find $(OBJSDIR) -type f -delete -print
