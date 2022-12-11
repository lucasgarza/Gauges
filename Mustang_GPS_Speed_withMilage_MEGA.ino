#include <TimerOne.h>
#include <Adafruit_GPS.h>
#include "U8glib.h"
//#include <Wire.h>
#include <TinyGPS++.h>
#include "SwitecX25.h"
#include <EEPROM.h>

#include <SoftwareSerial.h>
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);

//const int UTC_offset = -6;   // Central Time

//GPS stuff
int gpsFlag;
//long tmr;
String NMEA = "";
char c2;
char c;
String GPRMC_ONLY = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
String UPDATE_10_sec = "$PMTK220,10000*2F\r\n";
char speed[3];
char dist[8];
char trip[6];
char cnt[3];
//char satsBuf[16];
char satsBuf1[3];
char satsBuf2[3];
//float speed1;
int speed_round;
//int count =0;
long TotalDistance;
long TripCounter;
float MileCounter;
float flon, flat,holdlat, holdlon;
//int speed_test;
char* spinner = "/-\\|";
byte screenRefreshSpinnerPos = 0;
byte gpsUpdateSpinnerPos = 0;
//String heading ="";
//const start_lat, start_lon,flat, flon;
//bool flipflop = false;
int buttonState = 0;
int lastButtonState =0;
int buttonCounter;
static unsigned long stateChangeTime =0;
int resetPin = 12;


// The TinyGPS++ object
TinyGPSPlus gps;


SoftwareSerial GPSSerial(10, 11);
//#define GPSSerial Serial;

SwitecX25 motor1(945,4,5,6,7);

void draw()
{
  u8g.setFont(u8g_font_courB24);
  u8g.drawStr( 1, 45, "Tech!");
}

void gaugeSpeed() {
  sprintf(satsBuf1, "%c", spinner[screenRefreshSpinnerPos]);
  sprintf(satsBuf2, "%d", gps.satellites.value());
  u8g.setFont(u8g_font_courB24);

   u8g.drawStr( 25, 45, speed);  
 
  u8g.setFont(u8g_font_fur11);
  u8g.drawStr( 65, 20, dist);
  u8g.drawStr( 5, 20, trip);
  u8g.drawStr( 85, 35, satsBuf1);
  u8g.drawStr( 105, 45, satsBuf2);

}


// ------------------------------------------------- void setup() ------------------------------------------
void setup(void) {
  digitalWrite(resetPin, HIGH);
  delay(200);
  pinMode(resetPin, OUTPUT);
  pinMode(2, INPUT);
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  //GPS.begin(9600);
  GPSSerial.begin(9600);
  delay(100);
  GPSSerial.print(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  delay(100);
  GPSSerial.print(PMTK_SET_NMEA_UPDATE_1HZ);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(readGPS);
  holdlat = 0;
  //new engine at 103707
//  EEPROM.put(0,108000);
  
  EEPROM.get(0,TotalDistance);
 // TotalDistance =108000;
 
// EEPROM.put(sizeof(TotalDistance),55);
 EEPROM.get(sizeof(TotalDistance), TripCounter);
// TripCounter = 0.0;
//  Wire.begin();
  u8g.firstPage();
  do {
    // draw splash page
    draw();
  } while (u8g.nextPage());
    motor1.zero();
    motor1.setPosition(850);
    motor1.updateBlocking();
    delay(1000);
    motor1.zero();
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
    speed_round = round(gps.speed.mph());
    //speed_round = 55; 
    itoa(speed_round, speed, 10);
  }
  else
  {  
    //speed_round = 45;
    //itoa(speed_round, speed, 10);
    // Serial.print(F("INVALID"));
  }
  // Serial.println();
}

void loop(void) {
   motor1.updateBlocking();
   buttonState = digitalRead(2);
   
   if (buttonState != lastButtonState) {
    stateChangeTime = millis();
    if(buttonState == HIGH){
       // reset TripCopunter:
      TripCounter =0.0;
      //   EEPROM.put(sizeof(TotalDistance), 0);
    }else{
      buttonCounter ++;
      Serial.println("Button count: ");
      Serial.println(buttonCounter);
    }
     lastButtonState = buttonState;
    }else //button has not changed
    {
      if (buttonState==HIGH && millis() - stateChangeTime > 3000)
      {
        //Reset everything
         Serial.println("HARD RESET");
        // Serial.println(millis() -stateChangeTime);
        digitalWrite(resetPin, LOW);
      }
    }
 //  Serial.println(TripCounter); 
  if (gpsFlag == 1) {
    Timer1.stop();
    char *text = NMEA.c_str();
    while (*text) {
      if (gps.encode(*text++)) {
        displayInfo();
      }
    }
  //  NMEA.trim();
  //  Serial.println(NMEA);
    NMEA = "";
    gpsFlag = 0;
    // gpsUpdateSpinnerPos = (gpsUpdateSpinnerPos + 1) % 4;
    Timer1.restart();
  }
  //sprintf(heading, "%s",  gps.cardinal(gps.course.value()));

  u8g.firstPage();
  do {
   
   if ((holdlat != gps.location.lat())|| (holdlon != gps.location.lng())){
      flat= gps.location.lat();
      flon= gps.location.lng();
      

      // float distance = (unsigned long)TinyGPSPlus::distanceBetween(flat, flon, LONDON_LAT, LONDON_LON)/1000;
      if (holdlat !=0){
          float distance = (float)TinyGPSPlus::distanceBetween(flat, flon, holdlat, holdlon)/1000;
          //float distance = (float)TinyGPSPlus::distanceBetween(32.87, -97.16, 32.8715, -97.16)/1000;
        //  Serial.println("CalculatedDist");  
        //  Serial.println(distance,5);
          
          if(distance >0.005){
            Serial.println(distance,5);
              
            //  TripCounter = TripCounter +distance*0.62;
              MileCounter = MileCounter  +distance*0.62;
              Serial.println("C"); 
              Serial.println(MileCounter,5);
              if(MileCounter >= 1.0){
                 TotalDistance = TotalDistance +MileCounter;
                 TripCounter = TripCounter +MileCounter;
                 MileCounter = 0;
                 EEPROM.put(0, TotalDistance);
                 EEPROM.put(sizeof(TotalDistance), TripCounter);
                 Serial.println(TripCounter);  
                 Serial.println("T"); 
                 Serial.println(TotalDistance); 
                 
              }
               
            
              holdlat=flat;
              holdlon=flon;   
             
          }
          
        //  Serial.println(holdlat);     
          
      }else{
            holdlat= gps.location.lat();
            holdlon= gps.location.lng();
          }
//      start_lat= holdlat;
//      start_lon= holdlon;
      
   //   Serial.println(flat,5);
   //   Serial.println(flon,5);
     
   }
   dtostrf(TotalDistance, 6, 0,dist);
   dtostrf(TripCounter, 5, 0,trip);
  // dtostrf(MileCounter, 2,2,cnt);
//  PString(dist, 6,TotalDistance, 1);
   //   itoa(TotalDistance, dist, 10);
   //   itoa(TripCounter, trip, 10);
  
     // Serial.println(flat);
     // Serial.println(holdlat);
      
    //  Serial.println(start_lon);
   
   
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
