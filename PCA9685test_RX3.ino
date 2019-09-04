

//This is the board ESP-01 A || IP:192.168.43.197
//AS SERVER COM16
//Version 0.1.4
//fix the servo jitter problem
//At指令失敗要用迴圈重跑
//改lirbray
//改成用MOXA無線控制
//改成使用PCA9685來當作PWM控制
//將收到的值MAP放大範圍來對應到PCA9685的使用
//改善傳輸端會溢位問題
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define RX 4       //ESP8266-01 software RX pin
#define TX 7       //TX pin Tiner2的CORA是11腳，因此改用12腳代替
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

SoftwareSerial USVRx(RX, TX); //啟動softSerial





void setup() {
  Serial.begin(9600);
  USVRx.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);// Analog servos run at ~60 Hz updates
}

void loop() {
  
  if (USVRx.available() > 0) {  
    byte databyte = USVRx.read();
    Serial.print(databyte);   //if you delete this column the program will not work
    if (databyte == 255) {
      byte servo_num = USVRx.read();
      if (servo_num == 1) {
        Serial.print("servo_num = 1");
        //int servo1_angle = USVRx.read();
        int servo1_angle = map(USVRx.read(), 0, 255, 120, 505);
        Serial.print("servo1_angle = ");
        Serial.println(servo1_angle);
        //motor.write(servo1_angle);
        pwm.setPWM(0, 0, servo1_angle);
      }
      else if (servo_num == 2) {
        Serial.print("servo_num = 2");
        //int servo2_angle = USVRx.read();
        int servo2_angle = map(USVRx.read(), 0, 255, 545, 180);
        Serial.print("servo2_angle = ");
        Serial.println(servo2_angle);
        //rudder.write(servo2_angle);
        pwm.setPWM(1, 0, servo2_angle);
      }
    }
  }

}
