#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>

// Ethernet shield MAC address (sticker in the back)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Azure Mobile Service address
// You can find this in your service dashboard
const char *server = "arduinouno.azure-mobile.net";
EthernetClient client;
const int MPU_addr=0x68;
long accelX, accelY, accelZ;
int16_t AcX,AcY,AcZ
// Azure Mobile Service table name
// The name of the table you created
const char *table_name = "telemetry";

// Azure Mobile Service Application Key
// You can find this key in the 'Manage Keys' menu on the dashboard
const char *ams_key = "YaRWxxxxzKGRxxxxLPiNxxxxXYOvxxxx";

EthernetClient client;

char buffer[64];

/*
** Send an HTTP POST request to the Azure Mobile Service data API
*/

void send_request(float value)
{
  Serial.println("\nconnecting...");

  if (client.connect(server, 80)) {

    Serial.print("sending ");
    Serial.println(value);

    // POST URI
    sprintf(buffer, "POST /tables/%s HTTP/1.1", table_name);
    client.println(buffer);

    // Host header
    sprintf(buffer, "Host: %s", server);
    client.println(buffer);

    // Azure Mobile Services application key
    sprintf(buffer, "X-ZUMO-APPLICATION: %s", ams_key);
    client.println(buffer);

    // JSON content type
    client.println("Content-Type: application/json");

    // POST body
    sprintf(buffer, "{\"value\": %d}", value);

    // Content length
    client.print("Content-Length: ");
    client.println(strlen(buffer));

    // End of headers
    client.println();

    // Request body
    client.println(buffer);
    
  } else {
    Serial.println("connection failed");
  }
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
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  Serial.print(" AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  if(GyZ>0){
    Serial
  }
 
//  Serial.println(gForceZ);
  send_request(gForceZ);
  wait_response();
  read_response();
  end_request();

  delay(1000);
}

