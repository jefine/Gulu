#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include "../lib/ArduinoJson/ArduinoJson.h"
#include "net.h"
#include "secrets.h"


const char* mqtt_server = "jefine.ga";
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
  // unsigned char t[] = {0XAA ,0X55 ,0X01 ,0X0A ,0X00 ,0X00 ,0X00 ,0X0B,0X00 ,0X00 ,0X00 ,0X55 ,0XAA};
  // //虽然ESP32每次启示发送时，会有串口乱码输出，但不影响，有校验
  // Serial.write(t,13);

	Serial.print("Connecting.. ");

	WiFi_Connect();

	Serial.println("WiFi connected");

	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}



String current_cityname ="";
String current_cityid = "";
String current_time ="";
// some useful UrlAPI 
String iplocationUrl = "http://ip-api.com/json/";
String timeUrl = "http://worldtimeapi.org/api/ip";
String weatherUrl = "https://devapi.qweather.com/v7/weather/now?";
String lookupcityidUrl = "https://geoapi.qweather.com/v2/city/lookup?";

DynamicJsonDocument doc(1024);
//update current_time
void GetCurrentTime()
{
  HTTPClient http;
	http.begin(timeUrl); 
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
      // "2021-11-29T19:05:16.737515+08:00"
			String time = doc["datetime"];
      // 如果是自己之前写的String 会失败，必须在这里声明一个新的变量
      char buf[50]="";
      current_time = time.substring(time.indexOf('T')+1,time.indexOf('T')+6);
      current_time.toCharArray(buf,sizeof(buf),0);
      Serial.print(current_time);
      client.publish("Gulu/ESP32", buf);
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
}

//update current_cityname,then update current_cityid
void GetCurrentLocation()
{
	HTTPClient http;
	http.begin(iplocationUrl); 
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

			String cityname = doc["city"];
			char buf[50]="";
      current_cityname = cityname;
			current_cityname.toCharArray(buf,sizeof(buf),0);
			Serial.print("current_cityname is: ");
			Serial.println(current_cityname);
			client.publish("Gulu/ESP32", buf);
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
  
}
void GetCurrentCityid()
{
  HTTPClient http;
  //Then get the City's ID(for weather request)
	http.begin(lookupcityidUrl+"location="+current_cityname+"&key="+WEATHERKEY+"&number=1&Gzip=n");
  Serial.println(lookupcityidUrl+"location="+current_cityname+"&key="+WEATHERKEY+"&number=1&Gzip=n"); 
	int httpCode = http.GET();

	if (httpCode > 0)
	{
		// httpCode will be negative on error
		Serial.printf("lookupcityidUrl:HTTP Get Code: %d\r\n", httpCode);

		if (httpCode == HTTP_CODE_OK) 
		{
			String resBuff = http.getString();

			Serial.println(resBuff);
			
			deserializeJson(doc, resBuff); //开始使用Json解析

			String id = doc["location"]["0"]["id"];
			char buf[50]="";
      current_cityid = id;
			current_cityid.toCharArray(buf,sizeof(buf),0);
			Serial.print("current_cityid is: ");
			Serial.println(current_cityid);
			client.publish("Gulu/ESP32", buf);
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();

}
//todo
void GetWeather()
{
  HTTPClient http;
	http.begin(weatherUrl); //HTTP begin
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
	GetCurrentTime();
  GetCurrentLocation();
  GetCurrentCityid();
	delay(10000);
}
