#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <DHTesp.h>

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2

DHTesp dht;

constexpr uint8_t RST_PIN = D3;
constexpr uint8_t SS_PIN = D4;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

String tag;
String ligthThreshold;
String temperatureThreshold;

const int buzzerPin = D0; //Buzzer control port
const int dht11 = D1; // dht11 pin
const int LED = D2; // LED with the switch

const char* ssid = "Please_Let_Me_See_My_Kids";
const char* password = "sussybaka";

const char* mqtt_server = "192.168.1.101";

WiFiClient vanieriot;
PubSubClient client(vanieriot);


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messagein;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messagein += (char)message[i];
  }

  if(topic=="room/light"){
    if (messagein == "ON"){
      digitalWrite(LED, HIGH);
      Serial.println("Light is ON");
    }else{
      digitalWrite(LED, LOW);
      Serial.println("Light is OFF");
    }
  }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("vanieriot")) {
      Serial.println("connected");  
      client.subscribe("room/light");
      client.subscribe("room/RFIDTag");
      client.subscribe("room/lightTreshold");
      client.subscribe("room/temperatureThreshold");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  pinMode(buzzerPin, OUTPUT); // setup the buzzer
  digitalWrite (buzzerPin, LOW);// Initially buzzer off

  pinMode(LED, OUTPUT);

  dht.setup(dht11, DHTesp::DHT11);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("vanieriot");

  if(!rfid.PICC_IsNewCardPresent())
    return;
  if(rfid.PICC_ReadCardSerial()){
    for(byte i = 0; i < 4; i++){
      tag += rfid.uid.uidByte[i];
    }

    if(tag == "92109218110"){
      ligthThreshold = "900";
      temperatureThreshold = "20";
    }else if(tag == "1311033513"){
      ligthThreshold = "15000";
      temperatureThreshold = "30";
    }else{ // None register user
      ligthThreshold = "NULL";
      temperatureThreshold = "NULL";
      digitalWrite (buzzerPin, HIGH);
      delay(5000); 
      digitalWrite (buzzerPin, LOW);
      delay(5000);
    }

    char tagArr[8];
    tag.toCharArray(tagArr, 8);
    char lightArr[8];
    ligthThreshold.toCharArray(lightArr, 8);
    char temperatureArr[8];
    temperatureThreshold.toCharArray(temperatureArr, 8);
    
    Serial.println(tag);
    client.publish("room/RFIDTag", tagArr);
    client.publish("room/lightTreshold", lightArr);
    client.publish("room/temperatureThreshold", temperatureArr);
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  float temp= dht.getTemperature();
  float hum= dht.getHumidity();
  //float photoResistor = analogRead(photoResist);

  char tempArr [8];
  dtostrf(temp,6,2,tempArr);
  char humArr [8];
  dtostrf(hum,6,2,humArr);
//  char resistArr[8];
//  dtostrf(photoResistor, 6, 2, resistArr);

  client.publish("IoTlab/temperature", tempArr);
  client.publish("IoTlab/humidity", humArr);

  delay(5000);
}
