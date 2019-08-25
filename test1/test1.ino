#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <ESP8266WebServer.h>

//DHT11温湿度传感器使用D4接口
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);

//网络名和密码
//const char *ssid = "335Room";
//const char *password = "helloworld";
const char *ssid = "OPPO R11";
const char *password = "rsijpkvs";

ESP8266WebServer server(80);

bool isLedTurnOn = false;
String Led_Content = "已关闭";

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
String ledReport = String("OFF");
String temReport = String("0");
String humReport = String("0");

String homePage = String("");
String mainPage1 = String("") +
	"<html5>" +
	"    <head>" +
	"        <meta charset = \"utf-8\"/>" +
	"        <title>ESP8266局域网通信系统</title>" +
	"        <style type=\"text/css\">" +
	"            .qui-page{" +
	"                position: absolute;" +
	"                left: 200px;" +
	"                width: 640px;" +
	"                -webkit-transition: -webkit-transform 0.4s;" +
	"                transition: transform 0.4s" +
	"            }" +
	"            .qui-header{" +
	"                width: 100%;" +
	"                height: 44px;" +
	"                overflow:hidden;" +
	"                background-color: #2bab79" +
	"            }" +
	"            .qui-header a {" +
	"                display: block;" +
	"                padding-left: 15px;" +
	"                font-size: 18px;" +
	"                text-align: center;" +
	"                line-height: 44px;" +
	"                color: white" +
	"            }" +
	"            .container{" +
	"                width: 100%;" +
	"                -webkit-box-sizing: border-box;" +
	"                -moz-box-sizing: border-box;" +
	"                box-sizing: border-box;" +
	"                margin-top: 50px;" +
	"                background-color: aliceblue" +
	"            }" +
	"            .container p{" +
	"                text-align: center;" +
	"                font-size: 24px;" +
	"            }" +
	"            .container a {" +
	"                text - align: center;" +
    "                font - size: 24px;" +
    "                background - color: darkgrey" +
	"			 }"
	"            .qui-asides{" +
	"                position: center;" +
	"                left: -200px;" +
	"                top: 0;" +
	"                width: 200px" +
	"            }" +
	"            .qui-aside {" +
	"                -webkit-transition: -webkit-transform 0.4s;" +
	"                transition: transform 0.4s;" +
	"                /*-webkit-overflow-scrolling: touch;*/" +
	"                /*overflow-scrolling: touch;*/" +
	"                position: fixed;" +
	"                top: 0;" +
	"                width: 200px;" +
	"                bottom: 0;" +
	"                overflow-y: scroll;" +
	"                background-color: #2d3741" +
	"            }" +
	"            .qui-asideHead {" +
	"                padding: 13px 10px 10px;" +
	"            }" +
	"            .qui-asideNav{" +
	"            }" +
	"            .qui-asideNav li {" +
	"                border-top: 1px solid #232d34;" +
	"                background-color: #36424b" +
	"            }" +
	"            .qui-asideNav li:nth-child(even) {" +
	"                background-color: #364049" +
	"            }" +
	"            .qui-asideNav a {" +
	"                display: block;" +
	"                padding-left: 15px;" +
	"                font-size: 16px;" +
	"                line-height: 44px;" +
	"                color: #ced1d5" +
	"            }" +
	"            .qui-asideTool {" +
	"                border-top: 9px solid #232d34;" +
	"                background-color: #2d3741" +
	"            }" +
	"            .qui-asideTool li{" +
	"                border-top: 1px solid #232d34;" +
	"                background-color: #364049" +
	"            }" +
	"            .qui-asideTool a {" +
	"                display: block;" +
	"                padding-left: 15px;" +
	"                font-size: 16px;" +
	"                line-height: 44px;" +
	"                color: #ced1d5" +
	"            }" +
	"        </style>" +
	"    </head>" +
	"    <body>" +
	"        <section class=\"qui-page\">" +
	"            <!--header-->" +
	"            <header class=\"qui-header\">" +
	"                <a>ESP8266局域网通信系统</a>" +
	"            </header>" +
	"            <!--header end-->" +
	"            <!--begin-->" +
	"            <section class=\"container\">";
