#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>

// Initialize the Ethernet server library
EthernetClient client;

const int MPU_addr=0x68;
long accelX, accelY, accelZ;
int16_t AcX,AcY,AcZ;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
//IPAddress ip(192,168,8,80);

char server[] = "sdgp.uk.ht"; 

void send_request(int value,int AcX)
{
  if (client.connect(server, 80)) {
    Serial.println("Connection success");
    client.print("GET /write_data.php?"); // This
    client.print("gyroscope="); // This
    client.print(value); 
    client.print("&accelerometer="); // This
    client.print(AcX);
    client.println(" HTTP/1.1"); // Part of the GET request
    client.println("Host: sdgp.uk.ht"); 
    client.println("Connection: close"); // Part of the GET request telling the server that we are over transmitting the message
    client.println(); // Empty line
    client.println(); // Empty line
    client.stop();    // Closing connection to server
  }

  else {
    // If Arduino can't connect to the server (your computer or web page)
    Serial.println("--> connection failed\n");
  }
 
  // Give the server some time to recieve the data and store it. I used 10 seconds here. Be advised when delaying. If u use a short delay, the server might not capture data because of Arduino transmitting new data too soon.
  delay(10000);

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
  
}

void loop() {
 
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
  send_request(ornt,AcX);
  wait_response();
  read_response();
  end_request();

  delay(1000);
  
}
