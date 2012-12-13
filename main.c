#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>

#include "complex.h"
#include "util.h"

int master(int num_tasks, Input in, char* file_out)
{
	int width, height, this_row, row, col;
	int **colors;
	MPI_Status status;
	int *data_msg;
	
	width = floor((in.x_max - in.x_min) / in.rezolutie);
	height = floor((in.y_max - in.y_min) / in.rezolutie);
	
	data_msg = malloc((width + 1) * sizeof(int));
		
	colors = malloc(height * sizeof(int*));
	for(row = 0; row < height; ++row)
		colors[row] = malloc(width * sizeof(int));
		
	for(row = 0; row < height; ++row) {
		
		MPI_Recv(data_msg, width + 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
		
		this_row = data_msg[0];
	
		for(col = 0; col < width; ++col) {
			colors[this_row][col] = data_msg[col + 1];
		}
	}
	
	plot_pgm(file_out, width, height, colors);
	return EXIT_SUCCESS;
}

int slave(int rank, int num_tasks, Input in)
{
	int width, height, this_row, row, col;
	int **colors;
	MPI_Status status;
	int *data_msg;
	complex_t z, c;
	double u, v;
	
	width = floor((in.x_max - in.x_min) / in.rezolutie);
	height = floor((in.y_max - in.y_min) / in.rezolutie);
	
	data_msg = malloc((width + 2) * sizeof(int));
	
	colors = malloc(height * sizeof(int*));
	for(row = 0; row < height; ++row)
		colors[row] = malloc(width * sizeof(int));
	int step = 0;
	for(row = (rank - 1); row < height; row = row + num_tasks) {
		data_msg[0] = row;
		
		for(col = 0; col < width; ++col) {
			z = new_zero_complex();
			step = 0;
			u = in.x_min + row * in.rezolutie;
			v = in.y_min + col * in.rezolutie;
			c = new_complex(u,v);
			while(modul(z) < 2 && step < in.max_steps) {			
				z = add(mult(z, z), c);
				step++;
			}
			data_msg[col + 1] = step % NUM_COLORS; 
		}
		 MPI_Send(data_msg, width+1, MPI_INT, 0, 1,
                MPI_COMM_WORLD);
	}
		
	return EXIT_SUCCESS;
}

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
	
	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(hostname, &len);
	
	//fprintf(stderr, "Number of tasks = %d \n My rank= %d \n Running on %s\n",
//			num_tasks,rank,hostname);
	
	Input in;
	in = parse_input_file(argv[1]);
	
	if(in.tip_multime == -1) {
		exit(EXIT_FAILURE);
	}
	
	if(rank == 0) {
		rc = master(num_tasks, in, argv[2]);
	} else {
		rc = slave(rank, num_tasks, in);
	}
	
/*	width = floor((in.x_max - in.x_min) / in.rezolutie);
	height = floor((in.y_max - in.y_min) / in.rezolutie);
		
	colors = malloc(height * sizeof(int*));
	for(j = 0; j < height; ++j)
		colors[j] = malloc(width * sizeof(int));
		
		
	switch (in.tip_multime) {
		case 0:
			z = new_zero_complex();
			i = j = step = 0;
			
			// for(u = in.x_min ; u < in.x_max; u = u + in.rezolutie){
			//	for(v = in.y_min ; v < in.y_max; v = v + in.rezolutie) { 
			for(j = 0; j < height; ++j) {
				for(i = 0; i < width; ++i) {	
					z = new_zero_complex();
					step = 0;
					u = in.x_min + i * in.rezolutie;
					v = in.y_min + j * in.rezolutie;
					c = new_complex(u,v);
					while(modul(z) < 2 && step < in.max_steps) {			
						z = add(mult(z, z), c);
						step++;
					}
					colors[i][j] = step % NUM_COLORS;
//					colors[i / width][i % width] = step % NUM_COLORS;
//					lastCounted = i;				
//					++i;
//					if(lastCounted / width != i / width || i / width == width) {
//						break;
//					}
				}
//					if(i / width == width){
//						break;
//					}
			}
			plot_pgm(argv[2], width, height, colors);
			break;
		case 1:
			z = new_zero_complex();
			c = new_complex(in.julia_param1, in.julia_param2);
			i = j = step = 0;
//			for(u = in.x_min; u < in.x_max; u = u + in.rezolutie){
//				j = 0;			
//				for(v = in.y_min ; v < in.y_max; v = v + in.rezolutie) {
			for(j = 0; j < height; ++j) {
				for(i = 0; i < width; ++i) {
						u = in.x_min + i * in.rezolutie;
						v = in.y_min + j * in.rezolutie;
						z = new_complex(u, v);
						step = 0;
						while( modul(z) < 2 && step < in.max_steps) {
							z = add(mult(z, z), c);
							step++;
						}
						colors[i][j] = step % NUM_COLORS;
//						colors[i / width][j] = step % NUM_COLORS;
//						lastCounted = i;				
//						++i; ++j;
//						if(j == height){
//							break;
//						}
					}
//					if(i / width == width){
//						break;
//					}	
				}
			plot_pgm(argv[2], width, height, colors);
			break;
		
		default:
			printf("ERROR: Set not implemented. Accepting only Mandelbrot (0) or Julia (1) \v");
			exit(EXIT_FAILURE);
	}
	*/
	
	MPI_Finalize();
	return rc;
}
