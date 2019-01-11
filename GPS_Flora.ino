#include <Adafruit_GPS.h>

#define GPS_Serial Serial
#define GPSECHO false

Adafruit_GPS GPS(&GPS_Serial);

uint8_t delay_1 = 50;
unsigned long timer_1;

void setup(){
  Serial.begin(115200);
  gpsSetup();
  timer_1 = millis();
}
void loop(){
  const long now = millis();
  if(timer_1 - now > delay_1){
    locationCheck();
  }
}
void gpsSetup(){
  Serial.println("Initializing GPS Setup.....");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
  delay(500);

  GPS_Serial.println(PMTK_Q_RELEASE);
  Serial.println("GPS Initializing is Completed.....");
}
void locationCheck(){
  char loc = GPS.read();

  if (GPSECHO){
    if (loc){
      Serial.print("[1]");
      Serial.println(loc);
    }
  }
  if (GPS.newNMEAreceived()){
    Serial.print("[2]");
    Serial.println(GPS.lastNMEA());
    if(!GPS.parse(GPS.lastNMEA()))
    {
      return;
    }
  }
  Serial.print("Time: ");
  Serial.print(GPS.hour, DEC);
  Serial.print(":");
  Serial.print(GPS.minute,DEC);
  Serial.print(":");
  Serial.print(GPS.seconds, DEC);
  Serial.print(".");
  Serial.println(GPS.milliseconds);
  Serial.print("Date: ");
  Serial.print(GPS.day,DEC);
  Serial.print('/');
  Serial.print(GPS.month, DEC);
  Serial.print("/20");
  Serial.println(GPS.year,DEC);
  Serial.print("Fix:  ");
  Serial.println((int)GPS.fix);
  Serial.print("Quality:  ");
  Serial.println((int)GPS.fixquality);
  if(GPS.fix){
    Serial.print("Location:  ");
    Serial.print(GPS.latitude,4);
    Serial.print(GPS.lat);
    Serial.print("  |   ");
    Serial.print(GPS.longitude,4);
    Serial.println(GPS.lon);
    Serial.print("Speed (knots):  ");
    Serial.println(GPS.speed);
    Serial.print("Angle:  ");
    Serial.println(GPS.angle);
    Serial.print("Altitude: ");
    Serial.println(GPS.altitude);
    Serial.print("Satellites:   ");
    Serial.println((int)GPS.satellites);
  }
  else
    Serial.print("Attempting to connect.....")
 }
