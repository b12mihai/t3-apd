#pragma once

#define NUM_COLORS 256
/* Constants for MPI */
#define START_TAG   1           /* master to slave - start work */
#define DATA_TAG    2           /* slave to master - send data */
#define START_MSG_LEN   11

#include "mpi.h"

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

/* Descrie ceea ce trebuie sa faca un master:
 * Citeste fisierul de intrare
 * Imparte task-uri slave-urilor, dupa linii, in functie de num_tasks
 * Trimite slave-ului si datele de intrare fara de care nu poate calcula
 * Preia date de la "sclavi" si salveaza in matricea ce urmeaza sa 
 fie scrisa in fisier
 */
int master(int num_tasks, char *file_in, char* file_out);

/* Descrie ce trebuie sa faca slave-ul cu rank-ul rank:
 * Primeste linia de inceput pe care o are de prelucrat si 
 * deplasamentul (numarul de linii) de la master
 * Preia input si calculeaza partea lui si apoi o trimite la master
 * Nu are acces la fisierul de intrare
 */
int slave(int rank);
