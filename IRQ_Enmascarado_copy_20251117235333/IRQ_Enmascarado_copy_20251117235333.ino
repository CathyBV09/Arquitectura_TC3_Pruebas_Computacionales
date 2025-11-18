// --- Definición de Pines ---
const int boton1Pin = 2; // boton 1 para SOS (Pin 2)
const int boton2Pin = 3; // boton 2 para Blink (Pin 3)
const int boton3Pin = 4; // boton 3 para Pulse (Pin 4)
const int ledPin = 9;    // LED (Pin 9 - Debe ser PWM)

// --- Control de Estado y Variables Globales ---
int modoActual = 0; // 0: Apagado, 1: SOS, 2: Blink, 3: Pulse

// --- Variables para Debouncing (Rebote) ---
// Tiempo mínimo entre pulsaciones válidas
const unsigned long tiempoDebounce = 50; // 50 milisegundos
// Almacena el tiempo del último toque registrado para cada botón
unsigned long tiempoUltimoToque[3] = {0, 0, 0}; 

// Variables para detectar el flanco (borde negativo con PULLUP)
int ultimoEstadoBoton1 = HIGH;
int ultimoEstadoBoton2 = HIGH;
int ultimoEstadoBoton3 = HIGH;

// --- VARIABLES PARA MODO BLINK (No Bloqueante) ---
unsigned long tiempoAnteriorBlink = 0;
const long intervaloBlink = 500; 
int estadoLedBlink = LOW;
int contadorBlinks = 0;
const int MAX_BLINKS = 14; 

// --- VARIABLES PARA MODO SOS (No Bloqueante) ---
unsigned long tiempoAnteriorSOS = 0;
int pasoSOS = 0; 
const int corto = 100;
const int largo = 300;
const int pausa = 100;
const int pausaLetra = 300;
const int pausaFinal = 1000;
const int duracionesSOS[] = {
  corto, pausa, corto, pausa, corto, pausaLetra, // S
  largo, pausa, largo, pausa, largo, pausaLetra, // O
  corto, pausa, corto, pausa, corto, pausaLetra, // S
  pausaFinal 
};
const int MAX_PASOS_SOS = sizeof(duracionesSOS) / sizeof(duracionesSOS[0]);

// --- VARIABLES PARA MODO PULSE (No Bloqueante) ---
unsigned long tiempoAnteriorPulse = 0;
int pasoPulse = 0; 
int intensidad = 0; 
const int tiempoPaso = 10; 
const int incremento = 5; 
int contadorPulsos = 0; 
const int MAX_PULSES = 7;

// --- Funciones de Parpadeo (No Bloqueantes) ---

void hacerSOS() {
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnteriorSOS >= duracionesSOS[pasoSOS]) {
    tiempoAnteriorSOS = tiempoActual;
    digitalWrite(ledPin, (pasoSOS % 2 == 0) ? HIGH : LOW);
    pasoSOS++;
    if (pasoSOS >= MAX_PASOS_SOS) {
      pasoSOS = 0; 
    }
  }
}

void hacerBlink() {
  if (contadorBlinks < MAX_BLINKS) {
    unsigned long tiempoActual = millis();
    if (tiempoActual - tiempoAnteriorBlink >= intervaloBlink) {
      tiempoAnteriorBlink = tiempoActual;
      estadoLedBlink = !estadoLedBlink;
      digitalWrite(ledPin, estadoLedBlink);
      contadorBlinks++;
    }
  } else {
    digitalWrite(ledPin, LOW);
    modoActual = 0; // Apaga el modo al terminar
  }
}

void hacerPulse() {
  unsigned long tiempoActual = millis();
  if (tiempoActual - tiempoAnteriorPulse >= tiempoPaso) {
    tiempoAnteriorPulse = tiempoActual;

    if (pasoPulse == 0) { // Aumentar
      intensidad += incremento;
      if (intensidad >= 255) { intensidad = 255; pasoPulse = 1; }
    } else { // Disminuir
      intensidad -= incremento;
      if (intensidad <= 0) { 
        intensidad = 0; 
        pasoPulse = 0; 
        contadorPulsos++;
        if (contadorPulsos >= MAX_PULSES) {
          digitalWrite(ledPin, LOW);
          modoActual = 0; // Apaga el modo al terminar
          contadorPulsos = 0;
        }
      }
    }
    analogWrite(ledPin, intensidad);
  }
}

