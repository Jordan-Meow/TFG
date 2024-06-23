/*===========  DEFINICIONES COMPONENTES, ACTUADOR,LEDS, LIBRERIAS ==============*/
// Librerias utilizadas
#include "Horno.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Sensor Pt100
#define PT100 A4
// Actuador MOSFET
#define MOSFET 6
// Indicadores luminosos LEDS
#define COLD 8
#define HOT 10
// Ventilador
#define FAN 40
// Definir constantes de la pantalla
// Ancho pantalla OLED
#define ANCHO_PANTALLA 128
// Alto pantalla OLED
#define ALTO_PANTALLA 64
// Creación del objeto display
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);
// Definicion de constantes
const unsigned long INTERVALO_DATOS = 1000UL;
const unsigned long INTERVALO_MUESTREO = 20000UL;
unsigned long actual, actual2;
unsigned long evento_datos = 1000UL;
unsigned long evento_muestreo = 0UL;
unsigned long ultimoCambioModo = 0.0;
const float parar = 300.0;
const float auxM = 301.0;
const float auxA = 302.0;
bool bandeA = false;
bool bandeM = false;
bool bandeR = false;
bool automatico = false;
bool cambioModo = false;
/*===========    VARIABLES GLOBALES ==============*/
//Temperatura del sensor
float T1;
//Actuador
float H1 = 0.0;
// Referencia
float r = 0.0;

float auxmodo;

//Parámetros del PID teóricos interactivos
float kc, ti, td;
//Parámetros del PID teóricos esquema paralelo
float kp, ki, kd;
//Variables para el controlador
float proporcional, derivativo, aux_u, Tt;
float lastintegral = 0.0;
float integral;
float es = 0.0;
float salida, lastsalida, dsalida;
//Señal de control
float u_control, u_max, u_min;
//Señal del error
float error;
float lasterror;
//Variables auxiliares
String comparar;
//Variable para definir el modo de control Manual o Automatico
bool controlA = false;
bool controlM = false;
bool Control = false;
/*===========   MODELADO DEL SISTEMA  ==============*/
//tramo 1 de temperatura ambiente a 142 grados
float K = 6.1826, tau1 = 452.62, tau2 = 334.93, tr = 10;
float lmd = 394;
//Periodo de Muestreo
int Ts = 20;  

/*=========== FUNCION SETUP ==============*/

void setup() {
  pinMode(HOT, OUTPUT);  //Led "Caliente" como salida
  pinMode(COLD, OUTPUT);
  pinMode(FAN, OUTPUT);
  digitalWrite(HOT, LOW);
  digitalWrite(COLD, LOW);
  digitalWrite(FAN, LOW);
  analogReference(INTERNAL2V56);  //Referencia analógica PIN AREF (2.56V)
  //Configuración del puerto serial
  Serial.begin(9600);
  Serial.setTimeout(100);
  analogWrite(MOSFET, 0);
  //Inicializacion de la pantalla oled
  Pantalla();
  /*===========  PARAMETROS DEL CONTROLADOR NO INTERACTIVO ==============*/
  kc = 0.3153;
  ti = 787.55;
  td = 192.4907;

  //Pasar los parámetros del controlador no interactivo a paralelo
  kp = kc;
  ki = (kc) / (ti);
  kd = kc * (td);
  //Tiempo de tracking para el back calculation
  Tt = 389.3533;
  //Definición de los valores máximos del actuador (%PWM)
  u_max = 60.0;
  u_min = 0.0;
}
/*===========  CONTROLADOR PID DIGITAL UTILIZANDO APROXIMACIONES POR BLOQUES ==============*/

/*=========== FUNCION PRINCIPAL  ==============*/

