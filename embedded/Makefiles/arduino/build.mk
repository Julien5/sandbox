ifndef SRCSDIR
SRCSDIR := /usr/share/arduino/libraries/$(NAME)
endif
OBJSDIR := /tmp/build/arduino/$(NAME)

# todo: remove duplicates.
INCLUDE += -I$(SRCSDIR) $(XINCLUDE)

ifndef SRCS
SRCS := $(shell find $(SRCSDIR) -maxdepth 1 \( \( -name "*.cpp" -and -not -name "*_*" \) -or  -name "*_arduino.cpp" \))
endif

OBJS := $(patsubst $(SRCSDIR)/%.cpp,$(OBJSDIR)/%.o,$(SRCS))

dir:
	mkdir -p $(OBJSDIR) 

$(OBJSDIR)/lib$(NAME).a: $(OBJS)
	$(AR) rcs $(OBJSDIR)/lib$(NAME).a $(OBJS)

lib: dir $(OBJS) $(OBJSDIR)/lib$(NAME).a

hex: deps elf
	$(OBJCOPY) -O ihex -R .eeprom $(OBJSDIR)/$(NAME).elf $(OBJSDIR)/$(NAME).hex

elf: dir $(OBJS) 
	$(CXX) $(CFLAGS) $(LDFLAGS) $(OBJS) $(XLIBS) $(LIBS) -o $(OBJSDIR)/$(NAME).elf

showsize: elf
	$(AVRSIZE) --mcu=atmega328p -C --format=avr  $(OBJSDIR)/$(NAME).elf

#  uno: ttyACM? 115200
ARDUINO_PORT:=/dev/ttyACM0
ARDUINO_BAUD:=115200
# nano: ttyUSB? 57600
#ARDUINO_PORT:=/dev/ttyUSB0
#ARDUINO_BAUD:=57600

reset:
	@$(if $(strip $(ARDUINO_PORT)),,echo could not find arduino port!)
	/usr/bin/ard-reset-arduino $(ARDUINO_PORT)

flash: hex reset showsize
	$(AVRDUDE) -q -V -p atmega328p \
	-C /usr/share/arduino/hardware/tools/avrdude.conf \
	-D -c arduino -b $(ARDUINO_BAUD) -P $(ARDUINO_PORT) \
	-U flash:w:$(OBJSDIR)/$(NAME).hex:i

monitor:
	rm -f screenlog.*
	screen -L $(ARDUINO_PORT) 9600

$(OBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(DEFINES) $(CXXFLAGS) -include Arduino.h $(XINCLUDE) $(INCLUDE) -c -o $@ $<

clean: dir
	find $(OBJSDIR) -type f -delete -print

print:
	$(info MAKEFILESDIR is $(MAKEFILESDIR))	
	$(info INCLUDE is $(INCLUDE))
	$(info XINCLUDE is $(XINCLUDE))
	$(info OBJS is $(OBJS))
	$(info CXXOBJS is $(CXXOBJS))
	$(info COBJS is $(COBJS))

.PHONY: clean elf showsize hex lib dir reset flash monitor print
