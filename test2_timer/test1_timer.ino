#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

//网络名和密码
const char *ssid = "335Room";
const char *password = "helloworld";

//使用UDP获取当前时间
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
WiFiServer server(80);

//用于存放从客户端接收到的数据
String readString = "";

//响应头
String responseHeaders =
String("") +
"HTTP/1.1 200 OK\r\n" +
"Content-Tyoe: text/html\r\n" +
"Connection: close\r\n" +
"\r\n";

//网页
String myhtmlPage =
  String("") +
  "<html>" +
  "<head>" +
  "    <meta charset=\"utf - 8\" />" +
  "    <title>ESP8266 Web Test</title>" +
  "    <script defer=\"defer\">" +
  "        function ledSwitch() {" +
  "            var xmlhttp;" +
  "            if (window.XMLHttpRequest) {" +
  "                xmlhttp = new XMLHttpRequest();" +
  "            } else {" +
  "                xmlhttp = new ActiveXObject(\"Microsoft.XMLHTTP\");" +
  "            }" +
  "            xmlhttp.onreadystatechange = function () {" +
  "                if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {" +
  "                    document.getElementById(\"txtState\").innerHTML = xmlhttp.responseText;" +
  "                }" +
  "            }," +
  "            xmlhttp.open(\"GET\", \"Switch\", true);" +
  "            xmlhttp.send(); " +
  "        }" +
  "    </script>" +
  "</head>" +
  "<body>" +
  "    <div id=\"txtState\">LED</div>" +
  "    <input type=\"button\" value=\"Switch\" Onclick=\"ledSwitch()\">" +
  "</body>" +
  "</html>";

bool isLedTurnOn = false;
long timer = 0; 

void setup() {
  // put your setup code here, to run once:
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);//熄灭LED

  Serial.begin(115200);
  Serial.println();

  //连接WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();

  //获取当前时间
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  
  if (client)
  {
    //Serial.println("nb");
    boolean currentLineIsBlank = true;
    Serial.println("[Client connected]");

    while (client.connected())
    {
      //Serial.println("connected");
      if (client.available())
      {
        //Serial.println("available");
        char c = client.read();
        readString += c;
        //等待请求头接收完成
        if (c == '\n' && currentLineIsBlank) 
        {
          //Serial.println("c==n");
          if (readString.startsWith("GET / HTTP/1.1"))
          {
            //Serial.println("gethttp");
            client.print(responseHeaders);
            client.print(myhtmlPage);
            client.print("\r\n");
          }
          else if (readString.startsWith("GET /Switch"))
          {
            if (isLedTurnOn == false)
            {
              digitalWrite(4, LOW);
              client.print("LED has been turned on");
              isLedTurnOn = true;
            }
            else
            {
              digitalWrite(4, HIGH);
              client.print("LED has been turned off");
              isLedTurnOn = false;
            }
          }
          else
          {
            client.print("\r\n");
          }
          break;
        }

        if (c == '\n')
        {
          //Serial.println("true");
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          //Serial.println("false");
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("[Client disconnected]");

    Serial.println(readString);
    readString = "";
  }


}
