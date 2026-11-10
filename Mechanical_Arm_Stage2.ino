#include <Servo.h>

// Servos (arriba hacia abajo)
Servo servo1; // Garra (Rango de 35 --> 145 Grados)
Servo servo2; // Se mantendrá en 35 todo el tiempo
Servo servo4;
Servo servo5;
Servo servo6;   // Rotación

// Pines
const int PIN_SERVO1 = 3;
const int PIN_SERVO2 = 5;
const int PIN_SERVO4 = 9;
const int PIN_SERVO5 = 10;
const int PIN_SERVO6 = 11;



// CONTROLES

const int PIN_POTENCIOMETRO = A0;
const int PIN_BOTON = 2;

// Límites del Servo 6
const int SERVO6_MIN = 20;
const int SERVO6_MAX = 160;



// ESTADO DEL SISTEMA

// false = garra abierta / sin objeto
// true  = garra cerrada / transportando objeto
bool objetoAgarrado = false;

// Variables para detectar pulsación
bool estadoAnteriorBoton = HIGH;

unsigned long ultimoCambio = 0;
const unsigned long debounce = 200;



// CONTRACCIONES PARA CONTROLAR SERVO 6 (Potenciometro)
const int UMBRAL_CONTRACCION = 818;
const int UMBRAL_REPOSO = 512;

bool contraccionActiva = false;

int posicionActual = 0;

// 1 = avanzar de posición 0 hacia posición 3
// -1 = regresar de posición 3 hacia posición 0
int direccion = 1;

const int posicionesServo6[4] = {
  20,62,104,146
};



// VOIDS DE POSICIÓN

void posicionEncorvada() {
// Servo 1 depende de estado de boton
  servo4.write(180);
  servo5.write(115);
//Servo 6 no se mueve aqui
}

void posicionAgarre() {
// Servo 1 depende de estado de boton
  servo4.write(180);
  servo5.write(55);
//Servo 6 no se mueve aqui
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  pinMode(PIN_BOTON, INPUT_PULLUP);

  // Conectar servos
  servo1.attach(PIN_SERVO1);
  servo2.attach(PIN_SERVO2);
  servo4.attach(PIN_SERVO4);
  servo5.attach(PIN_SERVO5);
  servo6.attach(PIN_SERVO6);


  // POSICIÓN INICIAL (Verticalmente)
  servo1.write(90);
  servo2.write(35); //Constante siempre
  servo4.write(70);
  servo5.write(90);
  servo6.write(posicionesServo6[0]);

  delay(2000);
}



// =====================================================
// LOOP
// =====================================================

void loop() {

  // ===================================================
  // CONTROL DEL SERVO 6 POR CONTRACCIONES
  // ===================================================

  int lecturaPot = analogRead(PIN_POTENCIOMETRO);

  // Una contracción se activa al superar aproximadamente 80%
  if (lecturaPot >= UMBRAL_CONTRACCION &&
      contraccionActiva == false) {

    contraccionActiva = true;

    // Cambiar una posición en la dirección actual
    posicionActual = posicionActual + direccion;

    // Al llegar a posición 3, la siguiente contracción será de regreso
    if (posicionActual >= 3) {
      posicionActual = 3;
      direccion = -1;
    }

    // Al llegar a posición 0, la siguiente contracción será de ida
    else if (posicionActual <= 0) {
      posicionActual = 0;
      direccion = 1;
    }

    servo6.write(posicionesServo6[posicionActual]);
  }
  

  // Debe bajar aproximadamente de 50% antes de aceptar otra contracción
  if (lecturaPot <= UMBRAL_REPOSO) {
    contraccionActiva = false;
  }


  // ===================================================
  // LEER BOTÓN
  // ===================================================

  bool estadoBoton = digitalRead(PIN_BOTON);

  // Detectar una nueva pulsación
  if (estadoAnteriorBoton == HIGH &&
      estadoBoton == LOW &&
      millis() - ultimoCambio > debounce) {

    ultimoCambio = millis();


    // =================================================
    // CASO 1
    // NO TENEMOS OBJETO -> AGARRAR
    // =================================================

    if (objetoAgarrado == false) {

      // Esperar 5 segundos antes de bajar
      delay(5000);

      // BAJAR A POSICIÓN DE AGARRE
      posicionAgarre();


      // Esperar a que llegue
      delay(1000);

      // CERRAR GARRA
      servo1.write(155);

      objetoAgarrado = true;

      // Esperar a que cierre
      delay(1000);


      // REGRESAR A POSICIÓN ENCORVADA
      // CON LA GARRA CERRADA

      posicionEncorvada();

      delay(2000);
    }


    // =================================================
    // CASO 2
    // TENEMOS OBJETO -> SOLTAR
    // =================================================

    else {

      // BAJAR A POSICIÓN DE AGARRE
      posicionAgarre();

      // Esperar a que llegue
      delay(2000);

      // ABRIR GARRA
      servo1.write(90);

      objetoAgarrado = false;

      // Esperar a que abra
      delay(2000);


      // REGRESAR A POSICIÓN ENCORVADA
      // CON LA GARRA ABIERTA
      posicionEncorvada();

      delay(2000);
    }
  }


  estadoAnteriorBoton = estadoBoton;

  delay(15);
}