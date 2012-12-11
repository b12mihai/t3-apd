#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>

#include "complex.h"
#include "util.h"

int master(int num_tasks, Input in, char* file_out);
int slave(int slave_rank, int num_tasks, Input in);

int main(int argc, char **argv) 
{
	if(argc != 3) {
		printf("USAGE: ./tema3 input_file output_image");
		exit(EXIT_FAILURE);
	}
	
	int num_tasks, rank, len, rc;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	
	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
 	printf ("ERROR: could not start MPI program. QUIT.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	
	rc = MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
	if (rc != MPI_SUCCESS) {
 	printf ("ERROR: could not get number of tasks. QUIT.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	rc = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rc != MPI_SUCCESS) {
 	printf ("ERROR: could not get current rank/ID. QUIT.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	rc = MPI_Get_processor_name(hostname, &len);
	if (rc != MPI_SUCCESS) {
 	printf ("ERROR: could not get processor host name. QUIT.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
	
	fprintf(stderr, "Number of tasks = %d \n My rank= %d \n Running on %s\n",
			num_tasks,rank,hostname);
	
	int return_value;
	Input in;
	in = parse_input_file(argv[1]);
	
	if(in.tip_multime == -1) {
		exit(EXIT_FAILURE);
	}
		
	if(rank == 0) {
		//do master work
		return_value = master(num_tasks - 1, in, argv[2]);
	} else {
		return_value = slave(rank, num_tasks - 1, in);
	}
	
	MPI_Finalize();
	return 0;
}

int master(int num_tasks, Input in, char* file_out) 
{
	int row, col, this_row;
	int *data_msg;
	MPI_Status status;
	
	int width = floor((in.x_max - in.x_min) / in.rezolutie);
	int height = floor((in.y_max - in.y_min) / in.rezolutie);
	
	int **colors;
	colors = malloc(height * sizeof(int*));
	for(row = 0; row < height; ++row)
		colors[row] = malloc(width * sizeof(int));
	
	data_msg = malloc((width + 1) * sizeof(int));
	
	for(row = 0; row < height; ++row) {
		//Receive results from slaves
		MPI_Recv(data_msg, width + 1, MPI_INT, MPI_ANY_SOURCE, DATA_TAG,
				MPI_COMM_WORLD, &status);
		this_row = data_msg[0];
		
		for(col = 0; col < width; ++col) {
			//save colors for pgm plot
			colors[this_row][col] = data_msg[col + 1];
		}
	}
	
	plot_pgm(file_out, width, height, colors);
	
	//free(data_msg);
	return EXIT_SUCCESS;
}

int slave(int rank, int num_tasks, Input in)
{
	int row, col, steps = 0;
	int width = floor((in.x_max - in.x_min) / in.rezolutie);
	int height = floor((in.y_max - in.y_min) / in.rezolutie);
	
	int *data_msg;
	data_msg = malloc((width + 1) * sizeof(int));
	
	complex_t z, c;
	if(in.tip_multime == 0) {
		for(row = (rank - 1); row < height; ++row) {
			data_msg[0] = row;
			for(col = 0; col < width; ++col) {
				z = new_zero_complex();
				c.re = in.x_min + ((double) col * in.rezolutie);
				c.im = in.y_min + ((double) (height - row - 1) * in.rezolutie);
			
				steps = 0;
				do {
					z = add(mult(z, z), c);
					steps++;
				} while(modul(z) < 2 && steps < in.max_steps);
				
				data_msg[col + 1] = steps % NUM_COLORS;
			}
			MPI_Send(data_msg, width + 1, MPI_INT, 0, DATA_TAG, MPI_COMM_WORLD);
		}
	} else if(in.tip_multime == 1) {
		//TODO: Julia
	} else 
		return EXIT_FAILURE;
	
	

	return EXIT_SUCCESS;
}








