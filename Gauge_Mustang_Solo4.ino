/*
 Analog gauge v1.02
 - Shows half circle gauge with scale labels
 - Shows digital value, 00 - 100 aligned
 - Shows gauge label
 21/12/2015 - Rudi Imbrechts
 http://arduinows.blogspot.com

*/
#include "U8glib.h"


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0); // SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9, Res = 12
                                                // For OLED display with SH1106 driver. If you use another display,
                                                // then please check the u8glib documentation and website at
                                                // https://github.com/olikraus/u8glib

int xmax=128;                                   // max length x-axis
int ymax=62;                                    // max length y-axis
int xcenter=xmax/2;                             // center of x-axis
int ycenter=ymax/2+10;                          // center of y-axis
int arc=ymax/2;                            
int angle=0;                                  
char* label[] = {"LOAD","COOLANT","INTAKE", "PSI"};    // some custom gauge labels
int labelXpos[] = {53, 45, 49, 0};                     // predefined x-position of a gauge label
#define potmeterPin A1                                  // simulate analogue value with potentiometer
int p, m,a=10;
u8g_uint_t xx=0;
int frame = 0;
char psi [2];
char water[4];
int psi1 =0;
char volts [6];
float w;
int value = 0;
float vout = 0.0;
float vin = 0.0;
float R1 = 100000.0; // resistance of R1 (100K)
float R2 = 10000.0; // resistance of R2 (10K)
float AverageValue = 0;
float AverageValue2 = 0;
int MeasurementsToAverage = 32;


//int ThermistorPin = 0;
int Vo;
float R1a = 10000; // resistance of R1a (10K)
float logR2, R2a, T;
//float c1 = 3.481515861e-03, c2 = -2.206377112e-04, c3 = 35.05773575e-07;
//float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float c1 = 2.571202905e-03, c2 = -0.4402423056e-04, c3 = 9.838653961e-07; // SING F LTD Thermostat https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm



float buffer=0;
float VoutA= 0;



