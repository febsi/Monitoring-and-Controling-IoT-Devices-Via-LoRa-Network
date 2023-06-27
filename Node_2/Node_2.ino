#include <LoRa.h>
#include <ArduinoJson.h>
#define RELAY_PIN 7
#define RELAY_PIN1 9
#define RELAY_PIN2 8
#include "DHT.h"
#define DHTPIN 5 
#define DHTTYPE DHT11
#define MAX_ADC_READING 1023
#define ADC_REF_VOLTAGE 5.0
#define REF_RESISTANCE 10000
#define LUX_CALC_SCALAR 125235178.3654270
#define LUX_CALC_EXPONENT -1.604568157

DHT dht(DHTPIN, DHTTYPE);

int ledPin = 6;
int buttonPin = 2; // push button pin
int state = 0; // button state
int relayState = 0;
String msg;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  //Mendeklarasikan Frekuensi Yang digunakan yaitu 922 MHZ
  if (!LoRa.begin(922E6)) {
    Serial.println("LoRa initialization failed.");
    while (1);
  }
  dht.begin();
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(A0, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
}

void loop() {
  state = digitalRead(buttonPin);

  if (state == LOW) {
    //Jika nilai Push Button adalah Low, maka LoRa berfungsi menjadi Transmitter
    digitalWrite(ledPin, HIGH);
    
    //Mengubah value Ldr sensor menjadi satuan intensitas cahaya yaitu LuX
    int ldrValue = analogRead(A0);
    float resistorVoltage, ldrVoltage;
    float ldrResistance;
    float ldrLux;

    float intensity = map(ldrValue, 1023, 0, 0, 1000);
    //------------Konversi  dari data analog ke nilai lux------------//
    resistorVoltage = intensity / MAX_ADC_READING * ADC_REF_VOLTAGE;
    ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;
    ldrResistance = ldrVoltage / resistorVoltage * REF_RESISTANCE;
    ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
    
    int flameValue = digitalRead(3);
    float temperature = dht.readTemperature();   
    float humidity = dht.readHumidity();
    byte node2 = 0x02; // address of the receiver

    if ( ldrValue >= 0 || flameValue >= 0 || humidity >= 0 || temperature >= 0){
    
    //Mengubah data menjadi JSON
    StaticJsonDocument<200> doc;
    doc["ID"]= "Node_2";
    doc["Value_FL"] = flameValue;
    doc["Value_Ldr"] = ldrLux;
    doc["Value_humi"] = humidity;
    doc["Value_temp"] = temperature;
    char jsonData[512];
    serializeJson(doc, jsonData);
  
    //Mengirim data melalui LoRa
    LoRa.beginPacket();
    LoRa.write(node2);
    LoRa.print(jsonData);
    LoRa.endPacket();
    Serial.println("Data terkirim: " + String(jsonData));
    delay(2000);
    }
  }
  else if (state == HIGH){
  //Jika nilai Push Button adalah Low, maka LoRa berfungsi menjadi Reciver  
  digitalWrite(ledPin, LOW);
  
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    byte node = LoRa.read();
    if (node == 0x03) {
      //Menerima data dari Lora yang memiliki Address 0x03
      String data = "";
      while (LoRa.available()) {
        data += (char)LoRa.read();
      }
      Serial.print("Message received: ");
      Serial.println(data);

    if (data == "OFF3") {
      // Nyalakan relay
      Serial.println("Relay turned OFF");
      digitalWrite(RELAY_PIN, HIGH);
    } else if (data == "ON3") {
      // Matikan relay
      Serial.println("Relay turned ON");
      digitalWrite(RELAY_PIN, LOW);
    } else if (data == "OFF4") {
      // Nyalakan relay
      Serial.println("Relay turned OFF");
      digitalWrite(RELAY_PIN1, HIGH);
    } else if (data == "ON4") {
      // Matikan relay
      Serial.println("Relay turned ON");
      digitalWrite(RELAY_PIN1, LOW);
    }else if (data == "OFF5") {
      // Nyalakan relay
      Serial.println("Relay turned OFF");
      digitalWrite(RELAY_PIN2, HIGH);
    } else if (data == "ON5") {
      // Matikan relay
      Serial.println("Relay turned ON");
      digitalWrite(RELAY_PIN2, LOW);
    }else if (data == "ON6") {
      // Matikan relay
      Serial.println("Relay turned ON");
      digitalWrite(RELAY_PIN2, LOW);
    }else {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(RELAY_PIN1, LOW);
      digitalWrite(RELAY_PIN2, LOW);
      }
    
}
}
}
}

