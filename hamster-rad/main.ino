#include <LowPower.h>

const int led_pin = 4;
const int reed_pin = 2;

int n=0;
bool wake_on_rising_reed=false;
void on_rising_reed() {
	n++;
	wake_on_rising_reed=true;
}

void setup() {
  for(int k=0; k<10; ++k)
	pinMode(led_pin+k, OUTPUT);
  pinMode(reed_pin, INPUT_PULLUP);
  attachInterrupt(0,on_rising_reed,RISING);
  Serial.begin(9600);
}

void sleepNow() {
	Serial.println("going to sleep"); Serial.flush();
	// LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF); 
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
	
	display(n);
	if (!wake_on_rising_reed)
	{	
		delay(250);	
		display(0); // save power.
	}
	
	np=n;
	wake_on_rising_reed=false;
	sleepNow();
}
