#include <WiFi.h>
//TEST BASIC READING WITH TFMINI LIDAR
#include <TFMini.h>
//#include <HardwareSerial.h>
#include <PubSubClient.h>
//Bibliotecas necessárias para a leitura do GPS
#include <TinyGPS++.h>
#include <TinyGPSPlus.h>
//#include <SoftwareSerial.h>   //espSoftwareSerial

// Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
#define RXD2 18
#define TXD2 19
#define TFMINI_BAUDRATE 115200
TFMini tfmini;
uint16_t dist;
uint16_t strength;
uint16_t Lat;
uint16_t Lng;
uint16_t Spd;
String s_lat, s_lng, s_spd;
String s_dist, s_strength;
String url, url_A;
//Para conectar a rede movel celular
const char*ssid = "Ing_LM"; 
const char* password = "eqtc1876"; 
const char* host = "192.168.43.161"; // Servidor (Apache)
//Para conectar a wifi diogo
//const char*ssid = "MEO-B88119"; 
//const char* password = "85CB3E2C93"; 
//const char* host = "192.168.1.164"; // Servidor (Apache)
//Para conectar a wifi
//const char*ssid = "NOS-44D6"; 
//const char* password = "GZPWSAE9"; 
//const char* host = "192.168.1.44"; // Servidor (Apache)
WiFiClient TCP_client; // WiFiClient, permite enviar ou receber dados por TCP/IP
PubSubClient client(TCP_client);  //objecto do tipo cliente MQTT

//Configuring the serial pins and baudrate
static const int TXGPS = 17, RXGPS = 16; //Na placa chamam-se Tx e Rx
static const uint32_t GPSBaud = 9600;
// The TinyGPSPlus object
TinyGPSPlus gps;
//The serial connection to the gps device
//SoftwareSerial ss(RXPin, TXPin);

void setup() {
  
  Serial.begin(TFMINI_BAUDRATE,SERIAL_8N1);
  Serial.println("\nBenewake TFmini UART LiDAR Program");
  Serial.println ("Initializing...");
  Serial1.begin(TFMINI_BAUDRATE, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(GPSBaud, SERIAL_8N1, RXGPS, TXGPS);
 // ss.begin(GPSBaud); //initializing the GPS serial
//definir o esp para se conectar 
  WiFi.mode(WIFI_STA);
//Stabilishing wifi connection:
  Serial.print("Conectando: "); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   // O ESP envia por porta série, para o PC local esta informaçao:
  Serial.println("");
  Serial.println("O ESP está ligado por WiFi ao Router, o Router deu-lhe o IP");
  
  // IP address
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP()); // Imprimir IP do ESP
  //MQTT
  client.setServer(host,1883);
  client.connect("esp32");
  Serial.print("esp32 conectado ao broker");
  // Initialize the TF Mini sensor
  tfmini.begin(&Serial1);    
  
}

void loop() {
  
  if(!client.connected()){
          client.connect("esp32"); //se nao estiver connectado regista-se no broker
  }
    //coletar os dados
  if((Serial1.available()>1) || (Serial2.available() >1)){ 
    Serial.println("Há ligação com o lidar ou o gps");
    dist = tfmini.getDistance();
    strength = tfmini.getRecentSignalStrength();
    Serial.print("Dist: "); Serial.println(dist);
    Serial.println("Coletados dados do lidar");
    if (gps.encode(Serial2.read())){
      Serial.print("Satelites: ");
      Serial.println(gps.satellites.value()); // Number of satellites in use (u32)
      displayInfo();
    }
  
    //Tentar publicar a dist por MQTT
    
    if((strength < 2000) && (strength > 20)){ //if strength is not between these values the measurement csn't be trusted
         //Passar os valores para strings:
      if (dist<5000){
        s_dist= String(dist);
        s_strength= String(strength);
        Serial.print("Publish dist on MQTT: ");
        Serial.println(dist);
        client.publish("Distance",s_dist.c_str());
        Serial.print("Publish str on MQTT: ");
        Serial.println(strength);
        client.publish("Strength",s_strength.c_str());
        Serial.print("Publish latitude on MQTT: ");
        Serial.println(Lat, 6);
        client.publish("Latitude",s_lat.c_str());
        //read and publish longitude
        Serial.print("Publish longitude on MQTT: ");
        Serial.println(Lng, 6);
        client.publish("Longitude",s_lng.c_str());
        Serial.print("Publish speed on MQTT: ");
        Serial.println(Spd, 6);
        client.publish("Speed",s_spd.c_str()); 
      }
      else{
        Serial.print("Values not valid by the lidar");  
      }
    }
    Serial1.flush(); 
    Serial2.flush();
  
  }
  // Wait some short time before taking the next measurement
  
  client.loop();
  delay(50);  
}                                                                                                                                
//Declaring functions used in the sketch

void displayInfo(){
  Serial.print("Satelites: ");
  Serial.println(gps.satellites.value()); // Number of satellites in use (u32)
  Serial.print("Location: ");
  if (gps.location.isValid()){
    Lat=gps.location.lat();  //read the latitude value and save it to variable Lat
    Lng=gps.location.lng();
    s_lat=String(Lat);
    s_lng=String(Lng);
    Serial.print(Lat);
    Serial.println(Lng);
//    Serial.print("Publish latitude on MQTT: ");
//    Serial.print(Lat, 6);
//    client.publish("Latitude",s_lat.c_str());
//    //read and publish longitude
//    Serial.print("Publish longitude on MQTT: ");
//    Serial.print(Lng, 6);
//    client.publish("Longitude",s_lng.c_str());
    
    }
  else {
    Serial.print("INVALID");
    }
  Serial.print(" Speed: ");
  if (gps.speed.isValid()){
    Spd=gps.speed.kmph(); //read and save the speed in km per hour
    s_spd=String(Spd);
    Serial.println(Spd);
//    Serial.print("Publish speed on MQTT: ");
//    Serial.print(Spd, 6);
//    client.publish("Speed",s_spd.c_str());
    } 
  else{
    Serial.print("INVALID");}   
//  Serial.print(" Date/Time ");
//  if (gps.date.isValid()){
//    Serial.print(gps.date.month());
//    Serial.print("/");
//    Serial.print(gps.date.day());
//    Serial.print("/");
//    Serial.print(gps.date.year());
//    }
//  else{
//    Serial.print("INVALID");
//    }
//
//  Serial.print(" ");
//  if (gps.time.isValid()){
//    if (gps.time.hour() <10) Serial.print("0");
//    Serial.print(gps.time.hour());
//    Serial.print(":");
//    if (gps.time.minute() <10) Serial.print("0");
//    Serial.print(gps.time.minute());
//    Serial.print(":");
//    if (gps.time.second() <10) Serial.print("0");
//    Serial.print(gps.time.second());
//    Serial.print(":");
//    if (gps.time.centisecond() <10) Serial.print("0");
//    Serial.print(gps.time.centisecond());
//    Serial.print(":");
//    }
//  else{
//    Serial.print("INVALID");
//    }
  Serial.println();
  
  }