String mainPage2 = String("") +
	"            </section>" +
	"            <!--end-->" +
	"        </section>" +
	"        <!--侧边栏-->" +
	"        <aside class=\"qui-asides\">" +
	"            <section class=\"qui-aside\">" +
	"                <section class=\"qui-asideHead\"></section>" +
	"                <nav class=\"qui-asideNav\">" +
	"                    <ul>" +
	"                        <li><a href=\"/\" data-bn-ipg=\"mindex-left-nav-index\"><span>首页</span></a></li>" +
	"                        <li><a href=\"LED\"><span>LED灯</span></a></li>" +
	"                        <li><a href=\"#\"><span>温湿数据</span></a></li>" +
	"                        <li><a href=\"#\"><span>烟霾数据</span></a></li>" +
	"                        <li><a href=\"#\"><span>火焰指数</span></a></li>" +
	"                    </ul>" +
	"                </nav>" +
	"                <section class=\"qui-asideTool\">" +
	"                    <ul>" +
	"                        <li><a href=\"#\"><span>问题反馈</span></a></li>" +
	"                        <li><a href=\"#\"><span>关于我们</span></a></li>" +
	"                    </ul>" +
	"                </section>" +
	"            </section>" +
	"        </aside>" +
	"        <!--侧边栏-->" +
	"    </body>" +
	"</html5>";

String RewritePage(String postPage) 
{
	String page = mainPage1 + postPage + mainPage2;
	return page;
}

void handleRoot()
{
	byte tem = 0;
	byte hum = 0;
	dht11.read(&tem, &hum, NULL);
	temReport = String(tem);
	humReport = String(hum);
	String postPage = String("") +
		"                <p>LED: " + ledReport + "</p>" +
		"                <p>温度: " + temReport + "℃</p>" +
		"                <p>湿度: " + humReport + "%</p>";
	homePage = RewritePage(postPage);
	server.send(200, "text/html", homePage);
	Serial.println("用户访问了主页");
}

void handleLED() 
{
	String postPage = String("") +
		"                <p>LED" + Led_Content + "</p>" +
		"                <p><a href = \"Switch\">开/关</a></p>";
	String ledPage = RewritePage(postPage);
	server.send(200, "text/html", ledPage);
	Serial.println("用户访问了LED页面");
}

void handleSwitch()
{
	if (isLedTurnOn == false)
	{
		digitalWrite(4, LOW);
		Serial.println("用户打开了LED");
		isLedTurnOn = true;
		Led_Content = "已打开";
		ledReport = "ON";
		handleLED();
	}
	else
	{
		digitalWrite(4, HIGH);
		Serial.println("用户关闭了LED");
		isLedTurnOn = false;
		Led_Content = "已关闭";
		ledReport = "OFF";
		handleLED();
	}
}

void handleNotFound()
{
	server.send(404, "text/plain", "访问网页不存在");
	Serial.println("用户访问了一个不存在的网页");
}

void setup() {
	//初始化LED
	pinMode(4, OUTPUT);
	digitalWrite(4, HIGH);//熄灭LED
	ledReport = "OFF";

	//初始化串口
	Serial.begin(115200);
	Serial.println();

	//初始化网络并连接WiFi
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
		//report1 = String("") + "<p>temperature/humitiy： " + temperature + "°C, " + humidity + "% "+ "</p>"; 
		temReport = String(temperature);
		humReport = String(humidity);
	}

	//初始化WebServer
	server.on("/", handleRoot);
	server.on("/LED", handleLED);
	server.on("/Switch", handleSwitch);
	server.onNotFound(handleNotFound);
	server.begin();
	Serial.println("HTTP server started");
}

void loop() {
	server.handleClient();
}
