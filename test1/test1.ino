#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include "Ticker.h"
#include "FS.h"

#define QUEUE_LENGTH 30
#define MAX_SRV_CLIENTS 5 //最大同时连接数

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);


//发送数据缓存
StaticJsonDocument<1024> doc;
int bufLen = 0;
byte sbuf[QUEUE_LENGTH * 2];

//DHT11温湿度传感器 D4
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);

//火焰传感器 D5
int pinFire = 14;

//蜂鸣器D6
int pinBuz = 12;

//HC-SR501红外传感器 D7
int pinPIR = 13;

//TCP Server
WiFiServer tcpServer(8266);
WiFiClient serverClients[MAX_SRV_CLIENTS];

//Web Server
ESP8266WebServer webServer(80);

//网络名和密码
const char *ssid = "335Room";
const char *password = "helloworld";
//const char *ssid = "OPPO R11";
//const char *password = "rsijpkvs";

bool isLedTurnOn = false;
bool isReadData = false;
bool isFire = false;
String Led_Content = "已关闭";

//温湿度数据
byte temQueue[QUEUE_LENGTH];
byte humQueue[QUEUE_LENGTH];

//定时调度
Ticker myTicker;
Ticker ticker2;
Ticker ticker3;
Ticker ticker4;

//网页
String ledReport = String("OFF");
String temReport = String("0");
String humReport = String("0");

String getContentType(String filename)
{
  if (webServer.hasArg("download"))
    return "application/octet-stream";
  else if (filename.endsWith(".htm"))
    return "text/html";
  else if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".xml"))
    return "text/xml";
  else if (filename.endsWith(".pdf"))
    return "application/x-pdf";
  else if (filename.endsWith(".zip"))
    return "application/x-zip";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

void handleRoot() //访问主页
{
  File file = SPIFFS.open("/index.html", "r");
  if (!file)
  {
    Serial.println("File open failed");
  }
  size_t sent = webServer.streamFile(file, "text/html");
  file.close();
  Serial.println("用户访问了主页");
}

void handleLED() //访问LED页面
{
  File file = SPIFFS.open("/LED.html", "r");
  if (!file)
  {
    Serial.println("File open failed");
  }
  size_t sent = webServer.streamFile(file, "text/html");
  file.close();
  Serial.println("用户访问了LED页面");
}

void handleHomedata() //发送主页数据
{
  String ledStr, thStr, smokeStr, fireStr, content;
  if (isLedTurnOn == false)
  {
    ledStr = "状态: OFF";
  }
  else
  {
    ledStr = "状态: ON";
  }
  if (isFire == true)
  {
    fireStr = "状态: 危险";
  }
  else
  {
    fireStr = "状态: 正常";
  }
  smokeStr = "指数: ";
  thStr = "温度: " + String(temQueue[QUEUE_LENGTH - 1]) + "℃  湿度: " + String(humQueue[QUEUE_LENGTH - 1]) + "%";
  content = ledStr + ";" + thStr + ";" + smokeStr + ";" + fireStr;
  webServer.send(200, "text/html", content);
}

void handleLeddata() //发送LED数据
{
  String led;
  if (isLedTurnOn == false)
  {
    led = "OFF";
  }
  else
  {
    led = "ON";
  }
  webServer.send(200, "text/html", led);
}

void handleWenshidata() //发送温湿度数据
{
  String wen, shi, content;
  wen = String(temQueue[QUEUE_LENGTH - 1]) + "℃";
  shi = String(humQueue[QUEUE_LENGTH - 1]) + "%";
  content = wen + ";" + shi;
  webServer.send(200, "text/html", content);
}

void handleTem() //发送温度
{
  webServer.send(200, "text/html", String(temQueue[QUEUE_LENGTH - 1]));
}

void handleHum() //发送湿度
{
  webServer.send(200, "text/html", String(humQueue[QUEUE_LENGTH - 1]));
}

