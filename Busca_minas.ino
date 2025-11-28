const int NUM_CELDAS = 6;
// Pines de los botones (Entradas 2 a 7)
const int BOTONES_PIN[NUM_CELDAS] = {2, 3, 4, 5, 6, 7}; 
// Pines de los LEDs (Salidas 8 a 13)
const int LEDS_PIN[NUM_CELDAS] = {8, 9, 10, 11, 12, 13}; 

// Variables del Juego
const int MAX_OPORTUNIDADES = 3;
int oportunidadesRestantes = MAX_OPORTUNIDADES;
int posicionMina; // La celda correcta (0 a 5)
bool juegoActivo = true;


// --- 2. SETUP: CONFIGURACIÓN INICIAL ---
// -----------------------------------------

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0)); // Mejor semilla aleatoria

  // Configuración de Pines
  for (int i = 0; i < NUM_CELDAS; i++) {
    // Botones como entrada (usando pull-down físico, detecta HIGH al presionar)
    pinMode(BOTONES_PIN[i], INPUT); 
    // LEDs como salida
    pinMode(LEDS_PIN[i], OUTPUT);
  }

  iniciarJuego();
}

void iniciarJuego() {
  // Apagar todos los LEDs al inicio
  for (int i = 0; i < NUM_CELDAS; i++) {
    digitalWrite(LEDS_PIN[i], LOW);
  }

  posicionMina = random(0, NUM_CELDAS); // Mina en posición 0 a 5
  oportunidadesRestantes = MAX_OPORTUNIDADES;
  juegoActivo = true;
  
  Serial.println("\n--- NUEVO JUEGO: BUSCAMINAS INVERSO ---");
  Serial.print("Mina oculta. Tienes ");
  Serial.print(MAX_OPORTUNIDADES);
  Serial.println(" oportunidades.");
  Serial.println("Presiona un boton (Pines 2 al 7) para intentar.");
  // Si deseas ver la solución para pruebas, descomenta la siguiente línea:
  // Serial.print("DEBUG: Mina en posicion (indice 0-5): "); Serial.println(posicionMina); 
}


// --- 3. LOOP: LÓGICA DEL JUEGO ---
// ----------------------------------

void loop() {
  if (!juegoActivo) {
    // Si el juego ha terminado, esperamos un momento
    delay(500); 
    
    // Si se perdió, esperamos un botón para reiniciar (reinicio manual)
    if (oportunidadesRestantes <= 0 && algunBotonPresionado()) {
        iniciarJuego();
    } 
    // Si se ganó, se reinicia inmediatamente (reinicio automático)
    else if (oportunidadesRestantes > 0) {
        // La función mostrarVictoria ya se encargó de la pausa y la señalización
        iniciarJuego(); 
    }
    
    return;
  }

  // Recorrer los 6 botones (índices 0 a 5)
  for (int i = 0; i < NUM_CELDAS; i++) {
    // Si el botón está presionado (HIGH debido al pull-down)
    if (digitalRead(BOTONES_PIN[i]) == HIGH) {
      delay(50); // Debounce
      if (digitalRead(BOTONES_PIN[i]) == HIGH) { // Confirmar la pulsación
        realizarIntento(i); // i es la celda seleccionada (0, 1, 2, 3, 4, 5)
      }
      
      // Esperar a que el botón se suelte para evitar múltiples pulsaciones
      while (digitalRead(BOTONES_PIN[i]) == HIGH) {
        delay(10);
      }
    }
  }
}

void realizarIntento(int intento) {
  if (intento == posicionMina) {
    // *** ¡GANASTE! ***
    Serial.println("\n¡FELICIDADES! Encontraste la mina.");
    mostrarVictoria(intento); // Ejecuta la secuencia de LEDs
    juegoActivo = false;
  } else {
    // *** ¡FALLASTE! ***
    oportunidadesRestantes--;
    Serial.print("\nFallaste. Te quedan ");
    Serial.print(oportunidadesRestantes);
    Serial.println(" oportunidades.");
    mostrarFallo(intento);

    if (oportunidadesRestantes <= 0) {
      // *** ¡PERDISTE! ***
      Serial.println("\nGAME OVER. Se acabaron tus oportunidades.");
      Serial.print("La mina estaba en el pin: ");
      Serial.println(LEDS_PIN[posicionMina]);
      mostrarDerrota(posicionMina);
      juegoActivo = false;
    }
  }
}


// --- 4. FUNCIONES DE VISUALIZACIÓN (FEEDBACK) ---
// --------------------------------------------------

void mostrarFallo(int celda) {
  // Parpadeo del LED fallido
  digitalWrite(LEDS_PIN[celda], HIGH);
  delay(150);
  digitalWrite(LEDS_PIN[celda], LOW);
  delay(150);
  digitalWrite(LEDS_PIN[celda], HIGH);
  delay(150);
  digitalWrite(LEDS_PIN[celda], LOW);
}

void mostrarVictoria(int celdaMina) {
  // 1. Encender todos los LEDs
  for(int j = 0; j < NUM_CELDAS; j++){
      digitalWrite(LEDS_PIN[j], HIGH);
  }
  delay(500); // Se mantienen encendidos medio segundo

  // 2. Parpadeo de todos los LEDs
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < NUM_CELDAS; j++){
      digitalWrite(LEDS_PIN[j], LOW);
    }
    delay(200);
    for(int j = 0; j < NUM_CELDAS; j++){
      digitalWrite(LEDS_PIN[j], HIGH);
    }
    delay(200);
  }
  // 3. Apagar todos los LEDs para prepararse para el reinicio
  for(int j = 0; j < NUM_CELDAS; j++){
      digitalWrite(LEDS_PIN[j], LOW);
  }
}

void mostrarDerrota(int celdaMina) {
  // Mostrar la posición de la mina (parpadeo rápido)
  for (int i = 0; i < 8; i++) {
    digitalWrite(LEDS_PIN[celdaMina], HIGH);
    delay(100);
    digitalWrite(LEDS_PIN[celdaMina], LOW);
    delay(100);
  }
}

// Función auxiliar para reiniciar el juego si se perdió
bool algunBotonPresionado() {
  for (int i = 0; i < NUM_CELDAS; i++) {
    if (digitalRead(BOTONES_PIN[i]) == HIGH) {
      delay(50); // Debounce
      if (digitalRead(BOTONES_PIN[i]) == HIGH) {
        return true;
      }
    }
  }
  return false;
}