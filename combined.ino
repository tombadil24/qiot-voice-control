#include <LWiFi.h>
#include <PubSubClient.h>
#define USER_NAME "50efbc33-f68d-47db-83e7-e457b3cb46ca"    // CHANGE QIOT MQTT USERNAME HERE
#define USER_PASS "r:c86c34ccc5ce9a56da4763f50b9ed67f"      // CHANGE QIOT MQTT PASSWORD HERE
#define CLIENT_ID "RELAY7697_1563246515"                    // CHANGE QIOT MQTT CLIENTID HERE

#include "DHT.h"
#define dht_dpin 3
#define DHTTYPE DHT11   // DHT 11
DHT dht(dht_dpin, DHTTYPE);

IPAddress server(172, 17, 36, 9);      // CHANGE QBOAT SUNNY'S IP ADDRESS HERE (AKA QIOT'S IP ADDRESS)
char data[80];
char temp1[30];
char hum1[30];

char ssid[] = "TP-LINK_1A9F";         // CHANGE WIFI SSID HERE
char pass[] = "04210252";             // CHANGE WIFI PASSWORD HERE
int keyIndex = 0;
int status = WL_IDLE_STATUS;
WiFiClient client;
PubSubClient upload(client);
PubSubClient download(client);

#define RELAY   4

void reconnect() {    
  // Loop until we're reconnected
  while (!download.connected()) {      
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (download.connect(CLIENT_ID, USER_NAME, USER_PASS )) {
      Serial.println("connected");
      download.subscribe("qiot/things/admin/RELAY7697/HomeRelay");  
    } 
    else
    {                                         
      Serial.print("failed, rc=");
      Serial.print(download.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();   

  upload.setServer(server, 21883);        // CHANGE PORT NUMBER HERE
  delay(1500);

  download.setServer(server, 21883);      // AND HERE
  download.setCallback(callback);
  
  // Activate DHT11
  dht.begin();
}

void loop()
{
  Serial.print("Humidity = ");   
  Serial.print(dht.readHumidity());   
  Serial.print("% ");   
  Serial.print("temperature = ");   
  Serial.print(dht.readTemperature());   
  Serial.println("C "); 

  String h= String(dht.readHumidity());
  String t= String(dht.readTemperature());
  String payload = "{\"value\":{\"temperature\":"+ t +",\"humidity\":"+h+"}}";   

  payload.toCharArray(data, (payload.length() + 1));
  if (!upload.connected()) {
    reconnect();
  }
  else{
    upload.publish("qiot/things/admin/MON7697/HomeEnv",data);
    delay(100);
  }
  upload.loop();
  delay(2000);
  download.loop();
}

void printWifiStatus() { 
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if(payload[0] == '1')
  {
    digitalWrite(RELAY, HIGH);
  }
  else
  {
    digitalWrite(RELAY, LOW);
  }
}
