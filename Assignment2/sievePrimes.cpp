#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

std::vector<long> finalPrimes;
long token;
std::mutex mutex;

int helpText(char *program)
{
    std::cout << "Usage: " << program << " [optional command] T N [optional argument]" << std::endl;
    std::cout << std::endl;
    std::cout << program + 2 << ":\nFind primes using a segmented sieve of Erathemes parallely." <<std::endl;
    std::cout << "\n{Implemented for testing different work distribution} -- Optional argument 'P' can be used to break each section of numbers into smaller partitions. Threads can pickup blocks to calculation.\n" <<std::endl;
    std::cout << "Required Inputs: " << std::endl;
    std::cout << "  T\tNumber of Threads. (a natural number)" << std::endl;
    std::cout << "  N\tMaximum number. (a natural number.)" << std::endl;
    std::cout << std::endl;
    std::cout << "Optional Argument: " << std::endl;
    std::cout << "  P\tNumber of Partitions. (a natural number.)" << std::endl;
    std::cout << std::endl;
    std::cout << "Optional Command: " << std::endl;
    std::cout << "  -h\tDisplay this information." << std::endl;
    std::cout << std::endl;

    exit(1);
}

void *workerThread(long threadNo, long maxVal, long sqrtMaxVal, std::vector<long> seeds, long size, long mod)
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

    std::vector<long> myPrimes(vectSize);
   
    //Filling vector with natural number sequence starting from startVal
    std::iota (std::begin(myPrimes), std::end(myPrimes), startVal);

    for (auto it = seeds.begin(); it != seeds.end(); ++it)
    {
        for (auto firstIdx =  myPrimes.begin(); firstIdx != myPrimes.end(); ++firstIdx)
        {
            if (*firstIdx % *it == 0)
            {
                myPrimes.erase(firstIdx);
                firstIdx--;
            }
        }
    }

    mutex.lock();
    finalPrimes.resize(finalPrimes.size() + myPrimes.size());
    std::copy(myPrimes.begin(), myPrimes.end(), finalPrimes.end() - myPrimes.size());
    mutex.unlock();
    // std::cout << std::endl;

    return NULL;
}

void *workerThreadParts(long threadNo, long maxVal, long sqrtMaxVal, std::vector<long> seeds, long size, long maxTokens)
{
    long startVal, vectSize, myToken;
    std::vector<long> myPrimes(size);
    vectSize = size;

    while (myToken < maxTokens)
    {
        mutex.lock();
        myToken = token;
        token++;
        mutex.unlock();

        if (myToken < maxTokens)
        {
            startVal = sqrtMaxVal + 1 + (myToken * size);
            
            if (startVal + vectSize > maxVal)
            {
                vectSize = maxVal - startVal;
            }

            myPrimes.clear();
            myPrimes.resize(vectSize);
            //Filling vector with natural number sequence starting from startVal
            std::iota (std::begin(myPrimes), std::end(myPrimes), startVal);

            for (auto it = seeds.begin(); it != seeds.end(); ++it)
            {
                for (auto firstIdx =  myPrimes.begin(); firstIdx != myPrimes.end(); ++firstIdx)
                {
                    if (*firstIdx % *it == 0)
                    {
                        myPrimes.erase(firstIdx);
                        firstIdx--;
                    }
                }
            }
        }
        mutex.lock();
        finalPrimes.resize(finalPrimes.size() + myPrimes.size());
        std::copy(myPrimes.begin(), myPrimes.end(), finalPrimes.end() - myPrimes.size());
        mutex.unlock();
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    long maxThreads, maxNumber, maxParts = 1;
    // Print help text if '-h' argument is passed 
    if (argc == 1 || "-h" == std::string(argv[1]) || argc < 3 || argc > 4)
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
    
    if (argc == 4)
    {
        try
        {
            maxParts = std::stoi(argv[3]);
        } catch (...)
        {
            helpText(argv[0]);
        }

        // if (maxNumber < maxThreads * maxParts)
        // {
        //     helpText(argv[0]);
        // }
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

    // *** Creating Threads ***
    long remValues = (maxNumber - maxSqrtValue);
    long blockSize = remValues / maxThreads;
    long lastBlocks = remValues % maxThreads;

    token = 0;
    std::thread *threadHandles = new std::thread[maxThreads];

    // *** Starting Timer ***
    auto start_time = std::chrono::system_clock::now();

    if (maxParts == 1)
    {
        for (long threadNo = 0; threadNo < maxThreads; threadNo++)
        {
            threadHandles[threadNo] = std::thread(workerThread,threadNo, maxNumber, maxSqrtValue, seeds, blockSize, lastBlocks);
        }
    } else
    {
        for (long threadNo = 0; threadNo < maxThreads; threadNo++)
        {
            threadHandles[threadNo] = std::thread(workerThreadParts,threadNo, maxNumber, maxSqrtValue, seeds, blockSize/maxParts, maxThreads * maxParts);
        }
    }
    for (long threadNo = 0; threadNo < maxThreads; threadNo++)
    {
        threadHandles[threadNo].join();
    }

    std::chrono::duration<double> duration = (std::chrono::system_clock::now() - start_time);

    // Printing the Vector
    // for (auto it = finalPrimes.begin(); it != finalPrimes.end(); ++it)
    //     std::cout << ' ' << *it;
    // std::cout << std::endl;

    //std::cout << "Finished in " << duration.count() << " seconds (wall clock)." << std::endl;
    //std::cout << "Result with " << maxThreads << " threads and " << maxTrapezes << " trapezes is " << finalResult << ". Calculation time is " << duration.count() << " seconds (wall clock). \n";
    printf("Found %ld primes with %ld thread(s), %ld maximum value and %ld partition(s) in %.15f second(s) (wall clock). \n",finalPrimes.size(), maxThreads, maxNumber, maxParts, duration.count());
    return 0;
}