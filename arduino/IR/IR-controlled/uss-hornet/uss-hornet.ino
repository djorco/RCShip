/* Codigo IR para manejar el USS Hornet
 *  El codigo contiene accion para manejar con el control remoto lo siguiente:
 *  1. Servo timon - Con movimiento gradual marcando destino con el IR
 *  2. 2 Motores controlados independientemente marcando destino con el IR
 *  3. Controladores de Luces de Bajo Cubierta y en Torre
 */

#include "IRremote.h"
//#include <Servo.h>


/* PINs Declaration */
const int PIN_IR_RECEIVER = 13;
const int PIN_LED_CASCO_INT = 3;
const int PIN_LED_CASCO_EXT  = 4;
const int PIN_LED_TORRE = 12; 
const int PIN_SERVO_TIMON = 11;
const int PIN_MTE_DIR_A = 8;
const int PIN_MTE_DIR_B = 7;
const int PIN_MTB_DIR_A = 10;
const int PIN_MTB_DIR_B = 9;
const int PIN_MTE_EN = 6;
const int PIN_MTB_EN = 5;
const int PIN_BUZZER = 2;



/* Servo Objects */
//Servo svTimon;  // create servo object to control a servo

/* IR Objects*/
IRrecv irrecv(PIN_IR_RECEIVER);     // create instance of 'irrecv'
decode_results irCommand;      // create instance of 'decode_results'

/* Ship intants */
const int TIMON_MIN = 60;
const int TIMON_MAX = 120;
const int MOTOR_MAX = 255;
const int MOTOR_MIN = -255;
const int MOTOR_STEP = 64;
const int TIMON_STEP = 10;
const int TIMON_STOP = 90;

/* Ship Status */
int posTimon = TIMON_STOP;
int velMotorBabor = 0;
int velMotorEstribor = 0;
int luzCasco = LOW;
int luzTorre = LOW;
int posTimonDest = TIMON_STOP;
int velMotorBaborDest = 0;
int velMotorEstriborDest = 0;

/* Apply change */
bool cambiarTimon = true;
bool cambiarMotorB = true;
bool cambiarMotorE = true;
bool cambiarLuzCasco = true;
bool cambiarLuzTorre = true;

void translateIR2() {
  if (irrecv.decode(&irCommand)) {
    switch (irCommand.value)
    {
    case 0x2FD48B7: Serial.println("POWER"); break;
    case 0x2FDD827: Serial.println("ARRIBA"); break;
    case 0x2FDF807: Serial.println("ABAJO"); break;
    case 0x2FD08F7: Serial.println("PARLANTE"); break;
    case 0x2FD7887: Serial.println("IZQUIERDA"); break;
    case 0x2FD58A7: Serial.println("DERECHA"); break;
    case 0x2FD28D7: Serial.println("AV-TV"); break;  
    default: 
      Serial.println(irCommand.value, HEX);
      Serial.println(irCommand.decode_type);
    }// End Case
    irrecv.resume(); // Receive the next value
  }
  //delay(1000);
  //Serial.println("test");
}


void manageIRAction2() {
  if (irrecv.decode(&irCommand)) { // have we received an IR signal?
    switch (irCommand.value)
    {
      case 0x2FD48B7: Serial.println("POWER"); 
        posTimonDest = TIMON_STOP;
        velMotorBaborDest = 0;
        velMotorEstriborDest = 0;          
        break;
      case 0x2FDD827: Serial.println("ARRIBA"); 
        velMotorBaborDest += MOTOR_STEP;
        velMotorEstriborDest += MOTOR_STEP;
        break;
      case 0x2FDF807: Serial.println("ABAJO"); 
        velMotorBaborDest -= MOTOR_STEP;
        velMotorEstriborDest -= MOTOR_STEP;
        break;
      case 0x2FD08F7: Serial.println("PARLANTE"); 
        break;
      case 0x2FD7887: Serial.println("IZQUIERDA"); 
        posTimonDest -= TIMON_STEP;
        break;
      case 0x2FD58A7: Serial.println("DERECHA"); 
        posTimonDest += TIMON_STEP;
        break;
      case 0x2FD28D7: Serial.println("AV-TV"); 
        luzCasco = switchLed(luzCasco);
        luzTorre = switchLed(luzTorre);
        break;  
      //default: 
      //  Serial.println(" other button   ");
      //  Serial.println(irCommand.value, HEX);
    
    }// End Case
    irrecv.resume();
  }
}


