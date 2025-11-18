
const int boton1Pin = 2; // boton 1 para SOS (Pin 2)
const int boton2Pin = 3; // boton 2 para Blink (Pin 3)
const int boton3Pin = 4; // boton 3 para Pulse (Pin 4)
// para hacerPulse()
const int ledPin = 9;


int modoActual = 0; // 0: apagado, 1: SOS, 2: Blink, 3: Pulse

// tiempo (para Blink)
unsigned long tiempoAnterior = 0;
const long intervaloBlink = 500; // Blink (parpadeo regular)
int estadoLed = LOW;

// control de Blink de 7 veces
int contadorBlinks = 0;
// 7 ciclos (ON/OFF) = 14 cambios de estado
const int MAX_BLINKS = 14; 


int ultimoEstadoBoton1 = LOW;
int ultimoEstadoBoton2 = LOW;
int ultimoEstadoBoton3 = LOW;

// funciones de parpadeo

// SOS: bloqueante (usa delay)
void hacerSOS() {
  const int corto = 100;    
  const int largo = 300;    
  const int pausa = 100;    
  const int pausaLetra = 300; 

  // S (tres puntos)
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH); delay(corto);
    digitalWrite(ledPin, LOW); delay(pausa);
  }
  delay(pausaLetra);

  // O (tres rayas)
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH); delay(largo);
    digitalWrite(ledPin, LOW); delay(pausa);
  }
  delay(pausaLetra);

  // S (tres puntos)
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH); delay(corto);
    digitalWrite(ledPin, LOW); delay(pausa);
  }
  delay(1000);
}

// Blink: 7 veces, no bloqueante
void hacerBlink() {
  // Solo se ejecuta si aun no ha completado los 7 ciclos
  if (contadorBlinks < MAX_BLINKS) { 
    unsigned long tiempoActual = millis();

    if (tiempoActual - tiempoAnterior >= intervaloBlink) {
      tiempoAnterior = tiempoActual;
      estadoLed = !estadoLed; // cambia el estado (HIGH/LOW)
      digitalWrite(ledPin, estadoLed);
      
      contadorBlinks++; // incrementa el contador cada vez que el estado cambia
    }
  } else {
    // cuando se completa
    digitalWrite(ledPin, LOW); // led apagado
  }
}

// Pulse: sube y baja la intensidad 7 veces (bloqueante, usa delay y PWM)
void hacerPulse() {
  const int repeticiones = 7;
  const int tiempoPaso = 10; 

  for (int j = 0; j < repeticiones; j++) {
    // aumentar intensidad (Fade In)
    for (int intensidad = 0; intensidad <= 255; intensidad += 5) {
      analogWrite(ledPin, intensidad);
      delay(tiempoPaso);
    }
    // disminuir intensidad (Fade Out)
    for (int intensidad = 255; intensidad >= 0; intensidad -= 5) {
      analogWrite(ledPin, intensidad);
      delay(tiempoPaso);
    }
  }

  // led apagado
  digitalWrite(ledPin, LOW);
}

// Setup y Loop

void setup() {
  // inicializacion de pines
  pinMode(ledPin, OUTPUT);
  pinMode(boton1Pin, INPUT);
  pinMode(boton2Pin, INPUT);
  pinMode(boton3Pin, INPUT);

  // apagar el LED al inicio
  digitalWrite(ledPin, LOW);
}

void loop() {
  int estadoBoton1 = digitalRead(boton1Pin);
  int estadoBoton2 = digitalRead(boton2Pin);
  int estadoBoton3 = digitalRead(boton3Pin);

  // logica de Deteccion de toque (positivo)

  // boton 1 (SOS - se activa con un toque)
  if (estadoBoton1 == HIGH && ultimoEstadoBoton1 == LOW) {
    modoActual = 1;
  }
  ultimoEstadoBoton1 = estadoBoton1;

  // boton 2 (Blink - se activa con un toque)
  if (estadoBoton2 == HIGH && ultimoEstadoBoton2 == LOW) {
    modoActual = 2;
  }
  ultimoEstadoBoton2 = estadoBoton2;

  // boton 3 (Pulse - se activa con un toque)
  if (estadoBoton3 == HIGH && ultimoEstadoBoton3 == LOW) {
    modoActual = 3;
  }
  ultimoEstadoBoton3 = estadoBoton3;

  // ejecucion del modo
  switch (modoActual) {
    case 1:
      // SOS (bloqueante): se ejecuta y luego se apaga el modo
      hacerSOS();
      modoActual = 0;
      break;

    case 2:
      // Blink (No Bloqueante, 7 veces)
      
      // reiniciamos el contador de Blink si estamos en modo Blink pero el contador esta completo
      // (esto maneja el caso de que Blink se complete y presione de nuevo)
      if (contadorBlinks >= MAX_BLINKS) {
        contadorBlinks = 0;
      }
      
      hacerBlink();
      
      // si el contador llego al limite, apagamos el modo
      if (contadorBlinks >= MAX_BLINKS) {
        modoActual = 0; 
      }
      break;

    case 3:
      // Pulse (bloqueante): se ejecuta y luego se apaga el modo
      hacerPulse();
      modoActual = 0;
      break;

    default:
      // modo 0: apagado
      digitalWrite(ledPin, LOW);
      tiempoAnterior = millis(); 
      estadoLed = LOW;
      // reiniciamos el contador de Blink cuando el sistema esta en modo 'apagado'
      contadorBlinks = 0; 
      break;
  }
}