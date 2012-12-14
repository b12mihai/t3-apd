#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>

#include "complex.h"
#include "util.h"

/* Constants for MPI */
#define START_TAG   1           /* master to slave - start work */
#define DATA_TAG    2           /* slave to master - send data */
#define START_MSG_LEN   2

int master(int num_tasks, Input in, char* file_out)
{
	MPI_Status status;
	long *data, start[START_MSG_LEN];
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
         * impreuna cu numarul de linii 
         */    
        start[0] = first_row;
        start[1] = block_size;
        /* Anunta procesul slave ca are de lucru! */
        MPI_Send(start, START_MSG_LEN, MPI_LONG, i + 1,
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

int slave(int rank, Input in)
{
    MPI_Status status;
    long *data, start[START_MSG_LEN];
    int first_row, block_size;
   	int width = floor((in.x_max - in.x_min) / in.rezolutie);
	
    data = malloc((width + 1) * sizeof(long));
    
    /* Primeste anuntul de START si obtine cantitatea de lucru:
     * first_row = linia de start 
     * block_size = cate linii prelucrez (deplasament)
     */ 
    MPI_Recv(start, START_MSG_LEN, MPI_LONG, 0, START_TAG,
            MPI_COMM_WORLD, &status);
    first_row = start[0];
    block_size = start[1];
    
    int i = 0, j = 0, step = 0;
    complex_t z, c;
    
    if(in.tip_multime == 0) { /* Mandelbrot set */
    	/* Prelucreaza blocul de linii alocat, conform algoritmului din cerinta*/
		for(i = first_row; i < (first_row + block_size); ++i) {
			data[0] = i; /* Pentru ca masterul sa stie ca linie a prelucrat acest slave */
			for(j = 0; j < width; ++j) {
				z = new_zero_complex();
				step = 0;
				c = new_complex(in.x_min + j * in.rezolutie, in.y_min + i * in.rezolutie);
				while(modul(z) < 2 && step < in.max_steps) {			
					z = add(mult(z, z), c);
					step++;
				}
				data[j + 1] = step % NUM_COLORS;
			}
			/* Trimite rezultatele liniei curente master-ului */
			MPI_Send(data, width + 1, MPI_LONG, 0, DATA_TAG, MPI_COMM_WORLD);
		}		
	} else if(in.tip_multime == 1) { /* Julia set */
	   	/* Prelucreaza blocul de linii alocat, conform algoritmului din cerinta
	   	 * Modul de lucru este identic cu cel de la Mandelbrot
	   	 */
		z = new_zero_complex();
		c = new_complex(in.julia_param1, in.julia_param2);
		i = j = step = 0;
		for(i = first_row; i < (first_row + block_size); ++i) {
			data[0] = i;
			for(j = 0; j < width; ++j) {
				step = 0;
				z = new_complex(in.x_min + j * in.rezolutie, in.y_min + i * in.rezolutie);
				while(modul(z) < 2 && step < in.max_steps) {			
					z = add(mult(z, z), c);
					step++;
				}
				data[j + 1] = step % NUM_COLORS;
			}
			MPI_Send(data, width + 1, MPI_LONG, 0, DATA_TAG, MPI_COMM_WORLD);
		}
	}
    
	return EXIT_SUCCESS;
}

int main(int argc, char **argv) 
{
	if(argc != 3) {
		printf("USAGE: ./tema3 input_file output_image");
		exit(EXIT_FAILURE);
	}
	
	int num_tasks, rank, len, rc = 0;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	
	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
 		printf ("ERROR: could not start MPI program. QUIT.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
		exit(EXIT_FAILURE);
	}
	
	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(hostname, &len);
	
	Input in;
	in = parse_input_file(argv[1]);
	
	if(in.tip_multime != 0 && in.tip_multime != 1) {
		printf("ERROR: Set not implemented or error parsing input file\n");
		MPI_Abort(MPI_COMM_WORLD, 911);
		exit(EXIT_FAILURE);
	}
	
	if(rank == 0) {
		rc = master(num_tasks - 1, in, argv[2]);
	} else {
		rc = slave(rank, in);
	}
	
	MPI_Finalize();
	return rc;
}
