#include <SPI.h>
#include <TimerOne.h>
#include <Ethernet.h>
#include <Arduino.h>
#include <Encoder.h>

#define BELT_ID "Belt-1"

IPAddress ip(192, 168, 69, 100);                   // Static IP address
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA}; // MAC address of your Arduino

// Define the pins connected to the encoder
const int encoderPinA = 2;
const int encoderPinB = 3;

int serverPort = 1234;                             // Port number of the server
IPAddress serverIP(192, 168, 69, 150);             // IP address of the server
IPAddress gateway(192, 168, 69, 1);                // Gateway IP address
IPAddress subnet(255, 255, 255, 0);                // Subnet mask

// Create an encoder object
Encoder myEncoder(encoderPinA, encoderPinB);

// Initialize ethernet client
EthernetClient client;

int counter = 0;

void setup() {
  // Initialize Ethernet library
  // Ethernet.begin(mac);
  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  // Wait for Ethernet to be initialized
  delay(1000);

  Serial.println("Connecting to server...");
  // Connect to the server
  connectToServer();
  // Set the encoder pins as inputs
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);

  // Start the serial monitor
  Serial.begin(9600);
}

void loop() {
  if (!client.connected()) {
    // Client is disconnected
    Serial.println();
    Serial.println("Server disconnected!");

    // Attempt to reconnect to the server
    connectToServer();
  }
  // Read the encoder value
  int encoderValue = myEncoder.read();

  if (client.connected()) {
    // Check if the encoder value has changed
    if (encoderValue != counter) {
      // Update the counter
      // Send the counter value to server
      client.print(BELT_ID);
    }
  }
}

void connectToServer() {
  Serial.println("Connecting to server...");
  // Connect to the server
  if (client.connect(serverIP, serverPort)) {
    Serial.println("Connected!");
  } else {
    Serial.println("Connection failed!");
    // Retry connection after a delay
    delay(1000);
  }
}