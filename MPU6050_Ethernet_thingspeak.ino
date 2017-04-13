
//EthernetClient client;

#include "ThingSpeak.h"
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>

EthernetClient client;
const int MPU_addr=0x68;
long accelX, accelY, accelZ;
int16_t AcX,AcY,AcZ
;
unsigned long myChannelNumber = 206734;
const char * myWriteAPIKey = "009QO9KUWZVTL512";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char buffer[64];

/*
** Send an HTTP POST request to the Azure Mobile Service data API
*/

void send_request(float value)
{
  Serial.println("\nconnecting...");

 ThingSpeak.writeField(myChannelNumber, 1, value, myWriteAPIKey);

}

/*
** Wait for response
*/

void wait_response()
{
  while (!client.available()) {
    if (!client.connected()) {
      return;
    }
  }
}

/*
** Read the response and dump to serial
*/

void read_response()
{
  bool print = true;
  
  while (client.available()) {
    char c = client.read();
    // Print only until the first carriage return
    if (c == '\n')
      print = false;
    if (print)
      Serial.print(c);
  }
}

/*
** Close the connection
*/

void end_request()
{
  client.stop();
}

/*
** Arduino Setup
*/

void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
//  Wire.begin();
//  setupMPU();
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  Serial.println("ethernet");

  if (Ethernet.begin(mac) == 0) {
    Serial.println("ethernet failed");
    for (;;) ;
  }
  
  // give the Ethernet shield a second to initialize:
  delay(1000);
  ThingSpeak.begin(client);
  
}

/*
** Arduino Loop
*/

void loop()
{
   Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
int ornt = 0;
if(AcZ>0){
//  Serial.print("UP");
    if(AcX>AcY){
      Serial.println("UP LEFT");
      ornt = 1;
    }
    else{
      Serial.println("UP RIGHT");
      ornt = 2;
    }
}
else{
 if(AcX>AcY){
      Serial.println("DOWN LEFT");
      ornt = 3;
    }
    else{
      Serial.println("DOWN RIGHT");
      ornt = 4;
    }
}
//  recordAccelRegisters();
//  Serial.println(gForceZ);
  send_request(ornt);
  wait_response();
  read_response();
  end_request();

  delay(1000);
}
