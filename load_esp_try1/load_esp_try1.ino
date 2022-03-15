 
//Prateek
//www.prateeks.in

#include <ESP8266WiFi.h>
#include "HX711.h"
//#define BLYNK_PRINT Serial
//#include <Blynk.h>
//#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);
//#define BLYNK_PRINT Serial
 
const char *ssid = "PatelsWifi4G"; // replace with your wifi ssid and wpa2 key
const char *pass = "9820769386";
//char auth[] = "jf5vqqdqSziGVbsdgncbvP5CffTAgqiz";    // You should get Auth Token in the Blynk App.
 
WiFiClient client;
 
HX711 scale(D4, D5);
 
//int rbutton = D4; // this button will be used to reset the scale to 0.
float weight;
float calibration_factor = -467300; // for me this vlaue works just perfect 419640
// -107325
void setup() 
{
  Serial.begin(9600);
//  Wire.begin(D1, D2);
   lcd.begin();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Welcome To");
  lcd.setCursor(0,1);
  lcd.print("JustDoElectronic");
  delay(3000);
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("IoT");
  lcd.setCursor(1,1);
  lcd.print("Weight Scale");
  delay(3000);
  lcd.clear();
//  Serial.begin(115200);
//  pinMode(rbutton, INPUT_PULLUP); 
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
//  Blynk.begin(auth, ssid, pass);
 
 
  lcd.print("Connecting Wifi");
  
  WiFi.begin(ssid, pass);
  {
  delay(1000);
  Serial.print(".");
  lcd.clear();
  }
  Serial.println("");
  Serial.println("WiFi connected");
  lcd.clear();
  lcd.print("WiFi connected");
  
  delay(2000);
}
 
void loop() 
 
{
 
//  Blynk.run();
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
 
  weight = scale.get_units(5); 
 
  lcd.setCursor(0, 0);
  lcd.print("Measured Weight");
  lcd.setCursor(0, 1);
  lcd.print(weight);
  lcd.print(" KG  ");
//  Blynk.virtualWrite(V3, weight);
  delay(100);
  lcd.clear();
  
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" KG");
  
 
//  if ( digitalRead(rbutton) == LOW)
//{
//  scale.set_scale();
//  scale.tare(); //Reset the scale to 0
//}
// 
}
