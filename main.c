#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

#include "complex.h"
#include "util.h"

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
	
	if(rank == 0) {
		rc = master(num_tasks - 1, argv[1], argv[2]);
	} else {
		rc = slave(rank);
	}
	
	MPI_Finalize();
	return rc;
}
