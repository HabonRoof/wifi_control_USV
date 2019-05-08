


//This is the board ESP-01 B || IP:192.168.43.75
//AS CLIENT COM3
//virsion 0.1.3 
//fix the servo jitter problem

#include <SoftwareSerial.h>
#define RX 10       //ESP8266-01 software RX pin
#define TX 11       //TX pin
String AP = "Infinity";       // CHANGE ME
String PSWD = "galaxys8";     // CHANGE ME
String WifiType = "TCP";
String ServerIP = "192.168.43.197";
String ServerPort = "5001";
String Srtinglength = "3";
String testText = "123";
SoftwareSerial esp8266(RX,TX); 
void sendCommand();
void ESP8266init();
int countTrueCommand;
int countTimeCommand; 
boolean found = false;

//////////////////////////////////////////////////////////////////////////
//建立串口傳輸資料
struct DATA{                     //建立資料結構
  uint8_t posx;
  uint8_t posy;
};
DATA data1;                        //定義結構變量1
//DATA data2;                        //定義結構變量2
int ctrlPin = A0;
unsigned long int lastTime;
//////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  esp8266.begin(9600);
  ESP8266init();
}

void loop() {
  if(esp8266.available()){
  int VR = map(analogRead(A0),0,1023,13,37);    //讀取可變電阻值，因Timer2的TCNT2頻率為16M/(256*CS2)=61.2Hz，意即每1/60秒由0數到255，而servo的PWM寬度為880us~2400us
                                                //因此經過轉換後得到計數範圍應為13~37
  String Dir = String (VR,DEC);                 //將int轉換為字串以適用AT命令
  if(millis() - lastTime >= 200){
    static uint8_t i = 1;
    Serial.print("send: ");
    Serial.println(i);
    (i<4)? i++ : i=1;
    if (esp8266.available()) {
      sendCommand("AT+CIPSEND=0,"+ Srtinglength,2,">"); //傳送資料給server
      sendCommand(Dir,2,"SEND OK");
    }
    lastTime = millis();
    }
  }
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
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
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
}

void ESP8266init(){
  pinMode(13,OUTPUT);
  sendCommand("AT",5,"OK");             //測試ESP-01是否正常工作
  sendCommand("AT+CWMODE=1",5,"OK");    //查詢當前模式
  sendCommand("AT+CIPMUX=1",5,"OK");    //允許一對多連線
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PSWD +"\"",10,"OK");  //連線至AP
  sendCommand("AT+CIPSTART=0,\""+ WifiType +"\",\""+ServerIP+"\","+ ServerPort,10,"OK");
  sendCommand("AT+CIPSEND=0,"+ Srtinglength,2,">");
  sendCommand(testText,2,"SEND OK");
  Serial.println("ESP8266 Done!");
  digitalWrite(13,HIGH);
  }
