#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

DHTesp dht;

const char * ssid = "";
const char * password = "";

const char * mqtt_server = "";

//const int enable = 4;  //d2
//const int input1 = 5; //d1
//const int input2 = 16; //d0

//const int dht11 = 2; //d4

//const int LED = 12; //D6

const char photoResist = A0;
const int photoLights = 5; //d1
  
WiFiClient iotvanier;
PubSubClient client(iotvanier);

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
  Serial.print("WiFi connected - ESP-8266 IP address: ");
  Serial.println(WiFi.localIP());
}

int inputLight;
int lightThreshold;

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messagein;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messagein += (char)message[i];
  }

//  if (topic == "room/light") {
//    if (messagein == "true") {
//      Serial.println(" Light is ON");
//      digitalWrite(LED, HIGH);
//    } else {
//       Serial.println(" Light is OFF");
//       digitalWrite(LED, LOW);
//    }
//  }

//  if (topic == "room/fan") {
//        if (messagein == "yes") {
//            Serial.println(" Fan is ON");
//            digitalWrite(enable,HIGH);
//            digitalWrite(input1,LOW);
//            digitalWrite(input2,HIGH);
//        } else {
//            Serial.println(" Fan is OFF");
//            digitalWrite(enable, HIGH);
//            digitalWrite(input1, LOW);
//            digitalWrite(input2, LOW);
//        }
//   }

    if (topic == "room/photoresistor") {
      inputLight = messagein.toInt();
//        if (messagein == "ON") {
//            Serial.println(" Lights are ON");
//            digitalWrite(photoLights, HIGH);
//        } else {
//            Serial.println(" Lights are OFF");
//            digitalWrite(photoLights, LOW);
//        }
    }

    
    if (topic == "room/returnLight") {
      lightThreshold = messagein.toInt();

   }

Serial.println(inputLight);

Serial.println(lightThreshold);

//    if (inputLight < lightThreshold) {
//    
//      Serial.println(" Lights are ON");
//      digitalWrite(photoLights, HIGH);
//    } else {
//      Serial.println(" Lights are OFF");
//      digitalWrite(photoLights, LOW);
//    }

    
    if (inputLight < lightThreshold) {
    
      Serial.println(" Send email");
      char emailArr[8];
      dtostrf(1, 6, 2, emailArr);
      client.publish("room/sendEmail", emailArr);
    } else {
      char emailArr[8];
      dtostrf(0, 6, 2, emailArr);
      client.publish("room/sendEmail", emailArr);
      Serial.println(" Don't send email");
   
    }

     if (topic == "room/recieveEmail") {
      
        if (messagein == "Yes") {
            Serial.println(" Lights are ON");
            digitalWrite(photoLights, HIGH);
        } else {
            Serial.println(" Lights are OFF");
            digitalWrite(photoLights, LOW);
        }
    }
    
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
 
    
   //  String clientId = "ESP8266Client-";
   // clientId += String(random(0xffff), HEX);
    // Attempt to connect
   // if (client.connect(clientId.c_str())) {
    if (client.connect("iotvanier")) {

      Serial.println("connected");  
      //client.subscribe("room/light");
      client.subscribe("room/fan");
      client.subscribe("room/photoresistor");
      client.subscribe("room/returnLight");
      client.subscribe("room/recieveEmail");
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
  //dht.setup(dht11, DHTesp::DHT11);
  //pinMode(LED, OUTPUT);
//  pinMode(enable,OUTPUT);
//  pinMode(input1,OUTPUT);
//  pinMode(input2,OUTPUT);
  pinMode(photoResist, INPUT);
  pinMode(photoLights, OUTPUT);


//  digitalWrite(enable, LOW);
//  digitalWrite(input1, LOW);
//  digitalWrite(input2, LOW);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("iotvanier");

  float temp= dht.getTemperature();
  float hum= dht.getHumidity();
  //float photoResistor = analogRead(photoResist);
  float photoResistor = analogRead(photoResist);
    
    //char tempArr [8];
    //dtostrf(temp,6,2,tempArr);
    //char humArr [8];
    //dtostrf(hum,6,2,humArr);
    char resistArr[8];
    dtostrf(photoResistor, 6, 2, resistArr);
      
       //client.publish("IoTlab/ESP","Hello IoTlab");
       //client.publish("IoTlab/temperature", tempArr);
      //client.publish("IoTlab/humidity", humArr);
       client.publish("IoTlab/photoResistor", resistArr);
      //  client.publish("device/alh",hh);

//  if (inputLight < lightThreshold) {
//      Serial.println(" Lights are ON");
//      digitalWrite(photoLights, HIGH);
//    } else {
//      Serial.println(" Lights are OFF");
//      digitalWrite(photoLights, LOW);
//    }

  delay(5000);

//  if (temp < tempThreshold) {
//    //send email to ask to turn on fan
//    digitalWrite(enable,HIGH);
//    digitalWrite(input1,LOW);
//    digitalWrite(input2,HIGH);
//  
//    delay(5000);
//  }
    

}
