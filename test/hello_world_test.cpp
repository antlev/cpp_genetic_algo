#include "gtest/gtest.h"
#include "../src/genetic.h"
struct Trait;

struct MyGene {
	MyRandom& random;
	char letter;

	MyGene(MyRandom& random) : random(random), letter(getRandomLetter()) {
	}

	MyGene(const MyGene& myGene): random(myGene.random), letter(myGene.letter) {
	}

	MyGene& operator=(const MyGene& myGene){
		random = myGene.random;
		letter = myGene.letter;
		return *this; 
	} 
	char toString() const noexcept {		
		return letter;
	}
	/////////////////////////// RAND WORD FUNCTION
	char getRandomLetter(){
		return random.getIntAsciiChar(); 
	}
	void mutation(){
		letter = getRandomLetter();
	}
};

constexpr size_t stringSize(const char* str){
    return (*str == 0) ? 0 : stringSize(str + 1) + 1;
}

template<typename _Trait> 
struct EvalHelloWorld
{
	double evaluate(Chromosome<_Trait>& chromosome){
		double fitness = 0;
		for(size_t i=0; i<chromosome.genes.size(); i++){
			if(chromosome.genes[i].letter != _Trait::TARGET[i]){ fitness ++; }
		}
		return fitness;
	}
};

struct Trait {
	// HELLO WORLD
	static constexpr const char* TARGET = "Hello, World!";
	static constexpr int NB_GENES = stringSize(TARGET);
	static constexpr const char* SAVE_FILE_PATH = "../save/state.save";

	// Configuration
	static constexpr const long MAX_ERROR = NB_GENES;
	static constexpr long MAX_ITERATIONS = 100000;
	static constexpr int LIMIT_STAGNATION = 50000;
	
	static constexpr int POP_SIZE = 20;
	static constexpr double CROSSOVER_RATE = 0.90; // % of children produce from a crossover
	static constexpr double BEST_SELECTION_RATE = 0.25; // % of population consider as best
	static constexpr double MUTATION_RATE = 0.70;

	using Selection = SimpleSelection<Trait>;
	using Crossover = SinglePointCrossover<Trait>;
	using Mutation = SimpleMutation<Trait>;
	using Sort = Minimise<Trait>;
	using Evaluate = EvalHelloWorld<Trait>;
	using Gene = MyGene;

	using BuildHeader = BuildMyHeader<Trait>;
	using RecoverHeader = RecoverMyHeader<Trait>;

	using Random = MyRandom;
	static Random random;

	static bool isFinished(long bestFitness, size_t iterations, int stagnation){
		if(bestFitness == 0 || iterations == MAX_ITERATIONS || stagnation == LIMIT_STAGNATION){
			return true;
		}else{
			return false;
		}
	}
};
Trait::Random Trait::random;



TEST (hello_world_test, toto) { 
	GeneticAlgo<Trait> gen;
	gen.start();

	EXPECT_TRUE (gen.getBestFitness() == 0);
	EXPECT_TRUE (gen.getCurrentIterationCount() < 1500);
}

std::ostream& operator<<(std::ostream& os, const Chromosome<Trait>& c){
	os << c.toString();
	return os;
}	