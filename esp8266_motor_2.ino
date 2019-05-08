


//This is the board ESP-01 A || IP:192.168.43.197
//AS SERVER COM16
//Version 0.1.3
//fix the servo jitter problem

#include<Servo.h>
#include <SoftwareSerial.h>
#define RX 10       //ESP8266-01 software RX pin
#define TX 11       //TX pin
String AP = "Infinity";       // CHANGE ME
String PSWD = "galaxys8";     // CHANGE ME
String ServerPort = "5001";
String Srtinglength = "3";
SoftwareSerial esp8266(RX,TX); //啟動softSerial
void sendCommand();
void ESP8266init();
Servo rudderServo;                //宣告myservo為servo物件'
byte servopin = 9;
int countTrueCommand;
int countTimeCommand;
int rudder_old; 
boolean found = false;

//////////////////////////////////////////////////////////////////////////
//建立串口傳輸資料
String Recv = "";           //遙控器接收值
String rudder_str = "";
int rudder = 0;
unsigned long int lastTime;
//////////////////////////////////////////////////////////////////////////

void setup() {
  //pinMode(servopin,OUTPUT);
  //digitalWrite(servopin,LOW);
  pinMode(3,OUTPUT);
  Serial.begin(9600);
  esp8266.begin(9600);
  esp8266.setTimeout(200);
  ESP8266init();
  rudderServo.attach(servopin);
//==========================  HardWare PWM setup  ========================
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20); //set output mode A and B are fast pwm, set WGM fast pwm 
  TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20);       //PWM frequency = 16M/256(timer2 fast PWM mode TOP) / 1024(CS2 register)
}

void loop() {
  if (esp8266.available()) {
       if(esp8266.find("+IPD,")){
        Recv = esp8266.readString();    //從ESP826讀取AT回應字串
        //Serial.print("Receive data =");
        //Serial.println(Recv);
        rudder_str = Recv.substring(4);      //取字串最後3個字元為servo字串
        //Serial.print("rudder_str=");
        //Serial.println(rudder_str);
        rudder = rudder_str.toInt();            //將servo字串轉換為帶號整數pwm
        //Serial.print("rudder position=");
        Serial.println(rudder);
        if(abs(rudder-rudder_old) > 2){
          rudder_old = rudder;
          //rudderServo.write(rudder);           //伺服機位置設定為rudder
          OCR2B = rudder;          //Set output compare register to rudder
                                   //OC2B is pin 3, connect servo at pin3,and u can use Timer2 HW PWM
        }
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
void ESP8266init(){                     //初始化設定
  pinMode(13,OUTPUT);
  sendCommand("AT",5,"OK");             //測試ESP-01是否正常工作
  sendCommand("AT+CWMODE=1",5,"OK");    //設定模式為Station
  sendCommand("AT+CIPMUX=1",5,"OK");    //允許一對多連線
  sendCommand("AT+CIPSERVER=1,"+ ServerPort ,5,"OK");  //開啟伺服器,等待客戶透過指定port連線
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PSWD +"\"",10,"OK");  //連線至AP
  Serial.print("Server Setup Done!");
  digitalWrite(13,HIGH);
  }

