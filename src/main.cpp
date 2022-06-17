#include <Arduino.h>
#include <IBMIOTF8266.h>


String user_html = "";
int                 cmdBaseLen = 10;

char*               ssid_pfix = (char*)"Edge_UltraSonic";
//unsigned long       lastPublishMillis = - pubInterval;

const int trigPin = D5;
const int echoPin = D4;

int distance;

boolean detect;
boolean enable = true;

/*
void publishData() {
    StaticJsonDocument<512> root;
    JsonObject data = root.createNestedObject("d");

// USER CODE EXAMPLE : command handling
    data["Detect"] = "success";
    //data["Detect"] =  distance < 5.0 ? "detect" : "none";
// USER CODE EXAMPLE : command handling

    serializeJson(root, msgBuffer);
    client.publish("iot-2/type/Edge/id/eSensor/evt/status/fmt/json", msgBuffer);
} */


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
    client.loop();
    
    //UltraSonic Sensor
    digitalWrite(trigPin, LOW);
    //digitalWrite(echoPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    distance = pulseIn(echoPin, HIGH) * 0.017;
    Serial.println(distance);
    delay(50);
    
    if(distance >= 24 && distance <= 25){
        detect = false;
        enable = true;
    }else{
        detect = true;
    }

    if((detect == true) && (enable == true)){
        Serial.println("Detected!");
        client.publish("iot-2/type/Edge/id/eSensor/evt/status/fmt/json", "on");
        enable = false;
    }
}