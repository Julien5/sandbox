#include <LowPower.h>
#include <Arduino.h>
const int led_pin = 4;
const int reed_pin = 2;

int n=0;
bool wake_on_rising_reed=false;
long last_time_rising_reed=0;

void on_rising_reed() {
	wake_on_rising_reed=true;
}

void alive_check() 
{
	pinMode(LED_BUILTIN, OUTPUT);
	for(int k=0;k<2;k++) {
		digitalWrite(LED_BUILTIN, HIGH);
		for(int i=0; i<10; ++i) 
			digitalWrite(led_pin+i, HIGH);
		delay(1000);
		digitalWrite(LED_BUILTIN, LOW);
		for(int i=0; i<10; ++i) 
			digitalWrite(led_pin+i, LOW);
		delay(1000);
	}
	for(int i=0; i<10; ++i) 
	{
		digitalWrite(led_pin+i, HIGH);
		delay(200);
		digitalWrite(led_pin+i, LOW);
		delay(200);
	}	
}

void setup() {
  for(int k=0; k<10; ++k)
  	  pinMode(led_pin+k, OUTPUT);
  alive_check();
  pinMode(reed_pin, INPUT_PULLUP);
  attachInterrupt(0,on_rising_reed,RISING);
}

void sleepNow() {
	LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
}

void display(int m)
{
	int k=0;
	for(int c = 9; c >= 0; c--) {
		k = m >> c;
		if (k & 1) 
			digitalWrite(led_pin+c, HIGH);
		else
			digitalWrite(led_pin+c, LOW);
	}
}

void clear_display() {
	display(0);
}

int np=0;

void loop() {
	long current_time=millis();
	int time_since_last_rising_reed = current_time-last_time_rising_reed;
	if (wake_on_rising_reed)
	{
		if (time_since_last_rising_reed>200)
			n++;
		last_time_rising_reed=current_time;
		wake_on_rising_reed=false;	
	}
	
	display(n);
	
	if (!wake_on_rising_reed && time_since_last_rising_reed>5000)
	{	
		delay(250);	
		display(0); // save power.
		sleepNow();
	}
}
