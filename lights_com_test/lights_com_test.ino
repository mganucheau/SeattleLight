/* Test communication for the LPD8806 RGB strips over serial

 Z - Clears the strip 
 H - Color Chase Mode
 W - Color Wipe Mode
 C - Rainbow Mode
 T - Tinkle Mode
 O - Solid ColorMode

 U - Fades brightness up
 D - Fades brightness down

 R0 - R9  - sets Red Value
 G0 - G9  - sets Green Value
 B0 - B9  - sets Blue Value
 P0 - P9  - sets Speed
 Z0 - Z9  - sets Brightness

 S0 - All Sides
 S1 - First Side
 S2 - Second Side
 S3 - Third Side
 S4 - Last Side

 N1 - First Corner
 N2 - Second Corner
 N3 - Third Corner
 N4 - Last Corner
 
 */

#include "LPD8806.h"
#include "SPI.h"

int totalLEDs = 75;
int dataPin   = 11;
int clockPin  = 13;

char cmd;
char ch;
int num;
int effectCounter;
LPD8806 strip       = LPD8806(totalLEDs,dataPin,clockPin); // (numLeds, dataPin, clockPin)
int twinkIndex      = 0;
const int numTwinks = 40;
int rndPx[numTwinks];
int pxVal[numTwinks];
int R = 0;
int G = 0;
int B = 100;
int lastSend = 0;
int lastRec  = 0;
int beginning = 0; 
int ending = totalLEDs;
int sPeed = 4;
int tmpclock = 1;
float brightness = 1;
int dir = 0.5;

void setup()
{
  Serial.begin(9600);
  strip.begin();
}

void loop() 
{
  tmpclock++; 
  if (tmpclock % sPeed == 0)
    effectCounter++;

  strip.show();
  serialFilter();

  if (dir == 1) {
     brightness += 0.001;
        if (brightness > 1)
          dir = 0;
  } else if (dir == -1) {
    brightness -= 0.001;
        if (brightness < 0)
          dir = 0;
  }

  switch(cmd)
  {
  case 'Z':
    clearStrip(); 
    break; 
  case 'C':
    cycle();
    break; 
  case 'H':
    chase();
    break; 
  case 'T':
    twinkle();
    break;
  case 'Q':
    setPixelSpan();
    break;
  case 'O':
    solidColor();  
    break;  
  case 'W':
    colorWipe();
    break; 
  case 'F':
    cCycle();
    break;   
  }

  // soloscreensaver();
}

//--------------------------------------------------------------
//void soloscreensaver()
//{
//}

void serialFilter()
{ 
  if (Serial.available() > 0) 
    ch = Serial.read();

  if (ch == 'Z' || 
    ch == 'C' || 
    ch == 'H' || 
    ch == 'T' || 
    ch == 'Q' || 
    ch == 'O' ||
    ch == 'W' ||
    ch == 'F')
    cmd = ch;

  if (ch == 'U')
    dir = 1;
  
  if (ch == 'D')
    dir = -1;

  if (ch == 'R'){
    R = Serial.read();   
    if (R >= '0' && R <= '9')
      R = map(R, '0', '9', 0, 255);
  }     

  if (ch == 'G'){
    G = Serial.read();   
    if (G >= '0' && G <= '9')
      G = map(G, '0', '9', 0, 255);
  }

  if (ch == 'B'){
    B = Serial.read();   
    if (B >= '0' && B <= '9')
      B = map(B, '0', '9', 0, 255);
  }

  if (ch == 'Z'){
    int Z = Serial.read();   
    if (Z >= '0' && Z <= '9')
      brightness = map(Z, '0', '9', 0, 10);
    brightness *= .1;  
  } 

  if (ch == 'S'){
    int S = Serial.read();   
    if (S == '0'){
      beginning = 0;  
      ending = totalLEDs;
    }  //all leds
    if (S == '1'){
      beginning = 0;  
      ending = 54;
    } 
    if (S == '2'){
      beginning = 55; 
      ending = 109;
    } 
    if (S == '3'){
      beginning = 110; 
      ending = 164;
    }  
    if (S == '4'){
      beginning = 165; 
      ending = 219;
    } 
  }
  
   if (ch == 'N'){
    int N = Serial.read();   
    if (N == '1'){
      beginning = 27; 
      ending = 54;
    } 
    if (N == '2'){
      beginning = 55; 
      ending = 82;;
    } 
    if (N == '3'){
      beginning = 83; 
      ending = 110;
    }  
    if (N == '4'){
      beginning = 165; 
      ending = 192;
    } 
  }
  
  

  if (ch == 'P'){
    sPeed = map(sPeed, '0', '9', 15 , 2);    
  }  

}

