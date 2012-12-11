#pragma once

typedef struct _complex_t {
	double re; 
	double im; 
} complex_t; 

complex_t new_zero_complex();
complex_t new_complex(double a, double b);
complex_t complexdup(complex_t z);
complex_t add(complex_t z, complex_t w);
complex_t mult(complex_t z, complex_t w);
double modul(complex_t z);
