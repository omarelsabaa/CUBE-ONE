//This is our code for the first cube since each one has different topics
//and different calibrations for the accelerometers.

//Below are the libraries needed to get the code working
#include <AWS_IOT.h>
#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
# define idle 0
# define throw_me 1
# define sending_face_number 2
# define check_if_correct 8
# define play_sound_correct 9
# define play_sound_wrong 10
# define send_back_score 11

int state = idle;
float FaceNumber;
AWS_IOT ewisp; //instance of AWS
LSM9DS1 imu; //instance of the accelerometer

 float id=0;

//WiFi details
char WIFI_SSID[] = "AhmedBokhary";
char WIFI_PASSWORD[] = "bokhary11";

//Connection to AWS
char HOST_ADDRESS[] = "a39furswz813o0.iot.us-east-1.amazonaws.com";
char CLIENT_ID[] = "ESP8266-1";
char TOPIC_NAME[] = "$aws/things/esp32_2C95EC/shadow/update";
char TOPIC_2[] = "esp32_2C95EC/topic1";

int status = WL_IDLE_STATUS;
int tick = 0, msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW
#define PRINT_SPEED 250 // 250 ms between prints
static unsigned long lastPrint = 0;
#define DECLINATION -8.58

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad) //getting message from AWS and if messages are recieved then change msg recieved flag to 1
{
  strncpy(rcvdPayload, payLoad, payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

//Calibration for getting the face number.
float getFaceNumber()
{
  // calcAccel helper function to convert a raw ADC value to
  // g's. Give the function the value that you want to convert.
  Serial.print("A: ");
  Serial.print(imu.calcAccel(imu.ax) * 100, 2);
  Serial.print(", ");
  Serial.print(imu.calcAccel(imu.ay) * 100, 2);
  Serial.print(", ");
  Serial.print(imu.calcAccel(imu.az) * 100, 2);
  Serial.println(" g");
  int faceNumberLocal;
  float x = imu.calcAccel(imu.ax) * 100;
  float y = imu.calcAccel(imu.ay) * 100;
  float z = imu.calcAccel(imu.az) * 100;

  if ((x >= 90) && (x <= 120) && (y >= -10) && (y <= 10) && (z >= -12) && (z <= 8)) {
    faceNumberLocal = 0;
  }
  else if ((x >= -110) && (x <= -87) && (y >= -12) && (y <= 10) && (z >= -7) && (z <= 15)) {
    faceNumberLocal = 1;
  }
  else if ((x >= -72) && (x <= -52) && (y >= -86) && (y <= -66) && (z >= -9) && (z <= 13)) {
    faceNumberLocal = 2;
  }
  else if ((x >= -10) && (x <= 10) && (y >= -88) && (y <= -67) && (z >= -72) && (z <= -52)) {
    faceNumberLocal = 3;
  }
  else if ((x >= 51) && (x <= 71) && (y >= -59) && (y <= -39) && (z >= 50) && (z <= 70)) {
    faceNumberLocal = 4;
  }
  else if ((x >= -6) && (x <= 15) && (y >= -10) && (y <= 10) && (z >= 90) && (z <= 110)) {
    faceNumberLocal = 5;
  }
  else if ((x >= -5) && (x <= 15) && (y >= 70) && (y <= 90) && (z >= 50) && (z <= 70)) {
    faceNumberLocal = 6;
  }
  else if ((x >= -72) && (x <= -53) && (y >= 39) && (y <= 59) && (z >= -70) && (z <= -50)) {
    faceNumberLocal = 7;
  }
  else if ((x >= -10) && (x <= 10) && (y >= -10) && (y <= 10) && (z >= -110) && (z <= -90)) {
    faceNumberLocal = 8;
  }
  else if ((x >= 50) && (x <= 70) && (y >= 70) && (y <= 95) && (z >= -10) && (z <= 10)) {
    faceNumberLocal = 9;
  }
  else
    faceNumberLocal = 20; //TO INDICATE ERROR

  delay(100); // delay for stability 0.1 sec
  Serial.print("Face Number: "); Serial.println(faceNumberLocal);
  return faceNumberLocal;
}

void setup() {
  
  Serial.begin(115200);
  delay(2000);
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  if (!imu.begin())
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                   "work for an out of the box LSM9DS1 " \
                   "Breakout, but may need to be modified " \
                   "if the board jumpers are.");
    while (1)
      ;
  }
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // wait 5 seconds for connection:
    delay(5000);
  }

  Serial.println("Connected to wifi");
  if (ewisp.connect(HOST_ADDRESS, CLIENT_ID) == 0)
  {
    Serial.println("Connected to AWS");
    delay(1000);
    ewisp.publish(TOPIC_2, "Cube 1 Connected to AWS");
    if (0 == ewisp.subscribe(TOPIC_NAME, mySubCallBackHandler))
    {
      Serial.println("Subscribe Successfull");
    }
    else
    {
      Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
      while (1);
    }
  }
  else
  {
    Serial.println("AWS connection failed, Check the HOST Address");
    while (1);
  }
  delay(2000);
}
void loop() {
  delay(1000);
  //Serial.print(rcvdPayload);
  // Update the sensor values whenever new data is available
  if ( imu.accelAvailable() )
  {
    imu.readAccel();
  }
 
  switch (state) {
    
    case idle: //keep checking for a message. if received then go to state 1
      if (strcmp(rcvdPayload, "throwcube") == 0) { //if this message from alexa is received
        Serial.println(" Received throwcube");
        state = throw_me;
      } else {
        Serial.println("Nothing received");
        state = idle;
      }
      delay(10);
      break;
      
    case throw_me:
      Serial.print("child is throwing the cube");
      state = sending_face_number;
      break;
      
    case sending_face_number:
      FaceNumber = getFaceNumber();
      id=id+1;
      float humi;
      char sHumi[10];
      char sID[10];
      dtostrf(FaceNumber, 6, 2, sHumi);
      dtostrf(id, 6, 2, sID);
      char shadow[100];
      strcpy(shadow, "{\"state\":{\"reported\": {\"Face\":");
      strcat(shadow, sHumi);
      strcat(shadow, ", \"ShadowID\":");
      strcat(shadow, sID);
      strcat(shadow, "}}}");

      
      
      if (ewisp.publish(TOPIC_NAME, shadow) == 0)
      {

        Serial.print("Publish Message:");
        Serial.println(payload);
      }
      
      else
      {
        Serial.println("Publish failed");
      }
      delay(10);
      state = idle;
      break;
  }
}