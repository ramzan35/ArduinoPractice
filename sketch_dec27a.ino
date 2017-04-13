// Push DHT22 data from MRK1000 to Azure Event Hub

#include "DHT.h"
#include <SPI.h>
#include <ArduinoJson.h>
#include <RTCZero.h>
#include "time.h"
#include "ssid.h"

EthernetClient client;
const int MPU_addr=0x68;
long accelX, accelY, accelZ;
int16_t AcX,AcY,AcZ;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char buffer[64];

RTCZero rtc;

#define DHTPIN 6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Generate SAS key using https://github.com/sandrinodimattia/RedDog/releases
char hostname[] = "connectthedotsex-ns.servicebus.windows.net";
char authSAS[] = "SharedAccessSignature sr=https%3a%2f%2fconnectthedotsex-ns.servicebus.windows.net%2fehdevices%2fpublishers%2fd1%2fmessages&sig=OzFhN9z3ROSBqmaEz1em3DHIPsj7vcu3gY4BE60n%2fTo%3d&se=1460172816&skn=D1";

String hubName = "ehdevices";
String deviceName = "D1";
String uri = "/" + hubName + "/publishers/" + deviceName + "/messages";


void setup() {
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
  rtc.begin();
  setTimeUsingTimeServer(rtc);
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
  sendEvent(ornt);
  wait_response();
  read_response();
  end_request();


  delay(10000);
}

void sendEvent(int value) {

  String json = createJSON(evalue);
  httpPost(json);

  String response = "";
  char c;
  while (client.available()) {
    c = client.read();
    response.concat(c);
  }

  if (!response.equals("")) {
    if (response.startsWith("HTTP/1.1 201")) {
      Serial.println("Data sent to Azure"); 
    } else {
      Serial.println("Error posting " + json);
      Serial.println(response);
    }
  }

}

String createJSON(value) {

  // ISO8601 date string
  char dateString[25];
  sprintf(dateString, "2%03d-%02d-%02dT%02d:%02d:%02d.000Z", rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());

  // JSON is based on Microsoft Connect the Dots example
  // https://github.com/Azure/connectthedots/blob/master/Introduction.md#device-basics
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["subject"] = "wthr";
  root["organization"] = "Foo Industries";
  root["displayname"] = "MKR1000";
  root["measurename"] = measurename;
  root["unitofmeasure"] = unitofmeasure;
  root["value"] = String(value);
  root["timecreated"] = dateString;
  root["guid"] = "123456";

  char json[200];
  root.printTo(json, sizeof(json));
  return String(json);
}

// Borrowed from https://github.com/mohankrr/Arduino/blob/master/MKR1000AzureReceiveFull/MKR1000AzureReceiveFull.ino#L152-L186
void httpPost(String content)
{
    // close any connection before send a new request.
    client.stop();
  
    if (client.connectSSL(hostname, 443)) {
      client.print("POST ");
      client.print(uri);
      client.println(" HTTP/1.1"); 
      client.print("Host: "); 
      client.println(hostname);
      client.print("Authorization: ");
      client.println(authSAS); 
      client.println("Connection: close");

      client.print("Content-Type: ");
      client.println("application/json");
      client.print("Content-Length: ");
      client.println(content.length());
      client.println();
      client.println(content);
      
      delay(200);
      
    } else {
      Serial.println("Connection failed");
    }
}
