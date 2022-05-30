#include <Arduino.h>
#include <IBMIOTF8266.h>


String user_html = "";

char*               ssid_pfix = (char*)"Edge_UltraSonic";
unsigned long       lastPublishMillis = - pubInterval;

const int trigPin = 4;
const int echoPin = 5;

long duration;
float distance;

StaticJsonDocument<512> root;
JsonObject data = root.createNestedObject("d");

void publishData() {
    StaticJsonDocument<512> root;
    JsonObject data = root.createNestedObject("d");

// USER CODE EXAMPLE : command handling
    data["Detect"] =  "on";
// USER CODE EXAMPLE : command handling

    serializeJson(root, msgBuffer);
    client.publish("iot-2/type/Edge/id/eSonic/evt/status/fmt/json", msgBuffer);
}

/*
void message(char* topic, byte* payload, unsigned int payloadLength) {
    byte2buff(msgBuffer, payload, payloadLength);
    StaticJsonDocument<512> root;
    DeserializationError error = deserializeJson(root, String(msgBuffer));
  
    if (error) {
        Serial.println("handleCommand: payload parse FAILED");
        return;
    }

    handleIOTCommand(topic, &root);
    if (!strcmp(updateTopic, topic)) {
// USER CODE EXAMPLE : meta data update
// If any meta data updated on the Internet, it can be stored to local variable to use for the logic
// in cfg["meta"]["XXXXX"], XXXXX should match to one in the user_html
        
// USER CODE EXAMPLE
    } else if (!strncmp(commandTopic, topic, cmdBaseLen)) {            // strcmp return 0 if both string matches
        handleUserCommand(&root);
    }
}
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  initDevice();
    // If not configured it'll be configured and rebooted in the initDevice(),
    // If configured, initDevice will set the proper setting to cfg variable

    WiFi.mode(WIFI_STA);
    WiFi.begin((const char*)cfg["ssid"], (const char*)cfg["w_pw"]);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // main setup
    Serial.printf("\nIP address : "); Serial.println(WiFi.localIP());

    set_iot_server();
    //client.setCallback(message);
    iot_connect();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
        iot_connect();
  }
  //UltraSonic Sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.017;

  if(distance <= 10){
    Serial.println("PET Detected!");
    publishData();
  }
}