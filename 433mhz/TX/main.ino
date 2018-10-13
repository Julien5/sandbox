#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
void setup() {
  mySwitch.enableTransmit(12);  // Der Sender wird an Pin 10 angeschlossen
}
void loop() {
  mySwitch.send(1234, 24); // Der 433mhz Sender versendet die Dezimalzahl „1234“
  delay(1000);  // Eine Sekunde Pause, danach startet der Sketch von vorne
} 
	
