// Program to control the skinner box 
// Gabriela Chiuffa Tunes and Marcelo Bussotti Reyes
// Universidade Federal do ABC - 2016

// parametros do programa

int time = 1900 ; //tempo de barra pressionada deve ser >= a essa variável (em ms)
int countt=0;// contador de trials corretos consecutivos 
int InfraRed= 7; //variável de entrada do infravermelho
byte previous = HIGH; //status do nose poke 
byte debounceState = LOW;
signed long firstTime; //guarda o tempo, em ms, que ocorreu a condicional do debounce - considerado inicio do trial 
signed long secondTime;
int buttonpress; //
//int pos = 0;
//int count=0;
int analogPin = A3;
int flagdebounce=1;
int val=0; 
int val1=0;
int countlamb = 0; // contador de lambidas 
int flag=0;
unsigned long lastDebounceTime = 0;//salva o tempo quando altera o previous 
unsigned long debounceDelay = 50;//delay para realizar a comparação e verificar se o rato entrou *talvez precise ser alterado *ESSE É O PARAMETRO QUE ALTEREI


#include <Stepper.h> // inclui uma biblioteca para programar motor de passo (MP)


const int stepsPerRevolution = 200; // change this to fit the number of steps per revolution
                         // for your motor



// inicializa a biblioteca do MP nos pinos 8,9,10 e 11
Stepper myStepper(stepsPerRevolution, 8,9,10,11);            


void setup(){  //criando setup das portas a serem usadas e MP

  // velocidade de rotacao em 60 rpm (testei outras mas ficou travando o motor, era um motor fraco e ruim):
  myStepper.setSpeed(10);
  // inicializar a porta serial:
  Serial.begin(9600);
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, INPUT); 
  pinMode(3, OUTPUT);
}

void loop(){ //criando o loop que fica verificando o tempo todo o estado do IF
//da caixa de skinner

  buttonpress = digitalRead(InfraRed); // le o sinal do infravermelho para ver se esta HIgh (5v) ou low (<5v)
  
  if (buttonpress != previous){
   lastDebounceTime = millis(); 
    if (flagdebounce==1){
       if (buttonpress == LOW){ // se o sinal do IF for baixo (Rato cortou o feixe de IF) e antes era alto, entao guardar o tempo onde ocorreu
       firstTime = millis(); // guarda o tempo, em ms, que ocorreu a condicional
       digitalWrite(5,HIGH);
       //digitalWrite(13,HIGH);
       flagdebounce=0;
      }
    }
  }
    if (buttonpress == HIGH){
       secondTime=millis();
       // digitalWrite(5,LOW);
        digitalWrite(13,LOW);
    }
    if (buttonpress == LOW){
        // digitalWrite(5,LOW);
        digitalWrite(13,HIGH);
    }
     if (secondTime-firstTime<=debounceDelay){
      digitalWrite(5,LOW);
      flagdebounce=1;
    }
    
  if(millis() - lastDebounceTime > debounceDelay){
   
    if(buttonpress != debounceState){
      debounceState = buttonpress;

      if (buttonpress == HIGH){
        digitalWrite(5,LOW);
       // digitalWrite(13,LOW);
        flagdebounce=1;
        if(millis()-firstTime < time){
         countt=0;
         }         
        if (millis()-firstTime >= time){
          countt=(countt+1);
          if (countt>=5){
            time=time+100;
            countt=0;
          }
          if (flag==0){
          myStepper.step(50);
          digitalWrite(3,HIGH);
          flag=1;
          }
        }
      }
     }
   } 
  Serial.println(val);
  if(flag==1){
  val=analogRead(analogPin);
  if ( val-val1>=70){
    countlamb=countlamb+1;
 
  }
  if (countlamb>=5){
   myStepper.step(-50);
   digitalWrite(3,LOW);
   countlamb=0;
   flag=0;
  }
  }
  val1=val;
  previous = buttonpress; // salva o estado (HIGH ou LOW) para cada loop
}
