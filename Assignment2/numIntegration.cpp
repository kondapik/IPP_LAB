#include <iostream>
#include <thread>
#include <string>

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

void workerThread(int threadNo, int maxTrapezes, int size, int mod)
{
    int startIdx, lastIdx;
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

    std::cout << "Thread No: " << threadNo << std::endl;
    for(int threadIdx = startIdx; threadIdx < lastIdx; threadIdx++)
    {
        std::cout << threadIdx << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    int maxThreads, maxTrapezes, maxParts = 1;
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

    // *** Starting Timer ***

    int blockSize = maxTrapezes / maxThreads;
    int lastBlocks = maxTrapezes % maxThreads;

    // std::cout << "blockSize:\t" << blockSize << std::endl;
    // std::cout << "lastBlocks:\t" << lastBlocks << std::endl;

    for (int threadNo = 0; threadNo < maxThreads; threadNo++)
    {
        workerThread(threadNo, maxTrapezes, blockSize, lastBlocks);
    }
    return 0;
}