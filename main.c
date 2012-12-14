#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>

#include "complex.h"
#include "util.h"

/* Constants for message-passing */
#define START_TAG   1           /* "start" message (master to worker) */
#define DATA_TAG    2           /* "data" message (worker to master) */
#define START_LNG   2

int master(int nworkers, int width, int height, Input in, char* file_out)
{
	MPI_Status status;
	long *data_msg, start_msg[START_LNG];
	int first_row, this_row, rows, rows_per_worker_quot, rows_per_worker_rem;
	int p;
	
	data_msg = malloc((width + 1) * sizeof(long));
	
	/* Calculate which rows each worker should work on. */
    first_row = 0;
    rows_per_worker_quot = height / nworkers;
    rows_per_worker_rem = height % nworkers;
    for (p = 0; p < nworkers; ++p) {
        rows = rows_per_worker_quot;
        if (p < rows_per_worker_rem)
            ++rows;
        start_msg[0] = first_row;
        start_msg[1] = rows;
        MPI_Send(start_msg, START_LNG, MPI_LONG, p+1,
                START_TAG, MPI_COMM_WORLD);
        first_row += rows;
    }
    
    /* Receive results from workers and draw points */
    int row, col;
    int **colors;
   	colors = malloc(height * sizeof(int*));
	for(row = 0; row < height; ++row)
		colors[row] = malloc(width * sizeof(int));
    
    for (row = 0; row < height; ++row) {

        MPI_Recv(data_msg, width+1, MPI_LONG, MPI_ANY_SOURCE,
                DATA_TAG, MPI_COMM_WORLD, &status);

        this_row = data_msg[0];

        for (col = 0; col < width; ++col) {
            //colors[this_row][col] = data_msg[col + 1];
            colors[col][this_row] = data_msg[col + 1];
           // printf("%d ", colors[this_row][col]);
        }
    }
    
    plot_pgm(file_out, width, height, colors);

	return EXIT_SUCCESS;
}

int slave(int rank, int width, int height, Input in)
{
	long start_msg[START_LNG];
    MPI_Status status;
    int first_row, rows;
    long *data_msg;
    data_msg = malloc((width + 1) * sizeof(long));
    
    /* Receive "start" message and extract data */
    MPI_Recv(start_msg, START_LNG, MPI_LONG, 0, START_TAG,
            MPI_COMM_WORLD, &status);
    first_row = start_msg[0];
    rows = start_msg[1];
    
    int row, col, step = 0;
    complex_t z,c;
    double u,v;
    
    if(in.tip_multime == 0) { // Mandelbrot set   
		for(row = first_row; row < (first_row + rows); ++row) {
			data_msg[0] = row;
			for(col = 0; col < width; ++col) {
				z = new_zero_complex();
				step = 0;
				u = in.x_min + col * in.rezolutie;
				v = in.y_min + row * in.rezolutie;
				c = new_complex(u,v);
				while(modul(z) < 2 && step < in.max_steps) {			
					z = add(mult(z, z), c);
					step++;
				}
				data_msg[col + 1] = step % NUM_COLORS;
			}
			MPI_Send(data_msg, width+1, MPI_LONG, 0, DATA_TAG, MPI_COMM_WORLD);
		}
	} else if(in.tip_multime == 1) { //Julia
		z = new_zero_complex();
		c = new_complex(in.julia_param1, in.julia_param2);
		row = col = step = 0;
		for(row = first_row; row < (first_row + rows); ++row) {
			data_msg[0] = row;
			for(col = 0; col < width; ++col) {
				step = 0;
				u = in.x_min + col * in.rezolutie;
				v = in.y_min + row * in.rezolutie;
				z = new_complex(u,v);
				while(modul(z) < 2 && step < in.max_steps) {			
					z = add(mult(z, z), c);
					step++;
				}
				data_msg[col + 1] = step % NUM_COLORS;
			}
			MPI_Send(data_msg, width+1, MPI_LONG, 0, DATA_TAG, MPI_COMM_WORLD);
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
	
	if(in.tip_multime == -1) {
		exit(EXIT_FAILURE);
	}
	
	int width = floor((in.x_max - in.x_min) / in.rezolutie);
	int height = floor((in.y_max - in.y_min) / in.rezolutie);
	
	if(rank == 0) {
		rc = master(num_tasks - 1, width, height, in, argv[2]);
		//printf("Cred ca a mers \n");
	} else {
		rc = slave(rank, width, height, in);
	}
	
	MPI_Finalize();
	return rc;
}
