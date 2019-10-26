Standalone Arduino ISP Breadboard.

Breadboard:
http://www.netzmafia.de/skripten/hardware/Arduino/Bootloader_Flashen/
https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard

- with 2 22pF for the quarz
- with 22uF between reset and GND
- and LED 

Good informations at:
https://learn.sparkfun.com/tutorials/installing-an-arduino-bootloader

* using arduino 1.0.2 
burn bootload works
upload using programmer works

what does the IDE.
burn bootloader:

first:
/home/julien/arduino/arduino-1.0.2/hardware/tools/avrdude -C/home/julien/arduino/arduino-1.0.2/hardware/tools/avrdude.conf -v -v -v -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -e -Ulock:w:0x3F:m -Uefuse:w:0x05:m -Uhfuse:w:0xde:m -Ulfuse:w:0xff:m 
...
avrdude: verifying ...
avrdude: 1 bytes of lfuse verified
avrdude: Send: Q [51]   [20] 
avrdude: Recv: . [14] 
avrdude: Recv: . [10] 

avrdude done.  Thank you.


This line of commands is
/home/julien/arduino/arduino-1.0.2/hardware/tools/avrdude -C/home/julien/arduino/arduino-1.0.2/hardware/tools/avrdude.conf -v -v -v -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -e -Ulock:w:0x3F:m -Uefuse:w:0x05:m -Uhfuse:w:0xde:m -Ulfuse:w:0xff:m 

=> ok from the shell
=> version 5.11

using arduino 1.8.6 => avrdude 6.3
/home/julien/arduino/arduino-1.8.6/hardware/tools/avr/bin/avrdude -C/home/julien/arduino/arduino-1.8.6/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P/dev/ttyACM0 -b19200 -e -Ulock:w:0x3F:m -Uefuse:w:0xFD:m -Uhfuse:w:0xDE:m -Ulfuse:w:0xFF:m 
avrdude: stk500_getparm(): (a) protocol error, expect=0x14, resp=0x14
avrdude: stk500_getparm(): (a) protocol error, expect=0x14, resp=0x02
avrdude: stk500_getparm(): (a) protocol error, expect=0x14, resp=0x10

Now trying to get the same result as 1.0.2 with avrdude from arduino-core in 18.04 LTS.
I change -carduino to -cstk500v1. (Note: ArduinoISP sketch is loaded from arduino 1.8.6.)

avrdude -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -e -Ulock:w:0x3F:m -Uefuse:w:0xFD:m -Uhfuse:w:0xDE:m -Ulfuse:w:0xFF:m 
output:
avrdude: verification error, first mismatch at byte 0x0000
         0xff != 0x3f

This is because of a "bug" (not really a bug) in avrdude. Default behavior is to check also unused fuse bits, not ignore them as in the previous version. 
0x3F = 0011 1111
0xFF = 1111 1111
for the lock bits, B7 and B6 are not used, to we can send 0xFF without changing the used bits.

=> 0xFF opens write access to bootloader section (and application section).

post-note: efuse:w:0xFD means extented bits set to FD = 11111101
BODLEVEL 101 means that we need VCC > 2.7 V.
Not good if i want to run with 2 AA batteries.
We need VCC > 1.8 V => BODLEVEL 110 => 11111110 => 0xFE
=> avrdude -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -Uefuse:w:0xFE:m 
still problems, my batteries are old, i don't get 1.8V with some
=> disable BOD => BODLEVEL 111
=> avrdude -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -Uefuse:w:0xFF:m


avrdude -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -e -Ulock:w:0xFF:m -Uefuse:w:0xFF:m -Uhfuse:w:0xDE:m -Ulfuse:w:0xFF:m


=> this succeeds. STK500 makes sense. Good.

Now flash the bootloader.

second
/home/julien/arduino/arduino-1.0.2/hardware/tools/avrdude -C/home/julien/arduino/arduino-1.0.2/hardware/tools/avrdude.conf -v -v -v -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -Uflash:w:/home/julien/arduino/arduino-1.0.2/hardware/arduino/bootloaders/optiboot/optiboot_atmega328.hex:i -Ulock:w:0x0F:m 

avrdude: Recv: . [10] 
################################################## | 100% 0.02s

avrdude: verifying ...
avrdude: 1 bytes of lock verified
avrdude: Send: Q [51]   [20] 
avrdude: Recv: . [14] 
avrdude: Recv: . [10] 

avrdude done.  Thank you.

Obviously i cannot see what command line arduino IDE 1.8.2 would trigger, since it fails at step 1 and stops.
So continue with command line from 1.0.2 and use avrdude 6.3 instead.
avrdude -v -v -v -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -Uflash:w:/home/julien/arduino/arduino-1.0.2/hardware/arduino/bootloaders/optiboot/optiboot_atmega328.hex:i -Ulock:w:0x0F:m 
fails with
avrdude: verification error, first mismatch at byte 0x0000
         0xcf != 0x0f
probable the same story as before: unused lock bits.
0xcf = 1100 1111
0x0F = 0000 1111
=> yeah.

So the correct command line is
avrdude -v -v -v -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -Uflash:w:/home/julien/arduino/arduino-1.0.2/hardware/arduino/bootloaders/optiboot/optiboot_atmega328.hex:i -Ulock:w:0xCF:m 
=> exit code = 0 => yeah.

What does 0xCF lock ? It locks the Bootloader section. Good. Makes sense.

Now we want to upload a sketch to the breadboarded atmega328p.

The arduino 1.0.2 does the following:
/home/julien/arduino/arduino-1.0.2/hardware/tools/avrdude -C/home/julien/arduino/arduino-1.0.2/hardware/tools/avrdude.conf -v -v -v -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -Uflash:w:/tmp/build2737034020291261776.tmp/Blink102.cpp.hex:i 

This yields:
avrdude -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -Uflash:w:/tmp/build2737034020291261776.tmp/Blink102.cpp.hex:i 

well is does not seem too complex. I want to make the hex from the command line.
Quickly write a "Hello" sketch. Compile with "make" and get ./build-uno/Hello.hex.
Lets try:
avrdude -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b19200 -Uflash:w:./build-uno/Hello.hex:i 
=> yeah
It works. The breadboarded atmega328p runs the sketch.

Now change the quarz to 4mhz and the led blinks slower.
=> yeah.


--

Downloading and compiling optiboot bootloader for custom freq:
https://github.com/Optiboot/optiboot/wiki/CompilingOptiboot

See also:
https://tttapa.github.io/Pages/Arduino/Bootloaders/ATmega328P-custom-frequency.html

/usr/share/arduino/hardware/arduino/boards.txt


--
microphone

EMY-6027P/N-R-42(IP67) => -42
EMY-63M/P => -38 => 12mV/pa
MCE 101 => -45 dB


TLC272
http://davidegironi.blogspot.com/2014/11/an-opamp-based-electret-condenser.html#.W6Yg6ESxX0p

http://tutorial45.com/arduino-projects-arduino-decibel-meter/
https://www.dfrobot.com/product-1663.html

http://www.analog.com/media/en/technical-documentation/data-sheets/lt1115fa.pdf
https://www.reichelt.com/de/en/operational-amplifier-dip-8-lt-1115-cn8-p148545.html


http://www.bdtic.com/en/linear/LT1677
http://www.allxref.com/ad/lt1677.htm
https://www.reichelt.de/operational-amplifier-4mhz-op-184-fsz-p185594.html?r=1

https://electronics.stackexchange.com/questions/53251/arduino-serial-communication-produces-noise-on-electret-microphone

https://learn.sparkfun.com/tutorials/sound-detector-hookup-guide

http://teelsys.com/?p=160