// Program to control the skinner box 
// Gabriela Chiuffa Tunes and Marcelo Bussotti Reyes
// Universidade Federal do ABC - 2016

// This line is a test
// todo
// check if last_head_out inicialized to high value is working


#include <Stepper.h>  // inclui uma biblioteca para programar motor de passo (MP)

// Task parameters
int criterion 	= 1600 ; 	//tempo de barra pressionada deve ser >= a essa variável (em ms)
int last_head_out = 1e8;	// detects when was last exit of the nosepoke
							// iniciate it to a very big value
  							// so the first real entrance will be detected  							// ??? check if this is working
int delta = 50;				// time for which the animal has to be out of the 
							// nosepoke for the TDT state to be set to LOW

int analog_baseline =0;		// detects what is the baseline of the analog port 
							// responsible for detecting the licking
byte head_in	=   LOW;	// LOW is the state of the port when the sujbect is
							// in the nosepoke.

byte lastState = !head_in;


byte debounceState = HIGH;

// Digital ports
//input 
const int INFRARED			=  7;   //variável de entrada do infravermelho
const int ANALOGPIN 		= A3;	// analog sensor for licking
//output
const int TDT_STATE_PIN 	=  5;	// bit that is sent do TDT to record the state
const int TDT_SENSOR_PIN 		= 13;	// bit that detects if the animal is in (LOW) or  
									// out (HIGH) of the nosepoke
const int TDT_STATE_GATE 	=  3; 	// bit that marks the opening and closing of the water gate
									

//counters and flags 
int countLick = 0; // contador de lambidas 
int flag=0;
bool give_water = false;
bool gate_open = false;			// flag monitor if the water gate is closed
int current_analog_state = 0; 
int previous_analog_state = 0;


//unsigned long lastDebounceTime = 0;//salva o tempo quando altera o previous 
//unsigned long debounceDelay = 2;//delay para realizar a comparação e verificar se o rato entrou *talvez precise ser alterado *ESSE É O PARAMETRO QUE ALTEREI

// Stepper motor parameter
const int stepsPerRevolution = 200; // change this to fit the number of steps 
									// per revolution for your motor

// inicializa a biblioteca do MP nos pinos 8,9,10 e 11
Stepper myStepper(stepsPerRevolution, 8,9,10,11);            


void setup(){  //criando setup das portas a serem usadas e MP

  // velocidade de rotacao em 60 rpm (testei outras mas ficou travando o motor, era um motor fraco e ruim):
  myStepper.setSpeed(10);
  // inicializar a porta serial:
  Serial.begin(9600);
  // initialize digital pin 13 as an output.
  pinMode(TDT_SENSOR_PIN, 	OUTPUT);
  pinMode(TDT_STATE_PIN, 	OUTPUT);
  pinMode(TDT_STATE_GATE, 	OUTPUT);
  pinMode(INFRARED, 		INPUT);     
  
  pinMode(4, 				INPUT); 
}

// criando o loop que fica verificando o tempo todo o estado do IF
// da caixa de skinner
void loop(){ 	


	if(digitalRead(INFRARED) == head_in){		// detecting if animal is in nosepoke
		digitalWrite(TDT_STATE_PIN, HIGH);		// send bit ON to TDT (filtered with a sort of debounce)
		digitalWrite(TDT_SENSOR_PIN,LOW);		// send raw sensor data to tdt
		lastState = head_in;					// saves the current state (in the nosepoke)
	}

	if(digitalRead(INFRARED) != head_in){		// detecting if infra-red beam is not broken
		digitalWrite(TDT_SENSOR_PIN,LOW);		// sends the raw sensor 
		if 	(lastState == head_in){				// detects if the animal just left the nosepoke
			last_head_out = millis(); 			// stores the last time it changed from in to out
		}	
		if(millis() - last_head_out > delta){	// checks if enough time elapsed from last head in 
												// if so, the animal has left the nosepoke for 
												// enough time.
			digitalWrite(TDT_STATE_PIN, LOW);	// send bit OFF to TDT
			give_water = true;
		}
	}

	if(give_water && !gate_open){
		give_water 	= false;
		gate_open 	= true;
		myStepper.step(50);
        digitalWrite(TDT_STATE_GATE,HIGH);
	}	
	
	if(gate_open){
		current_analog_state =analogRead(ANALOGPIN);
		if(current_analog_state-previous_analog_state > 70){
			countLick=countLick+1;
		}
		if(countLick >= 7){
   			myStepper.step(-50);
   			digitalWrite(TDT_STATE_GATE,LOW);
   			countLick=0;
  			gate_open= false;
  		}
	}
	
	previous_analog_state = current_analog_state;
	
}


