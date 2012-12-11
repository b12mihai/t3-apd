#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "complex.h"
#include "util.h"

int main(int argc, char **argv) 
{
	if(argc != 3) {
		printf("USAGE: ./tema3 input_file output_image");
		exit(EXIT_FAILURE);
	}
	
	
	int width, height, i = 0, j = 0, step, lastCounted;
	char **colors;
	double u, v;
	complex_t z, c;
	
	Input in;
	in = parse_input_file(argv[1]);
	
	if(in.tip_multime == -1) {
		exit(EXIT_FAILURE);
	}
	
	width = floor((in.x_max - in.x_min) / in.rezolutie);
	height = floor((in.y_max - in.y_min) / in.rezolutie);
		
	colors = malloc(height * sizeof(char*));
	for(j = 0; j < height; ++j)
		colors[j] = malloc(width * sizeof(char));
		
		
	switch (in.tip_multime) {
		case 0:
			z = new_zero_complex();
			i = j = step = 0;
			
			for(u = in.x_min ; u < in.x_max; u = u + in.rezolutie){
				for(v = in.y_min ; v < in.y_max; v = v + in.rezolutie) {
					z = new_zero_complex();
					step = 0;
					c = new_complex(u,v);
					while(modul(z) < 2 && step < in.max_steps) {			
						z = add(mult(z, z), c);
						step++;
					}
					colors[i / width][i % width] = step % NUM_COLORS;
					lastCounted = i;				
					++i;
					if(lastCounted / width != i / width || i / width == width) {
						break;
					}
				}
					if(i / width == width){
						break;
					}
			}
			plot_pgm(argv[2], width, height, colors);
			break;
		case 1:
			z = new_zero_complex();
			c = new_complex(in.julia_param1, in.julia_param2);
			i = j = step = 0;
		
			for(u = in.x_min; u < in.x_max; u = u + in.rezolutie){
				j = 0;			
				for(v = in.y_min ; v < in.y_max; v = v + in.rezolutie) {
						z = new_complex(u, v);
						step = 0;
						while( modul(z) < 2 && step < in.max_steps) {
							z = add(mult(z, z), c);
							step++;
						}
						colors[i / width][j] = step % NUM_COLORS;
						lastCounted = i;				
						++i; ++j;
						if(j == height){
							break;
						}
					}
					if(i / width == width){
						break;
					}
				
		
				}
			plot_pgm(argv[2], width, height, colors);
			break;
		
		default:
			printf("ERROR: Set not implemented. Accepting only Mandelbrot (0) or Julia (1) \v");
			exit(EXIT_FAILURE);
	}
	
	return 0;
}
