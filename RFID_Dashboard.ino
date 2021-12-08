#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* ssid = "Karasuno";
const char* password = "KarateWiFi03";

const char* mqtt_server = "192.168.188.4";

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
    if (messagein == "ON") 
      Serial.println("Light is ON");
  }else{
      Serial.println("Light is OFF");
  }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
 
    
   //  String clientId = "ESP8266Client-";
   // clientId += String(random(0xffff), HEX);
    // Attempt to connect
   // if (client.connect(clientId.c_str())) {
           if (client.connect("vanieriot")) {

      Serial.println("connected");  
      client.subscribe("room/light");
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

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("vanieriot");

 

  
    
         //  char hh [8];
         //  dtostrf(h,6,2,hh);
         //  char tt [8];
         //  dtostrf(t,6,2,tt);
      
        client.publish("IoTlab/ESP","Hello IoTlab");
      //  client.publish("device/alh",hh);

  delay(5000);
  }
