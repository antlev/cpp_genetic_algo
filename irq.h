#ifndef IRQ_H
#define IRQ_H

#include <iostream>
#include "genetic.h"
#include <fstream>
#include <stdio.h>
#include <unistd.h>

static const constexpr char* irqConfPathFile = "./irq.conf";
static const int __nbCore = 4;
static const int __sizeOfMask = (( ((__nbCore - 1) / 8) + 1)* 8);
static const std::string __irqPrefix = "/proc/irq/";
static const std::string __irqSufix = "/smp_affinity";
static std::vector<int> irqToConsider;

struct IrqGene {
    static MyRandom random;
    std::vector<bool> mask ;

    IrqGene() {
        int index = random.getIntRange(0, __sizeOfMask);
        for (int i = 0; i < index; ++i){
            mask.push_back(0);
        }       
        for (int i = index; i < __sizeOfMask; ++i){
            mask.push_back(1);
        }
    }
    IrqGene(const IrqGene& irqGene){
        random = irqGene.random;
        mask = irqGene.mask;
    }
    IrqGene& operator=(const IrqGene& irqGene){
        random = irqGene.random;
        mask = irqGene.mask;
        return *this; 
    } 
    std::string toString() const noexcept {        
        std::stringstream ss;
        for(size_t i=0; i<mask.size(); i++){
            if(mask[i]){
                ss << '1';
            }else{
                ss << '0';
            }
        }
        return ss.str();
    }
    void mutation(){
        int index = random.getIntRange(0, __sizeOfMask);
        mask[index] = !mask[index];
    }

};
MyRandom IrqGene::random;

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
    system("saveIrq.sh"); 

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
        ss << __irqPrefix << irqToConsider[i] << __irqSufix;
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