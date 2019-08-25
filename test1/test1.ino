#include <ESP8266WiFi.h>
#include <SimpleDHT.h>

//DHT11温湿度传感器使用D4接口
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);

//网络名和密码
const char *ssid = "335Room";
const char *password = "helloworld";

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
String myhtmlPage1 =
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
	"<body>";
String myhtmlPage2 = 
  String("") +
	"    <div id=\"txtState\">LED</div>" +
	"    <input type=\"button\" value=\"Switch\" Onclick=\"ledSwitch()\">" +
	"</body>" +
	"</html>";
String report1 = String("");

bool isLedTurnOn = false;

void setup() {
	// put your setup code here, to run once:
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);//熄灭LED

	Serial.begin(115200);
	Serial.println();

	//连接WiFi
	Serial.printf("Connecting to %s ", ssid);

	WiFi.mode(WIFI_STA);
	WiFi.setAutoConnect(false);

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println(" connected");

	server.begin();
	Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());

  //温湿度初始化
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) == SimpleDHTErrSuccess) {
    report1 = String("") + "<p>temperature/humitiy： " + temperature + "°C, " + humidity + "% "+ "</p>"; 
  }
}

void loop() {
	// put your main code here, to run repeatedly:
	WiFiClient client = server.available();
	if (client)
	{
		//Serial.println("nb");
		boolean currentLineIsBlank = true;
		Serial.println("[Client connected]");

		unsigned long previousMillis = millis();//获取当前时间

		while (client.connected())
		{
			if (millis() - previousMillis >= 50) 
			{
				yield();
			}
			if (millis() - previousMillis >= 5000)
			{
				break;
			}

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
						client.print(myhtmlPage1);
						client.print(report1);
            client.print(myhtmlPage2);
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
