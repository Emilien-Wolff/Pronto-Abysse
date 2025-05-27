#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
// ---------------------------------------------------------------------------
#define YAW      0
#define PITCH    1
#define ROLL     2
// --------------------- MPU650 variables ------------------------------------
// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// Orientation/motion vars
Quaternion q;        // [w, x, y, z]         quaternion container
VectorFloat gravity; // [x, y, z]            gravity vector
float ypr[3];        // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

volatile bool mpuInterrupt = false;     // Indicates whether MPU interrupt pin has gone high
// ---------------------------------------------------------------------------

/**
 * Interrup détection routine
 */
void dmpDataReady() {
    mpuInterrupt = true;
}

/**
 * Setup configuration
 */
void setup() {
    Wire.begin();
    //TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)

    Serial.begin(9600);
    
    Serial.println("45");

    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // Verify connection
    Serial.println(F("Testing device connections..."));

    if (mpu.testConnection()){
       Serial.println("MPU6050 connection successful");
    }
    else{
       Serial.println("MPU6050 connection failed");
    }

    // Load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    Serial.println("59");

    // MPU calibration: set YOUR offsets here.
    mpu.setXAccelOffset(-240);
    mpu.setYAccelOffset(-3189);
    mpu.setZAccelOffset(759);
    mpu.setXGyroOffset(42);
    mpu.setYGyroOffset(-31);
    mpu.setZGyroOffset(-56);
    Serial.println("64");

    // Returns 0 if it worked
    if (devStatus == 0) {
        // Turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // Enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0 : #pin2)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // Set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // Get expected DMP packet size for later comparison
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
}

/**
 * Main program loop
 */
void loop() {
    //Serial.println("je suis entré dans la loop");
    // If programming failed, don't try to do anything
    if (!dmpReady) {
        Serial.println("fail");
        return;
    }

    // Wait for MPU interrupt or extra packet(s) available
 /*   while (!mpuInterrupt && fifoCount < packetSize) {
        // Do nothing...
    }*/

    // Reset interrupt flag and get INT_STATUS byte
   /* mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();*/
   // Serial.println("j'ai récupéré le statut int");

    // Get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // Check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
       // Serial.println(F("FIFO overflow!"));

        // Otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // Wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) {
            fifoCount = mpu.getFIFOCount();
        }

        // Read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
      //  Serial.println("j'ai récupéré le paquet");

        // Track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        // Convert Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        // Print angle values in degrees.
        Serial.print(ypr[YAW] * (180 / M_PI));
        Serial.print("\t");
        Serial.print(ypr[PITCH] * (180 / M_PI));
        Serial.print("\t");
        Serial.println(ypr[ROLL] * (180 / M_PI));

    }
    
  delay(100);
}