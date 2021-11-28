#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include "../lib/ArduinoJson/ArduinoJson.h"
#include "net.h"

const char* mqtt_server = "broker.emqx.io";
WiFiClient espClient;
PubSubClient client(espClient);
// JOSN是一种轻量级的数据交换格式，遵循一下规则：
// 1.并列的数据之间用逗号(,)分隔
// 2.映射用冒号(:)表示
// 3.并列数据的集合(数组)用方括号([])表示
// 4.映射的集合(对象)用大括号({})表示

void setup()
{

	Serial.begin(115200);
  // put your setup code here, to run once:
  unsigned char t[] = {0XAA ,0X55 ,0X01 ,0X0A ,0X00 ,0X00 ,0X00 ,0X0B,0X00 ,0X00 ,0X00 ,0X55 ,0XAA};
  //ESP32每次发送时，会有串口乱码输出，不影响，有校验
  Serial.write(t,13);

	Serial.print("Connecting.. ");

	WiFi_Connect();

	Serial.println("WiFi connected");

	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}



// bilibili api: follower
// some useful UrlAPI 
String iplocationUrl = "http://api.bilibili.com/x/web-interface/zone";
String timeUrl = "http://worldtimeapi.org/api/ip";

DynamicJsonDocument doc(1024);

void getUrl()
{
	HTTPClient http;
	http.begin(iplocationUrl); //HTTP begin
	int httpCode = http.GET();

	if (httpCode > 0)
	{
		// httpCode will be negative on error
		Serial.printf("HTTP Get Code: %d\r\n", httpCode);

		if (httpCode == HTTP_CODE_OK) 
		{
			String resBuff = http.getString();

			Serial.println(resBuff);
			

			deserializeJson(doc, resBuff); //开始使用Json解析

			String addr = doc["data"]["addr"];
			char buf[50]="";
			addr.toCharArray(buf,sizeof(buf),0);
			Serial.print("send is: ");
			for(auto c : buf)
			{
				Serial.print(c);
			}
			Serial.println();
			client.publish("Gulu/ESP32", buf);
			client.publish("Gulu/ESP32", "测试中文");
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
}

void loop()
{
	if (!client.connected()) {
		reconnect();
	}
	client.loop();
	getURL();
	delay(5000);
}
