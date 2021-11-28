



long lastMsg = 0;
char msg[50];
int value = 0;

void WiFi_Connect();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();