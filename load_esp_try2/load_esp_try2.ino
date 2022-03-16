#include <ESP8266WiFi.h>
#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

const char *ssid = "PatelsWifi4G"; // replace with your wifi ssid and wpa2 key
const char *pass = "9820769386";

WiFiClient client;
HX711 scale(D4, D5);
 //d4 sck
 //d5 dt
float weight;
float calibration_factor = -467300; // for me this value works just perfect 419640
// -107325
void setup()
{
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
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
  delay(1000);
}

void loop() {
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  weight = scale.get_units(5);
  lcd.setCursor(0, 0);
  float noOfUnits = weight/0.175;
  if(noOfUnits - int(noOfUnits) < 0.5){
    noOfUnits = int(noOfUnits);
  }else{
    noOfUnits = int(noOfUnits)+1;
  }
  lcd.print("No of units left");
  lcd.setCursor(0, 1);
  lcd.print(noOfUnits);
  delay(100);
  Serial.print("No of units left: ");
  Serial.println(noOfUnits);
}
