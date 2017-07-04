/* Codigo IR para manejar el USS Hornet
 *  El codigo contiene accion para manejar con el control remoto lo siguiente:
 *  1. Servo timon - Con movimiento gradual marcando destino con el IR
 *  2. 2 Motores controlados independientemente marcando destino con el IR
 *  3. Controladores de Luces de Bajo Cubierta y en Torre
 */

#include "IRremote.h"
#include <Servo.h>


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

/* Ship Constants */
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
int luzBajoCubierta = LOW;
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
    case 0xFF02FD: Serial.println("PAUSE");    break;
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
      luzBajoCubierta = switchLed(luzBajoCubierta);
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


/* Move the transition values to the destinations */
void transitionToDestinationVariables() {
  
}

/* Controla que los limites del timon y los motores no se pasen*/
void controlLimites() {
  
}

/* Apply the calculated state to the elements on the ship */
void applyStateToShip() {
  controlLimites();
  svTimon.write(posTimon);
}

void setup()   /*----( SETUP: RUNS ONCE )----*/
{
  Serial.begin(9600);
  Serial.println("USS-Hornet IR-controlled Started"); 
  irrecv.enableIRIn(); // Start the receiver
  svTimon.attach(PIN_SERVO_TIMON);
  applyStateToShip();

}/*--(end setup )---*/


void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  manageIRAction();
  transitionToDestinationVariables();
  applyStateToShip();
}/* --(end main loop )-- */


