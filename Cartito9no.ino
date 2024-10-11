// Pines del sensor de ultrasonido
const int EchoPin = 4;   // GPIO4 para el Echo
const int TriggerPin = 16; // GPIO16 para el Trigger

// Pines de control para los motores
const int input1 = 33, input2 = 25, input3 = 26, input4 = 27;
const int enableA = 14, enableB = 12;

// Variables de control
const int distanciaDeteccion = 30;  // Distancia límite de detección en cm

void setup() {
  Serial.begin(9600);   
  pinMode(TriggerPin, OUTPUT); pinMode(EchoPin, INPUT);

  // Configurar pines de los motores
  pinMode(enableA, OUTPUT); pinMode(enableB, OUTPUT);
  pinMode(input1, OUTPUT); pinMode(input2, OUTPUT);
  pinMode(input3, OUTPUT); pinMode(input4, OUTPUT);

  pararMotores();
}

void loop() {
  float distancia = medirDistanciaCM();
  Serial.print("Distancia detectada: "); Serial.print(distancia); Serial.println(" cm");

  if (distancia > 0 && distancia <= distanciaDeteccion) {
    pararMotores();  // Detenerse de inmediato al detectar un obstáculo
    delay(500);

    // Gira a la derecha para revisar si hay obstáculos
    giroEsquinadoDerecha();  
    delay(500);  // Espera un momento para hacer el giro
    float distanciaDerecha = medirDistanciaCM();  // Mide distancia al lado derecho
    Serial.print("Distancia derecha: "); Serial.println(distanciaDerecha);

    if (distanciaDerecha > distanciaDeteccion) {
      // Si no hay obstáculos a la derecha, avanzar hacia la derecha
      avanzar(255);
    } else {
      // Si hay obstáculos a la derecha, girar a la izquierda para revisar
      pararMotores();
      delay(500);
      giroEsquinadoIzquierda();  
      delay(500);  // Espera un momento para hacer el giro
      float distanciaIzquierda = medirDistanciaCM();  // Mide distancia al lado izquierdo
      Serial.print("Distancia izquierda: "); Serial.println(distanciaIzquierda);

      if (distanciaIzquierda > distanciaDeteccion) {
        // Si no hay obstáculos a la izquierda, avanzar hacia la izquierda
        avanzar(255);
      } else {
        // Si hay obstáculos a ambos lados, girar hacia el lado contrario y avanzar
        pararMotores();
        delay(500);
        Serial.println("Obstáculos en ambos lados, girando y avanzando en el lado contrario...");
        giroSobreEje();  // Gira sobre su propio eje para cambiar de dirección
        delay(500);
        avanzar(255);  // Avanza después del giro
      }
    }
  } else {
    avanzar(255);  // Avanza normalmente si no hay obstáculo
  }

  delay(100);
}

// Medir la distancia con el sensor de ultrasonido
float medirDistanciaCM() {
  digitalWrite(TriggerPin, LOW); delayMicroseconds(2);
  digitalWrite(TriggerPin, HIGH); delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  unsigned long duracion = pulseIn(EchoPin, HIGH, 30000);
  return (duracion == 0) ? -1 : (duracion * 0.0343) / 2;
}

// Avanzar en el sentido de las manecillas del reloj
void avanzar(int velocidad) {
  Serial.println("Avanzando en sentido de las manecillas del reloj...");
  while (true) {
    float distancia = medirDistanciaCM();
    if (distancia > 0 && distancia <= distanciaDeteccion) {
      pararMotores();  // Detenerse si detecta un obstáculo mientras avanza
      break;  // Salir del bucle si se detecta un obstáculo
    }
    // Avanza correctamente, motor derecho más rápido que el izquierdo
    moverMotores(HIGH, LOW, HIGH, LOW, velocidad, velocidad / 2);  // Motor derecho más lento
    delay(100);
  }
}

// Retroceder en sentido contrario a las manecillas del reloj
void retroceder(int velocidad) {
  Serial.println("Retrocediendo en sentido contrario a las manecillas del reloj...");
  while (true) {
    float distancia = medirDistanciaCM();
    if (distancia > 0 && distancia <= distanciaDeteccion) {
      pararMotores();  // Detenerse si detecta un obstáculo mientras retrocede
      break;  // Salir del bucle si se detecta un obstáculo
    }
    // Retrocede correctamente, motor izquierdo más rápido que el derecho
    moverMotores(LOW, HIGH, LOW, HIGH, velocidad / 2, velocidad);  // Motor derecho más rápido
    delay(100);
  }
}

// Giro sobre su propio eje
void giroSobreEje() {
  Serial.println("Girando sobre su propio eje...");
  moverMotores(LOW, HIGH, HIGH, LOW, 150, 150);  // Motores en direcciones opuestas
}

// Giro esquinado a la derecha
void giroEsquinadoDerecha() {
  Serial.println("Girando esquinado a la derecha...");
  moverMotores(HIGH, LOW, HIGH, LOW, 200, 100);  // El motor derecho gira más lento que el izquierdo
}

// Giro esquinado a la izquierda
void giroEsquinadoIzquierda() {
  Serial.println("Girando esquinado a la izquierda...");
  moverMotores(HIGH, LOW, HIGH, LOW, 100, 200);  // El motor izquierdo gira más lento que el derecho
}

// Parar los motores
void pararMotores() {
  Serial.println("Deteniendo...");
  moverMotores(LOW, LOW, LOW, LOW, 0, 0);
}

// Controlar el movimiento de los motores
void moverMotores(int in1, int in2, int in3, int in4, int velA, int velB) {
  digitalWrite(input1, in1); digitalWrite(input2, in2);
  digitalWrite(input3, in3); digitalWrite(input4, in4);
  analogWrite(enableA, velA); analogWrite(enableB, velB);
}
