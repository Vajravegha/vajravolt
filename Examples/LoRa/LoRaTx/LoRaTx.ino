#include <SPI.h>
#include <LoRa.h>
#define ss 16
#define rst 17
#define dio0 -1

int counter = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("LoRa Sender");

  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(866E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSyncWord(0xF3);
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}
