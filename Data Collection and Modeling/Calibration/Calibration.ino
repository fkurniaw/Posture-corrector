
#include "I2Cdev.h"
#include "MPU6050.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;
int16_t sampleSize = 2500;

int16_t ax, ay, az;
int16_t gx, gy, gz;

//offset variables
float oax = 0, oay = 0, oaz = 0;
float ogx = 0, ogy = 0, ogz = 0; 

//difference
float dax = 0, day = 0, daz = 0;
float dgx = 0, dgy = 0, dgz = 0; 

//averages
float aax = 0, aay = 0, aaz = 0;
float agx = 0, agy = 0, agz = 0;

int16_t counter;
int16_t iteration = 1;

#define LED_PIN 13
bool blinkState = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    mpu.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // use the code below to change accel/gyro offset values
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);
    mpu.setXAccelOffset(0);
    mpu.setYAccelOffset(0); 
    mpu.setZAccelOffset(0);

    // configure Arduino LED for
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    Serial.print("Running iteration:\t"); Serial.println(iteration);
    delay(2500);
    counter = 0;
    while (counter < sampleSize)
    {
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      aax = aax+ax;
      aay = aay+ay;
      aaz = aaz+az;
      agx = agx+gx;
      agy = agy+gy;
      agz = agz+gz;

      counter++;

      Serial.print("a/g:\t");
      Serial.print(ax); Serial.print("\t\t");
      Serial.print(ay); Serial.print("\t\t");
      Serial.print(az); Serial.print("\t\t");
      Serial.print(gx); Serial.print("\t\t");
      Serial.print(gy); Serial.print("\t\t");
      Serial.println(gz);

            // blink LED to indicate activity
      blinkState = !blinkState;
      digitalWrite(LED_PIN, blinkState);
    }

    dax = aax/sampleSize;
    day = aay/sampleSize;
    daz = aaz/sampleSize -16384;
    dgx = agx/sampleSize;
    dgy = agy/sampleSize;
    dgz = agz/sampleSize;

      Serial.println("Data collection complete. Detected differences:");
      Serial.print("a/g:\t");
      Serial.print(dax); Serial.print("\t\t");
      Serial.print(day); Serial.print("\t\t");
      Serial.print(daz); Serial.print("\t\t");
      Serial.print(dgx); Serial.print("\t\t");
      Serial.print(dgy); Serial.print("\t\t");
      Serial.println(dgz);
      Serial.print("\n");
      

    if( dax<15 && dax>-15 && day<15 && day>-15 && daz<15 && daz>-15 && 
        dgx<3 && dgx>-3 && dgy<3 && dgy>-3 && dgx<3 && dgz>-3 || iteration > 25)
    {
      Serial.println("Calibration complete. Targets met using the following offsets:");
      Serial.print("a/g:\t");
      Serial.print(oax); Serial.print("\t\t");
      Serial.print(oay); Serial.print("\t\t");
      Serial.print(oaz); Serial.print("\t\t");
      Serial.print(ogx); Serial.print("\t\t");
      Serial.print(ogy); Serial.print("\t\t");
      Serial.println(ogz);
      Serial.print("\n");
      while(true)
      {
        Serial.print(".");
        delay(2500);
      }
    }
    else
    {
      oax = oax - (dax/7.8);
      oay = oay - (day/7.8);
      oaz = oaz - (daz/7.8);
      ogx = ogx - (dgx/4);
      ogy = ogy - (dgy/4);
      ogz = ogz - (dgz/4);

      Serial.println("Initiating next iteration. New offsets implemented:");
      Serial.print("a/g:\t");
      Serial.print(oax); Serial.print("\t\t");
      Serial.print(oay); Serial.print("\t\t");
      Serial.print(oaz); Serial.print("\t\t");
      Serial.print(ogx); Serial.print("\t\t");
      Serial.print(ogy); Serial.print("\t\t");
      Serial.println(ogz);
      Serial.println("\n");

      mpu.setXGyroOffset(ogx);
      mpu.setYGyroOffset(ogy);
      mpu.setZGyroOffset(ogz);
      mpu.setXAccelOffset(oax);
      mpu.setYAccelOffset(oay); 
      mpu.setZAccelOffset(oaz);
      
      dax = 0; day = 0; daz = 0;
      dgx = 0; dgy = 0; dgz = 0; 
      aax = 0; aay = 0; aaz = 0;
      agx = 0; agy = 0; agz = 0;

      delay(5000);      
      counter = 0;
      iteration++;
    }

}