void loop() {
  T1 = TempRead(PT100);
  actual = millis();
  String dato, degC;
  Iluminacion();
  Ventilacion();

  if (!cambioModo && Serial.available()) {
    //leemos el dato enviado
    String dato = Serial.readString();
    int ini = dato.indexOf('I') + 1;
    int fin = dato.indexOf('F', ini);
    if (ini > 0 && fin > ini) {
      String degC = dato.substring(ini, fin);
      auxmodo = degC.toFloat();

      if (auxmodo == parar) {
        Reset();
      }

      if (auxmodo == auxA) {
        controlA = true;
        cambioModo = true;
        ultimoCambioModo = actual;
        Control = true;
      }

      if (auxmodo == auxM) {
        controlM = true;
        cambioModo = true;
        Control = true;
      }
    }
  }


  if (controlA) {
    ControlAutomatico();
    if (actual - ultimoCambioModo >= evento_muestreo) {
      evento_muestreo = INTERVALO_MUESTREO;
      ultimoCambioModo = actual;
      PID_Back_calculation();
    }

  } else {
    ControlManual();
  }

  if (actual >= evento_datos) {
    EnviarDatos();
    evento_datos += INTERVALO_DATOS;
    Apantalla();
  }
}

/*=======================   PROCEDIMIENTOS  =======================*/

void Reset(void) {
  H1 = 0.0;
  analogWrite(MOSFET, 0);
  //bandeR=true;
  bandeA = false;
  bandeM = false;
  Control = false;
  controlA = false;
  controlM = false;
  automatico = false;
  cambioModo = false;
  proporcional = 0.0;
  derivativo = 0.0;
  lastintegral = 0.0;
  es = 0.0;
  salida = 0.0;
  lastsalida = 0.0;
  integral = 0.0;
  error = 0.0;
  auxmodo = 0.0;
  r = 0.0;
  aux_u = 0.0;
  u_control = 0.0;
  ultimoCambioModo = 0.0;
  evento_muestreo = 0.0;
}

/*=======================    PANTALLA  =======================*/
void Pantalla(void) {

  // Iniciar pantalla OLED en la dirección 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
#ifdef __DEBUG__
    Serial.println("No se encuentra la pantalla OLED");
#endif
    while (true)
      ;
  }
  // Clear the buffer.
  display.clearDisplay();
}


void Apantalla(void) {
  static const unsigned char PROGMEM datos_imagen[72] = { 
  0xff, 0xff, 0xfc, 0xbf, 0xff, 0xec, 0x9f, 0xff, 0xc4,
  0xa7, 0xff, 0x84, 0xb3, 0xff, 0x24, 0xa9, 0x8e, 0xd4,
  0xa4, 0x01, 0x94, 0xa3, 0xff, 0x14, 0xaf, 0xff, 0xd4,
  0xbf, 0xff, 0xf4, 0xbf, 0xff, 0xf4, 0xbf, 0xff, 0xf4,
  0xb8, 0xfc, 0xf4, 0xbd, 0xfc, 0xf4, 0xbf, 0xff, 0xf4,
  0xbf, 0x8f, 0xf4, 0xa0, 0x84, 0x34, 0x9d, 0xcf, 0xec,
  0xd6, 0x5b, 0xec, 0xc9, 0xce, 0xdc, 0xe6, 0x73, 0x9c,
  0xf3, 0xfe, 0x3c, 0xfc, 0x70, 0xfc, 0xff, 0x03, 0xfc 
  };

  static String temp, control;
  temp = String(T1, 2);
  control = String(H1, 2);
  // Tamaño del texto
  display.setTextSize(2);
  // Color del texto
  display.setTextColor(SSD1306_WHITE);
  // Posición del texto
  display.setCursor(0, 0);
  //Pintar bitmap en la pantalla
  display.drawBitmap(106, 40, datos_imagen, 22, 24, SSD1306_WHITE);
  display.display();
  // Escribir texto
  display.print(temp);
  display.setCursor(70, 0);
  display.setTextSize(1);
  display.write(248);
  display.setTextSize(2);
  display.println("C");
  display.print(control);
  display.setCursor(70, 16);
  display.write(37);
  display.print("PWM");
  // Enviar a pantalla
  display.display();

  if (Control) {
    display.setCursor(0, 48);
    display.println("ON");
    display.display();
  } else {
    display.setCursor(0, 48);
    display.println("OFF");
    display.display();
  }

  display.clearDisplay();
}

