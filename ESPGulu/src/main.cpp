#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include "../lib/ArduinoJson/ArduinoJson.h"
#include "net.h"
#include "secrets.h"
#include "cube.h"
#include <HardwareSerial.h>    

const char* mqtt_server = MQTTBROKER;
WiFiClient espClient;
PubSubClient client(espClient);

HardwareSerial MySerial_stm32(2); 

enum DemoStatus status;


String current_cityname ="";
String current_cityid = "";
String current_time ="";
String current_humidity = "";
String current_weather = "";
String current_airindex = "";

// some useful UrlAPI 

//return location lon lat,ip and city name
String iplocationUrl = "http://ip-api.com/json/";

//return time by ip with ip's utc
String timeUrl = "http://worldtimeapi.org/api/ip";

//limitied, if errcode==100,change appid and it's appsecret;
String weatherUrl = "https://tianqiapi.com/api?unescape=1&version=v6";

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
			// 如果是自己之前写的String 会无法通过编译，必须在这里声明一个新的变量
			char buf[50]="";
			current_time = time.substring(time.indexOf('T')+1,time.indexOf('T')+6);
			current_time.toCharArray(buf,sizeof(buf),0);
			Serial.println(current_time);
			client.publish("Gulu/ESP32", buf);
			Serial.print("$");
		 	Serial.print(current_time);
			Serial.print("&");
			MySerial_stm32.print("$");
		 	MySerial_stm32.print(current_time);
			MySerial_stm32.print("&");
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
}

void GetCurrentWeather()
{
  	HTTPClient http;
	http.begin(weatherUrl+"&appid="+BCKWEATHERPI_APPID+"&appsecret="+BCKWEATHERPI_APPSECRET); //HTTP begin
	Serial.println(weatherUrl+"&appid="+BCKWEATHERPI_APPID+"&appsecret="+BCKWEATHERPI_APPSECRET);
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
			current_cityname = cityname;
			String cityid = doc["cityid"];
			current_cityid = cityid;
			String weather = doc["tem"];
			current_weather = weather;
			String humidity = doc["humidity"];
			current_humidity = humidity;
			String airindex = doc["air"];
			current_airindex = airindex;

			char buf[100]="";
			resBuff.toCharArray(buf,sizeof(buf),0);
			client.publish("Gulu/ESP32", buf);
			int temp = current_weather.toInt();
			Serial.print("$");
			if(temp<10)Serial.print("0");
		 	Serial.print(current_weather);
			Serial.print("&");
			MySerial_stm32.print("$");
			if(temp<10)MySerial_stm32.print("0");
			MySerial_stm32.print(current_weather);
			MySerial_stm32.print("&");
			
			char buff[50]= "";
			current_weather.toCharArray(buff,sizeof(buff),0);
			client.publish("Gulu/ESP32", buff);
			//(current_cityname+" "+current_weather+" "+current_humidity).toCharArray(buff,sizeof(buff),0);
      		
		}
	}
	else
	{
		Serial.printf("HTTP Get Error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
}


void setup()
{
	Serial.begin(115200);
	MySerial_stm32.begin(115200);
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
	client.publish("Gulu/ESP","ESP has connectted mqtt");
	status = Time;// init status
	
	//test
	//StartColourfulEgg();
	GetCurrentTime();
	GetCurrentWeather();
}





void loop()
{
	if (!client.connected()) {
		reconnect();
	}
	client.loop();

	// GetCurrentTime();
 	// GetWeather();
	//
	//unsigned int num = 0;
	
	if(MySerial_stm32.find('$'))
	{
		int c = MySerial_stm32.parseInt();
		if(c==1){
			client.publish("Gulu/ESP32", "Serial receved 1 \n status change to Colourful_Egg\n");
			status = Colourful_Egg;
			StartColourfulEgg();
		}
		if(c==2){
			client.publish("Gulu/ESP32", "Serial receved 2 \n status change to Weather_Humidity\n");
			status = Weather_Humidity;
			GetCurrentWeather();
		}
		if(c==3){
			client.publish("Gulu/ESP32", "Serial receved 3 \n status change to Time\n");
			status = Time;
			GetCurrentTime();
			
		}
		client.publish("Gulu/ESP32Serial", "Serial receved $ \n");
		char a[1]="";
		a[0] = c+'0';
		Serial.print("ESP32Serial get ");
		Serial.println(c);
		client.publish("Gulu/ESP32Serial :", a);
	}
	// switch (status)
	// {
	// case Colourful_Egg:
	// 		StartColourfulEgg();
	// 	break;
	// case Weather_Humidity:
	// 	{
			
	// 	}
	// 	break;
	// case Time:
	// 	{
			
	// 	}
	// 	break;
	// default:
	// 	break;
	// }
}
