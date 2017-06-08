#ifndef IRQ_H
#define IRQ_H

#include <iostream>
#include "genetic.h"
#include <fstream>
#include <stdio.h>


// #include <dirent.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
#include <unistd.h>
// #include <stdlib.h>
// #include <time.h>
// #include <unistd.h>
// #include <math.h>

// // struct Bit{
// //     bool val;
// // }

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
        std::cout << "reponse" << atol(buffer) << std::endl;
        return atol(buffer);
    }
}


// void printBinArray(char* binArray);
// int asciiToDec(char* asciiStr, char* decStr);
// void generateRandomSolution(char* solution, int solutionSize);
// char* initPop(int popSize, char* population, int solutionSize);
// void cross(char* parent1, char* parent2, int solutionSize, char* child);
// void crossover(char* population, int solutionSize);
// void muteSolution(char* solution,int solutionSize);
// void mutation(char* population, int solutionSize);
// void selection(char* population, double* scoredPopulation, int solutionSize, char* irqToConsider, int irqToConsiderLength);
// double bench(char* solution, int solutionSize, char* irqToConsider, int irqToConsiderLength);
// char* append(char* str, char c);
// void printSol(char* solution, int solutionSize, int sizeofMask);
// void printPop(char* population, int solutionSize, int sizeofMask);
// void order(double* scoredPopulation);

// static const char* __irqConf = "./irq.conf";
// static int __nbCore = 4;
//     int sizeofMask = (( ((__nbCore - 1) / 8) + 1)* 8);

// static const int __iterationsMax = 10000;
// static const char* __irqPrefix = "/proc/irq/";
// static const char* __irqSufix = "/smp_affinity";
// static const int __irqMaxSize = 4;
// static const int __maxIrqToDealWith = 1000;
// static const int __popSize = 10;
// static const size_t __maxSolutionSize = 1024*sizeof(char);
// static const double __mutationRate = 0.1f;
// static const double __selectionRate = 0.25f;

// // Print the binary array with the good form, function of the nb core
// void printBinArray(char* binArray){
//     int i;
//     if(__nbCore == -1){
//         __nbCore =  sysconf(_SC_NPROCESSORS_ONLN);
//     }
//     i=0;
//     while(binArray[i] != '\0'){
//         if(i==0){
//             printf(">");
//         }else if((i % (( ((__nbCore - 1) / 8) + 1)* 8)) == 0 ){
//             printf("\n>");
//         }
//         printf("%c", binArray[i]);
//         i++;
//     }
//     printf("\n");
// }

// int asciiToDec(char* asciiStr, char* decStr){
//     int i=0;
//     while(asciiStr[i] != '\0'){
//         if(asciiStr[i] <= 57 && asciiStr[i] >= 48){
//             decStr[i] = asciiStr[i] - 48;
//         }else if(asciiStr[i] <= 102 && asciiStr[i] >= 97){
//             decStr[i] = asciiStr[i] - 87;
//         } else {
//             return -1;
//         }
//         i++;
//     }
//     return 0;
// }

// void generateRandomSolution(char* solution, int solutionSize){
//     int i;
//     for(i=0;i<solutionSize;++i){
//         solution[i] =  (rand()%2 ? '1' : '0');
//     }
// }

// char* initPop(int popSize, char* population, int solutionSize){
//     population = malloc(sizeof(char)*__popSize*solutionSize);
//     int i;
//     for(i=0; i<popSize; ++i){
//         generateRandomSolution(population+i*solutionSize, solutionSize);
//     }
//     return population;
// }

// void cross(char* parent1, char* parent2, int solutionSize, char* child){
//     int rdmSeparation = (rand() % solutionSize);
//     int i;

//     char* p1 = parent1;
//     char* p2 = parent2;
//     char* p3 = child;
//     char* r1 = p1+rdmSeparation;
//     char* r2 = p2+rdmSeparation;
//     char* r3 = p3+rdmSeparation;


//     memcpy(p3,p1,rdmSeparation*sizeof(char));
//     memcpy(r3,r1,solutionSize*rdmSeparation*sizeof(char));

// }

