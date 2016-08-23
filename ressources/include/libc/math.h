#ifndef _MATH_H
#define _MATH_H

#include "stddef.h"
#include "stdarg.h"

int abs(int _X);
long labs(long _X);

double acos(double _X);
double asin(double _X);
double atan(double _X);
double atan2(double _Y,double _X);

double cos(double _X);
double cosh(double _X);
double exp(double _X);
double expm1(double _X);
double fabs(double _X);
double fmod(double _X,double _Y);
double log(double _X);
double log10(double _X);
double pow(double _X,double _Y);
double sin(double _X);
double sinh(double _X);
double tan(double _X);
double tanh(double _X);
double sqrt(double _X);

double atof(const char *_String);

double ceil(double _X);
double floor(double _X);
double frexp(double _X,int *_Y);
double ldexp(double _X,int _Y);

double modf(double _X,double *_Y);

long double ldexpl(long double _X,int _Y);

float acosf(float _X);
float asinf(float _X);
float atanf(float _X);
float atan2f(float _X,float _Y);
float cosf(float _X);
float sinf(float _X);
float tanf(float _X);
float expm1f(float _X);
float logf(float _X);
float log10f(float _X);
float modff(float _X,float *_Y);
float sqrtf(float _X);
float ceilf(float _X);
float floorf(float _X);
float fmodf(float _X,float _Y);

#define fabsf(x) ((float)fabs((double)(x)))
#define expf(x) ((float)exp((double)(x)))
#define sinhf(x) ((float)sinh((double)(x)))
#define coshf(x) ((float)cosh((double)(x)))
#define tanhf(x) ((float)tanh((double)(x)))
#define frexpf(x, y) ((float)frexp((double)(x), y))
#define ldexpf(x, y) ((float)ldexp((double)(x), y))
#define powf(x, y) ((float)pow((double)(x), (double)(y)))
 
#endif
