#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include <omp.h>

#define   SIZE   10
double start_time, run_time;
int main()
{
	
	float a[SIZE][SIZE], x[SIZE], ratio;
	int row,col,k,n;
	
	
    /* Inputs */
	/* 1. Reading number of unknowns */
	printf("Enter number of unknowns: ");
    scanf("%d", &n);
	

	// n=3;                                     // Known values of the given 3x3 matrix in Exercise 4
    // a[0][0]=2;
	// a[0][1]=-3;
	// a[0][2]=0;
	// a[0][3]=3;
	// a[1][0]=4;
	// a[1][1]=-5;
	// a[1][2]=1;
	// a[1][3]=7;
	// a[2][0]=2;
	// a[2][1]=-1;
	// a[2][2]=-3;
	// a[2][3]=5;
	 
	/* 2. Reading Augmented Matrix */
	for(row=0;row<n;row++)
	{
	  for(col=0;col<=n;col++)
	  {
	   printf("a[%d][%d]=",row,col);
	   scanf("%f", &a[row][col]);
	  }
	}

	/* Gaussian Elimination */
	for(row=0;row<n-1;row++)
	{
	 for(col=row+1;col<n;col++)
		{
		ratio = a[col][row]/a[row][row];
		for(k=0;k<=n;k++)
		a[col][k] = a[col][k] - ratio*a[row][k];
		}
	}
	/* Obtaining Solution by Back Subsitution */
	for(row=0;row<n;row++)
	{
	  for(col=0;col<n+1;col++)
	  {
	   printf("Intermediate a[%d][%d] =%f \n",row,col,a[row][col]);
	   
	  }
	}


	start_time = omp_get_wtime();
	for (row = n-1; row >= 0; row--) {
		x[row] = a[row][n];
		#pragma omp parallel for num_threads(4) shared(row,n,a,x)
		for (col = row+1; col < n; col++)
		    #pragma omp atomic
			x[row] -= a[row][col] * x[col];
		x[row] /= a[row][row];
	}
    run_time = omp_get_wtime() - start_time;
    printf("\nRow Oriented Backward Substitution: %lf seconds\n ",run_time);

/* Displaying Solution */ 
printf("\nSolution:\n");
for(row=0;row<n;row++)
printf("x[%d] = %0.3f\n",row, x[row]);
 
return(0);
}