/*
void manageIRAction() {
  if (irrecv.decode(&irCommand)) { // have we received an IR signal?
    int valor = irCommand.value;
    switch(valor) {
    case 12: case 2060: Serial.println("STOP");    
      // Pongo en 0 el status de movimiento del barco con los destions 
      posTimonDest = TIMON_STOP;
      velMotorBaborDest = 0;
      velMotorEstriborDest = 0;      
      break;
    case 32: case 2080: Serial.println("UP"); 
      //Subo la velocidad general
      velMotorBaborDest += MOTOR_STEP;
      velMotorEstriborDest += MOTOR_STEP;
      break;
    case 33: case 2081: Serial.println("DOWN");    
      //Bajo la velocidad general
      velMotorBaborDest -= MOTOR_STEP;
      velMotorEstriborDest -= MOTOR_STEP;
      break;
    case 13: case 2061: Serial.println("SOUND");
      break;
    case 17: case 2065: Serial.println("BACK");    
      // Muevo el timon para Babor
      posTimonDest -= TIMON_STEP;
      break;
    case 16: case 2064: Serial.println("NEXT");   
      // Muevo el timon para Estribor
      posTimonDest += TIMON_STEP;
      break;
    case 11: case 2059: Serial.println("AVTV");    
      // cambio luz de bajo cubierta
      luzCasco = switchLed(luzCasco);
      luzTorre = switchLed(luzTorre);
      break;
    default: 
      Serial.print(" other button: ");
      Serial.println(valor);
    }// End Case
    irrecv.resume(); // receive the next value
  }
} //END translateIR
*/
int transitionVariable(int orig, int dest) {
  if (orig > dest) {
    return orig - 1;
  } else if (orig < dest) {
    return orig + 1;  
  } else {
    return orig;  
  }
}

int switchLed(int actualValue) {
  return HIGH - actualValue;  
}


/* Controla que los limites del timon y los motores no se pasen*/
void controlLimites() {
 posTimon = max(min(posTimon, TIMON_MAX), TIMON_MIN);
 velMotorBabor = max(min(velMotorBabor, MOTOR_MAX), MOTOR_MIN);
 velMotorEstribor = max(min(velMotorEstribor, MOTOR_MAX), MOTOR_MIN);
 posTimonDest = max(min(posTimonDest, TIMON_MAX), TIMON_MIN);
 velMotorBaborDest = max(min(velMotorBaborDest, MOTOR_MAX), MOTOR_MIN);
 velMotorEstriborDest = max(min(velMotorEstriborDest, MOTOR_MAX), MOTOR_MIN);
}

/* Apply the motor speed to the specific motor's pins, TODO: Check the direction of the pins*/
void applyVelMotorWithEn(int pinA, int pinB, int pinEn, int vel) {
  int absvel = abs(vel);
  if (absvel < 5) absvel = 0;
  if (vel > 0) {
    digitalWrite(pinB, LOW);
    digitalWrite(pinA, HIGH);
  } else if (vel < 0) {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
    analogWrite(pinEn, -vel);
  } else {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
  }
  analogWrite(pinEn, absvel);
}



void printTransition(char* control, int origen, int destino) {
   Serial.print(control);
   Serial.print(" - Dest: ");
   Serial.print(destino);
   Serial.print(", Actual: ");
   Serial.println(origen);  
}

