#include <ESP8266WiFi.h>
#define MAX_SRV_CLIENTS 5 //最大同时连接数

const char *ssid = "335Room";
const char *password = "helloworld";

WiFiServer server(8266); //端口号
WiFiClient serverClients[MAX_SRV_CLIENTS];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  pinMode(4, OUTPUT);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Connected");
  Serial.println(" IP Address: " + WiFi.localIP().toString());

  server.begin();
  server.setNoDelay(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  //blink();
  uint8_t i;
  String quest = "";
  if (server.hasClient()) {
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      //未连接时释放
      if (!serverClients[i] || !serverClients[i].connected()) {
        if (serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        continue;
      }
    }
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
  for (i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (serverClients[i] && serverClients[i].connected()) {
      //有链接时灯常亮
      digitalWrite(16, 0);
      if (serverClients[i].available()) {
        while (serverClients[i].available()) {
          quest += serverClients[i].read();
          Serial.write("Receive quest: " + serverClients[i].read());
          Serial.println("Receive quest : " + quest);
          //开灯
          if (quest == "0") {
            digitalWrite(4, LOW);
            Serial.println("LED is on");
          }
          
          //关灯
          else if (quest == "1") {
            digitalWrite(4, HIGH);
            Serial.println("LED is closed");
          }

        }
      }
    }
  }
  while (Serial.available()) {
    size_t len = Serial.available();
    String quest = "";
    for (i = 0; i < len; i++) {
      quest += Serial.read();
    }
    Serial.println("Receive quest : " + quest);
    if (quest == "0") {
      digitalWrite(4, HIGH);
      Serial.println("LED is closed");
    }
    else if (quest == "1") {
      digitalWrite(4, LOW);
      Serial.println("LED is on");
    }
    //向所有客户端发送数据
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (serverClients[i] && serverClients[i].connected()) {
        //serverClients[i].write(sbuf, len);
        delay(2);
      }
    }
    delay(2);
  }
}

void blink() {
  static long previousMillis = 0;
  static int currstate = 0;

  if (millis() - previousMillis > 200) //200ms{
    previousMillis = millis();
  currstate = 1 - currstate;
  digitalWrite(16, currstate);
}
