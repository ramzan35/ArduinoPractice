/*
 * Interfacing:
 * 
 * Vin: 3V3
 * GND: Gnd
 * SCL: SCL/AD5
 * SDA: SDA/Ad4
 * INT: D2
 */


#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

#define OUTPUT_READABLE_ACCELGYRO



void setup() {
    Wire.begin();   //begin I2c
    Serial.begin(115200);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

}

void loop() {

    
    accelgyro.getAcceleration(&ax, &ay, &az);
    accelgyro.getRotation(&gx, &gy, &gz);

    //you can also uncomment the following method to get the raw data
    // accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

//        Serial.print("a/g:\t");
        Serial.print("acceleromter x");Serial.print("\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print("acceleromter y");Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print("acceleromter z");Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print("gyroscope x");Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print("gyroscope x");Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.print("gyroscope x");
        Serial.println(gz);

        delay(1000);      // delay so that we can see the output   
}
