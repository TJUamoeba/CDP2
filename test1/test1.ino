#include <ESP8266WiFi.h>
#include <SimpleDHT.h>
#include <ESP8266WebServer.h>
#include "Ticker.h"
#define QUEUE_LENGTH 30

//DHT11温湿度传感器使用D4接口
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);

//网络名和密码
//const char *ssid = "335Room";
//const char *password = "helloworld";
const char *ssid = "OPPO R11";
const char *password = "rsijpkvs";

//Web服务器
ESP8266WebServer server(80);

bool isLedTurnOn = false;
bool isReadData = false;
String Led_Content = "已关闭";

//温湿度数据
byte temQueue[QUEUE_LENGTH];
byte humQueue[QUEUE_LENGTH];

//定时调度
Ticker myTicker;

//网页
String ledReport = String("OFF");
String temReport = String("0");
String humReport = String("0");

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
	"			 }" +
	"            .container canvas{" +
	"                margin-left: 70px;" +
    "                width: 500px;" +
    "                height: 300px;" +
    "                border: 1px solid black" +
	"			 }" +
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
	"                        <li><a href=\"TemHum\"><span>温湿数据</span></a></li>" +
	"                        <li><a href=\"/\"><span>烟霾数据</span></a></li>" +
	"                        <li><a href=\"/\"><span>火焰指数</span></a></li>" +
	"                    </ul>" +
	"                </nav>" +
	"                <section class=\"qui-asideTool\">" +
	"                    <ul>" +
	"                        <li><a href=\"/\"><span>问题反馈</span></a></li>" +
	"                        <li><a href=\"/\"><span>关于我们</span></a></li>" +
	"                    </ul>" +
	"                </section>" +
	"            </section>" +
	"        </aside>" +
	"        <!--侧边栏-->" +
	"    </body>" +
	"</html5>";

String RewritePage(String postPage) //组合html
{
	String page = mainPage1 + postPage + mainPage2;
	return page;
}

void handleRoot()//访问主页
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
	String homePage = RewritePage(postPage);
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

void handleTemHum() 
{
	byte tem = 0;
	byte hum = 0;
	dht11.read(&tem, &hum, NULL);
	temReport = String(tem);
	humReport = String(hum);
	String postPage1 = String("") +
		"                <p>温度:" + temReport + "℃&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp湿度:" + humReport + "%</p>" +
		"                <p>温度历史曲线</p>" +
		"                <canvas id=\"TemGraph\"></canvas>" +
		"                <p>湿度历史曲线</p>" +
		"                <canvas id=\"HumGraph\"></canvas>" +
		"                <script type=\"text/javascript\">" +
		"                    var canvas1 = document.getElementById(\'TemGraph\');" +
		"                    var ctx1 = canvas1.getContext(\'2d\');" +
		"                    ctx1.lineWidth = \"1px\";" +
		"                    ctx1.strokeStyle = \"#999\";" +
		"                    ctx1.moveTo(10, 25);" +
		"                    ctx1.lineTo(10, 125);" +
		"                    ctx1.lineTo(280, 125);";
	String postPage2 = String("") +
		"                    ctx1.stroke();" +
		"                    var canvas2 = document.getElementById(\'HumGraph\');" +
		"                    var ctx2 = canvas2.getContext(\'2d\');" +
		"                    ctx2.lineWidth = \"1px\";" +
		"                    ctx2.strokeStyle = \"#999\";" +
		"                    ctx2.moveTo(10, 25);" +
		"                    ctx2.lineTo(10, 125);" +
		"                    ctx2.lineTo(280, 125);";
	String postPage3 = String("") +
		"                    ctx2.stroke();" +
		"                </script>";
	String postPage = String("");
	//绘制温度曲线
	String graph1 = String("") + "                    ctx1.moveTo(10, " + (String)(125 - temQueue[0]) + ");";
	for (int i = 1; i < QUEUE_LENGTH - 1; i++) 
	{
		graph1 += "                    ctx1.lineTo(" + (String)(10 + 9 * i) + ", " + (String)(125 - temQueue[i]) + ");";
	}
	//绘制湿度曲线
	String graph2 = String("") + "                    ctx2.moveTo(" + (String)(10) + ", " + (String)(125 - humQueue[0]) + ");";
	for (int i = 1; i < QUEUE_LENGTH - 1; i++) {
		graph2 += "                    ctx2.lineTo(" + (String)(10 + 9 * i) + ", " + (String)(125 - humQueue[i]) + ");";
	}
	postPage = postPage1 + graph1 + postPage2 + graph2 + postPage3;
	String temHumPage = RewritePage(postPage);
	server.send(200, "text/html", temHumPage);
	Serial.println("用户访问了温湿数据页面");
}

void handleNotFound()
{
	server.send(404, "text/plain", "访问网页不存在");
	Serial.println("用户访问了一个不存在的网页");
}

void readData() 
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

	WiFi.mode(WIFI_AP_STA);
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
	queueInit(temQueue);
	queueInit(humQueue);
	int err = SimpleDHTErrSuccess;
	if ((err = dht11.read(&temperature, &humidity, NULL)) == SimpleDHTErrSuccess) {
		//report1 = String("") + "<p>temperature/humitiy： " + temperature + "°C, " + humidity + "% "+ "</p>"; 
		temReport = String(temperature);
		humReport = String(humidity);
		queuePush(temperature, temQueue);
		queuePush(humidity, humQueue);
	}

	//初始化WebServer
	server.on("/", handleRoot);
	server.on("/LED", handleLED);
	server.on("/Switch", handleSwitch);
	server.on("/TemHum", handleTemHum);
	server.onNotFound(handleNotFound);
	server.begin();
	Serial.println("HTTP server started");

	//定时调度
	myTicker.attach(1, readData);
}

void loop() {
	server.handleClient();
	if (isReadData == true) {
		byte tem = 0;
		byte hum = 0;
		int err = SimpleDHTErrSuccess;
		if ((err = dht11.read(&tem, &hum, NULL)) == SimpleDHTErrSuccess) {
			queuePush(tem, temQueue);
			queuePush(hum, humQueue);
		}
		isReadData = false;
	}
}
