#include <Arduino.h>
#include <HardwareSerial.h>
#include <stdio.h>
#include <string.h>
#include "lipkg.h"
#include <WiFi.h>
#include <WiFiUdp.h>

// #define ssid      "Livebox-00C2"
// #define password  "PzpeMTpEkJ4bnQmJmG"

// #define ssid      "NETGEAR_11g"
// #define password  "ESME94200"

#define ssid      "iPhone de Maxime"
#define password  "maximerouillard"

#define SERVER_IP "172.20.10.2"
#define SERVER_PORT 12345

WiFiUDP udp;

#define RXD2 16
#define TXD2 17
#define BaudLidar 230400

#define LED 2

#define brochePWMLidar  26 //Broche de la PWM du Lidar

#define POINT_PER_PACK 12

void setup() {
  
  Serial.begin ( 115200 );

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }  
  if(WiFi.status() == WL_CONNECTED){ 
    digitalWrite(LED, HIGH);
  }

  //config LEDs
  pinMode(LED,OUTPUT);

  // Start WebSocket client

  Serial.println(WiFi.localIP());

  Serial2.begin(BaudLidar, SERIAL_8N1, RXD2, TXD2);

}


LiPkg *lidar = new LiPkg;
void loop() {


    static bool frameStarted = false;
    static uint8_t buffer[sizeof(LiDARFrameTypeDef)];
    static int bufferIndex = 0;
    float angle_step;

    // Read a single byte from the UART
    int bytes_read = Serial2.readBytes((char*)&buffer[bufferIndex], 1);

    if (bytes_read == 1) {
      if (bufferIndex == 0 && buffer[bufferIndex] == 0x54) {
        // Start of a new frame
        frameStarted = true;
        bufferIndex++;
      } else if (frameStarted) {
        bufferIndex++;

        if (bufferIndex == sizeof(LiDARFrameTypeDef)) {
          // Complete frame received
          frameStarted = false;
          bufferIndex = 0;

          // Process the frame
          LiDARFrameTypeDef frame;
          char charbuffer[sizeof(LiDARFrameTypeDef)];
          memcpy(&frame, buffer, sizeof(frame));

          if (frame.header == 0x54 && frame.ver_len == 0x2C) {
            // Only process the frame if the header and ver_len match the expected value
        
              if (lidar->Parse((uint8_t*)&frame, sizeof(frame))) {
                  //std::cout << "packet verified" << std::endl;
                  //lidar->AssemblePacket();
                  //webSocket.binaryAll(lidar_frame_data_, lidar_frame_data_.size());
                  
              }
           
            
          
          }
        }

    }
  }
}