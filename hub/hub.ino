/*
 * Example code for SeattleLight hub
 * based on WiFlyHQ Example udpclient.ino
 */

#include <SoftwareSerial.h>
SoftwareSerial wifiSerial(8,9);

#include <WiFlyHQ.h>
WiFly wifly;

const char mySSID[] = "SeattleLight";
const char* unit[]  = {
  "169.254.1.0", "169.254.1.1", "169.254.1.2", "169.254.1.3",
  "169.254.1.4", "169.254.1.5", "169.254.1.6", "169.254.1.7",
  "169.254.1.8", "169.254.1.9", "169.254.1.10", "169.254.1.11"};

int myID = 0;
int sensorValue[11][8];
int currentSensor = 0;
int currentPlanter = 0;
int lastSensorSum = 0;

void setup()
{
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

  wifly.setDeviceID("Hub");
  Serial.print("DeviceID: ");
  Serial.println(wifly.getDeviceID(buf, sizeof(buf)));

  Serial.println("WiFly ready");
}

uint32_t lastSend = 0;
uint32_t count=0;

void loop()
{
  cycleUnits();
  parseSensorData();

  if(sumSensorValues() != lastSensorSum) {
    lastSensorSum = sumSensorValues();
    Serial.println("");
    Serial.print("total active sensors: ");
    Serial.println( lastSensorSum );
  }
}

int sumSensorValues() {
  int sum = 0;
  for(int p=0; p<11; p++) {
    for(int s=0; s<8; s++) {
      sum += sensorValue[p][s];
    }
  }
  return sum;
}

void parseSensorData() {
  if (wifly.available() > 0) {
    char x = wifly.read();
    if(x > 64) {
      // we got a letter, 'A' or above
      Serial.println("");
      currentPlanter = x-65;
      currentSensor = 0;
    }
    else if (x==48 || x==49) {
      // we got a '0' or '1'
      if(currentPlanter>=0 && currentPlanter <11 && currentSensor >=0 && currentSensor < 8) {
        sensorValue[currentPlanter][currentSensor] = x-48;
      }
      currentSensor++;
    }
    Serial.print(x);
  }
}

// steps through the planters sending a message
void cycleUnits() {
  if ((millis() - lastSend) > 2000) {
    count++;

    wifly.setHost(unit[count], 2000);
    wifly.write("H");

    for (int n=1; n<=11; n++) {
     if (n != count) {
       wifly.setHost(unit[n], 2000);
       wifly.write("Z1"); // set brightness to 1
       wifly.write("T");  // set to twinkle mode                        
       wifly.write("U");  // fade up the brightness
     }  
   }

   if(count > 11)
     count = 1;
     
    lastSend = millis();
  }
}


