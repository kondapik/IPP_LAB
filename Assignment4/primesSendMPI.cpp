#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <bits/stdc++.h>
#include <mpi.h>

std::vector<long> finalPrimes;
std::vector<long> localPrimes;

int helpText(char *program)
{
    std::cout << "Usage: " << program << " [optional command] N " << std::endl;
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

int main (int argc, char *argv[])
{
    long maxNumber;

    // Print help text if '-h' argument is passed 
    if (argc == 1 || "-h" == std::string(argv[1]) || argc != 3)
    {
        helpText(argv[0]);
    }
}