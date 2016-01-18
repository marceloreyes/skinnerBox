// Program to control the skinner box 
// Gabriela Chiuffa Tunes and Marcelo Bussotti Reyes
// Universidade Federal do ABC - 2016

// parametros do programa

int time = 1900 ; //tempo de barra pressionada deve ser >= a essa variável (em ms)
int countt=0;// contador de trials corretos consecutivos 
int InfraRed= 7; //variável de entrada do infravermelho
byte previous = HIGH; //status do nose poke no momento anterios 
byte debounceState = LOW;// status do nose poke, comparacao para aplicacao do debounce
signed long firstTime; //guarda o tempo, em ms, que ocorreu a condicional do debounce - considerado inicio do trial 
int buttonpress; // status presente do nose poke
int analogPin = A3; //leitura do sensor de lambidas 
int flagdebounce=1; // esta flag serve para garantir que o firsTime seja salvo somente depois que foi considerado o offset de fato(com aplicacao do debounce)
int val=0;// valor presente do sensor de lambidas
int val1=0;//valor anterior do sensor de lambidas
int countlamb = 0; // contador de lambidas 
int flag=0;// essa flag garante que o motor não abra duas vezes // ela é 1 quando o motor está aberto e 0 quando está fechado 
unsigned long lastDebounceTime = 0;//salva o tempo quando altera o previous 
unsigned long debounceDelay = 50;//delay para realizar a comparação e verificar se o rato realmente saiu do nose poke // exclui alteraçoes que ocorrem abaixo de 50 ms
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Stepper.h> // inclui uma biblioteca para programar motor de passo (MP)
const int stepsPerRevolution = 200; // change this to fit the number of steps per revolution // for your motor
Stepper myStepper(stepsPerRevolution, 8,9,10,11); // inicializa a biblioteca do MP nos pinos 8,9,10 e 11       
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup(){  //criando setup das portas a serem usadas e MP 
  myStepper.setSpeed(10);// velocidade de rotacao em 60 rpm (testei outras mas ficou travando o motor, era um motor fraco e ruim):
  Serial.begin(9600);// inicializar a porta serial:
  pinMode(13, OUTPUT);// envia o sinal verdadeiro do nose poke para tdt
  pinMode(5, OUTPUT);// envia o sinal com debounce no offset para tdt
  pinMode(4, INPUT); //recebe a entrada com liquometro
  pinMode(3, OUTPUT);//envia o status do motor para tdt 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop(){ //criando o loop que fica verificando o tempo todo o estado do IF//da caixa de skinner
buttonpress = digitalRead(InfraRed); // le o sinal do infravermelho para ver se esta HIgh (5v) ou low (<5v)

  if (buttonpress != previous){ // compara o status atual com o anterios 
   lastDebounceTime = millis(); // salva o tempo em que ocorreu a alteração do status
    if (flagdebounce==1){//verifica se o rato permaneceu dentro do nose poke
       if (buttonpress == LOW){ // se o sinal do IF for baixo (Rato cortou o feixe de IF) e antes era alto, entao guardar o tempo onde ocorreu
       firstTime = millis(); // guarda o tempo, em ms, que ocorreu a condicional
       digitalWrite(5,HIGH);//manda para tdt o onset
       digitalWrite(13,HIGH);//manda para tdt o onser // no onset os pinos 5 e 13 são iguais 
       flagdebounce=0;
      }
    }
  }
  
  if (buttonpress == HIGH){// verifica se o rato saiu do nose poke
       // digitalWrite(5,LOW);
        digitalWrite(13,LOW);// manda para a tdt o tempo real do offset 
  }
  
  if(millis() - lastDebounceTime > debounceDelay){// insere um atraso de 50 ms // para verificar se de fato o rato saiu do nose poke
    if(buttonpress != debounceState){// verifica se ocorreu alteração do status permaneceu após 50 ms da alteracao 
      debounceState = buttonpress;// atribui ao debounceState o valo atual do buttonpress 
      if (buttonpress == HIGH){// se após 50ms ocorreu te fato a alteração para HIGH // o rato saiu do nose poke por um tempo superior a 50ms
           digitalWrite(5,LOW);// envia para tdt o momento no qual o rato saiu do nosepoke //offset com atraso de 50ms 
           // digitalWrite(13,LOW);
           flagdebounce=1;// ativa a flag para verificar se o rato entrou no nose poke
           // abaixo os 2 ifs verificam o tempo que o animal permaneceu no nose poke se foi superior ou não ao críterio 
          if(millis()-firstTime < time){
           countt=0;
           }         
          if (millis()-firstTime >= time){
          countt=(countt+1);
            if (countt>=5){
            time=time+100;
            countt=0;
            }
            if (flag==0){// verifica se o motor está fechado e pode ser aberto
            myStepper.step(50);//abre o motor 
            digitalWrite(3,HIGH);//envia para tdt o momento que iniciou a abertura do motor
            flag=1;// altera a flag para indicar que o motor está aberto 
           }
         }
       }
     }
   } 
   
  if(flag==1){//se o motor estiver aberto verifica o contador de lambidas
      val=analogRead(analogPin);// recebe o valor atual do sensor de lambidas
      if ( val-val1>=70){//compara o valor atual ao valor anterior
      countlamb=countlamb+1;//conta o número de lambidas
      }
      if (countlamb>=5){// se o número de lambidas é maior ou igual a 5 inicia o procedimento para fechar o motor 
      myStepper.step(-50);
      digitalWrite(3,LOW);// envia para tdt o momento que o motor começou a fechar
      countlamb=0;
      flag=0;//atualiza a flag para dizer que o motor está fechado 
      }
  }
  val1=val;
  previous = buttonpress; // salva o estado (HIGH ou LOW) para cada loop
}
