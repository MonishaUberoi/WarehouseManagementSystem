int buzzer = 10;
int smokeA0 = A5;
// Threshold value
int sensorThres = 350;

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  Serial.begin(9600);
}

void loop() {
  int analogSensor = analogRead(smokeA0);
  if (analogSensor > sensorThres)
  {
    tone(buzzer, 1000, 200);
    Serial.print("Smoke detected with concentration of :");
    Serial.print(analogSensor);
    Serial.println("ppm");
  }g
  else
  {
    noTone(buzzer);
  }
  delay(100);
}
