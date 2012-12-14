#pragma once

/* Functii utile pentru lucrul cu numere complexe */

typedef struct _complex_t {
	double re; 
	double im; 
} complex_t; 

/* Initializeaza un numar complex z = 0 + 0i */
complex_t new_zero_complex();

/* Initializeaza un numar complex z = a + bi */
complex_t new_complex(double a, double b);

/* Copiaza numarul complex, similar cu comportamentul strdup */
complex_t complexdup(complex_t z);

/* Aduna doua numere complexe z + w */
complex_t add(complex_t z, complex_t w);

/* Inmulteste doua numere complexe z * w */
complex_t mult(complex_t z, complex_t w);

/* Intoarce modulul unui numar complex */
double modul(complex_t z);
