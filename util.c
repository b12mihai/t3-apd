#include "util.h"
#include "complex.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int master(int num_tasks, char *file_in, char* file_out)
{
	Input in;
	in = parse_input_file(file_in);
	
	if(in.tip_multime != 0 && in.tip_multime != 1) {
		printf("ERROR: Set not implemented or error parsing input file\n");
		MPI_Abort(MPI_COMM_WORLD, 911);
		exit(EXIT_FAILURE);
	}

	MPI_Status status;
	long *data;
	double start[START_MSG_LEN];
	int first_row, crt_row, block_size, default_block, remaining;
	int i, j;
	int width = floor((in.x_max - in.x_min) / in.rezolutie);
	int height = floor((in.y_max - in.y_min) / in.rezolutie);
	

	data = malloc((width + 1) * sizeof(long));
	
	/* Determina liniile pe care le va prelucra fiecare proces slave */
	first_row = 0;
    default_block = height / num_tasks; /* cate linii are de prelucrat un slave */
    remaining = height % num_tasks; /* cate linii mai raman */
    for (i = 0; i < num_tasks; ++i) {
        block_size = default_block;
        if (i < remaining)
            ++block_size;
        /* Transmite procesului slave prima linie pe care o are de prelucrat
         * impreuna cu numarul de linii si celelalte date utile
         * din fisierul de intrare de care are nevoie
         */    
        start[0] = first_row;
        start[1] = block_size;
        start[2] = in.tip_multime;
        start[3] = in.x_min;
        start[4] = in.x_max;
        start[5] = in.y_min;
        start[6] = in.y_max;
        start[7] = in.rezolutie;
        start[8] = in.max_steps;
        start[9] = in.julia_param1;
        start[10] = in.julia_param2;
        /* Anunta procesul slave ca are de lucru! */
        MPI_Send(start, START_MSG_LEN, MPI_DOUBLE, i + 1,
                START_TAG, MPI_COMM_WORLD);
        first_row += block_size;
    }

	/* Matricea de pixeli 0 - 255 */    
    int **colors;
   	colors = malloc(height * sizeof(int*));
	for(i = 0; i < height; ++i)
		colors[i] = malloc(width * sizeof(int));

    /* Primeste rezultate de la "sclavi" si salveaza in matricea de culori */    
    for (i = 0; i < height; ++i) {
        MPI_Recv(data, width + 1, MPI_LONG, MPI_ANY_SOURCE,
                DATA_TAG, MPI_COMM_WORLD, &status);
        /* Fiecare slave ii spune master-ului datele carei linii le-a trimis */
        crt_row = data[0];
        for (j = 0; j < width; ++j) {
            colors[crt_row][j] = data[j + 1];
        }
    }
    
    /* Creeaza imaginea */
    plot_pgm(file_out, width, height, colors);

	return EXIT_SUCCESS;
}

int slave(int rank)
{
    MPI_Status status;
    long *data;
    double start[START_MSG_LEN];
    int first_row, block_size;
    
    /* Primeste anuntul de START si obtine cantitatea de lucru:
     * first_row = linia de start 
     * block_size = cate linii prelucrez (deplasament)
     */ 
    MPI_Recv(start, START_MSG_LEN, MPI_DOUBLE, 0, START_TAG,
            MPI_COMM_WORLD, &status);
    Input in;
    first_row = (int)start[0];
    block_size = (int)start[1];
    in.tip_multime = (int)start[2]; 
    in.x_min = start[3];
    in.x_max = start[4];
    in.y_min = start[5];
    in.y_max = start[6];
    in.rezolutie = start[7]; 
    in.max_steps = start[8];
    in.julia_param1 = start[9];
    in.julia_param2 = start[10];
   	int width = floor((in.x_max - in.x_min) / in.rezolutie);
    data = malloc((width + 1) * sizeof(long));
    
    int i, j;
    
    if(in.tip_multime == 0) { /* Mandelbrot set */
    	/* Prelucreaza blocul de linii alocat, conform algoritmului din cerinta*/
		for(i = first_row; i < (first_row + block_size); ++i) {
			data[0] = i; /* Pentru ca masterul sa stie ca linie a prelucrat acest slave */
			for(j = 0; j < width; ++j) {
				data[j + 1] = compute_Mandelbrot(in, i ,j);
			}
			/* Trimite rezultatele liniei curente master-ului */
			MPI_Send(data, width + 1, MPI_LONG, 0, DATA_TAG, MPI_COMM_WORLD);
		}		
	} else if(in.tip_multime == 1) { /* Julia set */
	   	/* Prelucreaza blocul de linii alocat, conform algoritmului din cerinta
	   	 * Modul de lucru este identic cu cel de la Mandelbrot
	   	 */
		for(i = first_row; i < (first_row + block_size); ++i) {
			data[0] = i;
			for(j = 0; j < width; ++j) {
				data[j + 1] = compute_Julia(in, i, j);
			}
			MPI_Send(data, width + 1, MPI_LONG, 0, DATA_TAG, MPI_COMM_WORLD);
		}
	}
    
	return EXIT_SUCCESS;
}


Input parse_input_file(char* filename)
{
	FILE *in = fopen(filename, "r");
	Input data;
	
	if(!in) {
		printf("ERROR: while opening %s \n", filename);
		data.tip_multime = -1;
		return data;
	}
	
	fscanf(in, "%d", &data.tip_multime);
	fscanf(in, "%lf %lf %lf %lf", &data.x_min, &data.x_max, &data.y_min, &data.y_max);
	fscanf(in, "%lf", &data.rezolutie);
	fscanf(in, "%d", &data.max_steps);
	if(data.tip_multime == 1) {
		fscanf(in, "%lf %lf", &data.julia_param1, &data.julia_param2);
	} else {
		data.julia_param1 = 0./0.;
		data.julia_param2 = 0./0.;
	}
	
	fclose(in); 	
	return data;
}

void plot_pgm(char* filename, int width, int height, int **colors)
{
	FILE *f;
	f = fopen(filename, "w");
	int i, j;
	
	fprintf(f, "%s\n","P2");
	fprintf(f, "%d %d\n", width, height);
	fprintf(f, "%d\n", NUM_COLORS - 1);
	
	for(i = 0; i < height; ++i) {
		for(j = 0; j < width; ++j){
			fprintf(f, "%d ", colors[height - i - 1][j]);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}
