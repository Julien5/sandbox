ifndef SRCSDIR
$(error SRCSDIR not set)
endif
OBJSDIR := /tmp/build/esp8266/$(NAME)

# todo: remove duplicates.
INCLUDE += -I$(SRCSDIR) $(XINCLUDE)

ifndef SRCS
SRCS := $(shell find $(SRCSDIR) -maxdepth 1 \( \( -name "*.cpp" -and -not -name "*_*" \) -or  -name "*_esp8266.cpp" \))
endif

OBJS := $(patsubst $(SRCSDIR)/%.cpp,$(OBJSDIR)/%.o,$(SRCS))

.PHONY: dir lib elf flash core monitor

dir:
	mkdir -p $(OBJSDIR) 

lib: dir $(OBJS) 
	$(AR) rcs $(OBJSDIR)/lib$(NAME).a $(OBJS)

elf: dir $(OBJS)
	$(AR) cru $(OBJSDIR)/lib$(NAME).a $(OBJS)
	$(CXX) $(LDFLAGS) \
	-Wl,--start-group $(LIBS) $(OBJSDIR)/lib$(NAME).a $(XLIBS) -Wl,--end-group \
	-o $(OBJSDIR)/$(NAME).elf -Wl,-Map=$(OBJSDIR)/$(NAME).map

	python /opt/esp8266/esp8266-toolchain-espressif/ESP8266_RTOS_SDK/components/esptool_py/esptool/esptool.py --chip esp8266 \
	elf2image --flash_mode "dio" --flash_freq "40m" --flash_size "2MB" --version=3 -o $(OBJSDIR)/$(NAME).esp8266 $(OBJSDIR)/$(NAME).elf

flash: elf
	python /opt/esp8266/esp8266-toolchain-espressif/esptool/esptool.py --chip esp8266 \
	--port "/dev/ttyUSB1" --baud 460800 --before "default_reset" --after "hard_reset" write_flash -z --flash_mode "dio" --flash_freq "40m" --flash_size "2MB"   \
	0x0000 /tmp/build/esp8266/core/bootloader/bootloader.bin \
	0x10000 $(OBJSDIR)/$(NAME).esp8266  \
	0x8000 /tmp/build/esp8266/core//partitions_singleapp.bin

monitor:
	python /opt/esp8266/esp8266-toolchain-espressif/ESP8266_RTOS_SDK/tools/idf_monitor.py --enable-savelog screenlog.0 --baud 74880 --port "/dev/ttyUSB2" --toolchain-prefix "xtensa-lx106-elf-" --make "make" $(OBJSDIR)/$(NAME).elf

$(OBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	$(CXX) $(DEFINES) $(CXXFLAGS) $(XINCLUDE) $(INCLUDE) -c -o $@ $(shell realpath --relative-to=. $<)

clean: dir
	find $(OBJSDIR) -type f -delete -print