// void crossover(char* population, int solutionSize){
//     int i;
//     char* parent1 = malloc(sizeof(char)*solutionSize);
//     char* parent2 = malloc(sizeof(char)*solutionSize);
//     int floor = __popSize*__selectionRate;
//     for(i=floor*solutionSize;i<__popSize*solutionSize;i+=solutionSize){

//         parent1 = population+(rand() % floor);
//         parent2 = population+(rand() % floor);

//         cross(parent1, parent2, solutionSize, population+i);
//     }
//     // if(parent1)
//     //     free(parent1);
//     // if(parent2)
//     //     free(parent2);
// }

// void muteSolution(char* solution,int solutionSize){
//     int rdm = (int)(rand() % solutionSize+1);
//     if(*(solution+rdm) == '1'){
//         *(solution+rdm) = '0';
//     } else {
//         *(solution+rdm) = '1';
//     }
// }

// void mutation(char* population, int solutionSize){
//     int i;
//     for(i=0; i<__popSize; i++){
//         if((double)rand()/RAND_MAX < __mutationRate){
//             muteSolution(population+(i*solutionSize), solutionSize);
//         }
//     }
// }

// void selection(char* population, double* scoredPopulation, int solutionSize, char* irqToConsider, int irqToConsiderLength){
//     int i;
//     for(i=0; i<__popSize; ++i){
//         scoredPopulation[i] = bench(population+i*solutionSize, solutionSize, irqToConsider, irqToConsiderLength);
//         if (scoredPopulation[i] == -2)
//             exit -2;
//     }
//     order(scoredPopulation);
// }
// void order(double* scoredPopulation){
//     char isArraySorted = 0;
//     int taille = __popSize;
//     while(!isArraySorted)
//     {
//         isArraySorted = 1;
//         for(int i=0 ; i < taille-1 ; i++)
//         {   
//             double tmp;
//             if(*(scoredPopulation+i) > *(scoredPopulation+i+1))
//             {
//                 tmp = *scoredPopulation+i;
//                 *(scoredPopulation+i) = *(scoredPopulation+i+1);
//                 *(scoredPopulation+i+1) = tmp;
//                 isArraySorted = 0;
//             }
//         }
//         taille--;
//     }
// }

// double bench(char* solution, int solutionSize, char* irqToConsider, int irqToConsiderLength){
//     setIrq(solution, solutionSize, irqToConsider, irqToConsiderLength);

//     int pingpongRes = 0;
//     // Call ping-pong
//     // pingpongRes = pingpong();

//     return pingpongRes;
// }

   
// int main(int argc, char* argv){   
//     int iterations = 0;
//     srand(time(NULL));
//     int sizeofMask = (( ((__nbCore - 1) / 8) + 1)* 8);

//     // recover informations from IRQ conf file
//     int irqToConsiderLength;
//     char* irqToConsider = readIRQConf(&irqToConsiderLength, sizeofMask);
//     int solutionSize = irqToConsiderLength * sizeofMask;
//     // Génère une population de solution aléatoire
//     char* population = initPop(__popSize, population, solutionSize);

//     double* scoredPopulation = malloc(sizeof(double)*__popSize);

//     selection(population, scoredPopulation, solutionSize, irqToConsider, irqToConsiderLength);

//     while(1){

//         crossover(population, solutionSize);

//         mutation(population, solutionSize);
        
//         selection(population, scoredPopulation, solutionSize, irqToConsider, irqToConsiderLength);

//         printPop(population, solutionSize, sizeofMask);
//         //sleep(1);

//         if(iterations > __iterationsMax){
//             break;
//         }
//         iterations++;
//         sleep(1);
//     }

//     if(population)
//         free(population);
//     if(irqToConsider)
//         free(irqToConsider);
//     if(scoredPopulation)
//         free(scoredPopulation);

//     return 0;
// }
// void printSol(char* solution, int solutionSize, int sizeofMask){
//     int i;
//     for(i=0;i<solutionSize;i+=sizeofMask){
//         int j;
//         for (j = 0; j < sizeofMask; ++j){
//             printf("%c", solution[i+j] );
//         }
//         printf("\n");
//     }
// }

// void printPop(char* population, int solutionSize, int sizeofMask){
//     int i;
//     for(i=0;i<__popSize;i++){
//         printf("Individu %d :\n", i);
//         printSol(population+i, solutionSize, sizeofMask);
//     }

// }

#endif // GENETIC_H