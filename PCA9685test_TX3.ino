//將MAP值改成對應到的遙控器值
//傳輸時不可以同時連接電腦的Serial monitior 不然會抖動

#include <SoftwareSerial.h>
#include<Servo.h>
#define RX 7
#define TX 4
#define motor_pin A0
#define rudder_pin A1
SoftwareSerial MiniSSC(RX, TX);     //(RX,TX)

int motor = 0;
int rudder = 0;

void setup() {
  Serial.begin(9600);
  MiniSSC.begin(9600);
}

void loop() {
  //Serial.println(analogRead(A0));
  motor = map(analogRead(A0), 0, 1024, 0, 255);
  //Serial.println(analogRead(A1));
  rudder = map(analogRead(A1), 1024, 10, 0, 255);
  MiniSSC.write(255);
  MiniSSC.write(1);
  MiniSSC.write(motor);
  Serial.print("motor pwm =");
  Serial.println(motor);
  delay(100);

  MiniSSC.write(255);
  MiniSSC.write(2);
  MiniSSC.write(rudder);
  Serial.print("rudder pwm =");
  Serial.println(rudder);
  delay(100);

}
