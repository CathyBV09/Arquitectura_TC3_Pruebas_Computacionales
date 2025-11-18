// --- Pines ---
const int boton1Pin = 2; // SOS
const int boton2Pin = 3; // Blink
const int boton3Pin = 4; // Pulse
const int ledPin = 9;    // LED

// --- Estados ---
int modoActual = 0; // 0: apagado, 1: SOS, 2: Blink, 3: Pulse
int modoAnterior = 0;
bool modoPausado = false;

// --- SOS ---
unsigned long tiempoAnteriorSOS = 0;
int pasoSOS = 0;
bool cicloSOSCompletado = false;
const int corto = 200;
const int largo = 600;
const int pausa = 200;
const int pausaLetra = 400;
const int pausaFinal = 1000;
long intervaloSOS = 0;
bool sosActivo = false;

// --- Blink ---
unsigned long tiempoAnteriorBlink = 0;
const long intervaloBlink = 500;
int estadoLedBlink = LOW;
int contadorBlinks = 0;
const int MAX_BLINKS = 14;

// --- Pulse ---
unsigned long tiempoAnteriorPulse = 0;
int pasoPulse = 0;
int intensidadPulse = 0;
const int tiempoPasoPulse = 10;
const int MAX_REPETICIONES_PULSE = 14;
int contadorPulse = 0;

// --- Botones ---
int ultimoEstadoBoton1 = LOW;
int ultimoEstadoBoton2 = LOW;
int ultimoEstadoBoton3 = LOW;

// --- Funciones auxiliares ---
String nombreModo(int modo) {
  switch (modo) {
    case 1: return "SOS";
    case 2: return "Blink";
    case 3: return "Pulse";
    default: return "Apagado";
  }
}

void apagarLED() {
  digitalWrite(ledPin, LOW);
}

// --- Guardar el estado actual (pausar) ---
void pausarModoActual() {
  modoAnterior = modoActual;
  modoPausado = true;
  apagarLED();
  Serial.print("[PAUSA] ");
  Serial.println(nombreModo(modoAnterior));
}

// --- Restaurar modo anterior (reanudar) ---
void reanudarModoAnterior() {
  if (modoPausado) {
    modoActual = modoAnterior;
    modoPausado = false;
    Serial.print("[REANUDA] ");
    Serial.println(nombreModo(modoActual));
  }
}

// --- Reset completo de variables (solo si termina) ---
void resetCompleto() {
  pasoSOS = 0;
  cicloSOSCompletado = false;
  sosActivo = false;
  contadorBlinks = 0;
  contadorPulse = 0;
  pasoPulse = 0;
  intensidadPulse = 0;
  apagarLED();
}

// --- SOS ---
void hacerSOS() {
  unsigned long t = millis();

  if (!sosActivo) {
    sosActivo = true;
    pasoSOS = (pasoSOS == 0) ? 1 : pasoSOS; // Si estaba pausado, sigue desde ahí
    tiempoAnteriorSOS = t;
    intervaloSOS = corto;
  }

  if (pasoSOS > 11) {
    cicloSOSCompletado = true;
    sosActivo = false;
    return;
  }

  if (t - tiempoAnteriorSOS >= intervaloSOS) {
    tiempoAnteriorSOS = t;

    if (pasoSOS % 2 != 0) {
      digitalWrite(ledPin, HIGH);
      if (pasoSOS <= 3 || pasoSOS >= 7) intervaloSOS = corto;
      else if (pasoSOS <= 6) intervaloSOS = largo;
      pasoSOS++;
    } else {
      digitalWrite(ledPin, LOW);
      if (pasoSOS == 6 || pasoSOS == 10) intervaloSOS = pausaLetra;
      else if (pasoSOS < 10) intervaloSOS = pausa;
      else intervaloSOS = pausaFinal;
      pasoSOS++;
    }
  }
}

// --- Blink ---
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
  }
}

// --- Pulse ---
void hacerPulse() {
  unsigned long tiempoActual = millis();
  if (contadorPulse >= MAX_REPETICIONES_PULSE) {
    digitalWrite(ledPin, LOW);
    return;
  }

  if (tiempoActual - tiempoAnteriorPulse >= tiempoPasoPulse) {
    tiempoAnteriorPulse = tiempoActual;

    if (pasoPulse == 0) {
      intensidadPulse = 0;
      pasoPulse = 1;
    }

    if (pasoPulse == 1) {
      intensidadPulse += 5;
      if (intensidadPulse >= 255) {
        pasoPulse = 2;
        contadorPulse++;
      }
    } else {
      intensidadPulse -= 5;
      if (intensidadPulse <= 0) {
        pasoPulse = 1;
        contadorPulse++;
      }
    }
    analogWrite(ledPin, intensidadPulse);
  }
}

// --- Setup ---
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(boton1Pin, INPUT);
  pinMode(boton2Pin, INPUT);
  pinMode(boton3Pin, INPUT);

  Serial.begin(9600);
  Serial.println("=== Sistema iniciado ===");
  Serial.println("Botones: 1 (SOS), 2 (Blink), 3 (Pulse)");
}

// --- Loop ---
void loop() {
  int estadoBoton1 = digitalRead(boton1Pin);
  int estadoBoton2 = digitalRead(boton2Pin);
  int estadoBoton3 = digitalRead(boton3Pin);

  // --- Detectar flanco positivo ---
  if (estadoBoton1 == HIGH && ultimoEstadoBoton1 == LOW) {
    if (modoActual != 0 && modoActual != 1) pausarModoActual();
    modoActual = 1;
    Serial.println("[INICIA] SOS");
  }
  ultimoEstadoBoton1 = estadoBoton1;

  if (estadoBoton2 == HIGH && ultimoEstadoBoton2 == LOW) {
    if (modoActual != 0 && modoActual != 2) pausarModoActual();
    modoActual = 2;
    Serial.println("[INICIA] Blink");
  }
  ultimoEstadoBoton2 = estadoBoton2;

  if (estadoBoton3 == HIGH && ultimoEstadoBoton3 == LOW) {
    if (modoActual != 0 && modoActual != 3) pausarModoActual();
    modoActual = 3;
    Serial.println("[INICIA] Pulse");
  }
  ultimoEstadoBoton3 = estadoBoton3;

  // --- Ejecución de modos ---
  switch (modoActual) {
    case 1:
      hacerSOS();
      if (cicloSOSCompletado) {
        Serial.println("[FIN] SOS");
        resetCompleto();
        if (modoPausado) reanudarModoAnterior();
        else modoActual = 0;
      }
      break;

    case 2:
      hacerBlink();
      if (contadorBlinks >= MAX_BLINKS) {
        Serial.println("[FIN] Blink");
        resetCompleto();
        if (modoPausado) reanudarModoAnterior();
        else modoActual = 0;
      }
      break;

    case 3:
      hacerPulse();
      if (contadorPulse >= MAX_REPETICIONES_PULSE) {
        Serial.println("[FIN] Pulse");
        resetCompleto();
        if (modoPausado) reanudarModoAnterior();
        else modoActual = 0;
      }
      break;

    default:
      digitalWrite(ledPin, LOW);
      break;
  }
}
