#include "gtest/gtest.h"
#include <iostream>
#include "../src/genetic.h"
#include <fstream>
#include <stdio.h>
#include <unistd.h>
struct Trait ;

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
    if( system("../utils/saveIrq.sh") != 0 ){
        exit(-1);
    } 
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
    {   // child
        close(pipefd[0]);    // close reading end in the child
        dup2(pipefd[1], 1);  // send stdout to the pipe
        dup2(pipefd[1], 2);  // send stderr to the pipe
        close(pipefd[1]);    // this descriptor is no longer needed
        execlp("../utils/pingpong.sh", "pingpong.sh", 0); // return int in a nanosecond
        exit(0);
    }
    else
    {   // parent
        char buffer[1024];
        close(pipefd[1]);  // close the write end of the pipe in the parent
        while (read(pipefd[0], buffer, sizeof(buffer)) != 0){
        }
        std::cout << "PING PONG result >" << atol(buffer) << std::endl;
        return atol(buffer);
    }
}

template<typename _Trait> 
struct IrqGene {
    MyRandom& random;
    std::vector<bool> mask ;

    IrqGene(MyRandom& random) : random(random) {
        int index = random.getIntRange(0, _Trait::SIZE_OF_MASK);
        for (int i = 0; i < index; ++i){
            mask.push_back(0);
        }       
        for (int i = index; i < _Trait::SIZE_OF_MASK; ++i){
            mask.push_back(1);
        }
    }
    IrqGene(const IrqGene& irqGene) : random(irqGene.random), mask(irqGene.mask) {
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
        int index = random.getIntRange(0, _Trait::SIZE_OF_MASK);
        mask[index] = !mask[index];
    }

};
template<typename _Trait> 
struct EvalIrq
{
 double evaluate(Chromosome<_Trait>& chromosome){
     double fitness = 0;

     setIrq(chromosome);
     fitness = pingpong();
        
     return fitness;
 }
};

struct Trait {

    static constexpr int NB_GENES = 10;

    // Configuration
    static constexpr const long MAX_ERROR = 100000;
    static constexpr int POP_SIZE = 6;
    static constexpr long MAX_ITERATIONS = 10000;
    static constexpr double CROSSOVER_RATE = 0.90; // % of children produce from a crossover
    static constexpr double BEST_SELECTION_RATE = 0.25; // % of population consider as best
    static constexpr double MUTATION_RATE = 0.70;
    static constexpr int LIMIT_STAGNATION = 50000;

    static constexpr const int NB_CORE = 4;
    static constexpr const int SIZE_OF_MASK = (( ((NB_CORE - 1) / 8) + 1)* 8);
    static constexpr const char* IRQ_PREFIX = "/proc/irq/";
    static constexpr const char* IRQ_SUFFIX = "/smp_affinity";

    using Random = MyRandom; 
    using Selection = SimpleSelection<Trait>;
    using Crossover = SinglePointCrossover<Trait>;
    using Mutation = SimpleMutation<Trait>;
    using Sort = Minimise<Trait>;
    using Evaluate = EvalIrq<Trait>;
    using BuildHeader = BuildMyHeader<Trait>;
    using RecoverHeader = RecoverMyHeader<Trait>;
    using Gene = IrqGene<Trait>;

    static Random random;

    static constexpr const char* SAVE_FILE_PATH = "state.save";

    static bool isFinished(long bestFitness, size_t iterations, int stagnation){
        return false;
    }
};
Trait::Random Trait::random;

TEST (irq, global) { 
    readIRQConf();
    GeneticAlgo<Trait> gen;
    // gen.recoverState();
    gen.start();
}

std::ostream& operator<<(std::ostream& os, const Chromosome<Trait>& c){
    os << c.toString();
    return os;
}   