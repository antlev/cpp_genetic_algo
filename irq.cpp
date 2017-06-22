#include "genetic.h"
#include "irq.h"


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

	using Random = MyRandom; 
	using Selection = SimpleSelectionOfBests<Trait>;
	using SelectionOfBests = SelectionOfFirstBests<Trait>;
	using Crossover = SinglePointCrossover<Trait>;
	using Mutation = SimpleMutation<Trait>;
	using Sort = Minimise<Trait>;
	// using Evaluate = EvalHelloWorld<Trait>;
	using Evaluate = EvalIrq<Trait>;
	using BuildHeader = BuildMyHeader<Trait>;
	using Gene = IrqGene;
	
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