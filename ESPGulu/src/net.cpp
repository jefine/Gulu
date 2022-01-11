
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include "../lib/ArduinoJson/ArduinoJson.h"
#include "net.h"
#include "cube.h"
 
extern enum DemoStatus status;
extern WiFiClient espClient;
extern PubSubClient client;

void WiFi_Connect()
{
	WiFi.begin("abc", "123456789");
	while (WiFi.status() != WL_CONNECTED)
	{ //这里是阻塞程序，直到连接成功
		delay(300);
		Serial.print(".");
	}
}
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp="";
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  char buf[50]="";
  messageTemp.toCharArray(buf,sizeof(buf),0);
  client.publish("Gulu/ESP32", "ESP receved :");
  client.publish("Gulu/ESP32", buf);

  if(messageTemp=="Colourful_Egg"){
    client.publish("Gulu/ESP32", "MQTT receved Colourful_Egg \n status change to Colourful_Egg\n");
    status = Colourful_Egg;
  }
  if(messageTemp=="Weather_Humidity"){
    client.publish("Gulu/ESP32", "MQTT receved Weather_Humidity \n status change to Weather_Humidity\n");
    status = Weather_Humidity;
  }if(messageTemp=="Time"){
    client.publish("Gulu/ESP32", "MQTT receved Time \n status change to Time\n");
    status = Time;
  }
  
  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
//   if (String(topic) == "esp32/output") {
//     Serial.print("Changing output to ");
//     if(messageTemp == "on"){
//       Serial.println("on");
//       digitalWrite(ledPin, HIGH);
//     }
//     else if(messageTemp == "off"){
//       Serial.println("off");
//       digitalWrite(ledPin, LOW);
//     }
//   }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESPClient")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("Gulu/monitor");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
