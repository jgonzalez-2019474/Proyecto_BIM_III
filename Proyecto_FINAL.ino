 //Librerias
#include <Servo.h> // libreria para el servo
#include <SPI.h> //libreria para el RFID
#include <MFRC522.h> //Libreria para el RFID
#include <LiquidCrystal_I2C.h> //Libreria para controlar la LCD por medio de I2C


//Directivas de preprocesador
#define Address_LCD 0x27 //Direccion LCD
#define Filas 2 //Filas LCD
#define Columnas 10 //Columnas LCD
#define RST_PIN 9 //Pin RTS conectado a pin 9 de arduino
#define SS_PIN 10 //Pin SS conectado a pin 10 de arduino
#define LED1 2 //LED1 conectado a pin 2 de arduino
#define LED2 3 //LED2 conectado a pin 3 de arduino
#define LED3 4 //LED3 conectado a pin 4 de arduino
#define LED4 5 //LED4 conectado a pin 5 de arduino
#define LED_Poste1 22 //LED_Poste1 conectado a pin 22 de arduino
#define LED_Poste2 23 //LED_Poste2 conectado a pin 23 de arduino
#define LED_Poste3 24 //LED_Poste2 conectado a pin 24 de arduino
#define LED_Poste4 25 //LED_Poste2 conectado a pin 25 de arduino
#define LED_Poste5 26 //LED_Poste2 conectado a pin 26 de arduino

int trig = 27; //trig conectado a pin 27 de arduino  
int echo = 28; //echo conecetado a pin 28 de arduino
int tiempo;
int distancia;

//Funciones
void onOffLeds();
void cerrarPuerta();
void abrirPuerta();
void Postes_de_luz();
void Talanquera_Salida();


//Variables
char Incoming_value = 0; //valor entrante del modulo bluetooth


byte LecturaUID[4]; //almacenará el código del Tag leído
byte Usuario1[4]= {0xC3, 0xF7, 0x35, 0xAD} ; //usuario 1
byte Usuario2[4]= {0x22, 0xB9, 0xC9, 0x34} ; //usuario 2


//Constructores
MFRC522 mfrc522(SS_PIN, RST_PIN); //Creamos el objeto para RC522
LiquidCrystal_I2C LCD_Safe_Security(Address_LCD, Columnas, Filas);
Servo Servo_Puerta; //inicializo el servo
Servo Servo_Entrada; //inicializo el servo
Servo Servo_Salida; //inicializo el servo

void setup()
{
  pinMode(LED1, OUTPUT); //LED1 como salida
  pinMode(LED2, OUTPUT); //LED2 como salida
  pinMode(LED3,OUTPUT); //LED3 como salida
  pinMode(LED4,OUTPUT); //LED4 como salida
  pinMode(LED_Poste1, OUTPUT); //LED_Poste1 como salida
  pinMode(LED_Poste2, OUTPUT); //LED_Poste2 como salida
  pinMode(LED_Poste3, OUTPUT); //LED_Poste3 como salida
  pinMode(LED_Poste4, OUTPUT); //LED_Poste4 como salida
  pinMode(LED_Poste5, OUTPUT); //LED_Poste5 como salida
  pinMode(trig, OUTPUT); //trig como salida
  pinMode(echo, INPUT); //echo como entrada
  Servo_Entrada.attach(29); //pin del servo de la talanquera de entrada
  Servo_Puerta.attach(30); //pin del servo puerta
  Servo_Salida.attach(31); //pin del servo de la talanquera de salida
  onOffLeds(); //Funcion
  Postes_de_luz(); //Funcion
  Serial.begin(9600); //inicio la comunicacion serial  
  SPI.begin(); //Iniciamos el bus SPI
  mfrc522.PCD_Init(); //iniciamos el MFRC522
  Serial.println("listo");
  LCD_Safe_Security.init();
  LCD_Safe_Security.backlight();
 
}

