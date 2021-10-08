#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <bits/stdc++.h>
#include <omp.h>

std::vector<long> finalPrimes;
//long token;
#pragma omp declare reduction (merge : std::vector<long> : omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))

int helpText(char *program)
{
    std::cout << "Usage: " << program << " [optional command] T N " << std::endl;
    std::cout << std::endl;
    std::cout << program + 2 << ":\nFind primes using a segmented sieve of Erathemes parallely using OpenMP." <<std::endl;
    std::cout << "\n{Implemented for testing different work distribution} -- Optional argument 'P' can be used to break each section of numbers into smaller partitions. Threads can pickup blocks to calculation.\n" <<std::endl;
    std::cout << "Required Inputs: " << std::endl;
    std::cout << "  T\tNumber of Threads. (a natural number)" << std::endl;
    std::cout << "  N\tMaximum number. (a natural number.)" << std::endl;
    std::cout << std::endl;
    std::cout << "Optional Command: " << std::endl;
    std::cout << "  -h\tDisplay this information." << std::endl;
    std::cout << std::endl;

    exit(1);
}

int main(int argc, char *argv[])
{
    long maxThreads, maxNumber;

    // Print help text if '-h' argument is passed 
    if (argc == 1 || "-h" == std::string(argv[1]) || argc != 3)
    {
        helpText(argv[0]);
    }

    // Reading and checking arguments
    try
    {
      maxThreads = std::stoi(argv[1]);
    } catch (...)
    {
      helpText(argv[0]);
    }

    if (maxThreads < 1)
    {
      helpText(argv[0]);
    }

    try
    {
      maxNumber = std::stoi(argv[2]);
    } catch (...)
    {
      helpText(argv[0]);
    }

    // *** Finding Seeds ***
    long maxSqrtValue = (long) std::sqrt(maxNumber);
    std::vector<long> seeds(maxSqrtValue - 1);

    //Filling vector with natural number sequence starting from 2
    std::iota (std::begin(seeds), std::end(seeds), 2);

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

    // Adding seedes to finalPrimes
    finalPrimes.resize(seeds.size());
    std::copy(seeds.begin(), seeds.end(), finalPrimes.begin());


    double startTime = omp_get_wtime();

    #pragma omp parallel for num_threads(maxThreads) firstprivate(seeds) reduction(merge: finalPrimes)
    for(int currNo = maxSqrtValue + 1; currNo < maxNumber; currNo++)
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
            finalPrimes.push_back(currNo);
        }
    }
    double duration = omp_get_wtime() - startTime;
    std::cout << "Calculation Time: " << duration << std::endl;

    // Printing the Vector
    // sort(finalPrimes.begin(), finalPrimes.end());
    // for (auto it = finalPrimes.begin(); it != finalPrimes.end(); ++it)
    //     std::cout << ' ' << *it;
    // std::cout << std::endl;
    
    printf("Found %ld primes with %ld thread(s) and %ld maximum value in %.15f second(s) (wall clock). \n",finalPrimes.size(), maxThreads, maxNumber, duration);
    return 0;
}