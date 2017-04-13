#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>

const int MPU_addr=0x68;
long accelX, accelY, accelZ;
int16_t AcX,AcY,AcZ;
SoftwareSerial BTserial(10, 11); // RX | TX

int sensorPin = A0;
int sensorValue = 0;

void setup()
{
BTserial.begin(9600);
//Serial.begin(9600);
Wire.begin();
Wire.beginTransmission(MPU_addr);
Wire.write(0x6B); // PWR_MGMT_1 register
Wire.write(0); // set to zero (wakes up the MPU-6050)
Wire.endTransmission(true);
Serial.begin(9600);
// Wire.begin();
// setupMPU();
//while (!Serial) {
//; // wait for serial port to connect.
//}
}

void loop() {

Wire.beginTransmission(MPU_addr);
Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
Wire.endTransmission(false);
Wire.requestFrom(MPU_addr,14,true); // request a total of 14 registers
AcX=Wire.read()<<8|Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
AcY=Wire.read()<<8|Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
AcZ=Wire.read()<<8|Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
String request = "";
String acc = String(AcX);
if(AcZ>0){
if(AcX>AcY){
//Serial.println("UP LEFT");
//request = "UP_LEFT#"+acc+";";
request = "1";
//BTserial.print("1");
}
else{
//Serial.println("UP RIGHT");
//request = "UP_RIGHT#"+acc+";";
request = "2";
//BTserial.print("2");
}
}
else{
if(AcX>AcY){
//Serial.println("DOWN LEFT");
//request = "DOWN_LEFT#"+acc+";";
request = "3";
//BTserial.print("3");
}
else{
//Serial.println("DOWN RIGHT");
//request = "DOWN_RIGHT#"+acc+";";
request = "4";
//BTserial.print("4");
}
}
BTserial.print(request);
BTserial.print(",");
BTserial.print(acc);
BTserial.print(";");

delay(300);

}
