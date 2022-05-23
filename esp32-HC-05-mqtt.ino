//Trying to unite bluetooth and wifi at the same time on esp32. BT by using the HC-05 bt module and wifi
//by using the esp32's own antenna

#include <HardwareSerial.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

//Defining the pins for HC-05 - serial2
#define TX_HC 17
#define RX_HC 16
//HardwareSerial SerialHC(2); // RX | TX
#define SerialHC Serial2

//Para conectar a rede movel celular
const char*ssid = "Ing_LM";
const char* password = "eqtc1876";
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);
//MQTT Cloud Broker settings
const char *mqtt_server = "6e42497930bd4bd99a0aa8ac31773d8f.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char *mqtt_user = "EspOBD";
const char *mqtt_pass = "espOBD2s";
const char *mqtt_client_name = "Esp32OBD"; // Client connections cant have the same connection name

//Necessary commands for HC-05 initialization
//const char p1[] = "AT+INIT\r\n";
//const char p2[] = "AT+INQ\r\n";
//const char p3[] = "AT+LINK=A816,D0,4A7DDC\r\n";
//int nloop=0;

//CA Certificate

static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

void connect_wifi(){
  WiFi.mode(WIFI_STA);
  //Stabilishing wifi connection:
  Serial.print(F("Conectando: "));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  // IP address
  Serial.print(F("IP esp: "));
  Serial.println(WiFi.localIP()); // Imprimir IP do ESP
}
void readSerialHC() {
  if(SerialHC.available()){
        Serial.println(SerialHC.read());
      } 
}
void reconnectMQTT() {
  //Loop until reconnected
  while (!client.connected()){
    Serial.print(F("Attempting connetion to MQTT Broker..."));
    //Attempt to connect
    if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass)){
      Serial.println(F("Connected."));
      client.subscribe("Rpm"); //subscribe to topic Distance
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Enter AT commands:");
  //BTSerial.begin(115200);       // HC-05 has been changed with AT+UART=115200,0,0
  SerialHC.begin(38400, SERIAL_8N1, RX_HC, TX_HC);
//  SerialHC.write("AT");
//  Serial.println(SerialHC.read());
  //SerialBT.begin(115200);
  connect_wifi();
  wifiClient.setCACert(root_ca); //for secure connection
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}

