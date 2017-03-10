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
#define MPU4_PIN 8
#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards

#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady1 = false;  // set true if DMP init was successful
bool dmpReady2 = false;
bool dmpReady3 = false;
bool dmpReady4 = false;

uint8_t mpuIntStatus1;   // holds actual interrupt status byte from MPU
uint8_t mpuIntStatus2;
uint8_t mpuIntStatus3;
uint8_t mpuIntStatus4;

uint8_t devStatus1;      // return status after each device operation (0 = success, !0 = error)
uint8_t devStatus2;
uint8_t devStatus3;
uint8_t devStatus4;

uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

volatile bool mpuInterrupt1 = false;     // indicates whether MPU interrupt pin has gone high
volatile bool mpuInterrupt2 = false;
volatile bool mpuInterrupt3 = false;
volatile bool mpuInterrupt4 = false;

void dmpDataReady1() {
    mpuInterrupt1 = true;
}

void dmpDataReady2() {
    mpuInterrupt2 = true;
}

void dmpDataReady3() {
    mpuInterrupt3 = true;
}

void dmpDataReady4() {
    mpuInterrupt4 = true;
}

void mpuSwitch() {
	if(MPU1_PIN == LOW) //if MPU 1 is on, change to MPU 2
	{
		digitalWrite(MPU1_PIN, HIGH);
		digitalWrite(S0_PIN, LOW);
		digitalWrite(S1_PIN, HIGH);
		digitalWrite(MPU2_PIN, LOW);
	}
	else if(MPU2_PIN == LOW) //if MPU 2 is on, change to MPU 3
	{
		digitalWrite(MPU2_PIN, HIGH);
		digitalWrite(S0_PIN, HIGH);
		digitalWrite(S1_PIN, HIGH);
		digitalWrite(MPU3_PIN, LOW);
	}
	else if(MPU3_PIN == LOW) //if MPU 3 is on, change to MPU 4
	{
		digitalWrite(MPU3_PIN, HIGH);
		digitalWrite(S0_PIN, HIGH);
		digitalWrite(S1_PIN, LOW);
		digitalWrite(MPU4_PIN, LOW);
	}
	else if(MPU4_PIN == LOW) //if MPU 4 is on, change to MPU 1
	{
		digitalWrite(MPU4_PIN, HIGH);
		digitalWrite(S0_PIN, LOW);
		digitalWrite(S1_PIN, LOW);
		digitalWrite(MPU1_PIN, LOW);
	}
	else 
	{
    digitalWrite(MPU2_PIN, HIGH);
	digitalWrite(MPU1_PIN, HIGH);
    digitalWrite(MPU2_PIN, HIGH);
	digitalWrite(MPU1_PIN, LOW);
	digitalWrite(S0_PIN, LOW);
	digitalWrite(S1_PIN, LOW);
	}
}





void setup() {
    // initialize output pins
	pinMode(MPU1_PIN, OUTPUT);
    pinMode(MPU2_PIN, OUTPUT);
	pinMode(MPU3_PIN, OUTPUT);
    pinMode(MPU4_PIN, OUTPUT);
    pinMode(S0_PIN, OUTPUT);
    pinMode(S1_PIN, OUTPUT);
	
    //set all addresses for mpus as 0x69 (not detected)
    digitalWrite(MPU1_PIN, HIGH);
    digitalWrite(MPU2_PIN, HIGH);
	digitalWrite(MPU1_PIN, HIGH);
    digitalWrite(MPU2_PIN, HIGH);
	
	// join I2C bus (I2Cdev library doesn't do this automatically)
	#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

	
	
	
	
    // initialize device 1
	mpuSwitch();
    Serial.println(F("Initializing I2C device 1..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);
    Serial.println(mpu.testConnection() ? F("MPU6050 1 connection successful") : F("MPU6050 1 connection failed"));
	
    // initialize device 2
	mpuSwitch();
    Serial.println(F("Initializing I2C device 2..."));
    mpu.initialize();
    Serial.println(mpu.testConnection() ? F("MPU6050 2 connection successful") : F("MPU6050 2 connection failed"));
	
	// initialize device 3
	mpuSwitch();
    Serial.println(F("Initializing I2C device 2..."));
    mpu.initialize();
    Serial.println(mpu.testConnection() ? F("MPU6050 3 connection successful") : F("MPU6050 3 connection failed"));
	
	// initialize device 4
	mpuSwitch();
    Serial.println(F("Initializing I2C device 2..."));
    mpu.initialize();
    Serial.println(mpu.testConnection() ? F("MPU6050 4 connection successful") : F("MPU6050 4 connection failed"));

    // wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available());                 // wait for data
    while (Serial.available() && Serial.read()); // empty buffer again



    // load and configure the DMP for device 1
	mpuSwitch();
    Serial.println(F("Initializing DMP on device 1..."));
    devStatus = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

    // load and configure the DMP for device 2
    mpuSwitch(); 
    Serial.println(F("Initializing DMP on device 2..."));
    devStatus = mpu.dmpInitialize();
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady2, RISING);
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

	// load and configure the DMP for device 3
    mpuSwitch(); 
    Serial.println(F("Initializing DMP on device 3..."));
    devStatus = mpu.dmpInitialize();
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady3, RISING);
        mpuIntStatus3 = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady3 = true;

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
	
	// load and configure the DMP for device 2
    mpuSwitch(); 
    Serial.println(F("Initializing DMP on device 2..."));
    devStatus = mpu.dmpInitialize();
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady2, RISING);
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

    delay(750);
}

void loop() {
    digitalWrite(MPU2_PIN, HIGH);
    digitalWrite(MPU1_PIN, LOW);
    digitalWrite(S0_PIN, HIGH);
    digitalWrite(S1_PIN, HIGH);

    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
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

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);



    digitalWrite(MPU1_PIN, HIGH);
    digitalWrite(MPU2_PIN, LOW);
    digitalWrite(S1_PIN, LOW);
    digitalWrite(S0_PIN, LOW);  
    
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