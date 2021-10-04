#include <iostream>
#include <omp.h>

using namespace std;

int i,j,k;

int dim =1000;
int c[1000][1000] ;  // easier way
int a[1000][1000];  // easier way
int b[1000][1000] ;  // easier way
int sum;
double start_time, run_time;

/* static scheduling of matrix multiplication loops */
int main (){
    #pragma omp parallel shared (a, b, c, dim) 
    {
        start_time = omp_get_wtime();
        cout << omp_get_num_threads() << endl;
        #pragma omp for schedule(static)
        for (i = 0; i < dim; i++) {
            for (j = 0; j < dim; j++) {
                c[i][j] = 0;
                for (k = 0; k < dim; k++) {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }
    }
    run_time = omp_get_wtime() - start_time;
    printf("Multiplication complete for dim %d in %lf seconds\n ",dim,run_time);
}