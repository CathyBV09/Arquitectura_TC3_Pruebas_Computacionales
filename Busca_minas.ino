const int NUM_CELDAS = 6;
// pines de los botones (Entradas 2 a 7)
const int BOTONES_PIN[NUM_CELDAS] = {2, 3, 4, 5, 6, 7}; 
// pines de los LEDs (Salidas 8 a 13)
const int LEDS_PIN[NUM_CELDAS] = {8, 9, 10, 11, 12, 13}; 

// variables del juego
const int MAX_OPORTUNIDADES = 3;
int oportunidadesRestantes = MAX_OPORTUNIDADES;
int posicionMina; // la celda correcta (0 a 5)
bool juegoActivo = true;


// --- 2. SETUP: CONFIGURACIÓN INICIAL

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0)); // mejor semilla aleatoria

  // configuracion de pines
  for (int i = 0; i < NUM_CELDAS; i++) {
    // botones como entrada
    pinMode(BOTONES_PIN[i], INPUT); 
    // LEDs como salida
    pinMode(LEDS_PIN[i], OUTPUT);
  }

  iniciarJuego();
}

void iniciarJuego() {
  // apagar todos los LEDs al inicio
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
}


// LOOP: LÓGICA DEL JUEGO


void loop() {
  if (!juegoActivo) {
    // si el juego a terminado, esperamos un momento
    delay(500); 
    
    // si se perdio, esperamos un boton para reiniciar (reinicio manual)
    if (oportunidadesRestantes <= 0 && algunBotonPresionado()) {
        iniciarJuego();
    } 
    // si se gano, se reinicia inmediatamente (reinicio automatico)
    else if (oportunidadesRestantes > 0) {
       
        iniciarJuego(); 
    }
    
    return;
  }


  for (int i = 0; i < NUM_CELDAS; i++) {
  
    if (digitalRead(BOTONES_PIN[i]) == HIGH) {
      delay(50); 
      if (digitalRead(BOTONES_PIN[i]) == HIGH) {
        realizarIntento(i); // i es la celda seleccionada (0, 1, 2, 3, 4, 5)
      }
      
    
      while (digitalRead(BOTONES_PIN[i]) == HIGH) {
        delay(10);
      }
    }
  }
}

void realizarIntento(int intento) {
  if (intento == posicionMina) {
    // GANASTE
    Serial.println("\n¡FELICIDADES! Encontraste la mina.");
    mostrarVictoria(intento); // Ejecuta la secuencia de LEDs
    juegoActivo = false;
  } else {
    // FALLASTE
    oportunidadesRestantes--;
    Serial.print("\nFallaste. Te quedan ");
    Serial.print(oportunidadesRestantes);
    Serial.println(" oportunidades.");
    mostrarFallo(intento);

    if (oportunidadesRestantes <= 0) {
      // PERDISTE
      Serial.println("\nGAME OVER. Se acabaron tus oportunidades.");
      Serial.print("La mina estaba en el pin: ");
      Serial.println(LEDS_PIN[posicionMina]);
      mostrarDerrota(posicionMina);
      juegoActivo = false;
    }
  }
}


// --- 4. visualizacion

void mostrarFallo(int celda) {
  // parpadeo del LED fallido
  digitalWrite(LEDS_PIN[celda], HIGH);
  delay(150);
  digitalWrite(LEDS_PIN[celda], LOW);
  delay(150);
  digitalWrite(LEDS_PIN[celda], HIGH);
  delay(150);
  digitalWrite(LEDS_PIN[celda], LOW);
}

void mostrarVictoria(int celdaMina) {
  // encender todos los LEDs
  for(int j = 0; j < NUM_CELDAS; j++){
      digitalWrite(LEDS_PIN[j], HIGH);
  }
  delay(500);

  // parpadeo de todos los LEDs
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
  // apagar todos los LEDs para prepararse para el reinicio
  for(int j = 0; j < NUM_CELDAS; j++){
      digitalWrite(LEDS_PIN[j], LOW);
  }
}

void mostrarDerrota(int celdaMina) {
  // la posicion de la mina (parpadeo rapido)
  for (int i = 0; i < 8; i++) {
    digitalWrite(LEDS_PIN[celdaMina], HIGH);
    delay(100);
    digitalWrite(LEDS_PIN[celdaMina], LOW);
    delay(100);
  }
}

// para reiniciar el juego si se perdió
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
