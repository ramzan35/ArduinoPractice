#include <SoftwareSerial.h>

SoftwareSerial BTserial(10, 11); // RX | TX

int sensorPin = A0;
int num = 0;
int sensorValue = 0;

void setup() {
BTserial.begin(9600); 
}

void loop() {
num++;
//BTserial.print("UP_DOWN");
//BTserial.print(",");
//BTserial.print("UP_DOWN");
//BTserial.print(",");
//BTserial.print("UP_DOWN");
//BTserial.print(",");
//BTserial.print("UP_DOWN");
//BTserial.print(",");
//BTserial.print("UP,DOWN");
//BTserial.print(";");
//Serial.print(num);
BTserial.print(num);

delay(300);


}
