/*******************************************************/
/*      _- D I G I T A L   A C   S W I T C H E R -_    */
/*                                                     */
/*    El presente codigo controla de manera digital    */
/*  utilizando relevadores como dispositivo de potencia*/
/*  y aislamiento cuatro cargas alimentadas            */
/*  directamente de la toma de corriente CA.           */
/*                                                     */
/*  Caracteristicas:                                   */
/*  +Un boton de control para cada carga.              */
/*  +Un botón de “modo bloqueo” que al ser presionado  */
/*   bloquea todos los controles y los mantiene en su  */
/*   estado siendo posible su uso hasta presionar de   */
/*   nuevo el botón de “modo bloqueo”.                 */
/*  +Botón de “pánico” que desactivara y bloqueara     */
/*   todas de todas las cargas al ser presionado una   */
/*   vez y devolverlas a su estado inicial al ser      */
/*   presionado una vez más.                           */
/*  +Iniciar en modo bloqueo.                          */
/*  +Indicadores visuales con leds.                    */
/*  +Indicador sonoro con buzzer.                      */
/*                                                     */
/*  NOTA: Características sensibles a cambios          */
/*        según la experiencia de uso.                 */
/*                                                     */
/*  Autor: Jesus David Salazar Garcia. Aka: mataperros */
/*  21/06/2017                                         */
/*******************************************************/

// TODO Cambiar el pin 13 hacia otro pin libre.


// Librerias.
#include <Arduino.h>
#include <avr/sleep.h>

// Definiciones -----------------------------------------
// Configuracion.
#define INTERRUPT_AT_START // Descomentar para iniciar con interrupcion.
// Pines para botones.
#define BOTON_A     6
#define BOTON_B     7
#define BOTON_C     8
#define BOTON_D     9
#define BOTON_STOP  10
// Indicadores.
#define INDICADOR_A A0
#define INDICADOR_B 13
#define INDICADOR_C 12
#define INDICADOR_D A2
#define INDICADOR_INTERRUPT_ENABLED  A4
#define INDICADOR_INTERRUPT_DISABLED  A5
#define INDICADOR_STOP  A3
// Duracion parpadeo para estado de panico (ms).
#define PARPADEO_STOP 450
// Pines para reles.
#define RELAY_A     A1
#define RELAY_B     3
#define RELAY_C     4
#define RELAY_D     5
// Pin para buzzer.
#define BUZZER      11
// Pin para interrupcion.
#define INTERRUPT   2

// Prototipos -------------------------------------------
void interrupthandler();

// Variables globales -----------------------------------
// Banderas para conmutar estados.
bool aFlag, bFlag, cFlag, dFlag, stopFlag, iFlag;
// Variable para interrupcion al inicio por soft.
// Configurar en definiciones.
#ifdef INTERRUPT_AT_START
  bool softInterrupt = true;
#else
  bool softInterrupt = false;
#endif
// Bandera para parpadeo en panico usando millis.
bool stateStop = LOW;
// Almacenes de tiempo obtenido con millis.
unsigned long millisAnterior = 0;
unsigned long millisActual  = 0;


