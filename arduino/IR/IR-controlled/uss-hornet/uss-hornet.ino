/* Codigo IR para manejar el USS Hornet
 *  El codigo contiene accion para manejar con el control remoto lo siguiente:
 *  1. Servo timon - Con movimiento gradual marcando destino con el IR
 *  2. 2 Motores controlados independientemente marcando destino con el IR
 *  3. Controladores de Luces de Bajo Cubierta y en Torre
 */

#include "IRremote.h"
#include "Servo.h"


/* PINs Declaration */
int PIN_IR_RECEIVER = 13;
int PIN_LED_TORRE = 10;
int PIN_LED_CASCO  = 9; 
int PIN_SERVO_TIMON = 8;
int PIN_MTE_ENABLED = 7;
int PIN_MTB_ENABLED = 6;
int PIN_MTE_DIR_A = 5;
int PIN_MTE_DIR_B = 4;
int PIN_MTB_DIR_A = 3;
int PIN_MTB_DIR_B = 2;



/* Servo Objects */
Servo svTimon;  // create servo object to control a servo

/* IR Objects*/
IRrecv irrecv(PIN_IR_RECEIVER);     // create instance of 'irrecv'
decode_results irCommand;      // create instance of 'decode_results'

/* Ship intants */
int TIMON_MIN = 30;
int TIMON_MAX = 150;
int MOTOR_MAX = 255;
int MOTOR_MIN = -64;
int MOTOR_STEP = 32;
int TIMON_STEP = 30;
int TIMON_STOP = 90;

/* Ship Status */
int posTimon = TIMON_STOP;
int velMotorBabor = 0;
int velMotorEstribor = 0;
int luzCasco = LOW;
int luzTorre = LOW;
int posTimonDest = 0;
int velMotorBaborDest = 0;
int velMotorEstriborDest = 0;

/* calcula el proximo estado de un led*/
int switchLed(int previousValue) {
    return HIGH - previousValue;
}

void manageIRAction() {
  irrecv.resume(); // receive the next value
  if (irrecv.decode(&irCommand)) { // have we received an IR signal?
    switch(irCommand.value) {
    case 0xFFA25D: Serial.println("POWER"); break;
    case 0xFFE21D: //Serial.println("FUNC/STOP");
      // Pongo en 0 el status de movimiento del barco 
      posTimon = TIMON_STOP;
      velMotorBabor = 0;
      velMotorEstribor = 0;
      break;
    case 0xFF629D: //Serial.println("VOL+"); 
      //Subo la velocidad general
      velMotorBaborDest += MOTOR_STEP;
      velMotorEstriborDest += MOTOR_STEP;
      break;
    case 0xFF22DD: //Serial.println("FAST BACK");    
      // Muevo el timon para Babor
      posTimonDest -= TIMON_STEP;
      break;
    case 0xFF02FD: //Serial.println("PAUSE");    
      // Pongo en 0 el status de movimiento del barco con los destions 
      posTimonDest = TIMON_STOP;
      velMotorBaborDest = 0;
      velMotorEstriborDest = 0;      
      break;
    case 0xFFC23D: //Serial.println("FAST FORWARD");   
      // Muevo el timon para Estribor
      posTimonDest += TIMON_STEP;
      break;
    case 0xFFE01F: //Serial.println("DOWN");    
      // incremento motor babor solamente
      velMotorBaborDest += MOTOR_STEP;
      break;
    case 0xFFA857: //Serial.println("VOL-");    
      //Bajo la velocidad general
      velMotorBaborDest -= MOTOR_STEP;
      velMotorEstriborDest -= MOTOR_STEP;
      break;
    case 0xFF906F: //Serial.println("UP");    
      // Incremento estribor solamente;
      velMotorEstriborDest += MOTOR_STEP;
      break;
    case 0xFF9867: Serial.println("EQ");    break;
    case 0xFFB04F: //Serial.println("ST/REPT");    
      // Decremento estribor solamente
      velMotorEstriborDest -= MOTOR_STEP;
      break;
    case 0xFF6897: //Serial.println("0");    
      // Decremento motor babor solamente
      velMotorBaborDest -= MOTOR_STEP;
      break;
    case 0xFF30CF: Serial.println("1");    break;
    case 0xFF18E7: Serial.println("2");    break;
    case 0xFF7A85: Serial.println("3");    break;
    case 0xFF10EF: Serial.println("4");    break;
    case 0xFF38C7: Serial.println("5");    break;
    case 0xFF5AA5: Serial.println("6");    break;
    case 0xFF42BD: Serial.println("7");    break;
    case 0xFF4AB5: //Serial.println("8");    
      // cambio luz de bajo cubierta
      luzCasco = switchLed(luzCasco);
      break;
    case 0xFF52AD: //Serial.println("9");    
      // cambio luz de torre;
      luzTorre = switchLed(luzTorre);
      break;
    case 0xFFFFFFFF: Serial.println(" REPEAT");break;  
  
    default: 
      Serial.println(" other button   ");
  
    }// End Case
  }
} //END translateIR

int transitionVariable(int orig, int dest) {
  if (orig > dest) {
    return orig--;
  } else if (orig < dest) {
    return orig++;  
  } else {
    return orig;  
  }
}

/* Move the transition values to the destinations */
void transitionToDestinationVariables() {
  posTimon = transitionVariable(posTimon, posTimonDest);
  velMotorEstribor = transitionVariable(velMotorEstribor, velMotorEstriborDest);
  velMotorBabor = transitionVariable(velMotorBabor, velMotorBaborDest);
}


/* Controla que los limites del timon y los motores no se pasen*/
void controlLimites() {
 posTimon = min(max(posTimon, TIMON_MAX), TIMON_MIN);
 velMotorBabor = min(max(velMotorBabor, MOTOR_MAX), MOTOR_MIN);
 velMotorEstribor = min(max(velMotorEstribor, MOTOR_MAX), MOTOR_MIN);
}

/* Apply the motor speed to the specific motor's pins, TODO: Check the direction of the pins*/
void applyVelMotor(int pinE, int pinA, int pinB, int vel) {
  if (vel >= 0) {
    digitalWrite(pinB, LOW);
    digitalWrite(pinA, HIGH);
    analogWrite(pinE, vel);
  } else {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
    analogWrite(pinE, -vel);
  }
}

/* Apply the calculated state to the elements on the ship */
void applyStateToShip() {
  controlLimites();
  svTimon.write(posTimon);
  applyVelMotor(PIN_MTE_ENABLED, PIN_MTE_DIR_A, PIN_MTE_DIR_B, velMotorEstribor);
  applyVelMotor(PIN_MTB_ENABLED, PIN_MTB_DIR_A, PIN_MTB_DIR_B, velMotorBabor);
  analogWrite(PIN_LED_TORRE, luzTorre);
  analogWrite(PIN_LED_CASCO, luzCasco);
}

void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  Serial.begin(9600);
  Serial.println("USS-Hornet IR-controlled Started"); 
  irrecv.enableIRIn(); // Start the receiver
  svTimon.attach(PIN_SERVO_TIMON);
  applyStateToShip();

}/*--(end setup )---*/


void loop()   /*----( LOOP: RUNS intANTLY )----*/
{
  manageIRAction();
  transitionToDestinationVariables();
  applyStateToShip();
}/* --(end main loop )-- */


