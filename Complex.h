#pragma once

#include <math.h>
#include <stdlib.h>

class Complex {
	public:
		double re, im;
		Complex(double _re, double _im) : re(_re), im(_im) {}
		Complex () : re(0), im(0) {}
}
