#ifndef __NET_H_
#define __NET_H_

void WiFi_Connect();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();

#endif