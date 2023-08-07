#include <SPI.h>
#include <TimerOne.h>
#include <Ethernet.h>
#include "SystemFont5x7.h"
#include "Arial_black_16.h"
#include <Arduino.h>

#define outputA 3
#define outputB 2

IPAddress ip(192, 168, 1, 100);                   // Static IP address
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA}; // MAC address of your Arduino

int aState;
int aLastState;

int serverPort = 1234;                             // Port number of the server

IPAddress serverIP(192, 168, 1, 200);             // IP address of the server
IPAddress gateway(192, 168, 1, 1);                // Gateway IP address
IPAddress subnet(255, 255, 255, 0);                // Subnet mask

EthernetClient client;

void setup() {
  // Initialize Ethernet library
  // Ethernet.begin(mac);
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  // Wait for Ethernet to be initialized
  delay(1000);

  Serial.println("Connecting to server...");
  // Connect to the server
  connectToServer();

  pinMode (outputA,INPUT);
  pinMode (outputB,INPUT);
  
  Serial.begin (9600);
  // Reads the initial state of the outputA
  aLastState = digitalRead(outputA);
} 

void loop() { 
  if (client.connected())
  {
    // Client is connected
    if (client.available())
    {
      aState = digitalRead(outputA); // Reads the "current" state of the outputA
      // If the previous and the current state of the outputA are different, that means a Pulse has occured
      if (aState != aLastState){     
        // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
        if (digitalRead(outputB) != aState) {
          Serial.print("Rotated:");
          client.println("rotated");
        }
      } 
      aLastState = aState; // Updates the previous state of the outputA with the current state
    }
  }
  else
  {
    // Client is disconnected
    Serial.println();
    Serial.println("Server disconnected!");

    // Attempt to reconnect to the server
    connectToServer();
  }
}

void connectToServer()
{
  Serial.println("Connecting to server...");
  // Connect to the server
  if (client.connect(serverIP, serverPort))
  {
    Serial.println("Connected!");
  }
  else
  {
    Serial.println("Connection failed!");
    // Retry connection after a delay
    delay(1000);
  }
}