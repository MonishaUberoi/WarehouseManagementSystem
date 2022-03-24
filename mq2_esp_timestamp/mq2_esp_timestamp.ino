#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "warehouse-d9de2-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "rYNBFxHhOXZjQxl41cY7JLVcRVA4OxTvKqmmUHYp"
#define WIFI_SSID "Virus"
#define WIFI_PASSWORD "3833207173"

int smokeA0 = A0;
// Threshold value
int sensorThres = 50;
int n=0;

const long utcOffsetInSeconds = 19800;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup(){
  pinMode(smokeA0, INPUT);
  Serial.begin(115200);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  timeClient.begin();
}

void loop() {
  timeClient.update();
  int analogSensor = analogRead(smokeA0);
  Serial.println(analogSensor); 
  if (analogSensor > sensorThres){
    Serial.print("Smoke detected with concentration of :");
    Serial.print(analogSensor);
    Serial.println("ppm");
    // append a new value to /sensorValue
    String data = String(daysOfTheWeek[timeClient.getDay()]) + " " + String(timeClient.getFormattedTime()) + " " + String(analogSensor);
    String name = Firebase.pushString("MQ2Sensor", data);
  }
  // handle error
  if (Firebase.failed()) {
    Serial.println(Firebase.error());
    Serial.print("setting /number failed:");
    return;
  }
  Serial.print("pushed: /MQSensor/");
  Serial.println(data);
  delay(5000);
}
