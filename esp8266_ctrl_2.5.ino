


//This is the board ESP-01 B || IP:192.168.1.209
//遙控器
//AS CLIENT COM3
//virsion 0.1.3.5
//use ITEADLIB library
#include "ESP8266.h"
#include <SoftwareSerial.h>
#define RX 10       //ESP8266-01 software RX pin
#define TX 11       //TX pin
#define SSID "PP_RPT"      // CHANGE ME
#define PASSWORD "hurx6001"     // CHANGE ME
#define ServerIP "192.168.1.69"
#define ServerPort (5001)
String Srtinglength = "3";
uint8_t mux_id = 0;

SoftwareSerial esp8266(RX,TX); 
ESP8266 wifi(esp8266);

void sendCommand();
void ESP8266init();
int countTrueCommand;
int countTimeCommand; 
boolean found = false;


//////////////////////////////////////////////////////////////////////////
//建立串口傳輸資料

int ctrlPin = A0;
unsigned long int lastTime;

//////////////////////////////////////////////////////////////////////////

void setup() {
  //Serial.begin(9600);
  esp8266.begin(9600);
  ESP8266init();
}

void loop() { 
  if(esp8266.available()){
  int VR = map(analogRead(A0),191,829,13,37);     //讀取可變電阻值，因Timer2的TCNT2頻率為16M/(256*CS2)=61.2Hz，意即每1/60秒由0數到255，而servo的PWM寬度為880us~2400us
  String Dir = String(VR,DEC);                    //因此經過轉換後得到計數範圍應為13~37
  //char *DIR = Dir[8];
                                                  //將int轉換為字串以適用AT命令
  if(millis() - lastTime >= 50){
    if (esp8266.available()) {
      sendCommand("AT+CIPSEND=0,"+ Srtinglength,1,">"); //傳送資料給server
      //wifi.send(mux_id,(const uint8_t)DIR,strlen(DIR));
      sendCommand(Dir,2,"SEND OK");
    }
    lastTime = millis();
    }
  }
  else
    ESP8266init();
}


void sendCommand(String command, int maxTime, char readReplay[]) {
  //Serial.print(countTrueCommand);
  //Serial.print(". at command => ");
  //Serial.print(command);
  //Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    //Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    //Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
}


void ESP8266init(){
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
ESPRSTART:
  byte FailedCount = 0;  //錯誤計數器
  
//測試ESP-01是否正常工作 
  if(wifi.kick())
    Serial.println("AT OK");
  else
    Serial.println("AT Failed");
    
//設定station模式
  if(wifi.setOprToStationSoftAP())
    Serial.println("Set opration to Station");
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
//連線至AP 
  if(wifi.joinAP(SSID, PASSWORD))
    Serial.println("connected AP");
  else{
    Serial.println("Connect Failed");
    FailedCount++;
  }
//建立TCP連線，連線到mux_id(裝置ID)Server IP Server Port
  if(wifi.createTCP(mux_id,ServerIP,ServerPort)){
    Serial.print("create tcp ");
    Serial.print(mux_id);
    Serial.println(" ok");
    } 
  else {
    Serial.print("create tcp ");
    Serial.print(mux_id);
    Serial.println(" err");
    FailedCount++;
    }
//測試傳輸資料hello
  char *hello = "Hello, this is client!";
  if (wifi.send(mux_id, (const uint8_t*)hello, strlen(hello))) {
    Serial.println("send ok");
    } 
  else {
    Serial.println("send err");
    FailedCount++;
    }
  //若有錯誤重新跳回ESPRSTART
  if(FailedCount != 0){
    Serial.println(FailedCount);
    goto ESPRSTART;
    }
  else
  Serial.println("Client Done!");
  digitalWrite(13,HIGH);
  Serial.end();
  }
