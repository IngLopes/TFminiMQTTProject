#include <HardwareSerial.h>

//Experimenting with HC-05
//this is just a simple code to see how it works with esp32
#define TX_HC 17
#define RX_HC 16
//#define Key 4
//HardwareSerial SerialHC(2); // RX | TX
#define SerialHC Serial2
void setup() {
  // put your setup code here, to run once:
  

  Serial.begin(115200);
  Serial.println("Enter AT commands:");
  //BTSerial.begin(115200);       // HC-05 has been changed with AT+UART=115200,0,0
  SerialHC.begin(38400, SERIAL_8N1, RX_HC, TX_HC);
  //SerialBT.begin(115200);
}

void loop()
{
  if (SerialHC.available()>0)  {  // read from HC-05 and send to Arduino Serial Monitor
    //Serial.println("receiving message from bluetooth");
    uint8_t r= SerialHC.read();
    Serial.write(r);
    SerialHC.flush();
  }

  if (Serial.available()) {    // Keep reading from Arduino Serial Monitor and send to HC-05
    //Serial.println("serial available");
    char c=Serial.read();
    Serial.println(c);
    SerialHC.write(c);
  }
}

//  Serial.begin(34800);
//  Serial2.begin(34800,SERIAL_8N1, RX_HC, TX_HC);
//  pinMode(Key,OUTPUT);
//  digitalWrite(Key,HIGH);
//}
//
//void loop() {
  // put your main code here, to run repeatedly:
//  if (Serial2.available()){
//    Serial.write(Serial2.read());
//    //Serial.print("recebendo mensagens");
//    //Serial.println(Serial2.read());
//  }
//  if (Serial.available()){
//    Serial2.write(Serial.read());
//    //Serial.println(Serial.read());
  
//   if(Serial.available())
//  {
//    //Serial.println("if serial available");
//    char c = Serial.read();
////
//    Serial.write(c);
//    Serial2.write(c);
//  }
////
//  if(Serial2.available())
//  {
//    char c = Serial.read();
//
//    Serial.println(c);
//    Serial2.write(c);
//  }

//}
