#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include <Ethernet.h>
#include "SystemFont5x7.h"
#include "Arial_black_16.h"
#include <Arduino.h>

// Fire up the DMD library as dmd
#define BELT_ID "TLM-1"
IPAddress ip(192, 168, 69, 170);                   // Static IP address
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA}; // MAC address of your Arduino

#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 2

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

String belt_ids = "";
String belt_id_1 = "";
String belt_id_2 = "";
String count = "0";
String bag_limit = "0";
String vehicle_desctiption = "";
bool is_screen_value_updated = false;
bool is_screen_cleared = false;
int serverPort = 1234;                             // Port number of the server

IPAddress serverIP(192, 168, 69, 150);             // IP address of the server
IPAddress gateway(192, 168, 69, 1);                // Gateway IP address
IPAddress subnet(255, 255, 255, 0);                // Subnet mask

EthernetClient client;

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void ScanDMD()
{
  dmd.scanDisplayBySPI();
}

void setup()
{
  Serial.begin(9600);

  // Initialize Ethernet library
  // Ethernet.begin(mac);
  Ethernet.begin(mac, ip, gateway, gateway, subnet);

  // Wait for Ethernet to be initialized
  delay(1000);

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());

  Serial.println("Connecting to server...");
  // Connect to the server
  connectToServer();

  // initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
  Timer1.initialize(5000);         // period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
  Timer1.attachInterrupt(ScanDMD); // attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

  // clear/init the DMD pixels held in RAM
  dmd.clearScreen(true); // true is normal (all pixels off), false is negative (all pixels on)

  dmd.drawBox(0, 0, (32 * DISPLAYS_ACROSS) - 1, (16 * DISPLAYS_DOWN) - 1, GRAPHICS_NORMAL);
  dmd.selectFont(SystemFont5x7);
}

void loop()
{
  if (is_screen_value_updated)
  {
    dmd.selectFont(SystemFont5x7);
    String str = count + "/" + bag_limit;
    int slen = str.length() + 1;
    char b[slen];
    str.toCharArray(b, slen);
    dmd.drawString(5, 2, b, slen, GRAPHICS_NORMAL);
    int slen2 = vehicle_desctiption.length() + 1;
    char b2[slen2];
    vehicle_desctiption.toCharArray(b2, slen2);
    dmd.drawString(2, 1 + 16, b2, slen2, GRAPHICS_NORMAL);
    is_screen_value_updated = false;
  }
  if(is_screen_cleared){
    dmd.clearScreen(true); // true is normal (all pixels off), false is negative (all pixels on)

    dmd.drawBox(0, 0, (32 * DISPLAYS_ACROSS) - 1, (16 * DISPLAYS_DOWN) - 1, GRAPHICS_NORMAL);
    dmd.selectFont(SystemFont5x7);
    is_screen_cleared = false;
  }

  if (client.connected())
  {
    // Client is connected
    if (client.available())
    {
      // Read the data from the server until a newline character is encountered
      String receivedData = client.readStringUntil('.');
      // Convert the string to a character array
      char charArray[receivedData.length() + 1]; // +1 for null terminator
      receivedData.toCharArray(charArray, sizeof(charArray));
      char delimiter = '/';
      // Split the string
      char *token = strtok(charArray, &delimiter);
      while (token != NULL)
      {
        if (token[0] == 'b')
        {
          belt_ids = String(token).substring(2);
          belt_ids.toUpperCase();
          int index_of_delimiter = belt_ids.indexOf(',');
          belt_id_1=belt_ids.substring(0, index_of_delimiter);
          belt_id_2=belt_ids.substring(index_of_delimiter+1);
          Serial.println(belt_id_1);
          Serial.println(belt_id_2);
          if(belt_id_1!=BELT_ID && belt_id_2!=BELT_ID){
            Serial.println("Not my belt");
            break;
          }
        }
        if (token[0] == 'c' && String(token).substring(0)=="clear"){
          is_screen_cleared = true;
          Serial.print("Screen to be cleared--");
          Serial.print(belt_id_1);
          Serial.println(belt_id_2);
          break;
        }
        if (token[0] == 'c')
        {
          is_screen_value_updated = true;
          count = String(token).substring(2);
        }

        else if (token[0] == 'l')
        {
          is_screen_value_updated = true;
          bag_limit = String(token).substring(2);
        }

        else
        {
          is_screen_value_updated = true;
          vehicle_desctiption = String(token).substring(2);
          vehicle_desctiption.toUpperCase();
        }

        // Get the next token
        token = strtok(NULL, &delimiter);
      }
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
  delay(100);
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