const unsigned char pontiac[] PROGMEM = {0xff,0xff,0xff,0xff,0xff,0xff,0xf8,0x00,0x00,0x1f,0xff,0xff,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xff,0xf0,0x00,0x00,0x00,0x00,0x0f,0xff,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xc0,0x00,0x0f,0xff,0xff,0xf0,0x00,0x03,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xfc,0x00,0x0f,0xff,0xff,0xff,0xff,0xf0,0x00,0x3f,0xff,0xff,0xff
,0xff,0xff,0xff,0xe0,0x03,0xff,0xff,0x00,0x00,0xff,0xff,0xc0,0x07,0xff,0xff,0xff
,0xff,0xff,0xff,0x00,0x3f,0xf8,0x00,0x00,0x00,0x00,0x1f,0xfc,0x00,0xff,0xff,0xff
,0xff,0xff,0xfc,0x03,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xc0,0x3f,0xff,0xff
,0xff,0xff,0xe0,0x1f,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xf8,0x07,0xff,0xff
,0xff,0xff,0x80,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0x01,0xff,0xff
,0xff,0xfe,0x07,0xe0,0x00,0x07,0xfe,0x00,0x00,0x00,0x00,0x00,0x07,0xe0,0x7f,0xff
,0xff,0xfc,0x1f,0x00,0x00,0x3f,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0xf8,0x1f,0xff
,0xff,0xf0,0x7c,0x00,0x00,0xf0,0x00,0xff,0xe0,0x01,0xc0,0x00,0x00,0x3e,0x0f,0xff
,0xff,0xc1,0xf0,0x00,0x01,0xc0,0x00,0x3f,0xff,0xff,0xc0,0x00,0x00,0x07,0x83,0xff
,0xff,0x83,0xc0,0x00,0x03,0x80,0x00,0xc3,0xff,0xff,0x80,0x00,0x00,0x01,0xe1,0xff
,0xff,0x0f,0x00,0x00,0x07,0x00,0x01,0xc0,0xff,0xff,0x00,0x00,0x00,0x00,0xf0,0xff
,0xfe,0x1c,0x00,0x00,0x0e,0x00,0x03,0x80,0x1f,0xfc,0x00,0x00,0x00,0x00,0x3c,0x7f
,0xfc,0x78,0x00,0x00,0x0e,0x00,0x03,0x00,0x00,0xe0,0x00,0x00,0x00,0x00,0x0e,0x3f
,0xf8,0xe0,0x00,0x00,0x0e,0x04,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x1f
,0xf1,0xc0,0x00,0x00,0x0e,0x04,0x0c,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x8f
,0xe3,0x80,0x00,0x00,0x1e,0x0c,0x1c,0x50,0x00,0x00,0x00,0x00,0x70,0x00,0x01,0xc7
,0xe3,0x00,0x00,0x00,0x0f,0x38,0x18,0x50,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0xe3
,0xc6,0x00,0x00,0x00,0x0f,0xf8,0x30,0x50,0x00,0x00,0x00,0x00,0xe0,0x00,0x00,0x63
,0x8c,0x00,0x00,0x00,0x0f,0xf0,0x70,0xe0,0x00,0x00,0x00,0x01,0xe0,0x00,0x00,0x31
,0x8c,0x00,0x00,0x00,0x07,0xe0,0xff,0xc0,0x00,0x00,0x00,0x01,0xc0,0x00,0x00,0x31
,0x98,0x00,0x00,0x00,0x01,0x07,0xfc,0x20,0x00,0x00,0x00,0x03,0xc0,0x00,0x00,0x19
,0x18,0x00,0x00,0x00,0x00,0x1c,0xc0,0x00,0x00,0x00,0x00,0x03,0x80,0x00,0x00,0x18
,0x18,0x00,0x00,0x00,0x00,0x01,0xc0,0x00,0x00,0x00,0x00,0x07,0x80,0x00,0x00,0x18
,0x18,0x00,0x00,0x00,0x00,0x01,0x80,0x64,0x0f,0x1c,0x03,0xc7,0x00,0x00,0x00,0x0c
,0x18,0x00,0x00,0x00,0xc0,0x03,0x81,0xf6,0x1f,0xbe,0x0f,0xe7,0x00,0x00,0x00,0x0c
,0x18,0x00,0x00,0x01,0x80,0x07,0x03,0xc6,0x3f,0xfe,0x1e,0x6f,0x00,0x00,0x00,0x0c
,0x18,0x00,0x00,0x03,0x00,0x07,0x07,0x06,0x73,0xfe,0x38,0x3e,0x00,0x00,0x00,0x0c
,0x18,0x00,0x00,0x07,0x00,0x0e,0x06,0x02,0x63,0xde,0x78,0x3e,0x00,0x00,0x00,0x18
,0x98,0x00,0x00,0x06,0x00,0x1e,0x0e,0x03,0xc7,0xc0,0x70,0x3c,0x00,0x00,0x00,0x18
,0x9c,0x00,0x00,0x0e,0x00,0x1c,0x0c,0x03,0x87,0x80,0xe0,0x3c,0x00,0x00,0x00,0x19
,0x8c,0x00,0x00,0x0e,0x00,0x3c,0x1c,0x07,0x87,0xc1,0xe0,0x78,0x00,0x00,0x00,0x31
,0xc6,0x00,0x00,0x0e,0x00,0x78,0x18,0x07,0x0e,0xe7,0xc0,0x78,0x00,0x00,0x00,0x73
,0xc7,0x00,0x00,0x0e,0x00,0xf0,0x38,0x06,0x0e,0x7f,0xc0,0xf0,0x40,0x00,0x00,0x63
,0xe3,0x00,0x00,0x0f,0x01,0xf0,0x38,0x0e,0x1e,0x3d,0xc0,0xf0,0xc0,0x00,0x00,0xc7
,0xf1,0x80,0x00,0x0f,0x87,0xe0,0x78,0x0c,0x1c,0x01,0xc1,0xe1,0x80,0x00,0x01,0x87
,0xf0,0xc0,0x00,0x07,0xff,0xc0,0xf8,0x18,0x3c,0x01,0xc3,0xe3,0x00,0x00,0x03,0x0f
,0xf8,0x70,0x00,0x07,0xff,0xc0,0xdc,0x38,0x38,0x01,0xef,0xe6,0x00,0x00,0x0e,0x1f
,0xfc,0x38,0x00,0x03,0xff,0x80,0x9f,0xf0,0x78,0x01,0xff,0xfc,0x00,0x00,0x1c,0x3f
,0xfe,0x1e,0x00,0x01,0xfe,0x00,0x0f,0xe0,0x70,0x00,0xfc,0xf8,0x00,0x00,0x78,0x7f
,0xff,0x07,0x00,0x00,0x7c,0x00,0x03,0x80,0x70,0x00,0x20,0x60,0x00,0x00,0xe0,0xff
,0xff,0xc3,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xc3,0xff
,0xff,0xe0,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x07,0xff
,0xff,0xf8,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,0x1f,0xff
,0xff,0xfc,0x0f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf0,0x3f,0xff
,0xff,0xff,0x01,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xc0,0xff,0xff
,0xff,0xff,0xc0,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x03,0xff,0xff
,0xff,0xff,0xf8,0x0f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xf0,0x0f,0xff,0xff
,0xff,0xff,0xfe,0x00,0xff,0xc0,0x00,0x00,0x00,0x00,0x01,0xff,0x00,0x7f,0xff,0xff
,0xff,0xff,0xff,0xc0,0x0f,0xff,0x80,0x00,0x00,0x01,0xff,0xf0,0x03,0xff,0xff,0xff
,0xff,0xff,0xff,0xf8,0x00,0x7f,0xff,0xff,0xff,0xff,0xfe,0x00,0x1f,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xff,0x80,0x00,0x00,0x00,0x00,0x01,0xff,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};



