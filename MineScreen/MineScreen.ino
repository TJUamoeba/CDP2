#include <Arduino.h>
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "Ticker.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define AP_SSID "335Room"
#define AP_PSW "helloworld"

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);

const uint16_t port = 8266;
const char *host = "192.168.43.54";
const char *weatherAPI = "api.seniverse.com";

WiFiClient client;

Ticker questTicker;
Ticker smogTicker;
Ticker screenTicker;

//数据
int tem;
int hum;
float smogIndex = 0;
DynamicJsonDocument jsonBuffer(500);



void setup()
{
  Serial.begin(115200);

  tem = 0;
  hum = 0;

  u8g2.begin();
  u8g2.enableUTF8Print();//使用中文字符

  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoConnect(false);
  Serial.printf("Connecting to %s ", AP_SSID);
  WiFi.begin(AP_SSID, AP_PSW);//连接到局域网
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ");
  Serial.println("Connected");

  //获取天气Api数据
  Serial.print("Try to get weather data from XinZhi: ");
  Serial.println(weatherAPI);
  if (!client.connect(weatherAPI, 80))
  {
    Serial.println("Connection failed!");
    return;
  }
  String url = "/v3/weather/now.json?key=SamSGTUi3hx0dgm1i&location=tianjin&language=zh-Hans&unit=c";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  //发送数据请求
  client.print(String("GET ") + url + "HTTP/1.1\r\n" +
               "Host: " + weatherAPI + "\r\n" +
               "Connection: close\r\n\r\n");
  //等待数据传送完
  delay(100);
  String weatherReport;
  while (client.available()) {
    String line = client.readStringUntil('\r');
    weatherReport += line;
  }
  client.stop();
  Serial.println("Get report: " + weatherReport);
  Serial.println("Closing connection to XINZHI Weather Api");
  String jsonString;
  int jsonIndex;
  for (int i = 0; i < weatherReport.length(); i++)
  {
    if (weatherReport[i] == '{')
    {
      jsonIndex = i;
      break;
    }
  }
  jsonString = weatherReport.substring(jsonIndex);
  Serial.println();
  Serial.println("Weather Report(in Json) :");
  Serial.println(jsonString);

  //连接到ESP8266Server
  Serial.print("Client connecting to " + String(host) + " / " + String(port) + " :");
  while (!client.connect(host, port)) { //连接到服务器
    Serial.print(".");
    delay(500);
  }
  Serial.println(" ");
  Serial.println("Connected");

  screenTicker.attach(10, drawMainPage);
  questTicker.attach(10, sendquest);
  smogTicker.attach(10, readSmog);

}


void loop()
{
  while (client.available()) {
    delay(100);
    String line = client.readStringUntil('\n');
    Serial.println("Get line :" + line);
    parseJson(line);
  }

}

void parseJson(String str)
{
  Serial.println("Received data: " + str);
  deserializeJson(jsonBuffer, str);
  JsonObject object = jsonBuffer.as<JsonObject>();

  String dataType = object["DATA"];
  if (dataType == "CurrentTH")
  {
    tem = object["CTemperature"];
    hum = object["CHumitiy"];
  }
  else
  {
    Serial.println("Illegal data(Can't parse to Json).");
  }
}

//u8g2_font_pcsenior_8u （6 Pixel）
//u8g2_font_t0_11_tf （8 Pixel）

void drawMainPage(void)
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_t0_11_tf);//8 Pixel Height

  u8g2.setCursor(10, 20);//左下角坐标
  u8g2.print("Tem(oC):");
  u8g2.setCursor(80, 20);
  u8g2.print(String(tem));

  u8g2.setCursor(10, 35);
  u8g2.print("Hum(%):");
  u8g2.setCursor(80, 35);
  u8g2.print(String(hum));

  u8g2.setCursor(10, 50);
  u8g2.print("Smog(mg/m³):");
  u8g2.setCursor(80, 50);
  u8g2.print(String(smogIndex));
  u8g2.sendBuffer();

}

void sendquest()
{
  client.println("10");
  Serial.println("Send 10 to Server");
}

void readSmog()//读取 并 返回 烟雾数据
{
  smogIndex = analogRead(A0) * 0.5;
  Serial.println("Smog index: " + String(smogIndex) + "mg/m³");
  client.println("Smog:" + String(smogIndex));
}

void getWeather()//从心知天气获取天气信息
{
  Serial.print("Try to get weather data from XinZhi: ");
  Serial.println(weatherAPI);

  if (!client.connect(weatherAPI, 80))
  {
    Serial.println("Connection failed!");
    return;
  }

  String url = "/v3/weather/now.json?key=SamSGTUi3hx0dgm1i&location=tianjin&language=zh-Hans&unit=c";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  //发送数据请求
  client.print(String("GET ") + url + "HTTP/1.1\r\n" +
               "Host: " + weatherAPI + "\r\n" +
               "Connection: close\r\n\r\n");
  //等待数据传送完
  delay(100);
  String weatherReport;
  while (Serial.available()) {
    String line = client.readStringUntil('\r');
    weatherReport += line;
  }

  client.stop();
  Serial.println("Get report: " + weatherReport);
  Serial.println("Closing connection to XINZHI Weather Api");

  String jsonString;
  int jsonIndex;

  for (int i = 0; i < weatherReport.length(); i++)
  {
    if (weatherReport[i] == '{')
    {
      jsonIndex = i;
      break;
    }
  }

  jsonString = weatherReport.substring(jsonIndex);
  Serial.println();
  Serial.println("Weather Report(in Json) :");
  Serial.println(jsonString);

}
