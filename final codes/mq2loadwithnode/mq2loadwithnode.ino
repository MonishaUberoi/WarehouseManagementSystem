#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FirebaseArduino.h>
#include "HX711.h"
#include <Wire.h>

#define FIREBASE_HOST "warehouse-d9de2-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "rYNBFxHhOXZjQxl41cY7JLVcRVA4OxTvKqmmUHYp"
#define WIFI_SSID "Virus"
#define WIFI_PASSWORD "3833207173"

//MQ2
int smokeA0 = A0;
int sensorThres = 50;
int n=0;

//Load cell
WiFiClient client;
HX711 scale(D4, D5);
 //d4 sck, d5 dt
float weight;
float calibration_factor = -467300; // for me this value works just perfect 419640
// -107325

//NTP
const long utcOffsetInSeconds = 19800;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup(){
  pinMode(smokeA0, INPUT);
  Serial.begin(115200);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading

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
  //NTP
  timeClient.update();

  //MQ2
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
  Serial.print("pushed: /MQ2Sensor/");
  Serial.println(data);

  //Load Cell
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  weight = scale.get_units(5);
  float noOfUnits = weight/0.175;
  if(noOfUnits - int(noOfUnits) < 0.5){
    noOfUnits = int(noOfUnits);
  }else{
    noOfUnits = int(noOfUnits)+1;
  }
  Serial.print("No of units left: ");
  Serial.println(noOfUnits);
  String data = String(daysOfTheWeek[timeClient.getDay()]) + " " + String(timeClient.getFormattedTime()) + " " + String(noOfUnits);
  String name = Firebase.pushString("Inventory/Nirma Soap", data);
  // handle error
  if (Firebase.failed()) {
    Serial.println(Firebase.error());
    Serial.print("setting /number failed:");
    return;
  }
  Serial.print("pushed: /Inventory/Nirma Soap : ");
  Serial.println(data);
  
  delay(500);
}
