/*
 * Example code for SeattleLight planter
 * based on WiFlyHQ Example udpclient.ino
 */

#include <WiFlyHQ.h>
#include <SoftwareSerial.h>
#include "LPD8806.h"
#include "SPI.h"
SoftwareSerial wifiSerial(8,9);

int  myID           = 9;

const char mySSID[] = "SeattleLight";
const char* unit[]  = {
  "169.254.1.0", "169.254.1.1", "169.254.1.2", "169.254.1.3",
  "169.254.1.4", "169.254.1.5", "169.254.1.6", "169.254.1.7",
  "169.254.1.8", "169.254.1.9", "169.254.1.10", "169.254.1.11"};
int sensorPins[]  = { 
  2, 3, 4, 5, 6, 7, 10, 12 };
int dataPin   = 11;   
int clockPin  = 13;
int effectCounter   = 0;
int totalLEDs = 219;
int hubTimeOut = 5000; // if nothing from hub in this many milliseconds, soloscreensaver
char cmd;
char ch;
int spanSize[8];
int lastSensorValue[8];
int twinkIndex      = 0;
const int numTwinks = 40;
int num;
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

LPD8806 strip = LPD8806(totalLEDs, dataPin, clockPin);

WiFly wifly;

void setup()
{
  strip.begin();
  char buf[32];

  Serial.begin(9600);
  Serial.println("Starting");
  Serial.print("Free memory: ");
  Serial.println(wifly.getFreeMemory(),DEC);

  wifiSerial.begin(9600);

  if (!wifly.begin(&wifiSerial, &Serial)) {
    Serial.println("Failed to start wifly");
  }

  if (wifly.getFlushTimeout() != 10) {
    Serial.println("Restoring flush timeout to 10msecs");
    wifly.setFlushTimeout(10);
    wifly.save();
    wifly.reboot();
  }

  if (!wifly.isAssociated()) {
    Serial.println("Joining network");
    wifly.setSSID(mySSID);
    if (wifly.join()) {
      Serial.println("Joined wifi network");
    } 
    else {
      Serial.println("Failed to join wifi network");
    }
  } 
  else {
    Serial.println("Already joined network");
  }

  wifly.setIP(unit[myID]);
  wifly.setNetmask("255.255.255.0");
  wifly.setGateway(unit[0]);
  wifly.setIpProtocol(WIFLY_PROTOCOL_UDP);

  Serial.print("MAC: ");
  Serial.println(wifly.getMAC(buf, sizeof(buf)));
  Serial.print("IP: ");
  Serial.println(wifly.getIP(buf, sizeof(buf)));
  Serial.print("Netmask: ");
  Serial.println(wifly.getNetmask(buf, sizeof(buf)));
  Serial.print("Gateway: ");
  Serial.println(wifly.getGateway(buf, sizeof(buf)));

  String s_did = String("Unit " + myID);
  char c_buf[8];
  s_did.toCharArray(c_buf, 8);
  wifly.setDeviceID(c_buf);
  Serial.print("DeviceID: ");
  Serial.println(wifly.getDeviceID(buf, sizeof(buf)));

  // setHost causes a delay, so don't do it in loop
  wifly.setHost(unit[0], 2000);

  Serial.println("WiFly ready");
}

void loop() 
{
  if(diffSensors()) sendSensors();

  if (wifly.available() > 0) {
    cmd = wifly.read();
    Serial.println(cmd);
    lastRec = millis();
  }

  if((millis() - lastRec) > hubTimeOut) {
    cmd = 'M';
  }
  
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
  case 'O':
    solidColor();  
    break;  
  case 'W':
    colorWipe();
    break; 
  case 'F':
    cCycle();
    break;   
  case 'M':
    soloscreensaver();
    break;
  }
  

  tmpclock++; 
  if (tmpclock % sPeed == 0)
    effectCounter++;

  strip.show();
  serialFilter();
 
}

//--------------------------------------------------------------
void serialFilter()
{ 
  if (Serial.available() > 0) 
    ch = Serial.read();

  if (ch == 'Z' || 
    ch == 'C' || 
    ch == 'H' || 
    ch == 'T' || 
    ch == 'O' ||
    ch == 'W' ||
    ch == 'F' ||
	ch == 'M')
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

//--------------------------------------------------------------
void clearStrip()
{
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i,0);
  }
}

void setPixelSpan(int pFirst, int pLast, int r, int g, int b) {
  for(int i = pFirst; i<=pLast; i++) {
    strip.setPixelColor(i, strip.Color(r,g,b));
  }
}

void soloscreensaver() {
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i,0);
  }
  for (int n=0; n<8; n++) {
    if(digitalRead(sensorPins[n])) {
      spanSize[n] = 8;
    }
    int ss = spanSize[n];
    if(ss > 0) {
      int loc = n*(totalLEDs/8) + 9;
      if(n%2 == 1) loc += 9;
      setPixelSpan(loc-ss, loc+ss, 127, 0, 0);
      spanSize[n]--;
    }
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

void cycle() 
{
  uint16_t i;
  for(int i = beginning; i<=ending; i++) {
    strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + effectCounter) % 384) );
  }
}

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

void colorWipe() 
{
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

void cCycle() 
{
  uint16_t i;
  for(int i = beginning; i<=ending; i++) {
    strip.setPixelColor(i, Wheel( ((i * 384 / ending) + effectCounter) % 100)+164 );
  }
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

boolean diffSensors() {
  boolean diff = false;
  for (int n=0; n<8; n++) {
    if(digitalRead(sensorPins[n]) != lastSensorValue[n]) {
      diff = true;
      lastSensorValue[n] = digitalRead(sensorPins[n]);
    }
  }
  return diff;
}


void sendSensors(){
  wifly.print(convertIdToLetter(myID));
  for (int n=0; n<8; n++) {
    wifly.print(lastSensorValue[n]);
  }
}

char convertIdToLetter(int id) {
  char result = id + 65;
  return result;
}









