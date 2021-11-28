int led = 13;

void setup {
  Serial.begin(921600);
  pinMode(led, OUTPUT);
}

void loop  {
  Serial.println("On");
  digitalWrite(led, HIGH);
  delay(500);

  Serial.println("Off");
  digitalWrite(led, LOW);
  delay(500);
}