void Ventilacion(void) {

  //Indicación Luminica
  if (H1 > 5.0) {
    //HORNO CALIENTE
    digitalWrite(FAN, HIGH);
  } else {
    //HORNO FRIO
    digitalWrite(FAN, LOW);
  }
}

void Iluminacion(void) {

  //Indicación Luminica
  if (T1 > 30.0) {
    //HORNO CALIENTE
    digitalWrite(HOT, HIGH);
    digitalWrite(COLD, LOW);
  } else {
    //HORNO FRIO
    digitalWrite(COLD, HIGH);
    digitalWrite(HOT), LOW);
  }
}

/*=======================    FUNCION DEL CONTROL MANUAL =======================*/
void ControlManual(void) {
  static String datoManual, Maux;
  static float HManual;

  if (controlM && Serial.available()) {
    //leemos el dato enviado
    String datoManual = Serial.readString();
    int ini = datoManual.indexOf('I') + 1;
    int fin = datoManual.indexOf('F', ini);
    if (ini > 0 && fin > ini) {
      String Maux = datoManual.substring(ini, fin);
      HManual = Maux.toFloat();

      if (HManual == parar) {
        Reset();
      }
      if (HManual == auxA) {
        controlM = false;
        controlA = true;
        ultimoCambioModo = actual;
      }

      if ((HManual != parar) && (HManual != auxA)) {
        H1 = HManual;
        bandeM = true;
      }
    }
  }

  if (H1 > 60.0) {
    H1 = 60.0;
  }
  if (H1 < 0.0) {
    H1 = 0.0;
  }
  lastintegral = H1;
  es = 0.0;
  r = TempRead(PT100);
  lastsalida = r;
  analogWrite(MOSFET, map(H1, 0, 100, 0, 255));
}
// /*=======================    FUNCION DEL CONTROL AUTOMATICO =======================*/
void ControlAutomatico(void) {
  static String datoAuto, Aaux;
  static float HAuto;

  if (controlA && Serial.available()) {
    //leemos el dato enviado
    String datoAuto = Serial.readString();
    int ini = datoAuto.indexOf('I') + 1;
    int fin = datoAuto.indexOf('F', ini);
    if (ini > 0 && fin > ini) {
      String Aaux = datoAuto.substring(ini, fin);
      HAuto = Aaux.toFloat();

      if (HAuto == parar) {
        Reset();
      }
      if (HAuto != parar) {
        r = HAuto;
        bandeA = true;
      }
    }
  }
}

void PID_Back_calculation(void) {

  /*Calculo del controlador utilizando aproximaxiones euler hacia atrás  
  para la integral junto con back calculation para el antiwindup, 
  el derivativo utiliza la aproximación hacia atrás*/
  
  //leer salida actual
  salida = TempRead(PT100);

  //Cálculo de la señal del error actual
  error = r - salida;
  //Cálculo de la señal de salida actual
  dsalida = salida - lastsalida;

  //Cálculo de la Acción proporcional
  proporcional = kp * error;
  // Cálculo del termino integral
  integral = lastintegral + Ts * (ki * error + (es / (Tt)));
  // Cálculo del termino derivativo
  derivativo = -(kd * dsalida) / Ts;
  //derivativo = (kd * (error-lasterror)) / Ts;
  //Cálculo de la señal de control actual
  u_control = proporcional + integral + derivativo;
  aux_u = u_control;
  if (u_control > u_max) {
    u_control = u_max;
  }
  if (u_control < u_min) {
    u_control = u_min;
  }
  es = u_control - aux_u;
  H1 = u_control;
  //Aplica la acción de control en el PWM
  analogWrite(MOSFET, map(H1, 0, 100, 0, 255));  //Max=100, Min=0

  //Actualización de las señales de error y salida
  lasterror = error;
  lastsalida = salida;
  lastintegral = integral;
}

/*=======================   ENVIO DE DATOS POR EL PUERTO SERIE =======================*/
void EnviarDatos(void) {
  //Usar la interfaz de Matlab
  char str[50];
  sprintf(str, "%lu %d %d\n", millis(), int(T1 * 100), int(H1));
  Serial.print(str);
 
}


