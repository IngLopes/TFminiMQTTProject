#include <WiFi.h>
//TEST BASIC READING WITH TFMINI LIDAR
#include <TFMini.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>

// Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
#define RXD2 18
#define TXD2 19
#define TFMINI_BAUDRATE 115200
TFMini tfmini;
uint16_t dist;
uint16_t strength;
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
void setup() {
  
  Serial.begin(TFMINI_BAUDRATE,SERIAL_8N1);
  Serial.println("\nBenewake TFmini UART LiDAR Program");
  Serial.println ("Initializing...");
  Serial1.begin(TFMINI_BAUDRATE, SERIAL_8N1, RXD2, TXD2);
  
  // Initialize the TF Mini sensor
  tfmini.begin(&Serial1); 
  Serial.println(Serial1.read());
    
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
  //client.loop();
  Serial.print("esp32 conectado ao broker"); 
  
}

void loop() {
  Serial.println("Void loop");
  Serial.println(Serial1.read());
    //coletar os dados
 // if(Serial1.available()>1){ 
  Serial.println("Serial1 available");
  dist = tfmini.getDistance();
  strength = tfmini.getRecentSignalStrength();
  Serial.println(dist);
  //Tentar publicar a dist por MQTT
 
  if((strength < 2000) && (strength > 20)){ //if strength is not between these values the measurement csn't be trusted
       //Passar os valores para strings:
      s_dist= String(dist);
      s_strength= String(strength);
      Serial.print("Publish dist on MQTT: ");
      Serial.println(dist);
      client.publish("Distance",s_dist.c_str());
      Serial.print("Publish str on MQTT: ");
      Serial.println(strength);
      client.publish("Strength",s_strength.c_str());
  }
  Serial1.flush(); 
  if(!client.connected()){
        client.connect("esp32"); //se nao estiver connectado regista-se no broker
        }
  Serial.println("Cilent connected to broker");
   // Display the measurement
  //  Serial.print(dist);
  //  Serial.print(" cm      sigstr: ");
  //  Serial.println(strength);
  
 // }
  // Wait some short time before taking the next measurement
  delay(50);  
}
