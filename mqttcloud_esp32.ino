//TESTING TO CONNECT ESP32 AS A CLIENT TO CLOUD MQTT BROKER USING HIVEMQCLOUD

//INCLUDE NECESSARY LIBRARIES
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <TFMini.h>
//#include <HardwareSerial.h>

//FIRST STEP - connect to the wifi
//--Wifi settings
const char* ssid = "Ing_LM";
const char* password = "eqtc1876";

//---Function for connecting to wifi
void connect_wifi(){
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
}

//SECOND STEP - Configure client esp32 mqtt

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient); 

//MQTT Cloud Broker settings
const char *mqtt_server = "6e42497930bd4bd99a0aa8ac31773d8f.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char *mqtt_user = "LidarS";
const char *mqtt_pass = "TFmini4prox";
const char *mqtt_client_name = "Esp32Lidar"; // Client connections cant have the same connection name

// THIRD STEP - Configure the connection with the TFmini Lidar Sensor
#define RXD2 18
#define TXD2 19
#define TFMINI_BAUDRATE 115200
TFMini tfmini;
//Measurement variables
int dist, strength;
String s_dist, s_strength;

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

void reconnectMQTT() {
  //Loop until reconnected
  while (!client.connected()){
    Serial.print("Attempting connetion to MQTT Broker...");
    //Attempt to connect
    if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass)){
      Serial.println("Connected.");
      client.subscribe("Distance"); //subscribe to topic Distance
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" try again in 2 seconds");
      delay(2000);
    }
  }
}


void setup(){
  Serial.begin(TFMINI_BAUDRATE);
  Serial1.begin(TFMINI_BAUDRATE, SERIAL_8N1, RXD2, TXD2);
  // Initialize the TF Mini sensor on Serial1
  tfmini.begin(&Serial1);  
  // Initializing wifi and mqtt server
  connect_wifi();
  wifiClient.setCACert(root_ca); //for secure connection
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);  
}

void loop(){
  if (!client.connected()) reconnectMQTT();
  client.loop();
  
  if (Serial1.available()>1){
    dist=tfmini.getDistance();
    strength=tfmini.getRecentSignalStrength();
    Serial.println(dist);
    if ((strength < 2000) && (strength > 20)) { //if strength is not between these values the measurement csn't be trusted
      if (dist < 60000) {
        //Passar os valores para strings:
        s_dist = String(dist);
        s_strength = String(strength);
        Serial.print("Publish dist on MQTT: ");
        Serial.println(dist);
        client.publish("Distance", s_dist.c_str());
        Serial.print("Publish str on MQTT: ");
        Serial.println(strength);
        client.publish("Strength", s_strength.c_str());
      }      
    }
    Serial1.flush();
    
  } else {
   // Serial.println(Serial1.available());
  }
}

//The void callback is called every time we have a message from the broker
void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
  
  Serial.println("Message arrived ["+String(topic)+"]"+incommingMessage);
  }

   //  check for other commands
 /*  else  if( strcmp(topic,command2_topic) == 0){
     if (incommingMessage.equals("1")) {  } // do something else
  }
  */
