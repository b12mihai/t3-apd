#pragma once

#define NUM_COLORS 256

/* Date din fisierul de intrare */
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

/* Salveaza datele din fisierul de intrare */
Input parse_input_file(char* filename);

/* Scrie imaginea PGM */
void plot_pgm(char* filename, int width, int height, int **colors);