void loop() {

  LCD_Safe_Security.setCursor(0,0);
  LCD_Safe_Security.print("   Bienvenido  ");
  LCD_Safe_Security.setCursor(0,1);
  LCD_Safe_Security.print(" Safe Security  ");
 if ( ! mfrc522.PICC_IsNewCardPresent()) //Revision si hay tajetas nuevas
   return;
  if ( ! mfrc522.PICC_ReadCardSerial())  //Seleccionamos tarjeta
    return;
 Serial.println("Respuesta: ");
 onOffLeds();
 //Enviamos serialmente su UID
  for(byte i=0;i<mfrc522.uid.size ;i++){
    if(mfrc522.uid.uidByte[i]<0x10){
      Serial.print(" 0");
      }
    else{
      Serial.print(" ");
      }
    Serial.print(mfrc522.uid.uidByte[i],HEX);
    LecturaUID[i] = mfrc522.uid.uidByte[i];
 }
 
  Serial.print("\t");
  //Si la tarjeta esta registrada premite el acceso
  if(comparaUID(LecturaUID , Usuario1)){
    Serial.println("Acceso permitido, bienvenido");
    abrirPuerta();
    delay(2000);
    cerrarPuerta();
    }
  //Si la tarjeta no esta registrada no permitira el acceso
  else if(comparaUID(LecturaUID , Usuario2)){
      Serial.println("Accedo denegado, lo siento no tiene permitido ingresar");
    }
  mfrc522.PICC_HaltA(); //Terminamos la lectura de la tajeta
  Talanquera_Salida();
 
  if(Incoming_value == '1') //si se envia un valor uno el servo gira a noventa grados
  {
    Servo_Puerta.write(90); //servo a 90 grados
    digitalWrite(LED3,HIGH); //led verde prendido
    digitalWrite(LED4,LOW); //led rojo apagado    
  }
 
  else if(Incoming_value == '0') //si se envia un cero el servo gira a cero grados
  {
    Servo_Puerta.write(0); //servo a 0 grados
    digitalWrite(LED4,HIGH); //led rojo prendido
    digitalWrite(LED3,LOW); //led verde apagado
   }
}
//Funciones
void Talanquera_Salida(){
  digitalWrite(trig, HIGH); //Trig en alto
  delay(500); //tiempo de espera
  digitalWrite(trig, LOW); //Trig en low
  tiempo=pulseIn(echo, HIGH); //echi en alto
  distancia=tiempo/58.2;
  delay(500); //tiempo de espera
{
  if (distancia <=10) // si la distancia es menor o igual a 10 el servo se movera a 120 grados
  Servo_Salida.write(0);
  delay(2000);
  }
  {
  if (distancia >10); // si la distancia es mayor a 10 el servo se va a mover a 45 grados
  Servo_Salida.write(90);
 }
}
void Postes_de_luz(){
  digitalWrite(LED_Poste1, HIGH); //LED del poste1 en alto
  digitalWrite(LED_Poste2, HIGH); //LED del poste2 en alto
  digitalWrite(LED_Poste3, HIGH); //LED del poste3 en alto
  digitalWrite(LED_Poste4, HIGH); //LED del poste4 en alto
  digitalWrite(LED_Poste5, HIGH); //LED del poste5 en alto
  }

void onOffLeds(){
  digitalWrite(LED1,HIGH); //LED1 en alto
  digitalWrite(LED2,HIGH); //LED2 en alto
  delay(100); //tiempo de espera
  digitalWrite(LED1,LOW); //LED1 en bajo
  digitalWrite(LED2,LOW); //LED2 en bajo
  delay(100); //tiempo de espera
  digitalWrite(LED1,HIGH); //LED1 en alto
  digitalWrite(LED2,HIGH); //LED2 en alto
  delay(100); //tiempo de espera
  digitalWrite(LED1,LOW); //LED1 en bajo
  digitalWrite(LED2,LOW); //LED2 en bajo
  }
 
void cerrarPuerta(){
   int angulo = 0; //servo a 0 grados
   do{
     digitalWrite(LED2,HIGH); //LED2 en alto
     digitalWrite(LED1,LOW); //LED1 en bajo
     angulo = angulo+1; //angulo aumenta
     Servo_Entrada.write(angulo);  
     delay(50);
    }while(angulo<90); //angulo tiene que ser menor a 90 grados
  digitalWrite(LED2,LOW); //LED2 en bajo
  digitalWrite(LED1,LOW); //LED1 en bajo
}

void abrirPuerta(){
   int angulo=90;
   do{
     digitalWrite(LED1,HIGH); //LED1 en alto
     digitalWrite(LED2,LOW); //LED2 en bajo
     angulo = angulo-1; //angulo desciende
     Servo_Entrada.write(angulo);
     delay(50);
    }while(angulo>0); //angulo tiene quye ser mayor a 0 grados
}
boolean comparaUID(byte lectura[],byte usuario[]){ // comparacion de los usuarios
  for(byte i=0; i < mfrc522.uid.size; i++){
    if(lectura[i] != usuario[i]) //lectura de los usurios
    return(false); //regreso un valor falso
  }
  return(true); //regreso un valor verdadero
}