void testOutput() {
  Serial.println("Test Motor Estribor");  
  applyVelMotorWithEn(PIN_MTE_DIR_A, PIN_MTE_DIR_B, PIN_MTE_EN, MOTOR_MAX);
  delay(1000);
  applyVelMotorWithEn(PIN_MTE_DIR_A, PIN_MTE_DIR_B, PIN_MTE_EN, MOTOR_MIN);
  delay(1000);
  
  Serial.println("Test Motor Babor");
  applyVelMotorWithEn(PIN_MTB_DIR_A, PIN_MTB_DIR_B, PIN_MTB_EN, MOTOR_MAX);
  delay(1000);
  applyVelMotorWithEn(PIN_MTB_DIR_A, PIN_MTB_DIR_B, PIN_MTB_EN, MOTOR_MIN);
  delay(1000);
  
  Serial.println("Test Timon");
  //svTimon.write(TIMON_MAX);
  delay(1000);
  //svTimon.write(TIMON_MIN);
  delay(1000);

  Serial.println("Test lights");
  digitalWrite(PIN_LED_TORRE, HIGH);
  delay(1000);
  digitalWrite(PIN_LED_TORRE, LOW);
  digitalWrite(PIN_LED_CASCO_INT, HIGH);
  delay(1000);
  digitalWrite(PIN_LED_CASCO_INT, LOW);
  digitalWrite(PIN_LED_CASCO_EXT, HIGH);
  delay(1000);
  digitalWrite(PIN_LED_CASCO_EXT, LOW);
  delay(1000);
  
  Serial.println("Test sound");
  //tone(PIN_BUZZER, 200, 500);
  analogWrite(PIN_BUZZER, 240);
  delay(500);
  //tone(PIN_BUZZER, 100, 500);
  analogWrite(PIN_BUZZER, 200);
  delay(500);
  digitalWrite(PIN_BUZZER, LOW);
  
}

/* Apply the calculated state to the elements on the ship */
void applyStateToShip() {
  controlLimites();
  if (posTimon != posTimonDest) {
    posTimon = transitionVariable(posTimon, posTimonDest);
    printTransition("Timon", posTimon, posTimonDest);
    //svTimon.write(posTimon);
  }
  if (velMotorBabor != velMotorBaborDest) {
    velMotorBabor = transitionVariable(velMotorBabor, velMotorBaborDest);
    //velMotorBabor = velMotorBaborDest;
    printTransition("Babor", velMotorBabor, velMotorBaborDest);
    //applyVelMotor(PIN_MTB_DIR_A, PIN_MTB_DIR_B, velMotorBabor);
    applyVelMotorWithEn(PIN_MTB_DIR_A, PIN_MTB_DIR_B, PIN_MTB_EN, velMotorBabor);
  }
  if (velMotorEstribor != velMotorEstriborDest) {
    velMotorEstribor = transitionVariable(velMotorEstribor, velMotorEstriborDest);
    //velMotorEstribor = velMotorEstriborDest;
    printTransition("Estri", velMotorEstribor, velMotorEstriborDest);
    //applyVelMotor(PIN_MTE_DIR_A, PIN_MTE_DIR_B, velMotorEstribor);
    applyVelMotorWithEn(PIN_MTE_DIR_A, PIN_MTE_DIR_B, PIN_MTE_EN, velMotorEstribor);
  }
  digitalWrite(PIN_LED_TORRE, luzTorre);
  digitalWrite(PIN_LED_CASCO_INT, luzCasco);
  digitalWrite(PIN_LED_CASCO_EXT, luzCasco);
}

void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  Serial.begin(9600);
  Serial.println("USS-Hornet IR-controlled Starting"); 
  irrecv.enableIRIn(); // -Start the receiver
  //svTimon.attach(PIN_SERVO_TIMON);
/*  pinMode(PIN_MTE_DIR_A, OUTPUT);
  pinMode(PIN_MTE_DIR_B, OUTPUT);
  pinMode(PIN_MTB_DIR_A, OUTPUT);
  pinMode(PIN_MTB_DIR_B, OUTPUT);
  pinMode(PIN_MTE_EN, OUTPUT);
  pinMode(PIN_MTB_EN, OUTPUT);
  pinMode(PIN_LED_CASCO_INT, OUTPUT);
  pinMode(PIN_LED_CASCO_EXT, OUTPUT);
  pinMode(PIN_LED_TORRE, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  //testOutput();  
  applyVelMotorWithEn(PIN_MTE_DIR_A, PIN_MTE_DIR_B, PIN_MTE_EN, 0);
  applyVelMotorWithEn(PIN_MTB_DIR_A, PIN_MTB_DIR_B, PIN_MTB_EN, 0);
      
  applyStateToShip();
*/
  Serial.println("USS-Hornet IR-controlled Started"); 
  
}/*--(end setup )---*/



void loop()   /*----( LOOP: RUNS intANTLY )----*/
{
  //translateIR2();
  manageIRAction2();
  applyStateToShip();
}/* --(end main loop )-- */


