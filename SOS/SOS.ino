void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // led como salida
}

void loop() {
  // S (· · ·)
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200); // punto corto
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }

  // O (— — —)
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(800); // raya larga
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }

  // S (· · ·)
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }

  // tiempo de espera del mensaje SOS
  delay(1500);
}



