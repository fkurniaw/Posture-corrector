#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;
#define AD0_PIN1 4
#define AD0_PIN2 5

#define INTERRUPT_PIN1 2  // use pin 2 on Arduino Uno & most boards
#define INTERRUPT_PIN2 3  // use pin 2 on Arduino Uno & most boards

#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady1 = false;  // set true if DMP init was successful
bool dmpReady2 = false; 

uint8_t mpuIntStatus1;   // holds actual interrupt status byte from MPU
uint8_t mpuIntStatus2;

uint8_t devStatus1;      // return status after each device operation (0 = success, !0 = error)
uint8_t devStatus2;     

uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

volatile bool mpuInterrupt1 = false;     // indicates whether MPU interrupt pin has gone high
volatile bool mpuInterrupt2 = false;

void dmpDataReady1() {
    mpuInterrupt1 = true;
}

void dmpDataReady2() {
    mpuInterrupt2 = true;
}

void switchMPU() {
  if (AD0_PIN1 == HIGH) {
      digitalWrite(AD0_PIN1, LOW);
      digitalWrite(AD0_PIN2, HIGH);
  }
  else
  {
     digitalWrite(AD0_PIN2, LOW);
     digitalWrite(AD0_PIN1, HIGH);     
  }    
}

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    pinMode(AD0_PIN1, OUTPUT); //initializing pinMode
    pinMode(AD0_PIN2, OUTPUT); 
    digitalWrite(AD0_PIN1, HIGH);
    digitalWrite(AD0_PIN2, HIGH);
    
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately
//------------------------------------------------------------------------------------------------------------------------
    // initialize device 1
    switchMPU();
    Serial.println(F("Initializing I2C device 1..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN1, INPUT);

    // verify connection
    Serial.println(F("Testing device 1 connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 1 connection successful") : F("MPU6050 1 connection failed"));
//------------------------------------------------------------------------------------------------------------------------
    // initialize device 2
    switchMPU();
    Serial.println(F("Initializing I2C device 2..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN2, INPUT);

    // verify connection
    Serial.println(F("Testing device 2 connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 2 connection successful") : F("MPU6050 2 connection failed"));

    // wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again

 //------------------------------------------------------------------------------------------------------------------------   
    // load and configure the DMP for MPU 1
    switchMPU();
    Serial.println(F("Initializing DMP on device 1..."));
    devStatus1 = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (devStatus1 == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP for MPU 1..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN1), dmpDataReady1, RISING);
        mpuIntStatus1 = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady1 = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus1);
        Serial.println(F(")"));
    }
    
 //-------------------------------------------------------------------------------------------------  
    // load and configure the DMP for MPU 2
    switchMPU();
    Serial.println(F("Initializing DMP on MPU 2..."));
    devStatus2 = mpu.dmpInitialize();
    
    // make sure it worked (returns 0 if so)
    if (devStatus2 == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN2), dmpDataReady2, RISING);
        mpuIntStatus2 = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady2 = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus2);
        Serial.println(F(")"));
    }

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);

}

void loop() {
    digitalWrite(AD0_PIN2, HIGH);
    digitalWrite(AD0_PIN1, LOW);

    // if programming failed, don't try to do anything
    if (!dmpReady1) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt1 && fifoCount < packetSize) {
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt1 = false;
    mpuIntStatus1 = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus1 & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus1 & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        // display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        Serial.print(ypr[1], 5); 
        Serial.print(", ");

    digitalWrite(AD0_PIN1, HIGH);
    digitalWrite(AD0_PIN2, LOW);
    
    // if programming failed, don't try to do anything
    if (!dmpReady2) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt2 && fifoCount < packetSize) {
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt2 = false;
    mpuIntStatus2 = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus2 & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus2 & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        // display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        Serial.println(ypr[1], 5);

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
    }
  }
}
