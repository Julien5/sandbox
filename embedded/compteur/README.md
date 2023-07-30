# Energy Monitor Project

The goal of that project is to (have fun, learn something, and) monitor and log the total electric energy consumption in my house. The vision: I stand in my kitchen, turn on the 2000 W kettle on, and see on my smartphone that 2000 more Watts are drawn, in real-time. 

The requirements are the following:
1. The electricy meter of my house is an old, analog, "Ferraris" meter. It has a rotating metallic disc with a mark on its edge, as shown [here](https://de.wikipedia.org/wiki/Ferraris-Z%C3%A4hler#/media/Datei:ElectricityMeterMechanism.jpg). We can estimate the speed of rotation of the disc by *detecting this mark*.
2. We don't want to adjust any parameter manually. The program should adapt its parameter autonomously (*self-calibration*).
3. The electricy meter is in the entrance of the house, at a place where there is no power outlet. Therefore, the whole system has to run on batteries and must be **low-power**. Low-power can be achieved putting the microcontroller in deep-sleep most of the time.
4. The sensor data should be sent *per WLAN* to a server (on a raspberry pi).

#### First Attempts With ESP8266 And TRCT5000

I started my project googling `Ferraris electricy meter arduino`. (That's a mistake. Don't do this, don't google.) In the first result (and many others), the mark is detected with a ready-to-use infrared [TRCT5000](https://www.az-delivery.de/products/linienfolger-modul-mit-tcrt5000-und-analog-ausgang) arduino module. This module has an infrared LED, and a phototransistor that measure the (reflected) light.  Assuming the mark reflects infrared light differently than the rest of the disc, we could detect the mark. And since we need WLAN connectivity, it seems obvious to try with a ESP8266 first.
Unfortunately, my attempt with the TRCT500 with ESP8266 failed because the signal-to-noise ratio at the ADC was not good enough to detect the mark reliably. Moreover, the wake-up time of ESP8266 after deep-sleep is quite long (300ms, if I remember well), which make a low-power operation difficult.

#### Second Attempt With ATmega328P And Custom Sensor

The second design is based around an ATmega328P. It includes a self-made sensor and a ESP-01 module that transmits the data when needed (that is, when the energy consumption changes by certain amount).

![comp](documentation/camera/small/components.jpg)

- The ATMEGA block controls the sensor and the ESP8266 block: it controls the LED, processes ADC measures, sends data to the ESP8266 block.
- The ESP8266 transmits the processed data per WLAN to a HTTP server (hosted on a RPI). 
- The sensor block include the LED, the phototansistor, and some resistor.

See below for details.

#### The Sensor

I suspect the TRCT5000 failed because the infrared light is not particularly suitable for my Ferraris meter. Its mark is red, and the rest of the disc is metallic. Sending red light on should result in reflected red light, with or without the mark, so the phototransistor would deliver the more or less the same signal, with or without the mark, offering a poor signal to detect the mark. 

![sensor](documentation/camera/small/sensor.jpg)

I ordered a bunch of LED and phototransistor with other wavelengths. I could get a most contrast with the white LED and SFH 309-4 phototransistor. After tweaking the orientation of the LED and phototransistor, I could get a nice, clean signal. The orientation seems to play a significant role, maybe because the plastic front cover of the Ferraris meter reflects the light. 

LED turned off (left) and on (right):
![LED turned off and on](documentation/camera/small/LED_off_on.jpg "LED turned off and on")

For more reliability, 

Phototansistor ADC output (noted `x` in the sequel)
![adc](documentation/adc.png "ADC")


_Note_: The influence of ambient light can be compensated by removing an offset to the adc output:
```
adc - offset
```
where `offset` is the adc output when the LED is turned off.

#### Self-Calibration

To find the threshold, we exploit that there is an abrupt change in the phototansistor signal when the mark passes. So we use:
- A simple low-pass filter 
```
xalpha = alpha_1 * xalpha + (1 - alpha_1) * x;
```
Since the mark is only on a short section of the disc, `xalpha` represents the phototransistor signal level when the mark is not in front of the sensor. (The constant `alpha_1` is hard-coded.)
- The difference `delta` between `x` and `xalpha` is large (and below zero) when the mark passes:
```
delta = x - xalpha;
```

Now the question: how large is "large" ? 
Assuming we know the maximum value taken by `delta`, noted `delta_max`, `delta` would be large enough when it is closer to `delta_max` than `delta`.

In details, we track the mean and the maximum value of delta, `delta_mean` and `delta_max`, as: 
- `delta_mean`
```
delta_mean = alpha_2 * delta_mean + (1 - alpha_2) * delta;
```
The constant `alpha_2` is hard-coded.
```
delta_max = min(delta,alpha_3 * delta_max + (1 - alpha_3) * delta_mean);
```
The constant `alpha_3` is hard-coded, larger than `alpha_2`. `min()` is used get the largest of negative values. See the source code `detection.cpp` for details.

The graph below shows the values and ticks when the mark is detected:
![adc](documentation/calibration.png "ADC")

Zoom on the short spike, the mark passed fast, the energy consumption was around 2000W:
![adc](documentation/calibration2.png "ADC")

### Low-Power

The main power consumer of the circuit is the LED. The LED should be turned on as less as possible, but enough to detect the mark when it passes in front on the sensor. I turn on/off the LED at a fixed frequency, which sets a limit on the speed of rotation of the mark. Considering a maximum power of 5kW and two measures in front of the mark at that power, I set the interval to 200ms. At higher power, the estimation of the speed of rotation will be less reliable.

### The Results

#### Kettle turned off. 
![sensor](documentation/camera/small/before.jpg)

I started the experiment at around 20:19. As can be seen, the last update was sent at 19:59.

#### Kettle turned on, 30 seconds later.
![hey](documentation/camera/small/after_1.jpg)

#### Kettle turned on, 30 seconds later.
![sensor](documentation/camera/small/after_2.jpg)

The data are sent only when significant changes (larger than 200W) are detected. 

### Build Instructions


#### Electronic

The ATmega and ESP8266 have both their own power supply. (I tested with one power supply for both but the ESP8266 seems to draw much power at startup, occasionally causing a reset of the ATmega.) The power supply are 4 AA batteries, delivering 6V. Since both microcontrollers operate a 3.3 V, I used a HT7333 power regulator.
![atmega](documentation/camera/small/regulator.jpg)

#### ATMEGA
![atmega](documentation/camera/small/atmega.jpg)

#### Sensor Block
![sensor](documentation/camera/small/sensor.jpg)

#### ESP8266 Block
![esp](documentation/camera/small/esp.jpg)


#### Firmware
The build instructions are configured for my setup, that looks like:
```
/opt/arduino/ArduinoCore-avr-1.8.2
/opt/arduino/libraries/LowPower
etc.
```

The cmake toolchain files are:
```
embedded/cmake/arduino/toolchain.cmake
embedded/cmake/esp8266/toolchain.cmake
embedded/cmake/pc/toolchain.cmake # for unit testing
```

##### ATmega328P
```
BUILDDIR=/tmp/build/atmega
mkdir -p $BUILDDIR 
cmake -DCMAKE_TOOLCHAIN_FILE=$DIR/embedded/cmake/arduino/toolchain.cmake -B $BUILDDIR 
make -j3 -C $BUILDDIR
```

##### ESP8266
```
BUILDDIR=/tmp/build/esp8266
mkdir -p $BUILDDIR 
cmake -DCMAKE_TOOLCHAIN_FILE=$DIR/embedded/cmake/esp8266/toolchain.cmake -B $BUILDDIR 
make -j3 -C $BUILDDIR
```
