# Energy Monitor Project

The goal of that project is to (have fun and) monitor and log my electric energy consumption. The idea would be: I stand in my kitchen, turn on the 2000 W kettle on, and see on my smartphone how many Watts are drawn, in real-time. 

The requirements are the following:
1. The electricy meter of my house is an old, analog, "Ferraris" meter. It has a rotating metallic disc with a dark mark, as shown [here](https://de.wikipedia.org/wiki/Ferraris-Z%C3%A4hler#/media/Datei:ElectricityMeterMechanism.jpg). We will have to make a sensor to measure the speed of rotation of the disc. We can do this detecting the mark.
2. This mark detection implies measuring a signal value and comparing it to a threshold. We want the threshold to adapt automatically, no manual adjustment.
3. The electricy meter is in the entrance of the house, at a place where there is no power outlet. The whole system has to run on batteries and must be **low-power**. Low-power can be achieved putting the microcontroller in deep-sleep most of the time.
4. The sensor data should be sent per WLAN to a server on a raspberry pi.

#### First Attempts With ESP8266 And TRCT5000

As probably 99% of humanity, I started my project googling `Ferraris electricy meter arduino`. (That's a mistake. Don't google.) In the first result (and many others), the mark is detected with a ready-to-use infrared [TRCT5000](https://www.az-delivery.de/products/linienfolger-modul-mit-tcrt5000-und-analog-ausgang) arduino module. This module has (1) an infrared LED, and (2) a phototransistor that measure the (reflected) light.  Assuming the mark reflects infrared light differently (less or more) than the rest of the disc, we could detect the mark. And since we need WLAN connectivity, it seems obvious to try with a ESP8266 first.
Unfortunately, my attempt with to use the TRCT500 with ESP8266 failed because the signal-to-noise ratio at the ADC was not good enough to detect the mark reliably. Moreover, the wake-up time of ESP8266 after deep-sleep is quite long (300ms, if I remember well), which make a low-power operation impossible.

#### Second Attempt With ATmega328P And Custom Phototransistor

The second design is based around an ATmega328P. It includes a self-made sensor and a ESP-01 module that transmits the data when needed (that is, when the energy consumption changes by certain amount).

#### The Sensor

I suspected the TRCT5000 failed because the infrared light is not particularly suitable for my Ferraris meter, because the mark is red, and the rest of the disc is metallic. Sending red light on it would result in reflected red light, with or without the mark, so the phototransistor would deliver the more or less the same signal. 

So I had to work with any light, but not red. I ordered a bunch of LED and phototransistor with other wavelengths. I could get a nice contrast with the white LED and SFH 309-4 phototransistor. After tweaking the orientation of the LED and phototransistor, I could get a nice, clean signal. The orientation seems to play a significant role, maybe because the plastic front cover of the Ferraris meter reflects the light. 

![adc](documentation/adc.png "ADC")

_Note_: The influence of ambient light can be compensated by removing an offset to the adc output:
```
adc - offset
```
where `offset` is the adc output when the LED is turned off.

#### Self-Calibration

To find the threshold, we exploit that there is an abrupt change of the adc values when the mark passes. So we use:
- A simple low-pass filter of the adc output
```
xalpha = alpha_1 * xalpha + (1 - alpha_1) * adc;
```
Since the mark is only on a short section of the disc, `xalpha` will represent the `adc` value when the mark is not in front of the sensor. The constant `alpha_1` is hard-coded.
- The difference `delta` between the adc and `xalpha` is large (and below zero) when the mark passes:
```
delta = adc - xalpha;
```

Now the question: how large is "large" ? 
Assuming we know the maximum value taken by `delta`, noted `delta_max`, `delta` would be large enough when it is closer to `delta_max` than `delta`.

In details, we track the mean and the maximum value of delta, `delta_mean` and `delta_max`, as: 
- `delta_mean`
```
delta_mean = alpha_2 * delta_mean + (1 - alpha_2) * delta;
```
The constant `alpha_2` is hard-coded.
- If `delta < delta_max`, then 
```
delta_max = delta
```
- otherwise
```
delta_max = alpha_3 * delta_max + (1 - alpha_3) * delta;
```
The constant `alpha_3` is hard-coded, larger than `alpha_2`.

![adc](documentation/calibration.png "ADC")
See the source code `detection.cpp` for details.    

