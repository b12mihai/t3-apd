#pragma once

#define NUM_COLORS 256
#define DATA_TAG 1  /* "data" message (worker to master) */

typedef struct _input {
	int tip_multime;
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	double rezolutie;
	int max_steps;
	double julia_param1;
	double julia_param2;
} Input;

Input parse_input_file(char* filename);
void plot_pgm(char* filename, int width, int height, char **colors);
