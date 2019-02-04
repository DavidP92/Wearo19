//Written By David Paez | January 26, 2019 | The purpose was a wearable garment with environmental and health sensor for agent convience.
//But also to have some visual effects with Adafruit Neopixel Rings

#include <WiFi.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"


MAX30105 pulseSensor;

// Replace with your network credentials
const char network[] = "E_Hoodie";
const char key[] = "goldenGoose";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

//Variables for Heart Rate
const byte RATE_AMT = 4;  // Increase for Average Amount
byte rates[RATE_AMT];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMin;
int beatAvg;

//Variable for Infrared
long IR;

//Variables for agentCheck
long samples = 0;         // Counter for Caluculating the Hz or Reading Rate
long block;               //Average IR at power up
long startTime;           //Used to calculate measurement rate
long delta;

//Setup Variables for Particle Sensor
byte ledBrightness = 0xFF;   //Options: 0=Offto 255=50mA
byte sampleAverage = 4;      //Options: 1,2,4,8,16,32
byte ledMode = 2;            //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
int sampleRate = 400;        //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
int pulseWidth = 411;        //Options: 69, 118, 215, 411
int adcRange = 2048;         //Options: 2048, 4096, 8192, 16384

//Timer Variables
unsigned long timer_1;  // Timer -> agentCheck
unsigned long timer_2;  // Timer -> heartCheck
unsigned long timer_3;   // Timer -> temperatureCheck

uint8_t delay_1 = 15;   // agentCheck
uint8_t delay_2 = 50;    // heartCheck
uint8_t delay_3 = 200;   // temperatureCheck

//Assign Output Variables
String outputState_1 = "off";
String outputState_2 = "off";

void healthCheck(){
  IR = pulseSensor.getIR();
  if (checkForBeat(IR) == true)
  {
    delta = millis() - block;
    block = millis();
    beatsPerMin = 60 / (delta / 1000.0);

    if (beatsPerMin < 255 && beatsPerMin > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMin;
      rateSpot %= RATE_AMT;

      beatAvg = 0;
      for (byte x = 0; x < RATE_AMT; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_AMT;
    }
  }
  Serial.print("IR Value  ");
  Serial.print("Beats Per Minute   ");
  Serial.println("Beats Average     ");
  Serial.print(IR);
  Serial.print("      ");
  Serial.print(beatsPerMin);
  Serial.print("      ");
  Serial.println(beatAvg);
  /*
    if (IR < 50000)
      Serial.print("No Finger Present.....");
    Serial.println();
  */
}
void environmentCheck(){
  String tempC;
  String tempF;
  float temperatureC;
  float temperatureF;

  temperatureC = pulseSensor.readTemperature();
  temperatureF = pulseSensor.readTemperatureF();

  Serial.print("Temperature (c):  ");
  Serial.print(temperatureC, 4);
  Serial.print("          ");
  Serial.print("Temperature (F): ");
  Serial.println(temperatureF, 4);
  tempF = temperatureF;
  tempC = temperatureC;
}
void pulseSetup() {
  if (!pulseSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("Pulse Sensor not detected.... Check wiring or Connection...");
    while (1);
  }
  else
  Serial.println("Pressure on sensor may be needed....");

  pulseSensor.setup();                       //Configure sensor with default settings
  pulseSensor.setPulseAmplitudeRed(0x0A);    //Turn Red LED to low to indicate sensor is running
  pulseSensor.setPulseAmplitudeGreen(0);     //Turn off Green LED
}

void agentCheck() {
  IR = pulseSensor.getIR();
  if (IR > 10000) {
    Serial.println("Agent Detected!");
  }
  else
    Serial.println("Still Attempting to locate Agent Presence.......");
} 

void setup() {
 // pulseSetup();
  Serial.begin(115200);
  Serial.println("Starting Setup........!");
  WiFi.mode(WIFI_AP);
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(network, key);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  //pulseSetup();
  timer_1 = millis();
  timer_2 = timer_1;
  timer_3 = timer_2;

  server.begin();
  Serial.println("Setup Completed.......");
}

void loop() {
  unsigned long now = millis();
  WiFiClient client = server.available();   // Listen for incoming clients
  Serial.println("Starting Loop.......");
  if(client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html {background:radial-gradient(circle, rgba(242,236,10,1) 0%, rgba(121,9,24,1) 35%, rgba(0,255,162,1) 100%); font-family: Serif; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background:linear-gradient(to bottom, #ff0000 0%, #666699 100%); border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: grab;}");
            client.println(".button2 {background:linear-gradient(to bottom, #ff0000 0%, #666699 100%)}</style></head>");
            // Web Page Heading
            client.println("<body><h1>  E-Hoodie User Interface   </h1>");
            client.println("<h2> Developed By:David Paez </h2>");

            // Display current state, and ON/OFF buttons for GPIO 26
            client.println("<p>Environmental Reading state - State " + outputState_1 + "</p>");
            // If the output26State is off, it displays the ON button
            if (outputState_1 == "off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            }
            else {
              client.println("<p><a href=\"/26/off\"><button class=\"button\">OFF</button></a></p>");
            }
               
            // If the output27State is off, it displays the ON button
            client.println("<p>Agent Health Sensor State:  " + outputState_2 + "</p>");
            if (outputState_2 == "off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button\">OFF</button></a></p>");
            }
      //Edit Sensors Below this line:
            if (header.indexOf("GET /26/on") >= 0) {
              outputState_1= "on";
                if(now-timer_3 > delay_3){
                  environmentCheck();
                  timer_3 = now;                  }
            }
            else if (header.indexOf("GET /26/off") >= 0) {
              outputState_2 = "off";
            } 
            else if (header.indexOf("GET /27/on") >= 0) {
              outputState_2 = "on";
              if(now - timer_2 > delay_2){
                healthCheck();
                timer_2 = now;}
            } 
            else if (header.indexOf("GET /27/off") >= 0) {
              outputState_2 = "off";
            }
      // Display current state, and ON/OFF buttons for GPIO 27
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

