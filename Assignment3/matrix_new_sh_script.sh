for threads in 1 2 4 8 16 32 64 128 256 512 1000 1500 2000
do 
   echo "For ${threads} thread(s)"
   export OMP_NUM_THREADS=${threads}
   for i in 1 2 3 4 5
   do
     ./matrix_new
   done   
done
for threads in 1 2 4 8 16 32 64 128 256 512 1000 1500 2000    
do 
   echo "For ${threads} thread(s)"
   export OMP_NUM_THREADS=${threads}
   for i in 1 2 3 4 5
   do
     ./matrix_new_2for
   done   
done
for threads in 1 2 4 8 16 32 64 128 256 512 1000 1500 2000
do 
   echo "For ${threads} thread(s)"
   export OMP_NUM_THREADS=${threads}
   for i in 1 2 3 4 5
   do
     ./matrix_new_3for
   done   
done