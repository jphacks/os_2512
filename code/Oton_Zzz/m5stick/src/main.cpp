#include "M5StickCPlus2.h"
#include <IRrecv.h>
#include <IRutils.h>

void setup() {
  M5.begin();
  Serial.begin(115200);
}

void loop() {
  Serial.println("=== Hello World ===");
  delay(1000);
}