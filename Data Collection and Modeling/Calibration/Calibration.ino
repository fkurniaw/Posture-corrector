
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
    Serial.println("Checking to see if original offsets are somewhat OK...");
    delay(500);
    // use the code below to change accel/gyro offset values; might want to change to let it run one iteration to see if current calibration is acceptable so you don't waste too much time
    counter = 0;
    while(counter < sampleSize){
      
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      aax = aax+ax;
      aay = aay+ay;
      aaz = aaz+az;
      agx = agx+gx;
      agy = agy+gy;
      agz = agz+gz;
      
      Serial.print("a/g:\t");
      Serial.print(ax); Serial.print("\t\t");
      Serial.print(ay); Serial.print("\t\t");
      Serial.print(az); Serial.print("\t\t");
      Serial.print(gx); Serial.print("\t\t");
      Serial.print(gy); Serial.print("\t\t");
      Serial.println(gz);
      counter++;
    }
    
    dax = aax/sampleSize;
    day = aay/sampleSize;
    daz = aaz/sampleSize -16384;
    dgx = agx/sampleSize;
    dgy = agy/sampleSize;
    dgz = agz/sampleSize;
      
    if(abs(dax)<1000 && abs(day)<1000 && abs(daz)<1000 && abs(dgx)<60 && abs(dgy)<60  && abs(dgz)<60 )
    {
      oax = mpu.getXAccelOffset() - (dax/7.8); oay = mpu.getYAccelOffset() - (day/7.8); oaz = mpu.getZAccelOffset() - (daz/7.8);
      ogx = mpu.getXGyroOffset()  - (dgx/4.0); ogy = mpu.getYGyroOffset() - (dgy/4.0); ogz = mpu.getZGyroOffset() - (dgz/4.0); 
      Serial.println("Original offsets were OK. They are as follows:");
      Serial.print("a/g:\t");
      Serial.print(oax); Serial.print("\t\t");
      Serial.print(oay); Serial.print("\t\t");
      Serial.print(oaz); Serial.print("\t\t");
      Serial.print(ogx); Serial.print("\t\t");
      Serial.print(ogy); Serial.print("\t\t");
      Serial.println(ogz);
      Serial.println("\n");
      
      Serial.println("The differences were:");
      Serial.print("a/g:\t");
      Serial.print(dax); Serial.print("\t\t");
      Serial.print(day); Serial.print("\t\t");
      Serial.print(daz); Serial.print("\t\t");
      Serial.print(dgx); Serial.print("\t\t");
      Serial.print(dgy); Serial.print("\t\t");
      Serial.println(dgz);
      Serial.println("\n");
      delay(1000);
    }    
    else
    {        
      Serial.println("Inaccurate offsets. Resetting...");
      oax = -(dax/7.8); oay = -(day/7.8); oaz = -(daz/7.8);
      ogx = -(dgx/4.0); ogy = -(dgy/4.0); ogz = -(dgz/4.0);
      
      mpu.setXGyroOffset(ogx);
      mpu.setYGyroOffset(ogy);
      mpu.setZGyroOffset(ogz);
      mpu.setXAccelOffset(oax);
      mpu.setYAccelOffset(oay); 
      mpu.setZAccelOffset(oaz);
    }
    
    //reset differences and averages
    dax = 0;   day = 0;   daz = 0;
    dgx = 0;   dgy = 0;   dgz = 0;
    
    aax = 0;   aay = 0;   aaz = 0;
    agx = 0;   agy = 0;   agz = 0;
    
    // configure Arduino LED for
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    Serial.print("Running iteration:\t"); Serial.println(iteration); Serial.print("\n");
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

            // blink LED to indicate activity; seems too fast so I reduced the rate of changing blinkState
      if(counter%10 == 0) blinkState = !blinkState;
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
      
    // if calibrated 25 times or differences from last iteration to current < threshold, stop calibration
    if(abs(dax)<15 && abs(day)<15 && abs(daz)<15 && abs(dgx)<3 && abs(dgy)<3  && abs(dgz)<3 || iteration > 25)  
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