char convertIdToLetter(int id) {
  char result = id + 65;
  return result;
}

//--------------------------------------------------------------
// Strip Functions

void clearStrip()
{
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i,0);
  }
}

void solidColor()
{
  int r, g, b;
  r = R * brightness;
  g = G * brightness;
  b = B * brightness;
  
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i,r,g,b);
  }
}

void setPixelSpan() 
{
  int r, g, b;
  r = R * brightness;
  g = G * brightness;
  b = B * brightness;
 
  for(int i = beginning; i<=ending; i++) {
    strip.setPixelColor(i, strip.Color(r,g,b));
  }
}

void cycle() 
{
  uint16_t i;
  for(int i = beginning; i<=ending; i++) {
    strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + effectCounter) % 384) );
  }
}

void chase() 
{
  int r, g, b;
  r = R * brightness;
  g = G * brightness;
  b = B * brightness;
   
  for(int i = beginning; i<=ending; i++) {
    strip.setPixelColor(i,0);
  }

  int x = effectCounter%ending;
  strip.setPixelColor(x,   strip.Color(r,g,b));
  strip.setPixelColor(x+1, strip.Color(r,g,b));
  strip.setPixelColor(x+2, strip.Color(r,g,b));
  strip.setPixelColor(x+3, strip.Color(r,g,b));
}


// Needs work
void twinkle() 
{ 
  ///uint32_t c
  int tic = effectCounter%numTwinks;
  if(tic < 1){ 
    twinkIndex++; 
    if (twinkIndex > numTwinks){
      twinkIndex=0;
    }
    strip.setPixelColor(rndPx[twinkIndex], strip.Color(0,0,0));
    rndPx[twinkIndex]=random(beginning,ending); //new LED
    pxVal[twinkIndex]=random(50,150);
  }
  for(int i=0;i<10; i++){
    int val= pxVal[i];
    if (val < 100){
      val++;
    } else {
      val=0;
    }
    pxVal[i]=val; //take each one through its cycle and update it.
    int color=val;
    
    if(color > 64){
      color = 65 - color % 64;
    } 
    
    color += 5;
    if(color < 0){
      color = 0;
    }
    
    color = color * brightness;
    
    strip.setPixelColor(rndPx[i], strip.Color(color,color,0));
  }
}

void colorWipe() {
  int r, g, b;
  r = R * brightness;
  g = G * brightness;
  b = B * brightness;
 
  uint32_t c = 67;
  uint16_t i;
  int numPx = abs(beginning - ending);
  int x;
  if(ending > beginning){
    x = (effectCounter) %numPx + beginning;
  }
  if(ending < beginning){
    x = beginning - (effectCounter) %numPx;
  }
  strip.setPixelColor(x,strip.Color(r,g,b));
}

/* Helper functions */
//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
  case 0:
    r = 127 - WheelPos % 128;   //Red down
    g = WheelPos % 128;      // Green up
    b = 0;                  //blue off
    break; 
  case 1:
    g = 127 - WheelPos % 128;  //green down
    b = WheelPos % 128;      //blue up
    r = 0;                  //red off
    break; 
  case 2:
    b = 127 - WheelPos % 128;  //blue down 
    r = WheelPos % 128;      //red up
    g = 0;                  //green off
    break; 
  }
  
  r = r*brightness;
  g = g*brightness;
  b = b*brightness;
  
  return(strip.Color(r,g,b));
}

void cCycle() {
  uint16_t i;
  for(int i = beginning; i<=ending; i++) {
    strip.setPixelColor(i, Wheel( ((i * 384 / ending) + effectCounter) % 100)+164 );
  }
}



