#include "Horno.h"


float TempRead(int sen)
{
  float S1,aux;
  int i;
  //Filtro de promedio movil en la lectura ADC
  //aux=0;
  // for(i=0;i<10;i++){
  //   //cli();
  //   aux = aux + (((2.0*(float(analogRead(sen))*2.56/1023.0-0.04532))/(109.21*0.0459*0.0036137))+5.0); //pt100
  // //  sei();
  //   //delay(1);
  // }
  //S1 = aux/10;
  S1 =(((2.0*(float(analogRead(sen))*2.56/1023.0-0.04532))/(109.21*0.0459*0.0036137))+5.0); //pt100
  return(S1);
}

/*=======================    FUNCION DE ACTUALIZACIÓN  =======================*/

// Esta función desplaza todo el vector una unidad hacia atras para actualizar
// los valores pasados en la ecuaciones causales de los controladores
// float v: El vector que deseamos desplazar (Actualizar)
// int kT:  Es la ultima posición del vector v
void UpdatePast(float v[],int kT){
   int i;
   for(i=1;i<=kT;i++){
      v[i-1]=v[i];
   }
}
/*=======================    FUNCION DEL CONTROL PID  =======================*/

float PID(float u[], float e[3], float q0, float q1, float q2)
{
    float lu;
    // Control PID
    // e[2] = e(k)
    // e[1] = e(k-1)
    // e[0] = e(k-2)
    // u[0] = u(k-1)
    lu = u[0] + q0*e[2] + q1*e[1] + q2*e[0]; //Ley del controlador PID discreto
    
    
    //Anti - Windup
    if (lu >= 100.0) {
       lu = 100.0;
    }       
    
    if (lu <= 0.0){
      lu = 0.0;
    }
    
     return(lu);     
}