void setup() { //----------------------------------------
  // Configuracion de E/S pines -------------------------
  // Entradas para botones con pullup's habilitados
  pinMode(BOTON_A, INPUT_PULLUP);
  pinMode(BOTON_B, INPUT_PULLUP);
  pinMode(BOTON_C, INPUT_PULLUP);
  pinMode(BOTON_D, INPUT_PULLUP);
  pinMode(INTERRUPT, INPUT_PULLUP);
  pinMode(BOTON_STOP, INPUT_PULLUP);
  // Salidas para indicaores-
  pinMode(INDICADOR_A, OUTPUT);
  pinMode(INDICADOR_B, OUTPUT);
  pinMode(INDICADOR_C, OUTPUT);
  pinMode(INDICADOR_D, OUTPUT);
  pinMode(INDICADOR_INTERRUPT_ENABLED, OUTPUT);
  pinMode(INDICADOR_INTERRUPT_DISABLED, OUTPUT);
  pinMode(INDICADOR_STOP, OUTPUT);
  // Salidas para reles.
  pinMode(RELAY_A, OUTPUT);
  pinMode(RELAY_B, OUTPUT);
  pinMode(RELAY_C, OUTPUT);
  pinMode(RELAY_D, OUTPUT);
  // Salida para buzzer.
  pinMode(BUZZER, OUTPUT);

  // Inicializando indicaores al inicio -----------------
  digitalWrite(INDICADOR_A, LOW);
  digitalWrite(INDICADOR_D, LOW);
  digitalWrite(INDICADOR_C, LOW);
  digitalWrite(INDICADOR_B, LOW);
  digitalWrite(INDICADOR_INTERRUPT_ENABLED, LOW);
  digitalWrite(INDICADOR_INTERRUPT_DISABLED, HIGH);
  digitalWrite(INDICADOR_STOP, LOW);

  // Inicializando reles en al incio --------------------
  digitalWrite(RELAY_A, HIGH);
  digitalWrite(RELAY_D, HIGH);
  digitalWrite(RELAY_C, HIGH);
  digitalWrite(RELAY_B, HIGH);

  // Inicializando variables ----------------------------
  // Banderas para conmutar estados.
  aFlag = bFlag = cFlag = dFlag = stopFlag = iFlag = false;
  // Bandera y variables para parpadeo con millis.
  stateStop = LOW;
  millisAnterior = 0;
  millisActual  = 0;

  // Configurando interrupcion --------------------------
  // Estableciondo modo power down.
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // Definiendo pin para interrupcion, funcion para salir
  // de interrupcion y establecion el cambio de estado en
  // cuando se conmute de estado  alto a bajo.
  attachInterrupt(digitalPinToInterrupt(INTERRUPT), interrupthandler, FALLING);

  // Delay para asegurar la configuracion inicial.
  delay(300);
}