void loop()
{
  if (!client.connected()) reconnectMQTT();
  client.loop();
  
  if (SerialHC.available()>0)  {  // read from HC-05 and send to Arduino Serial Monitor
  //Serial.println("receiving message from bluetooth");
    String r= SerialHC.readStringUntil('>');
    Serial.println(r);
    r.remove(0,1);
    int response=r.toInt();  //convert string to integer to be able to see if it is hex, relevant data
    //if (isHexadecimalDigit(response)){
      //take the identifier out of the string r. that is the 4 first digits
    String ident=r;
    ident=r.substring(0,4);
    int lg=r.length();
    Serial.println(ident);
    if (ident=="410C"){ //rpm reading
      Serial.println("Rpm reading");
      String rpmhx = r.substring(4,lg); //takes the data from rpm from the message but in hex
      Serial.println(rpmhx);
      int rpmdc= strtol(rpmhx.c_str(), NULL, 16); //converts the rpm to decimal
      Serial.println(rpmdc);
      rpmdc=rpmdc/4;
      String rpm = String(rpmdc); //converting rpm to String
      Serial.println(rpm); //actual rpm value 
      client.publish("Rpm", rpm.c_str());     
    } else{
      Serial.print("not relevant rpm data from OBD: ");
      Serial.println(r);
    }
    if (ident=="4105"){ //current engine coolant temperature
      String engTemphx = r.substring(4,lg);
      Serial.println(engTemphx);
      int engTempdc= strtol(engTemphx.c_str(), NULL, 16); //converts the temperature to decimal
      Serial.println(engTempdc);
      engTempdc=engTempdc- 40; //substracting 40 because of the offset to allow subzero temperatures
      String engTemp = String (engTempdc);
      Serial.println(engTemp);
      client.publish("EngCoolTemp",engTemp.c_str());
    } else{
      Serial.print("not relevant engine coolant temperature data from OBD: ");
      Serial.println(r);
    }
    if (ident=="410D"){ //current vehicle speed - no math needed
      String spdhx = r.substring(4,lg);
      Serial.println(spdhx);
      int spddc= strtol(spdhx.c_str(), NULL, 16); //converts the speed to decimal
      Serial.println(spddc);
      String spd = String (spddc);
      client.publish("Speed",spd.c_str());
    } else{
      Serial.print("not relevant speed data from OBD: ");
      Serial.println(r);
    }
    if (ident=="4110"){//air flow rate 
      Serial.println("Air Flow reading");
      String airFlowhx = r.substring(4,lg); //takes the data from airFlow from the message but in hex
      Serial.println(airFlowhx);
      int airFlowrpmdc= strtol(airFlowhx.c_str(), NULL, 16); //converts the airFlow to decimal
      Serial.println(airFlowdc);
      airFlowdc=airFlowdc/100;
      String airFlow = String(airFlowdc); //converting rpm to String
      Serial.println(airFlow); //actual rpm value 
      client.publish("airFlow", airFlow.c_str());     
    } else{
      Serial.print("not relevant airFlow data from OBD: ");
      Serial.println(r);
    }
    if (ident="410F"){ //Intake air temperature
      Serial.println("Intake Air Temperature reading");
      String airTemphx = r.substring(4,lg); //takes the data from airTemp from the message but in hex
      Serial.println(airTemphx);
      int airTempdc= strtol(airTemphx.c_str(), NULL, 16); //converts the airTemp to decimal
      Serial.println(airTempdc);
      airTempdc=airTempdc-40; //substracting 40 because of the offset to allow subzero temperatures
      String airTemp = String(airTempdc); //converting airTemp to String
      Serial.println(airTemp); //actual aieTemp value 
      client.publish("airTemp", airTemp.c_str());     
    } else{
      Serial.print("not relevant airFlow data from OBD: ");
      Serial.println(r);
    }
    if (ident=="4104"){ //Engine Load
      Serial.println("Engine Load reading");
      String engLoadhx = r.substring(4,lg); //takes the data from engLoad from the message but in hex
      Serial.println(engLoadhx);
      int engLoaddc= strtol(engLoadhx.c_str(), NULL, 16); //converts the engLoad to decimal
      Serial.println(engLoaddc);
      engLoaddc=(engLoaddc/255)*100; 
      String engLoad = String(engLoaddc); //converting airTemp to String
      Serial.println(engLoad); //actual engLoad value 
      client.publish("engLoad", engLoad.c_str());     
    } else{
      Serial.print("not relevant engLoad data from OBD: ");
      Serial.println(r);
    }
    
    SerialHC.flush(); //waits for the transmit buffer to be empty / the transmission to be finished
  }
   
  if (Serial.available()) {    // Keep reading from Arduino Serial Monitor and send to HC-05
    //Serial.println("serial available");
//    if (nloop==0){
      //Seting up the bluetooth connection:
//      SerialHC.write("AT+ROLE=1\r\n"); //configure HC-05 as master
//      Serial.println("AT+ROLE=1");
//      Serial.println(SerialHC.read());
//      SerialHC.write("AT+CMODE=0\r\n"); // module can only connect by specific bluetooth address
//      Serial.println("AT+CMODE=0");
//      Serial.println(SerialHC.read());
    //  SerialHC.write("AT+INQM=0,5,9");
    //  Serial.println(SerialHC.read());
//      SerialHC.write(p1);
//      Serial.println("AT+INIT");
//      readSerialHC();
//      SerialHC.write(p2);
//      Serial.println("AT+INQ");
//      delay(9000);
//      readSerialHC();  
//      SerialHC.write(p3); //endereço do J7 Prime2
//      Serial.println("AT+LINK=A816,D0,4A7DDC\r\n");
//      readSerialHC();
//      nloop ++;
//    }
    char c=Serial.read();
    //Serial.println(c);
    SerialHC.write(c);
  }
}

//The void callback is called every time we have a message from the broker
void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
  
  Serial.println("Message arrived ["+String(topic)+"]"+incommingMessage);
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
