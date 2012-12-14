#include "complex.h"
#include <stdlib.h>
#include <math.h>
#include "util.h"

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

int compute_Mandelbrot(Input in, int i, int j)
{
	complex_t z = new_zero_complex();
	complex_t c = new_complex(in.x_min + j * in.rezolutie, in.y_min + i * in.rezolutie);
	int step = 0;
	while(modul(z) < 2 && step < in.max_steps) {			
		z = add(mult(z, z), c);
		step++;
	}
	return step % NUM_COLORS;
}

int compute_Julia(Input in, int i, int j)
{
	complex_t c = new_complex(in.julia_param1, in.julia_param2);
	int step = 0;
	complex_t z = new_complex(in.x_min + j * in.rezolutie, in.y_min + i * in.rezolutie);
	while(modul(z) < 2 && step < in.max_steps) {			
		z = add(mult(z, z), c);
		step++;
	}
	return step % NUM_COLORS;
}
