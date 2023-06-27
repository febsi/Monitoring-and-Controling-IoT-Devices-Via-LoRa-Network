#include <LoRa.h>
#include <ArduinoJson.h>

const int ledPin = 3;
int buttonPin = 2; // push button pin
int state = 0; // button state
String msg;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!LoRa.begin(922E6)) {
    Serial.println("LoRa initialization failed.");
    while (1);
  }
pinMode(ledPin, OUTPUT);
pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  state = digitalRead(buttonPin);

    if (state == LOW) {
        digitalWrite(ledPin, HIGH);
        byte gateway = 0x03;
        
      StaticJsonDocument<200> doc;
      doc["ID2"]= "Node_1";
      doc["Value_FL2"] = "0";
      doc["Value_Ldr2"] = "0";
      doc["Value_humi2"] = "0";
      doc["Value_temp2"] = "0";
      String jsonData;
      serializeJson(doc, jsonData);
      delay(5000);

      Serial.println(jsonData);

      StaticJsonDocument<200> msg;
      msg["ID"]= "Node_2";
      msg["Value_FL"] = "0";
      msg["Value_Ldr"] = "0";
      msg["Value_humi"] = "0";
      msg["Value_temp"] = "0";
      String jsonData1;
      serializeJson(msg, jsonData1);
      delay(6000);

      Serial.println(jsonData1);
      
        if (Serial.available()) {
        String data = Serial.readString(); // Baca command dari ESP32
        Serial.println("Data Diterim: ");
        Serial.print(data);
      
      LoRa.beginPacket();
      LoRa.write(gateway);
      LoRa.print(data);
      LoRa.endPacket();
      delay(2000);
      } 
    }
    else if (state == HIGH){
      digitalWrite(ledPin, LOW);
      int packetSize = LoRa.parsePacket();
      if (packetSize) {
        byte node = LoRa.read();
          if (node == 0x01 || node == 0x02) {
            String message = "";
            while (LoRa.available()) {
          message += (char)LoRa.read();
      }
      Serial.println();
      
      DynamicJsonDocument doc(256);
      DeserializationError error = deserializeJson(doc, message);
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

      String jsonData;
      serializeJson(doc, jsonData);
      
      Serial.print(jsonData);    
}
}
}
}

