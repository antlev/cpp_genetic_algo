#include <iostream>
#include <array>
#include <vector>
#include <cassert>
#include <algorithm>
#include "genetic.h"

struct MyGene {
	MyRandom random;
	char letter;

	MyGene() : random(), letter(getRandomLetter()) {
	}

	MyGene(const MyGene& myGene){
		random = myGene.random;
		letter = myGene.letter;
	}

	MyGene& operator=(const MyGene& myGene){
		random = myGene.random;
		letter = myGene.letter;
		return *this; 
	} 

	void init(){		
		letter = getRandomLetter();
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

struct Trait {
	static constexpr const char* TARGET = "Hello, world!";
	static constexpr int NB_GENES = 13;	
	static constexpr int MAX_ERROR = NB_GENES;
	// Configuration
	static constexpr int POP_SIZE = 20;
	static constexpr long MAX_ITERATIONS = 1;
	static constexpr double CROSSOVER_RATE = 0.80; // % of children produce from a crossover
	static constexpr double BEST_SELECTION_RATE = 0.20; // % of population consider as best
	static constexpr double MUTATION_RATE = 0.30;

	using Random = MyRandom;
	using Selection = SimpleSelection<Trait>;
	using Crossover = SinglePointCrossover<Trait>;
	using Mutation = SimpleMutation<Trait>;
	using Sort = Minimise<Trait>;
	using Evaluate = Eval<Trait>;
	static Random random;
	using Gene = MyGene;

	
};
Trait::Random Trait::random ;



int main(int argc, char* argv[]){
    (void)argc;
    (void)argv;

	GeneticAlgo<Trait> gen;

	gen.start();
}
 
std::ostream& operator<<(std::ostream& os, const Chromosome<Trait>& c){
	os << c.toString();
	return os;
}	