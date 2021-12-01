#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void merge(int *, int *, int, int, int);
void mergeSort(int *, int *, int, int);

int main(int argc, char** argv) {
  double startwtime,endwtime,totalTime;
  
  int n = 4;
  int *original_array = malloc(n * sizeof(int));
	
  int i;
  srand(time(NULL));

  for(i = 0; i < n; i++) {
    original_array[i] = rand() % n;
    printf("%d ", original_array[i]);
  }
  printf("\n");

  int rank;
  int world_size;
  
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  
  // Time
  totalTime=0;
  if(rank==0)
  {
     startwtime=MPI_Wtime();
  }
  
  // Dividing the array in equal-sized chunks
  int size = n/world_size;
  
  // Scattering each subarray to individual processes
  int *sub_array = malloc(size * sizeof(int));
  MPI_Scatter(original_array, size, MPI_INT, sub_array, size, MPI_INT, 0, MPI_COMM_WORLD);
  
  // Performing the mergesort on each process
  int *tmp_array = malloc(size * sizeof(int));
  mergeSort(sub_array, tmp_array, 0, (size - 1));
  
  // Gathering the sorted subarrays into one
  int *sorted = NULL;
  if(rank == 0) {
     sorted = malloc(n * sizeof(int));
  }
  
  MPI_Gather(sub_array, size, MPI_INT, sorted, size, MPI_INT, 0, MPI_COMM_WORLD);
  
  // Making the mergeSort call on the entire array
  if(rank == 0) {
     int *other_array = malloc(n * sizeof(int));
     mergeSort(sorted, other_array, 0, (n-1));
     
     // Displaying the sorted array
     printf("This is the sorted array: ");
     for(i = 0; i < n; i++) {
         printf("%d ", sorted[i]);
     }
     
     printf("\n");
     printf("\n");
     
     // Cleaning up root
     free(sorted);
     free(other_array);
  }
	
  // Cleaning up rest
  free(original_array);
  free(sub_array);
  free(tmp_array);
     
  // Finalize MPI
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0) {
     endwtime=MPI_Wtime();
     totalTime=endwtime-startwtime;
     printf("\nExecution time %f sec\n\n", totalTime);
  }
  MPI_Finalize();
}

/********** Merge Function **********/
void merge(int *a, int *b, int l, int m, int r) {
    int h, i, j, k;
    h = l;
    i = l;
    j = m + 1;
    
    while((h <= m) && (j <= r)) {
       if(a[h] <= a[j]) {
         b[i] = a[h];
         h++;
	} else {
	  b[i] = a[j];
	  j++;
	}
	
	i++;
    }
		
    if(m < h) {
    
      for(k = j; k <= r; k++) {
          b[i] = a[k];
          i++;
      }
    }
    else {
      for(k = h; k <= m; k++) {
          b[i] = a[k];
          i++;
      }
    }
    
    for(k = l; k <= r; k++) {
	a[k] = b[k];
		
    }
		
}

/********** Merge Function **********/
void mergeSort(int *a, int *b, int l, int r) {
    int m;
    if(l < r) {
       m = (l + r)/2;
       mergeSort(a, b, l, m);
       mergeSort(a, b, (m + 1), r);
       merge(a, b, l, m, r);
     }
}

