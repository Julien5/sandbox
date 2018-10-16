#include <RFTransmitter.h>

#define NODE_ID          1
#define OUTPUT_PIN       13

// Send on digital pin 11 and identify as node 1
RFTransmitter transmitter(OUTPUT_PIN, NODE_ID);

void setup() {
	pinMode(12, OUTPUT);
}

void loop() {
  char *msg = "Hello World!";
  digitalWrite(12, HIGH);
  transmitter.send((byte *)msg, strlen(msg) + 1);
  digitalWrite(12, LOW);
  delay(5000);
  digitalWrite(12, HIGH);
  transmitter.resend((byte *)msg, strlen(msg) + 1);
  digitalWrite(12, LOW);
}
