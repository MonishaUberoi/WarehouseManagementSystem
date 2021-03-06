#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FirebaseArduino.h>
#include "HX711.h"
#include <Wire.h>

//#define FIREBASE_HOST "warehouse-d9de2-default-rtdb.europe-west1.firebasedatabase.app"
//#define FIREBASE_AUTH "rYNBFxHhOXZjQxl41cY7JLVcRVA4OxTvKqmmUHYp"
#define FIREBASE_HOST "warehousemanagementsyste-a79b0-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "E1LiEDuFTcc6KCEYG4EYwCTfe23xNXZofuK8jjac"
//#define WIFI_SSID "PatelsWifi4G"
//#define WIFI_PASSWORD "9820769386"

//#define WIFI_SSID "Drp"
//#define WIFI_PASSWORD "asdfghjkl"

#define WIFI_SSID "Monisha Uberoi"
#define WIFI_PASSWORD "passsword"

//MQ2
int smokeA0 = A0;
int sensorThres = 200;
int n = 0;

//buzzer
#define BUZZ_PIN D8

//led
#define GATE_PIN D3

//Load cell
WiFiClient client;
HX711 scale;
//d5 sck, d6 dt
float weight;
float calibration_factor =-467300;  // for me this value works just perfect 419640
// -107325
//-467300;

//NTP
const long utcOffsetInSeconds = 19800;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  pinMode(smokeA0, INPUT);
  pinMode(GATE_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  digitalWrite(GATE_PIN, LOW);
  digitalWrite(BUZZ_PIN, LOW);
  Serial.begin(115200);
  scale.begin(D5, D6);
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
  String content;
  //MQ2
  int analogSensor = analogRead(smokeA0);
  Serial.println(analogSensor);
  
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
  String name = Firebase.pushString("MQ2Sensor", content);
  if (analogSensor > sensorThres) {
      Siren();
      return;
  }
  // handle error
  if (Firebase.failed()) {
    Serial.println(Firebase.error());
    Serial.print("setting /number failed:");
    return;
  }
  Serial.print("pushed: /MQ2Sensor/");
  Serial.println(content);

  //Load Cell
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  weight = scale.get_units(5);
  Serial.print("weight:");
  Serial.println(weight);
  int noOfUnits = int(weight/0.170);
  if (noOfUnits - int(noOfUnits) < 0.5) {
    noOfUnits = int(noOfUnits);
  } else {
    noOfUnits = int(noOfUnits) + 1;
  }
  Serial.print("No of units left: ");
  Serial.println(noOfUnits);
  Firebase.setString("Inventory/Nirma soap", "175gm " + String(int(noOfUnits)));
  // handle error
  if (Firebase.failed()) {
    Serial.println(Firebase.error());
    Serial.print("setting /number failed:");
    return;
  }
  Serial.print("pushed: /Inventory/Nirma Soap : ");
  Serial.println(noOfUnits);

  //Light conversion
  int no = Firebase.getInt("NumberOfPeople");
  if(no<1){
    digitalWrite(GATE_PIN, LOW);
  }else{
    digitalWrite(GATE_PIN, HIGH);
  }

  delay(500);
}
void Siren() {
  for (int hz = 1000; hz < 4000; hz++) {
    tone(BUZZ_PIN, hz, 50);
    delay(5);
  }
  for (int hz = 4000; hz > 1000; hz--) {
    tone(BUZZ_PIN, hz, 50);
    delay(5);
  }
  digitalWrite(BUZZ_PIN, LOW);
}
