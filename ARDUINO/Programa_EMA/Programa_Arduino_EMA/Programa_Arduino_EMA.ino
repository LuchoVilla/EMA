#include <DHT.h>          // DHT & AM2302 library
#include <Adafruit_BMP085.h>
#include "Wire.h"

Adafruit_BMP085 bmp;

// Definicion de pines
#define M485PIN 3 
#define DHTPIN 4
#define LM35PIN A0
#define Hecho 8  //pin de salida, pulso ultrasónico
#define Trig 9  //pin de entrada, tiempo del rebote

#define DHTTYPE DHT22       // DHT 22  (AM2302)


//---Valores_recibidos_por_UART-------

#define DHTtemp '0'
#define DHThum '1'
#define LM35temp '2'
#define BMPpres '3'
#define HC-SR04 '4'

//---Estados_Maquina_de_estado--------
#define ESPERA 0
#define HUMEDAD 1
#define TEMPERATURA 2
#define PRESION 3
#define DIR_VIENTO 4


//---Definiciones_Varias----------------

#define MAX_ENVIAR digitalWrite(M485PIN,HIGH)
#define MAX_RECIBIR digitalWrite(M485PIN,LOW)

#define DHT_22_temp 0
#define DHT_22_hum 1


#define N  10
#define S  2
#define E  14
#define O  6
#define NO  8
#define NE  12
#define SO  4
#define SE  16

//--------------------------------------

DHT dht(DHTPIN, DHTTYPE);   // LED pins

//----prototipos------------

void read_DHT(void);
void read_LM35(void);
void read_BMP(void);
void read_HC_SR04(void);

void inic_HC_SR04(void);

//---------------------------------------------------------
/*------------Definicion de variables/constantes-----------
//---------------------------------------------------------
* t -> temperatura
* h -> humedad
* p -> presion
* distancia,tiempo -> relacionadas con sensado de la direccion del viento 
*/



float t,h; 
int32_t p;  
long tiempo;  
int distancia;

int estado=ESPERA;
char valor;


//---------------------------------------------
void setup() {
  // Setup serial monitor
  pinMode(M485PIN,OUTPUT);
  Serial.begin(9600);
  dht.begin(); //inicializa DHT22
  bmp.begin(); //inicializa BMP085
  inic_HC_SR04();

}
  

void loop() {
  
  switch(estado){
  
    case ESPERA:
      if(Serial.available()){
        
        valor=Serial.read();

        switch (valor){
          case '0':    // Se lee la temperatura con el DHT22
            estado = HUMEDAD;
          break;
          
          case '1':    // Se lee la humedad con el DHT22
            estado = HUMEDAD;
            
          break;
            
          case '2': // Se lee la temperatura con el LM35
            estado = TEMPERATURA;
            
          break;
          
          case '3': //se le la presion con el BMP085
            estado = PRESION;
            
          break;
          
          case '4':
            estado = DIR_VIENTO;

            
        }
      }
    break;
    
    case HUMEDAD:
      read_DHT();
      estado=ESPERA;
    break; 
      
    case TEMPERATURA:
      read_LM35();
      estado = ESPERA;
    break;
    
    case PRESION:
      read_BMP();
      estado=ESPERA;
    break;
    
    case DIR_VIENTO:
      read_HC_SR04();
      estado=ESPERA;
    break;
  }
  
}
//-----------------------------------------------------
 /*-----Funcion para inicializaciones de HC-SR04-----*/
void inic_HC_SR04(){
  pinMode(Trig, OUTPUT); /*activación del pin 9 como salida: para el pulso ultrasónico*/
  pinMode(Hecho, INPUT); /*activación del pin 8 como entrada: tiempo del rebote del ultrasonido*/
}
//-------------------------------------
//-------------------------------------
//-------------------------------------
void read_DHT(){
  
  int long var1;
  char hum_ent,hum_dec;  //parte entera de la humedad y parte decimal
  
  h = dht.readHumidity();  
  
  var1= (int long) (h * 100.0);  //paso a entero a los dos decimales
  
  hum_ent =  (var1 / 100);
  hum_dec =  (var1 % 100);

  
  Serial.println(hum_ent);
  
  Serial.println(hum_dec);


}
//-------------------------------------
//-------------------------------------
//-------------------------------------
void read_LM35(){
  char lectura;
  MAX_ENVIAR;
  delay(1);
  lectura = analogRead(LM35PIN);
  Serial.println(lectura); //Se envia el dato leido del AD, la conversion se realiza en la Galileo   
  MAX_RECIBIR;
}
//-------------------------------------
//-------------------------------------
//-------------------------------------
void read_BMP(){
  int var[3];

  /*
  *  var[0] -> decena y centena de mil
  *  var[1] -> unidad de mil y dentena
  *  var[2] -> decena y unidad
  *  ej: 998877 pascales -> var[0]=99; var[1]=88; var[2]=77; 
  *  Esto se hace para enviar el dato
  */
  p=bmp.readPressure(); // lee la presion
  
  //Serial.println(p);
  
  var[0] = p/10000;
  var[1] = (p%10000)/100;
  var[2] = p%100;
  
 Serial.println((char)var[0]);
 Serial.println((char)var[1]);
 Serial.println((char)var[2]);

}
//-------------------------------------
//-------------------------------------
//-------------------------------------

void read_HC_SR04(){
    float aux;
    int i=0;
    int control;
    for(i=0;i<5;i++){
    digitalWrite(Trig,LOW); /* Por cuestión de estabilización del sensor*/
    delayMicroseconds(5);
    digitalWrite(Trig, HIGH); /* envío del pulso ultrasónico*/
    delayMicroseconds(10);
    tiempo = pulseIn(Hecho,HIGH); /* Función para medir la longitud del pulso entrante. Mide el tiempo que ha transcurrido entre el envío
    del pulso ultrasónico y cuando el sensor recibe el rebote, es decir: desde que el pin empieza a recibir el rebote, HIGH, hasta que
    deja de hacerlo, LOW, la longitud del pulso entrante. En microsegundos!*/
    aux = 0.01715*tiempo; /*fórmula para calcular la distancia, usando la velocidad del sonido (343 m/s) -> 0.01715=(34300/(2*10^6)) cm/s*/
    control = aux;
    }
    
    Serial.println((char)control);  
 
}
//-------------------------------------
//-------------------------------------
//-------------------------------------