void loop() { // ----------------------------------------

  // Escanenado interrupcion ----------------------------
  //Serial.println(digitalRead(INTERRUPT));
  //Serial.println(iFlag);
  //Serial.println("Fuera");
  if((!digitalRead(INTERRUPT) && !iFlag) || softInterrupt) {
    while (!digitalRead(INTERRUPT)) { /*Serial.println("En while");*/}

    // Conmutando bandera de estado.
    iFlag = !iFlag;

    // Si se hizo la interrupcion desde el programa
    // (softInterrupt) se desactiva.
    if(softInterrupt) {
      softInterrupt = !softInterrupt;
    }

    // Alarma buzzer (Sirve como delay 300ms).
    tone(BUZZER, 1000);
    delay(10);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 1000);
    delay(10);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 1000);
    delay(10);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 1000);
    delay(10);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 1000);
    delay(10);
    noTone(BUZZER);
    delay(50);

    // Conmutando indicadores.
    digitalWrite(INDICADOR_INTERRUPT_DISABLED, LOW);
    digitalWrite(INDICADOR_INTERRUPT_ENABLED, HIGH);

    // Inicializando y activando power down.
    sleep_enable();
    sleep_mode();
  }

  // Escaneando boton de panico -------------------------
  if(!digitalRead(BOTON_STOP)) {
    while(!digitalRead(BOTON_STOP)) { }

    // Encendiendo indicador y activando buzzer.
    digitalWrite(INDICADOR_STOP, !stopFlag);
    tone(BUZZER, 1500);
    delay(10);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 1500);
    delay(10);
    noTone(BUZZER);

    // En caso de entrar con la bandera desactivada
    // se apagan todos reles con sus respectivos
    // indicadorees.
    if(!stopFlag) {
      digitalWrite(INDICADOR_A, LOW);
      digitalWrite(RELAY_A, HIGH);

      digitalWrite(INDICADOR_B, LOW);
      digitalWrite(RELAY_B, HIGH);

      digitalWrite(INDICADOR_C, LOW);
      digitalWrite(RELAY_C, HIGH);

      digitalWrite(INDICADOR_D, LOW);
      digitalWrite(RELAY_D, HIGH);
    }

    // Si se entra con la bandera activada se
    // esta regresando del modo panico por lo
    // que los reles e indicadores vuelven a
    // los estados que tenian antes de presionar
    // el boton de panico.
    else if(stopFlag) {
      digitalWrite(INDICADOR_A, aFlag);
      digitalWrite(RELAY_A, !aFlag);

      digitalWrite(INDICADOR_B, bFlag);
      digitalWrite(RELAY_B, !bFlag);

      digitalWrite(INDICADOR_C, cFlag);
      digitalWrite(RELAY_C, !cFlag);

      digitalWrite(INDICADOR_D, dFlag);
      digitalWrite(RELAY_D, !dFlag);
    }

    // Si se presiona el boton para entrar en modo
    // panico se inhabilita el funcionamiento de
    // aculaquier otro boton hasta no salir del
    // modo panico.
    while(digitalRead(BOTON_STOP) && !stopFlag) {
      //Serial.println("En stop");
      // Parpadeo utilizndo la funcion millis para
      // seguir al pendiente del estado del boton de
      // panico y salir del modo cuando se presione.
      millisActual = millis();
      if((millisActual - millisAnterior) >= PARPADEO_STOP) {
        millisAnterior = millisActual;
        if(stateStop == LOW) {
          stateStop = HIGH;
        }
        else {
          stateStop = LOW;
        }
        digitalWrite(INDICADOR_STOP, stateStop);
      }
    }
    digitalWrite(INDICADOR_STOP, !stopFlag);

    // Conmutando bandera para indicar que se sale de
    // modo panico.
    stopFlag = !stopFlag;
  }

  // Escaneando el estado del boton A -------------------
  if(!digitalRead(BOTON_A)) {
    while (!digitalRead(BOTON_A)) { }
    // Conmutando bandera de estado.
    aFlag = !aFlag;
    // Utilizando bandera para activar indicador y rele.
    digitalWrite(INDICADOR_A, aFlag);
    digitalWrite(RELAY_A, !aFlag);
    // Alarma en buzzer.
    tone(BUZZER, 900);
    delay(10);
    noTone(BUZZER);
    delay(50);
  }

  // Escaneando el estado del boton B -------------------
  if(!digitalRead(BOTON_B)) {
    while (!digitalRead(BOTON_B)) { }
    bFlag = !bFlag;
    digitalWrite(INDICADOR_B, bFlag);
    digitalWrite(RELAY_B, !bFlag);
    tone(BUZZER, 900);
    delay(10);
    noTone(BUZZER);
    delay(50);
  }

  // Escaneando el estado del boton C -------------------
  if(!digitalRead(BOTON_C)) {
    while (!digitalRead(BOTON_C)) { }
    cFlag = !cFlag;
    digitalWrite(INDICADOR_C, cFlag);
    digitalWrite(RELAY_C, !cFlag);
    tone(BUZZER, 900);
    delay(10);
    noTone(BUZZER);
    delay(50);
  }

  // Escaneando el estado del boton D -------------------
  if(!digitalRead(BOTON_D)) {
    while (!digitalRead(BOTON_D)) { }
    dFlag = !dFlag;
    digitalWrite(INDICADOR_D, dFlag);
    digitalWrite(RELAY_D, !dFlag);
    tone(BUZZER, 900);
    delay(10);
    noTone(BUZZER);
    delay(50);
  }

  // Regreso de interrupcion ----------------------------
  // Si la bandera de intrrupcion esta
  // activada se ejecuta este bloque.
  if(iFlag) {
    // Alarma buzzer. (Sireve como delay 300ms).
    tone(BUZZER, 1000);
    delay(10);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 1000);
    delay(10);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 1000);
    delay(10);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 1000);
    delay(10);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 1000);
    delay(10);
    noTone(BUZZER);
    delay(50);

    // Hasta que no se suelte el boton se estara en este bucle.
    while (!digitalRead(INTERRUPT)) {/*Serial.println("Suelta el boton");*/}

    // Conmutando indicadores.
    digitalWrite(INDICADOR_INTERRUPT_ENABLED, LOW);
    digitalWrite(INDICADOR_INTERRUPT_DISABLED, HIGH);

    // Conmutando bandera.
    iFlag =! iFlag;
  }
}

// Definiciones de funciones ----------------------------
// Funciona para desactivar interrupcion.
void interrupthandler() {
  sleep_disable();
  //Serial.println("Regresando");
  //Serial.println(iFlag);
}
