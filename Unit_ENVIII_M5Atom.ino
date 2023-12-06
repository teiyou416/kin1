/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with Atom-Lite/Matrix sample source code
*                          配套  Atom-Lite/Matrix 示例源代码
* Visit for more information: https://docs.m5stack.com/en/unit/envIII
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/unit/envIII
*
* Product: ENVIII_SHT30_QMP6988.  环境传感器
* Date: 2022/7/20
*******************************************************************************
  Please connect to Port,Read temperature, humidity and atmospheric pressure and
  display them on the display Serial
  请连接端口,读取温度、湿度和大气压强并在显示屏上显示
*/
#include <M5Atom.h>
#include "M5_ENV.h"
#include "Ambient.h" // Ambientのヘッダーをインクルード

SHT3X sht30;
QMP6988 qmp6988;
Ambient ambient;
WiFiClient client;
const char* ssid = "TP-Link_218B";
const char* password = "01934977";
unsigned int channelId = 	72832; // AmbientのチャネルID
const char* writeKey = "3faca8f21e34475d"; // ライトキー

float tmp      = 0.0;
float hum      = 0.0;
float pressure = 0.0;
uint8_t
    DisBuff[2 + 5 * 5 * 3];  // Used to store RGB color values.  用来存储RBG色值

void setBuff(uint8_t Rdata, uint8_t Gdata,
             uint8_t Bdata) {  // Set the colors of LED, and save the relevant
                               // data to DisBuff[].  设置RGB灯的颜色
    DisBuff[0] = 0x05;
    DisBuff[1] = 0x05;
    for (int i = 0; i < 25; i++) {
        DisBuff[2 + i * 3 + 0] = Rdata;
        DisBuff[2 + i * 3 + 1] = Gdata;
        DisBuff[2 + i * 3 + 2] = Bdata;
    }
    M5.dis.displaybuff(DisBuff);
}
void setup() {
    M5.begin(true,false,true);          // Init M5Atom.  初始化M5Atom
    Wire.begin(26, 32);  // Initialize pin 26,32.  初始化26,32引脚
    qmp6988.init();
    Serial.println(F("ENVIII Unit(SHT30 and QMP6988) test"));
    sht30.init();
    Serial.begin(9600); 
    M5.begin(true, false,
             true);  // Init Atom-Matrix(Initialize serial port, LED).  初始化
                     // ATOM-Matrix(初始化串口、LED点阵)
    delay(10);  // delay10ms.  延迟10ms
    
    WiFi.begin(ssid, password);  //  Wi-Fi APに接続
    while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
        delay(100);
    }

    Serial.print("WiFi connected\r\nIP address: ");
    Serial.println(WiFi.localIP());
    M5.dis.clear();
    M5.dis.displaybuff(
        DisBuff);  // Display the DisBuff color on the LED.  同步所设置的颜色
    ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化
  

}

void loop() {
    pressure = qmp6988.calcPressure();
    if (sht30.get() == 0) {  // Obtain the data of shT30.  获取sht30的数据
        tmp = sht30.cTemp;   // Store the temperature obtained from shT30.
                      // 将sht30获取到的温度存储
        hum = sht30.humidity;  // Store the humidity obtained from the SHT30.
                               // 将sht30获取到的湿度存储
    } else {
        tmp = 0, hum = 0;
    }
   Serial.print("Temperature: ");
    Serial.println(tmp);  // Print temperature for debugging purposes

    if (tmp < 10) {
    setBuff(255, 255, 255); // White color for temperature below 10 degrees
  } else if (tmp >= 10 && tmp < 20) {
    setBuff(0, 0, 255); // Blue color for temperature between 10 and 20 degrees
  } else if (tmp >= 20 && tmp < 30) {
    setBuff(0, 255, 0); // Green color for temperature between 20 and 30 degrees
  } else if (tmp >= 30) {
    setBuff(255, 0, 0); // Red color for temperature above 30 degrees
  }
    ambient.set(1, tmp);
    ambient.set(2, hum);

    ambient.send();

    delay(2000);
}
