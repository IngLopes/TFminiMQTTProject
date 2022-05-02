//This program manages to read the signal strentgh and the distance measured by the tfmini LiDAR sensor and send it by bluetooth to a cellphone, using the app Serial Bluetooth Terminal


//#include <BluetoothSerial.h>
//#include <BTAddress.h>
//#include <BTAdvertisedDevice.h>
//#include <BTScan.h>

//TEST BASIC READING WITH TFMINI LIDAR
#include <TFMini.h>
#include <HardwareSerial.h>

// Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
#define RXD2 18
#define TXD2 19
#define TFMINI_BAUDRATE 115200
TFMini tfmini;
uint16_t dist;
uint16_t strength;
//Check if bluetooth configurations are enabled
//#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
//#error Bluetooth is not enabled! Please run 'make menuconfig' to and enable it
//#endif

//BluetoothSerial SerialBT;
void setup() {
  
  Serial.begin(TFMINI_BAUDRATE,SERIAL_8N1);
//  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("Bluetooth started, ready to pair...");
  Serial.println("\nBenewake TFmini UART LiDAR Program");
  Serial.println ("Initializing...");
  Serial1.begin(TFMINI_BAUDRATE, SERIAL_8N1, RXD2, TXD2);
  //Serial2.begin(TFMINI_BAUDRATE,SERIAL_8N1);    

  // Initialize the TF Mini sensor
  tfmini.begin(&Serial1);    
}

void loop() {
  if(Serial1.available()>1){  // GPS
  dist = tfmini.getDistance();
  strength = tfmini.getRecentSignalStrength();
  //Send measurements by bluetooth
  //SerialBT.print(dist);
  //SerialBT.print(", ");
  //SerialBT.println(strength);
  
 // Display the measurement
  Serial.print(dist);
  Serial.print(" cm      sigstr: ");
  Serial.println(strength);
  Serial1.flush();

  }


  // Wait some short time before taking the next measurement
  //delay(25);  
}
