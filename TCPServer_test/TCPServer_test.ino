#include <ESP8266WiFi.h>
#define MAX_SRV_CLIENTS 3 //最大同时连接数

const char *ssid = "335Room";
const char *password = "helloworld";

WiFiServer server(8266); //端口号
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  server.begin();
  server.setNoDelay(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  blink();
  uint8_t i;
  if(server.hasClient()){
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      //未连接时释放
      if(!serverClients[i] || !serverClients[i].connected()){
        if(serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        continue;
      }
    }
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
  for(i = 0; i < MAX_SRV_CLIENTS; i++){
    if(serverClients[i] && serverClients[i].connected()){
      //有链接时灯常亮
      digitalWrite(16, 0);
      if(serverClients[i].available()){
        while(serverClients[i].available())
          Serial.write(serverClients[i].read());
      }
    }
  }
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //向所有客户端发送数据
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if(serverClients[i] && serverClients[i].connected()){
        serverClients[i].write(sbuf, len);
        delay(2);
      }
    }
  }
}

void blink(){
  static long previousMillis = 0;
  static int currstate = 0;

  if(millis() - previousMillis > 200) //200ms{
  previousMillis = millis();
  currstate = 1 - currstate;
  digitalWrite(16, currstate);
}