void handleSwitch() //开关灯
{
  if (isLedTurnOn == false)
  {
    digitalWrite(15, LOW);
    Serial.println("用户打开了LED");
    isLedTurnOn = true;
    Led_Content = "已打开";
    ledReport = "ON";
  }
  else
  {
    digitalWrite(15, HIGH);
    Serial.println("用户关闭了LED");
    isLedTurnOn = false;
    Led_Content = "已关闭";
    ledReport = "OFF";
  }
}

void handleTemHum() //访问温湿数据页面
{
  File file = SPIFFS.open("/TemHum.html", "r");
  if (!file)
  {
    Serial.println("File open failed");
  }
  size_t sent = webServer.streamFile(file, "text/html");
  file.close();
  Serial.println("用户访问了温湿数据页面");
}

//获取当前温湿度
void getTH()
{
  byte tem = 0;
  byte hum = 0;
  dht11.read(&tem, &hum, NULL);

  doc["CTemperature"] = String(tem);
  doc["CHumitiy"] = String(hum);
}

//获取历史温湿度
void getTHhistory()
{
  uint8_t i;

  JsonArray Htemperature = doc.createNestedArray("Htemperature");
  JsonArray Hhumitiy = doc.createNestedArray("Hhumitiy");

  for (i = 0; i < QUEUE_LENGTH; i++)
  {
    Htemperature.add(temQueue[i]);
  }
  for (i = 0; i < QUEUE_LENGTH; i++)
  {
    Hhumitiy.add(humQueue[i]);
  }

}

void handleSmoke() //访问烟霾数据页面
{
  File file = SPIFFS.open("/Smoke.html", "r");
  if (!file)
  {
    Serial.println("File open failed");
  }
  size_t sent = webServer.streamFile(file, "text/html");
  file.close();
  Serial.println("用户访问了烟霾数据页面");
}

void handleFire() //访问火焰指数页面
{
  File file = SPIFFS.open("/Fire.html", "r");
  if (!file)
  {
    Serial.println("File open failed");
  }
  size_t sent = webServer.streamFile(file, "text/html");
  file.close();
  Serial.println("用户访问了火焰指数页面");
}

void handleFiredata() //发送火焰指数
{
  if (isFire)
  {
    webServer.send(200, "text/html", "危险");
  }
  else
  {
    webServer.send(200, "text/html", "正常");
  }
}

void handleNotFound()
{
  String path = webServer.uri();
  Serial.print("load url:");
  Serial.println(path);
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = webServer.streamFile(file, contentType);
    file.close();
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i = 0; i < webServer.args(); i++)
  {
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
  Serial.println("用户访问了一个不存在的网页");
}

void readData() //实时读取数据
{
  isReadData = true;
}

void queueInit(byte queue[])
{
  for (int i = 0; i < QUEUE_LENGTH; i++)
  {
    queue[i] = 0;
  }
}

void queuePush(byte newdata, byte queue[])
{
  for (int i = 0; i < QUEUE_LENGTH - 1; i++)
  {
    queue[i] = queue[i + 1];
  }
  queue[QUEUE_LENGTH - 1] = newdata;
}

void fireDetect() //火焰监测
{
  int x = analogRead(A0);
  if (x >= 550)
  {
    tone(pinBuz, 1000);
    isFire = true;
  }
  else
  {
    noTone(pinBuz);
    isFire = false;
  }
}

void pirDetect()
{
  int pirValue = digitalRead(pinPIR);
  //有人经过
  if (pirValue == HIGH && isLedTurnOn == false)
  {
    handleSwitch();
    Serial.println("Somebody get in the room/pass, the light is turned on.");
  }
  //无人经过
  else if (pirValue == LOW && isLedTurnOn == true)
  {
    handleSwitch();
    Serial.println("There is nobody in room/pass, the light is turned off.");
  }
}

/*void draw(void)
{
  byte tem = 0;
  byte hum = 0;
  dht11.read(&tem, &hum, NULL);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(5, 20);
  u8g2.print("TEMP(oC):");
  u8g2.setCursor(78, 20);
  u8g2.print((int)tem);
  u8g2.setCursor(5, 40);
  u8g2.print("HUMI(%):");
  u8g2.setCursor(78, 40);
  u8g2.print((int)hum);
  u8g2.sendBuffer();
}*/

