#include <iostream>
#include <array>
#include <vector>
#include <cassert>
#include <algorithm>
#include "genetic.h"

struct MyGene {
	static MyRandom random;
	char letter;

	MyGene() : letter(getRandomLetter()) {
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
MyRandom MyGene::random;

constexpr size_t stringSize(const char* str){
    return (*str == 0) ? 0 : stringSize(str + 1) + 1;
}
struct Trait {
	static constexpr const char* TARGET = "Hello, world!";
	static constexpr int NB_GENES = stringSize(TARGET);	
	static constexpr int MAX_ERROR = NB_GENES;
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
	using Evaluate = Eval<Trait>;
	using Gene = MyGene;
	
	static Random random;
};
Trait::Random Trait::random;

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