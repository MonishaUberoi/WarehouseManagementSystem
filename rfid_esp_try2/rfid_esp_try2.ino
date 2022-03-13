
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <LiquidCrystal_I2C.h>
#include <FirebaseArduino.h>
// Set these to run example.
#define FIREBASE_HOST "esptest-b8db8-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "bvvGw3P5Aq3bzD88H9UBsL9XYpKHVDdfdmI61ThU"
//#define FIREBASE_HOST "esptest-f9688-default-rtdb.europe-west1.firebasedatabase.app"
//#define FIREBASE_AUTH "5kOvV5fQbko6jDD2MZEjL7Y5NsfrbNtddjjJofIK"

//LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//RFID
#define SS_PIN D4
#define RST_PIN D0    // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

//buzzer
#define BUZZ_PIN D8

//led
#define GATE_PIN D3

//***********Things to change*******************
const char* ssid = "PatelsWifi4G";
const char* password = "9820769386";
//const char* ssid = "Monisha Uberoi";
//const char* password = "passsword";

//***********Things to change*******************
uint64_t openGateMillis = 0;
WiFiClientSecure client;

void setup() {
  pinMode(GATE_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  digitalWrite(GATE_PIN, LOW);
  digitalWrite(BUZZ_PIN, LOW);
  Serial.begin(115200);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  lcd.begin(); // Init with pin default ESP8266 or ARDUINO
  // lcd.begin(0, 2); //ESP8266-01 I2C with pin 0-SDA 2-SCL
  // Turn on the blacklight and print a message.
  lcd.backlight();
  LcdClearAndPrint("Loading");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Started");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  LcdClearAndPrint("Ready");
}

int n = 0;
byte readCard[4];
void LcdClearAndPrint(String text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
}
void loop() {
  if (openGateMillis > 0 && openGateMillis < millis()) {
    CloseGate();
  }
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.println(F("Scanned PICC's UID:"));
  String uid = "";
  for (uint8_t i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
    uid += String(readCard[i], HEX);
  }
  Serial.println("");
  Beep();
  LcdClearAndPrint("Please wait...");
  int no = Firebase.getInt("NumberOfUsers");
  bool flag = 0;
  for (int i = 1; i <= no; i++) {
    String data = "/Rfid User" + String(i);
    String userId = Firebase.getString(data + "/tag");
    if ((uid).equalsIgnoreCase(userId) ) {
      String name = Firebase.pushString("RFID Readings", uid);
      LcdClearAndPrint(Firebase.getString(data + "/Name"));
      flag = 1;
      Serial.print("pushed: /RFID Readings/");
      Serial.println(uid);
      Beep();
      delay(500);
      Beep();
      break;
    }
  }
  if (flag == 0) {
    LcdClearAndPrint("Acess Denied!");
    Siren();
  }
  flag = 0;
  delay(100);
  // String data = sendData("id=" + unitName + "&uid=" + uid, NULL);
  // HandleDataFromGoogle(data);
  mfrc522.PICC_HaltA();
  // handle error
  if (Firebase.failed()) {
    Serial.println(Firebase.error());
    Serial.print("setting /number failed:");
    return;
  }

  delay(5000);
}



void Siren() {
  for (int hz = 440; hz < 1000; hz++) {
    tone(BUZZ_PIN, hz, 50);
    delay(5);
  }

  for (int hz = 1000; hz > 440; hz--) {
    tone(BUZZ_PIN, hz, 50);
    delay(5);
  }
  digitalWrite(BUZZ_PIN, LOW);
}
void Beep() {
  for (int i = 0; i < 1000; i++)
  {
    analogWrite(BUZZ_PIN, i);
    delayMicroseconds(50);
  }
  digitalWrite(BUZZ_PIN, LOW);
}
void Beep2() {
  tone(BUZZ_PIN, 1000, 30);
  delay(300);
  digitalWrite(BUZZ_PIN, LOW);
}



void OpenGate() {
  openGateMillis = millis() + 5000;
  digitalWrite(GATE_PIN, HIGH);
  Beep();
  delay(100);
  Beep();
}
void CloseGate() {
  openGateMillis = 0;
  digitalWrite(GATE_PIN, LOW);
  Beep2();
  LcdClearAndPrint("Ready");
}
