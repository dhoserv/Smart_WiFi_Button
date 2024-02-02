/* Smart WiFi Button */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>

// Variablen
#define button 12     // Pin for Webupdate
#define mqtt_user "mqtt-user" //MQTT Broker User
#define mqtt_password "mqtt-password" //MQTT Broker Password

// Wifi
const char* ssid = "wlam-ssid"; //WLAN SSID
const char* password = "wlan-password";   //WLAN Password
const char* Hostname = "hostname";

//Static IP address configuration
IPAddress staticIP(xxx, xxx, xxx, xxx); //ESP static ip
IPAddress gateway(xxx, xxx, xxx, xxx);    //IP Address of your WiFi Router (Gateway)
IPAddress subnet(xxx, xxx, xxx, xxx);   //Subnet mask
IPAddress dns(xxx, xxx, xxx, xxx);            //DNS

// MQTT
const char* mqtt_server = "ip-address-mqtt-broker"; //IP Address MQTT Broker
const char* mqtt_clientId = "clientid";
const char* outTopicMsg = "WiFibutton/dashbutton1/message";
const char* outTopicVCC = "WiFiButton/dashbutton1/vcc";
char msg[50];                         // message for mqtt publish

// Set web server port number to 80
WiFiServer server(80);

WiFiClient espClient;
PubSubClient client(espClient);

// ADC
ADC_MODE (ADC_VCC);                   // VCC Read

void setup(){ 
  pinMode(button, INPUT_PULLUP);                          
  Serial.begin(115200);  

// Setup Wifi
  setup_wifi();

// Setup MQTT
  client.setServer(mqtt_server, 1883);
} 

void setup_wifi() {
  int wifiCounter = 0;
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.hostname(Hostname);

  while (WiFi.status() != WL_CONNECTED) {
    if(wifiCounter < 10){
      delay(500);
      Serial.print(".");
    }else{ESP.deepSleep(0, WAKE_RFCAL);}
    wifiCounter++;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Client ID connected
    if (client.connect(mqtt_clientId, mqtt_user, mqtt_password)) {
      Serial.print(mqtt_clientId);
      Serial.println(" connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop(){ 
  // MQTT Client
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
// Read the VCC from Battery
  float vcc = ESP.getVcc() / 1000.0;
  vcc = vcc - 0.12;     // correction value from VCC

  if(digitalRead(button == HIGH)){
    client.publish(outTopicMsg, "ON");
    delay(50);
    client.publish(outTopicMsg, "OFF");
    delay(50);
    dtostrf(vcc, sizeof(vcc), 2, msg);
    client.publish(outTopicVCC, msg);
    delay(50);
    ESP.deepSleep(0, WAKE_RFCAL);
  }

}
