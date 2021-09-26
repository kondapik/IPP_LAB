#include <iostream>
#include <thread>
#include <string>
#include <chrono>
#include <mutex>
//#include "timer.h"


double finalResult;
long token;
std::mutex mutex;

int helpText(char *program)
{
    std::cout << "Usage: " << program << " [optional command] T N [optional argument]" << std::endl;
    std::cout << std::endl;
    std::cout << program + 2 << ":\nCalculates the numerical integration of [4/(1 + x^2)] from 0 to 1 by summing up areas of trapezes." <<std::endl;
    std::cout << "[0 1] is divided into 'N' trapezes and their areas are calculated and summed by 'T' threads parallely." <<std::endl;
    std::cout << "\n{Implemented for testing different work distribution} -- Optional argument 'P' can be used to break each section of trapezes into smaller partitions. Threads can pickup blocks to calculation.\n" <<std::endl;
    std::cout << "Required Inputs: " << std::endl;
    std::cout << "  T\tNumber of Threads. (a natural number)" << std::endl;
    std::cout << "  N\tNumber of Trapezes. (a natural number. N should be greater than or equal to T)" << std::endl;
    std::cout << std::endl;
    std::cout << "Optional Argument: " << std::endl;
    std::cout << "  P\tNumber of Partitions. (a natural number. N should be greater than or equal to T*P)" << std::endl;
    std::cout << std::endl;
    std::cout << "Optional Command: " << std::endl;
    std::cout << "  -h\tDisplay this information." << std::endl;
    std::cout << std::endl;

    exit(1);
}

double getFunctionOut(double x)
{
    // double y;
    // y = (double) 1.0 + x^2;
    return ((4.0 / (1.0 + x*x)));
}

double getArea(long blockNo, double width)
{
    double x1, x2, y1, y2;
    x1 = blockNo * width;
    x2 = x1 + width;
    y1 = getFunctionOut(x1);
    y2 = getFunctionOut(x2);
    
    return ((std::min(y1,y2)*width) + (0.5*width*std::abs(y1-y2)));
}


void *workerThread(long threadNo, long maxTrapezes, long size, long mod)
{
    long startIdx, lastIdx;
    double mySum = 0.0;

    if (mod == 0)
    {
        startIdx = threadNo * size;
        lastIdx = startIdx + size;
    }else
    {
        if (threadNo < mod)
        {
            startIdx = threadNo * (size + 1);
            lastIdx = startIdx + (size + 1);
        }else
        {
            startIdx = (threadNo * size) + mod;
            lastIdx = (startIdx + size);
        }
    } 

    if (lastIdx > maxTrapezes)
    {
        lastIdx = maxTrapezes;
    }

    // std::cout << "Thread No: " << threadNo << std::endl;
    double width = 1.0 / (double) maxTrapezes;

    for(long threadIdx = startIdx; threadIdx < lastIdx; threadIdx++)
    {
        // std::cout << threadIdx << " ";
        mySum = mySum + getArea(threadIdx, width);
    }

    mutex.lock();
    finalResult = finalResult + mySum;
    mutex.unlock();
    // std::cout << std::endl;

    return NULL;
}

void *workerThreadParts(long threadNo, long maxTrapezes, long maxTokens, long size)
{
    long startIdx, lastIdx, myToken;
    double mySum = 0.0;

    // std::cout << "Thread No: " << threadNo << std::endl;
    double width = 1.0 / (double) maxTrapezes;

    while (myToken < maxTokens)
    {
        mutex.lock();
        myToken = token;
        token++;
        mutex.unlock();
        if (myToken < maxTokens)
        {
            startIdx = myToken * size;
            lastIdx = startIdx + size;

            for(long threadIdx = startIdx; threadIdx < lastIdx; threadIdx++)
            {
                // std::cout << threadIdx << " ";
                mySum = mySum + getArea(threadIdx, width);
            }
        }
    }
    
    mutex.lock();
    finalResult = finalResult + mySum;
    mutex.unlock();
    // std::cout << std::endl;

    return NULL;
}

int main(int argc, char *argv[])
{
    long maxThreads, maxTrapezes, maxParts = 1;
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
      maxTrapezes = std::stoi(argv[2]);
    } catch (...)
    {
      helpText(argv[0]);
    }

    if (maxTrapezes < maxThreads)
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

        if (maxTrapezes < maxThreads * maxParts)
        {
            helpText(argv[0]);
        }
    }

    // std::cout << "There are " << argc << " arguments:\n";
    // std::cout << "maxThreads:\t" << maxThreads << std::endl;
    // std::cout << "maxTrapezes:\t" << maxTrapezes << std::endl;
    // std::cout << "maxParts:\t" << maxParts << std::endl;

    long blockSize = maxTrapezes / maxThreads;
    long lastBlocks = maxTrapezes % maxThreads;

    // std::cout << "blockSize:\t" << blockSize << std::endl;
    // std::cout << "lastBlocks:\t" << lastBlocks << std::endl;

    finalResult = 0.0;
    token = 0;
    std::thread *threadHandles = new std::thread[maxThreads];

    // *** Starting Timer ***
     auto start_time = std::chrono::system_clock::now();

    if (maxParts == 1)
    {
        for (long threadNo = 0; threadNo < maxThreads; threadNo++)
        {
            threadHandles[threadNo] = std::thread(workerThread,threadNo, maxTrapezes, blockSize, lastBlocks);
        }
    } else
    {
        for (long threadNo = 0; threadNo < maxThreads; threadNo++)
        {
            threadHandles[threadNo] = std::thread(workerThreadParts,threadNo, maxTrapezes, maxThreads * maxParts, blockSize/maxParts);
        }
    }
    
    for (long threadNo = 0; threadNo < maxThreads; threadNo++)
    {
        threadHandles[threadNo].join();
    }

    std::chrono::duration<double> duration = (std::chrono::system_clock::now() - start_time);

    //std::cout << "Finished in " << duration.count() << " seconds (wall clock)." << std::endl;
    //std::cout << "Result with " << maxThreads << " threads and " << maxTrapezes << " trapezes is " << finalResult << ". Calculation time is " << duration.count() << " seconds (wall clock). \n";
    printf("Result with %ld thread(s), %ld trapeze(s) and %ld partition(s) is %.15f. Calculation time is %.15f second(s) (wall clock). \n",maxThreads, maxTrapezes, maxParts, finalResult, duration.count());
    return 0;
}