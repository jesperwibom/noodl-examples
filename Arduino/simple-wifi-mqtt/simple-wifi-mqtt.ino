/*
 *  Download Noodl at:
 *  http://www.getnoodl.com/
 *   
 *  Use Shiftr.io and create your own namespace:
 *  https://shiftr.io/try.
 *  
 */


//
// Libraries
//

#include <WiFi101.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>


//
// Configuration
//

const char *wifi_ssid = "myHotspot"; // write the name of the wifi INSIDE the quotation marks yourWifiNameHere
const char *wifi_pass = "password"; // write the password INSIDE the quotation marks yourPasswordHere

const char *mqtt_broker = "broker.shiftr.io";
const char *mqtt_name = "Arduino"; // you can change this to be anything
const char *mqtt_key = "c67b05a8"; //replace with token info from your shiftr namespace aBCd01234 5a8c5868
const char *mqtt_password = "206dd39e7aee15b5"; //replace with token info from your shiftr namespace aBcD0123ef56g7h8 173861e2b1ca817c

WiFiClient wifi_client; // Use ssl for arduino cloud
MQTTClient mqtt_client; // mqtt connection

#define NEOPIXEL_PIN 6
#define NEOPIXEL_NUM 16 // the number of leds connected together

Adafruit_NeoPixel led_strip = Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800); // neopixel led array

const unsigned int send_delay = 300;
unsigned long lastMillis = 0;


//
// SETUP
//

void setup() {

  
  pinMode(A1, OUTPUT);
  pinMode(A2, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, INPUT);

  //begin SERIAL, WIFI and MQTT communication
  Serial.begin(9600);
  WiFi.begin(wifi_ssid, wifi_pass);
  mqtt_client.begin(mqtt_broker, wifi_client);

  //start neopixel LED STRIP
  led_strip.begin();
  led_strip.show(); // all pixels 'off'
  setLedStripColor(20, 0, 0); //set to red starting color

}

//
// LOOP
//
// This method check if there is a connection then subscribe to the topic
// Then check for new message on that topic
// finaly it sends its own data to the publish topic
//
void loop() {

  // Check if Arduino lost connection
  if(!mqtt_client.connected()) {
    
    connectToWifi();
    connectToBroker();
    
    setSubscriptions();
    Serial.println("\nConnected and subscribed");
    
    setLedStripColor(0, 20, 0);
    delay(700);
    setLedStripColor(0, 0, 0);
  }


  // If there's an incoming message, the messageReceived callback function will be called
  mqtt_client.loop();

  if(millis() - lastMillis > send_delay) {
    sendData("/noodl");
    lastMillis = millis();
  }

  
}


void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  
  Serial.print("Topic received >> ");
  Serial.println(topic);
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(payload);
  
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  if (topic == "/arduino")
  {
    digitalWrite(4,root["D4"]);
    Serial.println("writing ALL values");
  }
  else if (topic == "/arduino/analog")
  {
    analogWrite(root["pin"],root["value"]);
    Serial.println("writing analog value");
  }
  else if (topic == "/arduino/digital")
  {
    digitalWrite(root["pin"],root["value"]);
    Serial.println("writing digital value");
  }
  else if (topic == "/arduino/neopixel")
  {
    int r = root["red"].as<int>();
    int g = root["green"].as<int>();
    int b = root["blue"].as<int>();
    setLedStripColor(r,g,b);
  }
  else
  {
    Serial.print("mqtt command >> ");
    Serial.print(topic);
    Serial.println(" << not recognized");
  }
  
  
}


void sendData(String topic){
  
  int cache;
  String payload;
  StaticJsonBuffer<400> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  cache = analogRead(A2);
  root["A2"] = cache;
  cache = digitalRead(4);
  root["D4"] = cache;
  
  root.printTo(payload);
  mqtt_client.publish(topic, payload);
  
}


void connectToWifi() {
  Serial.print("connecting wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nconnected!");
}

void connectToBroker() {
  Serial.print("\nconnecting mqtt...");
  while (!mqtt_client.connect(mqtt_name, mqtt_key, mqtt_password)) {
    Serial.print(":");
  }
  Serial.println("\nconnected!");
}

void setSubscriptions(){
  mqtt_client.subscribe("/arduino");
  mqtt_client.subscribe("/arduino/analog");
  mqtt_client.subscribe("/arduino/digital");
  mqtt_client.subscribe("/arduino/neopixel");
}


//
// HELPERS
//
void setLedStripColor(char r, char g, char b) {
  for(uint16_t i=0; i<led_strip.numPixels(); i++) {
    led_strip.setPixelColor(i, led_strip.Color(r, g, b));
    led_strip.show();
  }
}
