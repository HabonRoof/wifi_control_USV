


//This is the board ESP-01 A || IP:192.168.1.69
//AS SERVER COM16
//Version 0.1.3.5
//use ITEADLIB library 成功

#include "ESP8266.h"
#include <SoftwareSerial.h>
#define RX 10       //ESP8266-01 software RX pin
#define TX 11       //TX pin
#define SSID "PP_RPT"       // CHANGE ME
#define PASSWORD "hurx6001"    // CHANGE ME
#define ServerPort (5001)

SoftwareSerial esp8266(RX,TX); //啟動softSerial
ESP8266 wifi(esp8266);
void ESP8266init();

//////////////////////////////////////////////////////////////////////////
//建立串口傳輸資料
String Recv = "";           //遙控器接收值
String rudder_str = "";
int rudder = 0;
unsigned long int lastTime;
//////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(3,OUTPUT);
  Serial.begin(9600);
  esp8266.begin(9600);
  esp8266.setTimeout(200);
  ESP8266init();
//==========================  HardWare PWM setup  ========================
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20); //set output mode A and B are fast pwm, set WGM fast pwm 
  TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);       //PWM frequency = 16M/256(timer2 fast PWM mode TOP) / 1024(CS2 register)
}

void loop() {
  if (esp8266.available()) {
       if(esp8266.find("+IPD,")){
        Recv = esp8266.readString();    //從ESP826讀取AT回應字串
        rudder_str = Recv.substring(4);      //取字串最後3個字元為servo字串
        rudder = rudder_str.toInt();            //將servo字串轉換為帶號整數pwm
        //Serial.println(rudder);
        OCR2B = rudder;          //Set output compare register to rudder
                                   //OC2B is pin 3, connect servo at pin3,and u can use Timer2 HW PWM
       }
  }
  else{
    ESP8266init();
  }
    
}

void ESP8266init(){                     //初始化設定
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
ESPRSTART:
  byte FailedCount = 0;  //錯誤計數器
  if(wifi.kick())
//測試ESP-01是否正常工作
    Serial.println("AT OK");
  else
    Serial.println("AT Failed");
//設定station模式
  if(wifi.setOprToStationSoftAP())
    Serial.println("Set opr Station");
  else{
    Serial.println("Set Station Failed");
    FailedCount++;
  }
//允許一對多連線  
if(wifi.enableMUX())
    Serial.println("MUX OK");
  else{
    Serial.println("MUX Failed");
    FailedCount++;
  }
//開啟伺服器,等待客戶透過指定port連線
  if(wifi.startTCPServer(ServerPort))
    Serial.println("Server Done");
  else{
    Serial.println("Server Failed");
    FailedCount++;
  }
//連線至AP 
  if(wifi.joinAP(SSID, PASSWORD))
    Serial.println("connected AP");
  else{
    Serial.println("Connect Failed");
    FailedCount++;
  }
//若有錯誤重新跳回espstart
  if(FailedCount != 0){
    Serial.println(FailedCount);
    goto ESPRSTART;
    }
  else
  Serial.println("Server Done")
  digitalWrite(13,HIGH);
  Serial.end();
  }


