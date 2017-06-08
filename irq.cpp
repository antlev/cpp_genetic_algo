#include "genetic.h"
#include "irq.h"


struct Trait {
	// HELLO WORLD
	// static constexpr const char* TARGET = "Hello, world!";
	// static constexpr int NB_GENES = stringSize(TARGET);

	static constexpr int NB_GENES = IRQ_NUMBER;

	// Configuration
	static constexpr int POP_SIZE = 20;
	static constexpr long MAX_ITERATIONS = 10000;
	static constexpr double CROSSOVER_RATE = 0.90; // % of children produce from a crossover
	static constexpr double BEST_SELECTION_RATE = 0.25; // % of population consider as best
	static constexpr double MUTATION_RATE = 0.70;

	using Random = MyRandom;
	using Selection = SimpleSelectionOfBests<Trait>;
	using SelectionOfBests = SelectionOfFirstBests<Trait>;
	using Crossover = SinglePointCrossover<Trait>;
	using Mutation = SimpleMutation<Trait>;
	using Sort = Minimise<Trait>;
	// using Evaluate = EvalHelloWorld<Trait>;
	using Evaluate = EvalIrq<Trait>;
	using Gene = irqGene;
	
	static Random random;
};
Trait::Random Trait::random;


int main(int argc, char* argv[]){
    (void)argc;
    (void)argv;

    readIRQConf();

	GeneticAlgo<Trait> gen;

	gen.start();
}