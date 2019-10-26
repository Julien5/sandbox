#include <RH_ASK.h>
#include <SPI.h>

RH_ASK driver;

void setup()
{
    Serial.begin(9600);   // Debugging only
    if (!driver.init())
         Serial.println("init failed");
    pinMode(12, OUTPUT);
}

void loop()
{
    const char *msg = "hello";
    digitalWrite(12, HIGH);
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    digitalWrite(12, LOW);
    delay(1000);
}
