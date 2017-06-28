#include "genetic.h"
#include "irq.h"

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

struct Trait {

	static constexpr int NB_GENES = IRQ_NUMBER;

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
	// using SelectionOfBests = SelectionOfFirstBests<Trait>;
	using Crossover = SinglePointCrossover<Trait>;
	using Mutation = SimpleMutation<Trait>;
	using Sort = Minimise<Trait>;
	// using Evaluate = EvalHelloWorld<Trait>;
	using Evaluate = EvalIrq<Trait>;
	using BuildHeader = BuildMyHeader<Trait>;
	using RecoverHeader = RecoverMyHeader<Trait>;
	using Gene = IrqGene<Trait>;
	
	static Random random;

	static constexpr const char* SAVE_FILE_PATH = "state.save";

};
Trait::Random Trait::random;





int main(int argc, char* argv[]){
    (void)argc;
    (void)argv;

    if(argc > 1){
    	readIRQConf();
		GeneticAlgo<Trait> gen;
        gen.recoverState();
		gen.start();
    }else{
	    readIRQConf();
		GeneticAlgo<Trait> gen;
		gen.start();
    }

}

std::ostream& operator<<(std::ostream& os, const Chromosome<Trait>& c){
	os << c.toString();
	return os;
}	