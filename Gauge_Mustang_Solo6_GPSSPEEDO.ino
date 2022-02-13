#include <TimerOne.h>
#include <Adafruit_GPS.h>
#include "U8glib.h"
//#include "U8g2lib.h"
#include <Wire.h>
#include <TinyGPS++.h>
//#include <Time.h> // Time Library
//#include <TimeLib.h>
#include "SwitecX25.h"

#include <SoftwareSerial.h>
//U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 12, /* dc=*/ 4, /* reset=*/ 6);  // Arduboy (Production, Kickstarter Edition)
//U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0); // SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9, Res = 12
// For OLED display with SH1106 driver. If you use another display,
// then please check the u8glib documentation and website at
// https://github.com/olikraus/u8glib



const int UTC_offset = -6;   // Central Time

//GPS stuff
int gpsFlag;
//long tmr;
String NMEA = "";
char c2;
char c;
String GPRMC_ONLY = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
String UPDATE_10_sec = "$PMTK220,10000*2F\r\n";
char speed[3];
//char satsBuf[16];
char satsBuf1[3];
char satsBuf2[3];
//float speed1;
int speed_round;
int speed_test;
char* spinner = "/-\\|";
byte screenRefreshSpinnerPos = 0;
byte gpsUpdateSpinnerPos = 0;
//String heading ="";


// The TinyGPS++ object
TinyGPSPlus gps;


SoftwareSerial GPSSerial(10, 11);
//#define GPSSerial Serial;

SwitecX25 motor1(945,4,5,6,7);

void draw()
{
  u8g.setFont(u8g_font_courB24);
  u8g.drawStr( 1, 45, "GARZA");
}

void gaugeSpeed() {
  //u8g.setContrast(255);
  //u8g.setScale2x2();
  //sprintf(satsBuf, "%c %c %d", spinner[screenRefreshSpinnerPos], spinner[gpsUpdateSpinnerPos], gps.satellites.value());
  sprintf(satsBuf1, "%c", spinner[screenRefreshSpinnerPos]);
  sprintf(satsBuf2, "%d", gps.satellites.value());
  u8g.setFont(u8g_font_courB24);
  //u8g2.setFont(u8g2_font_ncenB14_tr);

  //u8g.drawStr( 60, 50, speed);
  
 //if (speed_round = 0){
     // int Year = gps.date.year();
    //   int Year = 2021;
     // byte Month = gps.date.month();
    //  byte Month = 8;
      //byte Day = gps.date.day();
     // byte Day = 11;
      //byte Hour = gps.time.hour();
     // byte Hour = 5;
     // byte Minute = gps.time.minute();
     // byte Minute = 17;
      //byte Second = gps.time.second();
   //   byte Second = 25;
    //  String myTime = "Noon";
        // Set Time from GPS data string
    //    setTime(Hour, Minute, Second, Day, Month, Year);
        // Calc current Time Zone time by offset value
    //    adjustTime(UTC_offset * SECS_PER_HOUR);  
   //      itoa(hourFormat12(), myTime, 30);
    //    u8g.drawStr( 25, 45, "test"); 
  
 //}else{
   u8g.drawStr( 25, 45, speed);
 //}
 
  //u8g.drawStr( 25, 45, "99");
  // u8g.setFont(u8g_font_courB10);

  // u8g.drawStr( 0, 20, heading.c_str());
  //u8g.undoScale();
  u8g.setFont(u8g_font_fur11);
  // u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 85, 35, satsBuf1);
  // u8g.setFont(u8g_font_fur11);
  u8g.drawStr( 105, 45, satsBuf2);
  


  //u8g.drawBitmapP( 0, 0, 16, 64, pontiac);
}

// ------------------------------------------------- void setup() ------------------------------------------
void setup(void) {

  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
 // Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  //GPS.begin(9600);
  GPSSerial.begin(9600);
  delay(100);
  GPSSerial.print(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  delay(100);
  GPSSerial.print(PMTK_SET_NMEA_UPDATE_1HZ);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(readGPS);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  // GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  //GPS.sendCommand(PGCMD_ANTENNA);

  //delay(1000);
  // Serial.println(mySerial.println(PMTK_Q_RELEASE));
  // Ask for firmware version
  // mySerial.println(PMTK_Q_RELEASE);

  Wire.begin();

  u8g.firstPage();
  do {
    // draw splash page
    draw();
  } while (u8g.nextPage());

 

    motor1.zero();
    motor1.setPosition(250);
    motor1.updateBlocking();
    motor1.zero();
   //motor1.update();
  // while (motor1.currentStep > 0) {
   // motor1.update();
  //}
    delay(6000);

}


void readGPS() {
  if (GPSSerial.available() > 0) {
    c2 = GPSSerial.read();
    NMEA.concat(c2);
  }
  if (c2 == '\r') {
    gpsFlag = 1;
  }
}

void displayInfo()
{
  if (gps.speed.isUpdated())
  {
    // Serial.print(F("SPEED      Fix Age="));
    //Serial.print(gps.speed.age());
    //Serial.print(F("ms Raw="));
    // Serial.print(gps.speed.value());
    //Serial.print(F(" Knots="));
    //Serial.print(gps.speed.knots());
    //Serial.print(F(" MPH="));
    //Serial.print(gps.speed.mph());
    //Serial.print(F(" m/s="));
    //Serial.print(gps.speed.mps());
    //Serial.print(F(" km/s="));
    //Serial.println(gps.speed.kmph());
   // speed1 = gps.speed.mph();   //Change this to display other speeds( kmph, mph);
    //speed_round = round(speed1);
    
    speed_round = round(gps.speed.mph());
    //speed_round = 55;
    
    //dtostrf(speed_round, 6, 0,speed);
    itoa(speed_round, speed, 10);
   // heading = gps.cardinal(gps.course.value());
   // Serial.println(gps.cardinal(gps.course.value()));
    //sprintf(heading, "%s",  gps.cardinal(gps.course.value()));

    //Serial.println(gps.satellites.value());
    //sprintf(satsBuf, "%c %c %d", spinner[screenRefreshSpinnerPos], spinner[gpsUpdateSpinnerPos], gps.satellites.value());
    //sprintf(speed, "%i", speed1);
    //Serial.println(speed);

  }
  else
  {  
    //speed_round = 55;
    //itoa(speed_round, speed, 10);
    // Serial.print(F("INVALID"));
  }
  // Serial.println();
}

void loop(void) {
  //Serial.println("Joy");
   motor1.updateBlocking();
   
  if (gpsFlag == 1) {
    Timer1.stop();
    char *text = NMEA.c_str();
    while (*text) {
      if (gps.encode(*text++)) {
        displayInfo();
      }
    }
   // NMEA.trim();
    //Serial.println(NMEA);
    NMEA = "";
    gpsFlag = 0;
    // gpsUpdateSpinnerPos = (gpsUpdateSpinnerPos + 1) % 4;
    Timer1.restart();
  }
  //sprintf(heading, "%s",  gps.cardinal(gps.course.value()));

  u8g.firstPage();
  do {
    gaugeSpeed();
    
    if(speed_round < 5){
       motor1.setPosition(speed_round*4);
    }else if(speed_round <15){
      motor1.setPosition(20+((speed_round -5)*6));
    }else{
      motor1.setPosition(80+((speed_round -15)*9));
    }
   
    //Serial.println(speed_round);
  }
  while ( u8g.nextPage() );
  screenRefreshSpinnerPos = (screenRefreshSpinnerPos + 1) % 4;
  //motor1.update();
  motor1.updateBlocking();
  
}
