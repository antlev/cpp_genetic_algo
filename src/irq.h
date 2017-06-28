#ifndef IRQ_H
#define IRQ_H

#include <iostream>
#include "genetic.h"
#include <fstream>
#include <stdio.h>
#include <unistd.h>

static const constexpr char* irqConfPathFile = "./irq.conf";
static std::vector<int> irqToConsider;


// Reads a configuration file listing irq to consider
// and concats the masks of all the irq(s) (as a bitform) in an array
// The array will represent our first solution
// Set irqToConsiderLength
// Return irqToConsider that contains the irq's mask to be set by the algo
int readIRQConf(){
    FILE * confFilePointer;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    // save the actual irq configuration
    system("../utils/saveIrq.sh"); 

    confFilePointer = fopen(irqConfPathFile, "r");
    if (confFilePointer == NULL)
        return -1;

    // Count the number of lines in file
    while ((read = getline(&line, &len, confFilePointer)) != -1) { 
        irqToConsider.push_back(std::stoi(line));
    }
    fclose(confFilePointer);

    return 0;
}

template<typename _Trait> 
int setIrq(Chromosome<_Trait>& ch){
    std::fstream fs;
    std::string irqFilePath;

    for(size_t i=0; i<irqToConsider.size(); i++){
        std::stringstream ss;
        ss << _Trait::IRQ_PREFIX << irqToConsider[i] << _Trait::IRQ_SUFFIX;
        irqFilePath = ss.str();
        fs.open(irqFilePath, std::fstream::in);
        fs << ch.genes[i].toString();
        fs.close();
    }
    return 0;
}

long pingpong(){
    std::cout << "DEBUG!!!!!!!" << std::endl;
    int pipefd[2];
    pipe(pipefd);
    int ret = fork();

    if (ret == 0)
    {
        close(pipefd[0]);    // close reading end in the child

        dup2(pipefd[1], 1);  // send stdout to the pipe
        dup2(pipefd[1], 2);  // send stderr to the pipe

        close(pipefd[1]);    // this descriptor is no longer needed

        execlp("./pingpong.sh", "pingpong.sh", 0); // return int in a nanosecond
        exit(0);
    }
    else
    {
        // parent

        char buffer[1024];

        close(pipefd[1]);  // close the write end of the pipe in the parent

        while (read(pipefd[0], buffer, sizeof(buffer)) != 0){

        }
        std::cout << "PING PONG result >" << atol(buffer) << std::endl;
        return atol(buffer);
    }
}

#endif // GENETIC_H