const unsigned char oil[] PROGMEM = { 
0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
,0x00,0x00,0x00,0x00
};

void draw()
{
    u8g.drawBitmapP( 0, 0, 16, 64, pontiac);
}

void gaugePSI() {
  u8g.setFont(u8g_font_courB24);
    
    u8g.drawStr( 0, 48, psi);
    u8g.drawStr( 50, 48, water);
    //u8g.drawBitmapP( 0, 0, 16, 64, pontiac);
    // u8g.setColorIndex(1);
    //u8g.drawStr( 0, 40, "Oil");
   //u8g.drawBitmapP( 0, 35, 6, 8, oil);

   
}

// ------------------------------------------------- void gauge(Volts) ------------------------------------------
void gauge(uint8_t angle) {
  dtostrf(w, 0, 1,volts);
 // u8g.setFont(u8g_font_courB24);
 // u8g.drawStr( 5, 30, "V");
  //u8g.drawStr( 30, 30, "12.5");
 // u8g.drawStr( 30, 30, volts);
  //u8g.drawStr( 85, 35, ".");
  //u8g.drawStr( 100, 35, "5");
  u8g.setFont(u8g_font_courB10);
  if (w < 12){
    u8g.drawStr( 0, 58, "B Low");
  }else{
    u8g.drawStr( 0, 58, "Bat :");
    u8g.drawBox(0,60, w*7, 5);
  }
  u8g.drawStr( 50, 58, volts);
}


// ------------------------------------------------- void setup() ------------------------------------------
void setup(void) {
  Serial.begin(9600);
 
  u8g.firstPage();
  do{
    // draw splash page
   draw();
 } while(u8g.nextPage());
 
 delay(3000);
  
  u8g.setFont(u8g_font_chikita);
  u8g.setColorIndex(1);                         // Instructs the display to draw with a pixel on.

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);                     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);                       // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);                       // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
}

// ------------------------------------------------- void loop() ------------------------------------------

void loop(void) {

  int sensorVal=analogRead(A1);
  //Serial.print("Sensor Value: ");
  //Serial.print(sensorVal);
  Vo=analogRead(A3);










  //buffer= Vo *5; //5volts
  //VoutA = (buffer)/1024.0;
  //buffer = (5/VoutA)-1;
  //R2a = R1a*buffer;
  R2a = R1a * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2a);
  
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  T = (T * 9.0)/ 5.0 + 32.0; 

  Serial.print("Temperature: "); 
  Serial.print(T);
  Serial.println(" F"); 
  //Serial.println("VoutA  value: "); 
  //Serial.println(VoutA);
  Serial.print("Resistance: "); 
  Serial.print(R2a);
  Serial.println(" Ohms"); 


      AverageValue = 0;
     
      for(int i = 0; i < MeasurementsToAverage; ++i)
      {
          AverageValue += analogRead(A1);
          delay(1);
      }
      AverageValue /= MeasurementsToAverage;
      sensorVal = AverageValue;

psi1 = ((sensorVal-102)*100)/818;

if (psi1 < 0){
        psi1=0;
      }


 dtostrf(psi1, 2, 0,psi); 
 dtostrf(T, 3, 0,water); 
 
    //  AverageValue2 = 0;
    //  for(int i = 0; i < MeasurementsToAverage; ++i)
    //  {
    //      AverageValue2 += analogRead(A2);
     //     delay(1);
     // }
     // AverageValue2 /= MeasurementsToAverage;
     // value = AverageValue2;
      
   value = analogRead(A2);

      vout = (value * 5.0) / 1024.0;
      vin = vout / (R2 / (R1 + R2));
      //Serial.print("Sensor Value: ");
      //Serial.println(value);

      //Serial.print("Vout Value: ");
      //Serial.println(vout);

      //Serial.print("Volt Value: ");
      //Serial.println(vin);
      // show needle and dial
     // xx = vin;                                // 135 = zero position, 180 = just before middle, 0 = middle, 45 = max
      w= vin;
  
     
      {
        u8g.firstPage(); 
        do {  
          gaugePSI();           
          gauge(xx);
        }
         while( u8g.nextPage() );
      }
     
 delay(500);
   
}       
