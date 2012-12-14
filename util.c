#include "util.h"
#include <stdio.h>
#include <stdlib.h>

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
