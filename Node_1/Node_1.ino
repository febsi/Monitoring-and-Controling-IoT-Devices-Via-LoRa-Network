#include <LoRa.h>
#include <ArduinoJson.h>
#define RELAY_PIN 7
#define RELAY_PIN1 8
#define RELAY_PIN2 9
#include <DHT.h>
#define DHTPIN 5 
#define DHTTYPE DHT11
#define MAX_ADC_READING 1023
#define ADC_REF_VOLTAGE 5.0
#define REF_RESISTANCE 10000
#define LUX_CALC_SCALAR 125235178.3654270
#define LUX_CALC_EXPONENT -1.604568157

DHT dht(DHTPIN, DHTTYPE);

const int ledPin = 4;
int buttonPin = 2; // push button pin
int state = 0; // button state
int counter = 0; // counter for message number
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
   
    Serial.print(ldrLux, 2);                     //Tampilkan intensitas cahaya
    
    
    int flameValue = digitalRead(3);
    float temperature = dht.readTemperature();   
    float humidity = dht.readHumidity();
    byte node1 = 0x01; // address of the receiver

    if ( ldrValue >= 0 || flameValue >= 0 || humidity  >= 0 || temperature >= 0){

    //Mengubah data menjadi JSON// 
    StaticJsonDocument<200> doc;
    doc["ID2"]= "Node_1";
    doc["Value_FL2"] = flameValue;
    doc["Value_Ldr2"] = ldrLux ;
    doc["Value_humi2"] = humidity;
    doc["Value_temp2"] = temperature;
    char jsonData[512];
    serializeJson(doc, jsonData);

    //Mengirim data melalui LoRa
    LoRa.beginPacket();
    LoRa.write(node1);
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

    if (data == "OFF") {
      // Nyalakan relay
      Serial.println("Relay turned OFF");
      digitalWrite(RELAY_PIN, HIGH);
    } else if (data == "ON") {
      // Matikan relay
      Serial.println("Relay turned ON");
      digitalWrite(RELAY_PIN, LOW);
    } else if (data == "OFF2") {
      // Nyalakan relay
      Serial.println("Relay turned OFF");
      digitalWrite(RELAY_PIN1, HIGH);
    } else if (data == "ON2") {
      // Matikan relay
      Serial.println("Relay turned ON");
      digitalWrite(RELAY_PIN1, LOW);
    }else if (data == "OFF1") {
      // Nyalakan relay
      Serial.println("Relay turned OFF");
      digitalWrite(RELAY_PIN2, HIGH);
    } else if (data == "ON1") {
      // Matikan relay
      Serial.println("Relay turned ON");
      digitalWrite(RELAY_PIN2, LOW);
    }else if (data == "ON7") {
      // Matikan relay
      Serial.println("Relay turned ON");
      digitalWrite(RELAY_PIN2, LOW);
    }
    
}
}
}
}

