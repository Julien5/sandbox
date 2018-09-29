#include <LowPower.h>

const int led_pin = 4;
const int reed_pin = 2;

int n=0;

void on_rising_reed() {
	n++;
}

void setup() {
  for(int k=0; k<10; ++k)
	pinMode(led_pin+k, OUTPUT);
  pinMode(reed_pin, INPUT_PULLUP);
  attachInterrupt(0,on_rising_reed,RISING);
  Serial.begin(9600);
}

void sleepNow() {
	return;
	Serial.println("going to sleep"); Serial.flush();
	delay(100);
	LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF); 
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

void loop() {
	display(n);
	delay(250);
}
