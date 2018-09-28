#include <LowPower.h>

const int led_pin = 4;
const int reed_pin = 2;

int last_time_change=0;

void setup() {
  for(int k=0; k<10; ++k)
	pinMode(led_pin+k, OUTPUT);
  pinMode(reed_pin, INPUT);
  Serial.begin(9600);
}

void sleepNow() {
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

int x=0;
int l=1;
int n=0;
int m=0;
int start_loop_time = 0;

void loop() {
	const int x=digitalRead(reed_pin);
	const bool changed=(x!=l);

	if (changed) {
		n++;
		last_time_change=millis();
		m=n/2;
		display(m);
	}
	
	int time_since_last_change = millis() - last_time_change;
	int measuring_time = millis() - start_loop_time;
	
	if (measuring_time > 1000 && time_since_last_change > 4000) {
		display(m);
		delay(250);
		clear_display();
		sleepNow();		
		start_loop_time = millis();
	}
	else
		delay(50);
	l=x;
}
