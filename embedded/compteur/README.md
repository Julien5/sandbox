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

The second design is based around an ATmega328P. It includes a self-made [sensor](#the-sensor) and a ESP-01 module that transmits the data when needed (that is, when the energy consumption changes by certain amount).

#### The Sensor

Probably the infrared light is not particularly suitable for my Ferraris meter, because the mark is red, and the rest of the disc is metallic. Sending red light on it would result in reflected red light, with or without the mark, so the phototransistor would deliver the more or less the same signal. I ordered a bunch of LED and phototransistor with other wavelengths. I could get a nice contrast with the white LED and SFH 309-4 phototransistor. After some tweaking of the orientations of the LED and phototransistor, I could get a nice, clean signal.

![alt text](documentation/replay.png "Title")

#### Self-Calibration


## Features:
    
- Low-Power 
- Self-Calibrating

## 
    

