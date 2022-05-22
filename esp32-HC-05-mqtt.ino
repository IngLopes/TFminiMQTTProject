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
    uint8_t r= SerialHC.read();
    Serial.write(r);
    String rpm=String(r);
    client.publish("Rpm", rpm.c_str());
    Serial.println(rpm);    
    SerialHC.flush();
  }

  if (Serial.available()) {    // Keep reading from Arduino Serial Monitor and send to HC-05
    //Serial.println("serial available");
    char c=Serial.read();
    Serial.println(c);
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