void setup()
{
  //初始化LED
  pinMode(15, OUTPUT);
  pinMode(pinPIR, INPUT);
  digitalWrite(15, HIGH); //熄灭LED
  ledReport = "OFF";

  //初始化串口
  Serial.begin(115200);
  Serial.println();

  //初始化网络并连接WiFi
  Serial.printf("Connecting to %s ", ssid);

  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoConnect(false);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ");
  Serial.println("Connected");

  tcpServer.begin();
  tcpServer.setNoDelay(true);
  Serial.println("TCP Server started, IP Address: " + WiFi.localIP().toString() + "  Port: 8266");

  webServer.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());

  //温湿度初始化
  byte temperature = 0;
  byte humidity = 0;
  queueInit(temQueue);
  queueInit(humQueue);
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) == SimpleDHTErrSuccess)
  {
    //report1 = String("") + "<p>temperature/humitiy： " + temperature + "°C, " + humidity + "% "+ "</p>";
    temReport = String(temperature);
    humReport = String(humidity);
    queuePush(temperature, temQueue);
    queuePush(humidity, humQueue);
  }

  //初始化SPIFFS
  bool ok = SPIFFS.begin();
  if (!ok)
  {
    Serial.println("begin failed");
  }

  //初始化WebServer
  webServer.on("/", handleRoot);
  webServer.on("/LED", handleLED);
  webServer.on("/homedata", handleHomedata);
  webServer.on("/leddata", handleLeddata);
  webServer.on("/Switch", handleSwitch);
  webServer.on("/TemHum", handleTemHum);
  webServer.on("/wenshidata", handleWenshidata);
  webServer.on("/getTem", handleTem);
  webServer.on("/getHum", handleHum);
  webServer.on("/Smoke", handleSmoke);
  webServer.on("/Fire", handleFire);
  webServer.on("/firedata", handleFiredata);
  webServer.onNotFound(handleNotFound);
  webServer.begin();
  Serial.println("HTTP server started");

  //定时调度
  myTicker.attach(1, readData);
  ticker2.attach(2, fireDetect);
  ticker3.attach(1, pirDetect);
  /*ticker4.attach(5, draw);
  u8g2.begin();*/
}

void loop()
{
  //Web Server
  webServer.handleClient();
  if (isReadData == true)
  {
    byte tem = 0;
    byte hum = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&tem, &hum, NULL)) == SimpleDHTErrSuccess)
    {
      queuePush(tem, temQueue);
      queuePush(hum, humQueue);
    }
    isReadData = false;
  }

  //TCP Server
  uint8_t i;
  String quest = "";
  if (tcpServer.hasClient())
  {
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
    {
      //未连接时释放
      if (!serverClients[i] || !serverClients[i].connected())
      {
        if (serverClients[i])
          serverClients[i].stop();
        serverClients[i] = tcpServer.available();
        continue;
      }
    }
    WiFiClient serverClient = tcpServer.available();
    serverClient.stop();
  }

  for (i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      //有链接时灯常亮
      digitalWrite(16, 0);
      if (serverClients[i].available())
      {
        while (serverClients[i].available())
        {
          quest += serverClients[i].read();
          Serial.write("Receive quest: " + serverClients[i].read());
          Serial.println("Receive quest : " + quest);
          //开灯
          if (quest == "0" && isLedTurnOn == true)
          {
            handleSwitch();
          }
          else if (quest == "1" && isLedTurnOn == false)
          {
            handleSwitch();
          }
          else if (quest == "50")
          { //请求当前温湿度
            getTH();
            serializeJson(doc, serverClients[i]);
            serverClients[i].write('\n');
            Serial.printf("TCP client %d get current TH data： \n", i);
            serializeJson(doc, Serial);
            Serial.println();
            Serial.println();
            doc.clear();
          }
          else if (quest == "51")
          { //请求历史温湿度
            getTHhistory();
            serializeJson(doc, serverClients[i]);
            serverClients[i].write('\n');
            Serial.printf("TCP client %d get history TH data： \n", i);
            serializeJson(doc, Serial);
            Serial.println();
            Serial.println();
            doc.clear();
          }
        }
      }
    }
  }
}
