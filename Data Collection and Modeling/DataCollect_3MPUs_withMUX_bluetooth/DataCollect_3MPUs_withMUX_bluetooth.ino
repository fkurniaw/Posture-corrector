#include <SoftwareSerial.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;
#define S0_PIN 3
#define S1_PIN 4
#define MPU1_PIN 5
#define MPU2_PIN 6
#define MPU3_PIN 7
#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards

SoftwareSerial mySerial(10, 11); // setting up rx and tx

#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
float outputAngles[3];  //container for holding the separate mpu values

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

void dmpDataReady() 
{
    mpuInterrupt = true;
}

void initializePins()
{
  // initialize output pins
    pinMode(MPU1_PIN, OUTPUT);
    pinMode(MPU2_PIN, OUTPUT);
    pinMode(MPU3_PIN, OUTPUT);
    pinMode(S0_PIN, OUTPUT);
    pinMode(S1_PIN, OUTPUT);

  //set all addresses for mpus as 0x69 (not detected)
    digitalWrite(MPU1_PIN, HIGH);
    digitalWrite(MPU2_PIN, HIGH);
    digitalWrite(MPU3_PIN, HIGH);
}

void mpuSwitchTo1()
{
    digitalWrite(MPU3_PIN, HIGH);
    digitalWrite(MPU1_PIN, LOW);
    digitalWrite(S0_PIN, HIGH);
    digitalWrite(S1_PIN, HIGH);
}

void mpuSwitchTo2()
{
    digitalWrite(MPU1_PIN, HIGH);
    digitalWrite(MPU2_PIN, LOW);
    digitalWrite(S1_PIN, LOW);
    digitalWrite(S0_PIN, LOW); 
}

void mpuSwitchTo3()
{
    digitalWrite(MPU2_PIN, HIGH);
    digitalWrite(MPU3_PIN, LOW);
    digitalWrite(S1_PIN, LOW);
    digitalWrite(S0_PIN, HIGH);
}

void setup() 
{
    initializePins();
  
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    Serial.begin(115200);
    mySerial.begin(115200);
    mySerial.listen();
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // initialize device 1
    mpuSwitchTo1();
    mySerial.println(F("Initializing I2C device 1..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);
    mySerial.println(mpu.testConnection() ? F("MPU6050 1 connection successful") : F("MPU6050 1 connection failed"));
  
    // initialize device 2
    mpuSwitchTo2(); 
    mySerial.println(F("Initializing I2C device 2..."));
    mpu.initialize();
    mySerial.println(mpu.testConnection() ? F("MPU6050 2 connection successful") : F("MPU6050 2 connection failed"));
  
    // initialize device 3 
    mpuSwitchTo3(); 
    mySerial.println(F("Initializing I2C device 3..."));
    mpu.initialize();
    mySerial.println(mpu.testConnection() ? F("MPU6050 3 connection successful") : F("MPU6050 3 connection failed"));
  
    // wait for ready
    mySerial.println(F("\nSend any character to begin DMP programming and demo: "));

    delay(50);
 
    // load and configure the DMP for device 1
    mpuSwitchTo1(); 
    mySerial.println(F("Initializing DMP on device 1..."));
    devStatus = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) 
    {
        // turn on the DMP, now that it's ready
        mySerial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        mySerial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        mySerial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } 
    else 
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        mySerial.print(F("DMP Initialization failed (code "));
        mySerial.print(devStatus);
        mySerial.println(F(")"));
    }

    // load and configure the DMP for device 2
    mpuSwitchTo2(); 
    mySerial.println(F("Initializing DMP on device 2..."));
    devStatus = mpu.dmpInitialize();
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) 
    {
        // turn on the DMP, now that it's ready
        mySerial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        mySerial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        mySerial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else 
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        mySerial.print(F("DMP Initialization failed (code "));
        mySerial.print(devStatus);
        mySerial.println(F(")"));
    }

    // load and configure the DMP for device 3
    mpuSwitchTo3(); 
    mySerial.println(F("Initializing DMP on device 3..."));
    devStatus = mpu.dmpInitialize();
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) 
    {
        // turn on the DMP, now that it's ready
        mySerial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        mySerial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        mySerial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else 
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        mySerial.print(F("DMP Initialization failed (code "));
        mySerial.print(devStatus);
        mySerial.println(F(")"));
    }
  
    // configure LED for output
    pinMode(LED_PIN, OUTPUT);

}

void loop() 
{

  if (Serial.available()){
    delay(10); // The delay is necessary to get this working!
    mySerial.write(Serial.read());
  }
  
  // Read values from device 1
    mpuSwitchTo1(); 
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) 
    {

    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) 
  {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        mySerial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } 
  else if (mpuIntStatus & 0x02) 
  {
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
        outputAngles[0] = ypr[1]; //switch to pitch for MFP
        
        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
  }

  // Read values from device 2
     mpuSwitchTo2();  
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) 
    {

    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) 
  {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        mySerial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } 
  else if (mpuIntStatus & 0x02) 
  {
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
        outputAngles[1] = ypr[1]; //switch to pitch for MFP

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
        
    }

    // Read values from device 3
    mpuSwitchTo3(); 
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) 
    {

    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) 
    {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        mySerial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } 
    else if (mpuIntStatus & 0x02) 
    {
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
        outputAngles[2] = ypr[1]; //switch to pitch for MFP

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
        
    }
    mySerial.print(outputAngles[0], 5); mySerial.print(", "); mySerial.print(outputAngles[1], 5); mySerial.print(", "); mySerial.println(outputAngles[2], 5);
 }
