
#include<Arduino.h>
// Encabezados de la librería horno

float TempRead(int sen);

void UpdatePast(float v[],int kT);

float PID(float u[], float e[], float q0, float q1, float q2);