// --- Función de Lectura de Pulsadores con Debounce ---
void leerPulsadores() {
  unsigned long tiempoActual = millis();
  int nuevoModo = 0;
  
  // --- Procesar Botón 1 (SOS) ---
  int estadoBoton1 = digitalRead(boton1Pin);
  if (estadoBoton1 == LOW && ultimoEstadoBoton1 == HIGH) { // Flanco Negativo (Presionado)
    if (tiempoActual - tiempoUltimoToque[0] > tiempoDebounce) {
      tiempoUltimoToque[0] = tiempoActual;
      nuevoModo = 1;
    }
  }
  ultimoEstadoBoton1 = estadoBoton1;

  // --- Procesar Botón 2 (Blink) ---
  int estadoBoton2 = digitalRead(boton2Pin);
  if (estadoBoton2 == LOW && ultimoEstadoBoton2 == HIGH) { 
    if (tiempoActual - tiempoUltimoToque[1] > tiempoDebounce) {
      tiempoUltimoToque[1] = tiempoActual;
      nuevoModo = 2;
    }
  }
  ultimoEstadoBoton2 = estadoBoton2;

  // --- Procesar Botón 3 (Pulse) ---
  int estadoBoton3 = digitalRead(boton3Pin);
  if (estadoBoton3 == LOW && ultimoEstadoBoton3 == HIGH) { 
    if (tiempoActual - tiempoUltimoToque[2] > tiempoDebounce) {
      tiempoUltimoToque[2] = tiempoActual;
      nuevoModo = 3;
    }
  }
  ultimoEstadoBoton3 = estadoBoton3;


  // --- Lógica de Cambio de Modo / Interrupción ---
  if (nuevoModo != 0) {
    // Si se presiona el botón, siempre se INTERRUMPE el modo anterior y se inicia el nuevo.
    // Si se presiona el mismo botón, el modo es interrumpido y se apaga (modo 0).
    if (modoActual == nuevoModo) {
        modoActual = 0; // Interrumpir y apagar
    } else {
        modoActual = nuevoModo; // Cambiar al nuevo modo
    }

    // Al cambiar o interrumpir, REINICIAMOS TODAS las variables de las secuencias
    tiempoAnteriorBlink = millis();
    contadorBlinks = 0;
    estadoLedBlink = LOW;
    
    tiempoAnteriorSOS = millis();
    pasoSOS = 0;
    
    tiempoAnteriorPulse = millis();
    pasoPulse = 0;
    intensidad = 0;
    contadorPulsos = 0;
    
    // Aseguramos que el LED esté apagado si el resultado fue una interrupción/apagado.
    if (modoActual == 0) {
        digitalWrite(ledPin, LOW);
    }
  }
}

// --- Setup y Loop ---

void setup() {
  // CONFIGURACIÓN DE PINES: Usamos INPUT_PULLUP
  // Esto hace que el pin esté HIGH por defecto y LOW al presionar el botón.
  pinMode(ledPin, OUTPUT);
  pinMode(boton1Pin, INPUT_PULLUP); 
  pinMode(boton2Pin, INPUT_PULLUP);
  pinMode(boton3Pin, INPUT_PULLUP);
  digitalWrite(ledPin, LOW);
}

void loop() {
  // 1. CHEQUEO CONSTANTE DE INTERRUPCIONES (NO BLOQUEANTE)
  leerPulsadores();

  // 2. EJECUCIÓN DEL MODO ACTUAL (NO BLOQUEANTE)
  switch (modoActual) {
    case 1:
      hacerSOS(); // Interrumpible
      break;
    case 2:
      hacerBlink(); // Interrumpible, se apaga solo
      break;
    case 3:
      hacerPulse(); // Interrumpible, se apaga solo
      break;
    default:
      digitalWrite(ledPin, LOW);
      break;
  }
}