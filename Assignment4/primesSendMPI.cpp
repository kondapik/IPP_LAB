#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <bits/stdc++.h>
#include <mpi.h>

long finalPrimes;
long localPrimes;

int helpText(char *program)
{
    std::cout << "Usage: " << program << " [optional command] N" << std::endl;
    std::cout << std::endl;
    std::cout << program + 2 << ":\nFind primes using a segmented sieve of Erathemes parallely using MPI." <<std::endl;
    //std::cout << "\n{Implemented for testing different work distribution} -- Optional argument 'P' can be used to break each section of numbers into smaller partitions. Threads can pickup blocks to calculation.\n" <<std::endl;
    std::cout << "Required Inputs: " << std::endl;
    //std::cout << "  T\tNumber of Threads. (a natural number)" << std::endl;
    std::cout << "  N\tMaximum number. (a natural number.)" << std::endl;
    std::cout << std::endl;
    std::cout << "Optional Command: " << std::endl;
    std::cout << "  -h\tDisplay this information." << std::endl;
    std::cout << std::endl;
    std::cout << "Number of process can be varied using '-n <number>' argument for mpiexec command" << std::endl;
    std::cout << std::endl;

   //exit(1);
}

long workerThread(long threadNo, long maxVal, long sqrtMaxVal, std::vector<long> seeds, long size, long mod)
{
    long startVal, vectSize;

    if (mod == 0)
    {
        startVal = sqrtMaxVal + 1 + (threadNo * size) ;
        vectSize = size;
    }else
    {
        if (threadNo < mod)
        {
            startVal = sqrtMaxVal + 1 + (threadNo * (size + 1));
            vectSize = size + 1;
        }else
        {
            startVal = sqrtMaxVal + 1 + (threadNo * size) + mod;
            vectSize = size;
        }
    } 

    if (startVal + vectSize > maxVal)
    {
        vectSize = maxVal - startVal;
    }

    std::vector<long> myPrimes;
   
    for(int currNo = startVal; currNo < vectSize + startVal; currNo++)
    {
        //If CurrNo is not divisible by all seed primes
        bool isDivisible = false;
        for (auto it = seeds.begin(); it != seeds.end(); ++it){
            if (currNo % *it == 0)
            {
                isDivisible = true;
                break;
            }
        }
        if (!isDivisible)
        {
            myPrimes.push_back(currNo);
        }
    }

    return (long) myPrimes.size();
}

int main (int argc, char *argv[])
{
    long maxNumber, maxSqrtValue, remValues, blockSize, lastBlocks;
    double elapsedTime;
    int rank, worldSize, seedSize;

    // Initializing MPI
    MPI_Init(&argc, &argv);

    // Starting timer
    MPI_Barrier(MPI_COMM_WORLD);
    elapsedTime =- MPI_Wtime();
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    
    // Print help text if '-h' argument is passed 
    if (argc == 1 || "-h" == std::string(argv[1]) || argc != 2)
    {
        if (rank == 0)
        {
            helpText(argv[0]);
            MPI_Finalize();
            exit(1);
        }
    }

    try
    {
        maxNumber = std::stoi(argv[1]);
    } catch (...)
    {
        helpText(argv[0]);
        MPI_Finalize();
        exit(1);
    }

    maxSqrtValue = (long) std::sqrt(maxNumber);
    remValues = (maxNumber - maxSqrtValue);
    // blockSize = remValues / worldSize;
    // lastBlocks = remValues % maxThreads;

    std::vector<long> seeds(maxSqrtValue - 1);


    if (rank == 0){
        // *** Finding Seeds ***
        //Filling vector with natural number sequence starting from 2
        std::iota (seeds.begin(), seeds.end(), 2);

        std::vector<long>::iterator findVal;
        long currNumber = 2;
        long currIdx = 0;
        while (std::pow(currNumber,2) <= maxSqrtValue)
        {
            // finding index of currNumner^2
            findVal = std::find (seeds.begin(), seeds.end(), std::pow(currNumber,2));
            for (auto firstIdx = findVal; firstIdx != seeds.end(); ++firstIdx)
            {
                if (*firstIdx % currNumber == 0)
                {
                    seeds.erase(firstIdx);
                    firstIdx--;
                }
            }
            currIdx = currIdx + 1;
            currNumber = seeds.at(currIdx);
        }

        finalPrimes = (long) seeds.size();

        if (worldSize == 1){
            // executing remaining values in a single thread
            finalPrimes =+ workerThread(0, maxNumber, maxSqrtValue, seeds, remValues, 0);
        }else
        {
            for (int i = 1; i < worldSize; i++)
            {
                // sending the size of vector with tag 0 and the vector with tag 1
                seedSize = seeds.size();
                MPI_Send(&seedSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&seeds[0], seeds.size(), MPI_LONG_INT, i, 1, MPI_COMM_WORLD);
            }

            for (int i = 1; i < worldSize; i++) {
                MPI_Recv(&localPrimes, 1,  MPI_LONG_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                finalPrimes =+ localPrimes;
            }
        }

        elapsedTime =+ MPI_Wtime();
        
        printf("Found %ld primes with %d thread(s) and %ld maximum value in %.15f second(s) (wall clock). \n",finalPrimes, worldSize, maxNumber, elapsedTime);
    } else
    {
        
        MPI_Recv(&seedSize, 1,  MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        seeds.resize (seedSize);
        MPI_Recv(&seeds[0], seedSize, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        blockSize = remValues / (worldSize - 1);
        lastBlocks = remValues % (worldSize - 1);

        localPrimes = workerThread(rank - 1, maxNumber, maxSqrtValue, seeds, blockSize, lastBlocks);

        MPI_Send(&localPrimes, 1, MPI_LONG_INT, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}