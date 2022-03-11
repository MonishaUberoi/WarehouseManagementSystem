#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>


// Set these to run example.
#define FIREBASE_HOST "esptest-b8db8-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "bvvGw3P5Aq3bzD88H9UBsL9XYpKHVDdfdmI61ThU"

//#define FIREBASE_HOST "esptest-f9688-default-rtdb.europe-west1.firebasedatabase.app"
//#define FIREBASE_AUTH "5kOvV5fQbko6jDD2MZEjL7Y5NsfrbNtddjjJofIK"
#define WIFI_SSID "Galaxy M21AD78"
#define WIFI_PASSWORD "eaua4459"

int smokeA0 = A0;
// Threshold value
int sensorThres = 50;

void setup() {
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
}
 int n=0;
void loop() {

    int analogSensor = analogRead(smokeA0);
    Serial.println(analogSensor);
    
    if (analogSensor > sensorThres){
      Serial.print("Smoke detected with concentration of :");
      Serial.print(analogSensor);
      Serial.println("ppm");
      // Firebase.setFloat("number", analogSensor);
      // append a new value to /sensorValue
      String name = Firebase.pushFloat("sensorValue", analogSensor);
    }
    
    // handle error
    if (Firebase.failed()) {
      Serial.println(Firebase.error());
      Serial.print("setting /number failed:");
      return;
    }
    Serial.print("pushed: /sensorValue/");
    Serial.println(analogSensor);
    delay(5000);
}
