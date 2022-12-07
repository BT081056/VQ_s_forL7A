//----------------------------------------- 0.96" OLED 32*128
#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
//----------------------------------------- 0.96" OLED 32*128
//------------------//PMS1003
#define LENG 32
uint8_t serial_char;
uint8_t check_char;
uint8_t data_buffer[LENG];
int PM01Value = 0;
int PM2_5Value = 0;
int PM10Value = 0;
int Cont_03u_Value = 0;
int Cont_05u_Value = 0;
int Cont_1u_Value = 0;
int Cont_2_5u_Value = 0;
int Cont_5u_Value = 0;
int Cont_10u_Value = 0;
int receiveflag = 0;
int index=0;
int index_1=0;
//--------------------------//PMS1003
//--------------------------//CIM 
const char mac_addr[]="0405623";
const char STX=0x02;
const char ETX=0x03;
const char CR=0x13;
const char LF=0x10;
//--------------------------//CIM 
//********************************************************
void setup(){
  Serial.begin(9600);
  Wire.begin(); //0.96" OLED //DS18S20
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //0.96" OLED
}
//**********************************************
void loop(){

    Particle();
    Serial.print("01");
    Serial.print(",");
    Serial.print(Cont_03u_Value);

    Serial.print(ETX);
    Serial.print(CR);
    Serial.println(LF);

     display.display();
     display.clearDisplay();
     display.setTextSize(1);
     display.setTextColor(WHITE);   
     display.setCursor(0,0);
     display.print("0.3um= "); 
     display.print(Cont_03u_Value);  
     display.setCursor(0,8);
     display.print("0.5um= ");
     display.print(Cont_05u_Value);
     display.setCursor(0,16);
     display.print("0.3um= ");
     display.print(Cont_10u_Value);
     display.setCursor(0,24);
     display.print("0.5um= ");
     display.print(Cont_2_5u_Value);
    Cont_03u_Value = 0;
    Cont_05u_Value = 0;
    Cont_1u_Value = 0;
    Cont_2_5u_Value = 0;
    Cont_5u_Value = 0; 
    Cont_10u_Value = 0;
    delay(500);

}
//************************************************
void Particle(void){
  while (Serial.available() > 0)
  {
    delay(4);
    check_char = Serial.read();
    if (check_char == 0x42)
    {
      data_buffer[0] = check_char;
      delay(4);
      check_char = Serial.read();
      if (check_char == 0x4d)
      {
        data_buffer[1] = check_char;
        delay(4);
        for (int i = 2; i < LENG; i++)
        {
          serial_char = Serial.read();
          data_buffer[i] = serial_char;
          delay(4);
        }
        data_buffer[LENG] = '\0';
      }
      if (data_buffer[0] == 0x42 && data_buffer[1] == 0x4d)
      {
        if (checkValue(data_buffer, LENG))
        {
          PM01Value = transmitPM01(data_buffer);
          PM2_5Value = transmitPM2_5(data_buffer);
          PM10Value = transmitPM10(data_buffer);
          Cont_03u_Value = transmit_03u_cont(data_buffer);
          Cont_05u_Value = transmit_05u_cont(data_buffer);
          Cont_1u_Value = transmit_1u_cont(data_buffer);
          Cont_2_5u_Value = transmit_2_5u_cont(data_buffer);
          Cont_5u_Value = transmit_5u_cont(data_buffer); 
          Cont_10u_Value = transmit_10u_cont(data_buffer);
        }
      }
    }
  }  
}

//////////////////////////////////////////////////////////////////////
char checkValue(uint8_t *thebuf, uint8_t leng)
{
  char receiveflag = 0;
  int receiveSum = 0;
  int data_sun = 0;
  int i = 0;
  for (i = 0; i < leng; i++)
  {
    receiveSum = receiveSum + thebuf[i];
  }
  data_sun = ((thebuf[leng - 2] << 8) + thebuf[leng - 1] + thebuf[leng - 2] + thebuf[leng - 1]);
  if (receiveSum == ((thebuf[leng - 2] << 8) + thebuf[leng - 1] + thebuf[leng - 2] + thebuf[leng - 1]))
  {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}

//-------------PM(1.0)  ug/m3 -------------------------------------------//
int transmitPM01(uint8_t *thebuf)
{
  int PM01Val;
  PM01Val = ((thebuf[4] << 8) + thebuf[5]); //count PM1.0 value of the air detector module
  return PM01Val;
}

//-------------PM(2.5)  ug/m3 -------------------------------------------//
int transmitPM2_5(uint8_t *thebuf)
{
  int PM2_5Val;
  PM2_5Val = ((thebuf[6] << 8) + thebuf[7]); //count PM2.5 value of the air detector module
  return PM2_5Val;
}

//-------------PM(10)  ug/m3 -------------------------------------------//
int transmitPM10(uint8_t *thebuf)
{
  int PM10Val;
  PM10Val = ((thebuf[8] << 8) + thebuf[9]); //count PM10 value of the air detector module
  return PM10Val;
}

//--------- 0.3u Particle cont ---------------------------------------//
int transmit_03u_cont(uint8_t *thebuf)
{
  int cont_03u;
  cont_03u = ((thebuf[16] << 8) + thebuf[17]);
  if (cont_03u < 0) {
    cont_03u = 0;
  }
  return cont_03u;
}

//--------- 0.5u Particle cont ---------------------------------------//
int transmit_05u_cont(uint8_t *thebuf)
{
  int cont_05u;
  cont_05u = ((thebuf[18] << 8) + thebuf[19]);
  if (cont_05u < 0) {
    cont_05u = 0;
  }
  return cont_05u;
}

//--------- 1.0u Particle cont ---------------------------------------//
int transmit_1u_cont(uint8_t *thebuf)
{
  int cont_1u;
  cont_1u = ((thebuf[20] << 8) + thebuf[21]);
  if (cont_1u < 0) {
    cont_1u = 0;
  }
  return cont_1u;
}

//--------- 2.5u Particle cont ---------------------------------------//
int transmit_2_5u_cont(uint8_t *thebuf)
{
  int cont_2_5u;
  cont_2_5u = ((thebuf[22] << 8) + thebuf[23]);
  if (cont_2_5u < 0) {
    cont_2_5u = 0;
  }
  return cont_2_5u;
}
//--------- 5.0u Particle cont ---------------------------------------//
int transmit_5u_cont(uint8_t *thebuf)
{
  int cont_5u;
  cont_5u = ((thebuf[24] << 8) + thebuf[25]);
  if (cont_5u < 0) {
    cont_5u = 0;
  }
  return cont_5u;
}
//--------- 10.0u Particle cont ---------------------------------------//
int transmit_10u_cont(uint8_t *thebuf)
{
  int cont_10u;
  cont_10u = ((thebuf[26] << 8) + thebuf[27]);
  if (cont_10u < 0) {
    cont_10u = 0;
  }
  return cont_10u;
}
//**********************************************************************
