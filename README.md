# CUBES

Here you can find the C code for the ESP32 placed in the first cube. 

This code contains 4 main parts:
  
  1. Reading and Calibrating accelerometer values to obtain the correct face number that the cube has landed on.
  2. Connecting the ESP32 to the AWS IoT cloud.
  3. Publishing and Subscribing to MQTT topics for two way communication between the ESP32 and AWS IoT
  4. Updating the state of the shadow object representing the cube on AWS IoT
