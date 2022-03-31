#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "warehouse-d9de2-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "rYNBFxHhOXZjQxl41cY7JLVcRVA4OxTvKqmmUHYp"
#define WIFI_SSID "Monisha Uberoi"
#define WIFI_PASSWORD "passsword"

int smokeA0 = A0;
// Threshold value
int sensorThres = 0;
int n=0;

const long utcOffsetInSeconds = 19800;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
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
  String name,content;
  timeClient.update();
  int analogSensor = analogRead(smokeA0);
  Serial.println(analogSensor); 
  if (analogSensor > sensorThres){
    Serial.print("Smoke detected with concentration of :");
    Serial.print(analogSensor);
    Serial.println("ppm");
    // append a new value to /sensorValue
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime ((time_t *)&epochTime);
    String currentDate = String(ptm->tm_mday) + "/" + String(ptm->tm_mon + 1) + "/" + String(ptm->tm_year + 1900);

    //pushing and printing
    content = currentDate + " " + String(daysOfTheWeek[timeClient.getDay()]) + " " + String(timeClient.getFormattedTime()) + " " + String(analogSensor);
    name = Firebase.pushString("MQ2Sensorrr", content);
  }
  // handle error
  if (Firebase.failed()) {
    Serial.println(Firebase.error());
    Serial.print("setting /number failed:");
    return;
  }
  Serial.print("pushed: /MQSensor/");
  Serial.println(name);
  delay(5000);
}
