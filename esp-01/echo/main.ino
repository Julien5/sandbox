void setup() {
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

char x='0';

void loop() {
  digitalWrite(11, HIGH);
  while(Serial.available()) {
    Serial.write("[");
    digitalWrite(12, HIGH);
    Serial.write(Serial.read());
    digitalWrite(12, LOW);
    Serial.write("[");    
  }
  digitalWrite(11, LOW);
  digitalWrite(13, HIGH);
  Serial.print("hi");
  digitalWrite(13, LOW);
  delay(250);
}
