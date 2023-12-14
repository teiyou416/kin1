 #include <M5Atom.h>
  #include "M5_ENV.h"
  #include "Ambient.h" // Ambientのヘッダーをインクルード
   #define PERIOD 60

  SHT3X sht30;
  QMP6988 qmp6988;
  Ambient ambient;
  WiFiClient client;
  const char* ssid = "TP-Link_218B";
  const char* password = "01934977";
  unsigned int channelId = 	72832; // AmbientのチャネルID
  const char* writeKey = "3faca8f21e34475d"; // ライトキー
float tempData[60]; // 存储过去60秒的温度数据
float humidData[60]; // 存储过去60秒的湿度数据
float pressureData[60]; // 存储过去60秒的气压数据
int currentIndex = 0;
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
}
  void setup() {
      M5.begin(true,false,true);          // Init M5Atom.  初始化M5Atom
      Wire.begin(26, 32);  // Initialize pin 26,32.  初始化26,32引脚
      qmp6988.init();
      Serial.println(F("ENVIII Unit(SHT30 and QMP6988) test"));
      sht30.init();
       Serial.begin(115200); 
      delay(10);  // delay10ms.  延迟10ms

      WiFi.begin(ssid, password);  //  Wi-Fi APに接続
      while (WiFi.status() != WL_CONNECTED) {  //  Wi-Fi AP接続待ち
          delay(100);
      }
      Serial.print("WiFi connected\r\nIP address: ");
      Serial.println(WiFi.localIP());
      
      ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化


  }

  void loop() {
    
float temp, humid, pressure;

const unsigned long interval = 10000; // 10 seconds interval
const unsigned long interval1 = 60000;
if (sht30.get() == 0) {
          temp = sht30.cTemp;   // Store the temperature obtained from shT30.
                        // 将sht30获取到的温度存储
          humid = sht30.humidity;  // Store the humidity obtained from the SHT30.
            }   else {
        temp = 0, humid = 0;
    }                 // 将sht30获取到的湿度存储
      
      pressure = qmp6988.calcPressure();
if (temp >= 30) {
            setBuff(0x40, 0x00, 0x00);
        } else if (temp >= 20 && temp < 30) {
            setBuff(0x00, 0x40, 0x00);
        } else if (temp >= 10 && temp < 20) {
            setBuff(0x00, 0x00, 0x40);
        } else if (temp < 10) {
            setBuff(0x20, 0x20, 0x20);
        }

        // 根据湿度显示 LED
        if (humid < 50.00) {
            M5.dis.displaybuff(DisBuff);
            delay(250);
            M5.dis.clear();
            delay(250);
        } else if (humid >= 50.00 && humid < 60.00) {
            M5.dis.displaybuff(DisBuff);
            delay(500);
            M5.dis.clear();
            delay(500);
        } else if (humid >= 60.00) {
            M5.dis.displaybuff(DisBuff);
        }

unsigned long currentMillis = millis();
 const unsigned long dataInterval = 10000; // 10秒采集数据间隔
    const unsigned long sendInterval = 60000; // 60秒发送数据间隔
       static unsigned long previousDataMillis = 0;
    static unsigned long previousSendMillis = 0;
     if ((currentMillis - previousDataMillis) >= dataInterval) {
      previousDataMillis = currentMillis;
      float temp1, humid1, pressure1;
      if (sht30.get() == 0) {
          temp1 = sht30.cTemp;   // Store the temperature obtained from shT30.
                        // 将sht30获取到的温度存储
          humid1 = sht30.humidity;  // Store the humidity obtained from the SHT30.
            }   else {
        temp1 = 0, humid1 = 0;
    }         pressure1 = qmp6988.calcPressure();
       Serial.printf(
        "Temp: %2.1f  \r\nHumi: %2.0f%%  \r\nPressure:%2.0fPa\r\n---\n", temp1,
        humid1, pressure1);  

     }  

if ((currentMillis - previousSendMillis)>= sendInterval) {
      previousSendMillis = currentMillis;

        float avgTemp = 0;
        float avgHumid = 0;
        float avgPressure = 0;
      tempData[currentIndex] = temp;
     humidData[currentIndex] = humid;
     pressureData[currentIndex] = pressure;
    currentIndex = (currentIndex + 1) % 6;

    Serial.printf("%f",currentIndex);
        for (int i = 0; i < 6 ; ++i) {
            avgTemp += tempData[i];
            avgHumid += humidData[i];
            avgPressure += pressureData[i];
        }
       
        // 发送平均值到Ambient
        ambient.set(1, avgTemp);
        ambient.set(2, avgHumid);
        ambient.set(3, avgPressure);
        ambient.send();

      
        currentIndex = 0;
        memset(tempData, 0, sizeof(tempData));
        memset(humidData, 0, sizeof(humidData));
        memset(pressureData, 0, sizeof(pressureData));
        Serial.printf("xxxxxxxxalready sentxxxxxxxxxxxx\n");
    }
    
    
    

   
       
     
         }

  
 
     
  
 