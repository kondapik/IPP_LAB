#include <iostream>
#include <omp.h>

//using namespace std;

int i,j,k;
#define DIM 1000

long c[DIM][DIM] ;  // easier way
int a[DIM][DIM];  // easier way
int b[DIM][DIM] ;  // easier way
int sum;
double start_time, run_time;

void init(int arr[][DIM]) {                                              //Initiazing 'a' and 'b' matrices with random values
    for (int i = 0; i <DIM; i++){
        for(int k = 0; k < DIM; k++){
            arr[i][k] = rand();
            c[i][k] = 0;
        }
    }
}

/* static scheduling of matrix multiplication loops */
int main (){

    init(a);
    init(b);

    #pragma omp parallel shared (a, b, c) private(j,k)
    {
        

        start_time = omp_get_wtime();
        #pragma omp for schedule(static) collapse (3)
        for (i = 0; i < DIM; i++) {
            for (j = 0; j < DIM; j++) {
                for (k = 0; k < DIM; k++) {
                #pragma omp atomic     
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }
    }
    run_time = omp_get_wtime() - start_time;
    //printf("Multiplication complete for DIM %d in %lf seconds\n ",DIM,run_time);
    printf("%lf seconds\n ",run_time);
}