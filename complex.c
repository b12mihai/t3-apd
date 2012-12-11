#include "complex.h"
#include <stdlib.h>
#include <math.h>

complex_t new_zero_complex()
{
	complex_t z;
	z.re = z.im = 0;
	return z;
}

complex_t new_complex(double a, double b)
{
	complex_t z;
	z.re = a; z.im = b;
	return z;
}

complex_t complexdup(complex_t z)
{
	complex_t w;
	w.re = z.re; w.im = z.im;
	return w;
}

complex_t add(complex_t z, complex_t w)
{
	complex_t u;
	u.re = z.re + w.re;
	u.im = z.im + w.im;
	return u;
}

complex_t mult(complex_t z, complex_t w)
{
	complex_t r;
	r.re = z.re * w.re - z.im * w.im;
	r.im = z.re * w.im + z.im * w.re;
	return r;
}

double modul(complex_t z)
{
	return sqrt(z.re * z.re + z.im * z.im);
}
