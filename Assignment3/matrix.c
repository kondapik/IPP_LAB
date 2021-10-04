#include <stdio.h>
#include <omp.h>


int i,j,k;

int dim =1000;
int c[1000][1000] ;  // easier way
int a[1000][1000];  // easier way
int b[1000][1000] ;  // easier way
int sum;
double start_time, run_time;
// this does the same

int main(){
	/* static scheduling of matrix multiplication loops */
	#pragma omp parallel shared (a, b, c, dim) \
	num_threads(8)
	{
		start_time = omp_get_wtime();
		#pragma omp for schedule(static)

			
			for (i = 0; i < dim; i++) {
		#pragma omp parallel for schedule(static)
				for (j = 0; j < dim; j++) {
					c[i][j] = 0;
			    #pragma omp parallel for schedule(static)
					for (k = 0; k < dim; k++) {
						c[i][j] = c[i][j] + a[i][k]*b[k][j];
					}
			
			}
		}
	}
	  run_time = omp_get_wtime() - start_time;
/* 	  	for (i = 0; i < dim; i++) {
      for (j = 0; j < dim; j++)
        printf("%d\t", c[i][j]);
 
      printf("\n"); 
    }*/
	  printf("\n Multiplication complete for dim %d in %lf seconds\n ",dim,run_time);
}