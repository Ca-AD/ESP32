/*
    This sketch sends data via HTTP GET requests to data.sparkfun.com service.

    You need to get streamId and privateKey at data.sparkfun.com and paste them
    below. Or just customize this script to talk to other HTTP servers.

*/

#include <WiFi.h>
#include <PubSubClient.h>
#include "Ticker.h"
const char* ssid     = "xxxxx";
const char* password = "xxxxx";


#define JDQ 16


const char* MQTT_SERVER  = "mqtts.heclouds.com";
const int   MQTT_PORT    = 1883;
const char* MQTT_USRNAME = "EQ3h46TlPn";
const char* MQTT_PASSWD  = "version=2018-10-31&res=products%2FEQ3h46TlPn%2Fdevices%2FDS18&et=1746673206&method=md5&sign=K0dQ11ovYdQ4ckPtimgPXQ%3D%3D";
const char* TOPIC = "$sys/EQ3h46TlPn/DS18/thing/property/post/reply";
const char* CLIENT_ID    = "DS18";  //当前设备的clientid标志

WiFiClient espClient;
PubSubClient  client(espClient);
char msgJson[80]; //发送信息缓冲区

//信息模板
// char dataTemplate[] = "{\"id\": \"123\",\"version\": \"1.0\",\"params\": {\"humi\": {\"value\": 66},\"temp\": {\"value\":10}}}";
char dataTemplate[] = "{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"humi\":{\"value\":66},\"temp\":{\"value\":10}}}";
Ticker tim1; //定时器,用来循环上传数据

// 	"id": "123",
// 	"version": "1.0",
// 	"params": {
// 		"humi": {
// 			"value": 10
// 		},
// 		"temp": {
// 			"value": 10
// 		}
// 	}
// }


//向主题发送模拟的温湿度数据
void sendTempAndHumi()
{
  if (client.connected())
  {
    // snprintf(msgJson, 80, dataTemplate, 22.5, 35.6); //将模拟温湿度数据套入dataTemplate模板中, 生成的字符串传给msgJson
    Serial.print("public the data:");
    Serial.println(msgJson);
    // client.publish("$sys/EQ3h46TlPn/DS18/thing/property/post", (uint8_t *)msgJson, strlen(msgJson));
     client.publish("$sys/EQ3h46TlPn/DS18/thing/property/post", (uint8_t *)dataTemplate, 79);
    //发送数据到主题
  }
}

long lastMsg = 0;

void setup()
{
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(JDQ, OUTPUT);
  client.setServer(MQTT_SERVER, MQTT_PORT); //设定MQTT服务器与使用的端口，1883是默认的MQTT端口
  client.setCallback(callback);        //设定回调方式，当ESP8266收到订阅消息时会调用此方法

  tim1.attach(10, sendTempAndHumi); //每10秒发送一次数据
}

int value = 0;

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(CLIENT_ID,MQTT_USRNAME,MQTT_PASSWD)) {
      Serial.println("connected");
      // 连接成功时订阅主题
      client.subscribe(TOPIC);
//      sendTempAndHumi();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);   // 打印主题信息
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]); // 打印主题内容
  }
  if ((char)payload[6] == '1') {
    digitalWrite(JDQ, HIGH);   // 亮灯
    Serial.print(" 开灯");
  } else {
    digitalWrite(JDQ, LOW);   // 熄灯
    Serial.print(" 关灯");
  }

  Serial.println();
}


void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}


