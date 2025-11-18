const int ledPin = 9;  // LED

// Tiempos SOS
const int corto = 200;
const int largo = 600;
const int pausa = 200;
const int pausaLetra = 400;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {

  // ------------------------------------------------------
  // 1. SECUENCIA SOS (· · · — — — · · ·)
  // ------------------------------------------------------

  // Tres cortos (S)
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(corto);
    digitalWrite(ledPin, LOW);
    delay(pausa);
  }
  delay(pausaLetra);

  // Tres largos (O)
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(largo);
    digitalWrite(ledPin, LOW);
    delay(pausa);
  }
  delay(pausaLetra);

  // Tres cortos (S)
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(corto);
    digitalWrite(ledPin, LOW);
    delay(pausa);
  }

  delay(1000); // Pausa general


  // ------------------------------------------------------
  // 2. BLINK 7 VECES (medio segundo)
  // ------------------------------------------------------
  for (int i = 0; i < 7; i++) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }

  delay(1000);


  // ------------------------------------------------------
  // 3. PULSE (fade in - fade out)
  // ------------------------------------------------------

  for (int i = 0; i < 255; i++) {   // Fade in
    analogWrite(ledPin, i);
    delay(10);
  }

  for (int i = 255; i >= 0; i--) {  // Fade out
    analogWrite(ledPin, i);
    delay(10);
  }

  delay(1000);
}

