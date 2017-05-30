#include <iostream>
#include <array>
#include <vector>
#include <cassert>
#include <algorithm>
unsigned int seed = 0;
struct MyRandom {

	MyRandom() : r(), gen(r()), get_real_between_one_and_zero(0, 1), get_int_ascii(32,126) {
	}

    double getBetweenOneAndZero() {
    	return get_real_between_one_and_zero(gen);
    }
    char getRealAsciiChar() {
    	return get_int_ascii(gen);
    }
    int getIntRange(int downlimit, int uplimit){
    	return std::uniform_int_distribution<int>(downlimit, uplimit)(gen);
    }

	private:
 	// Seed with a real random value, if available
    std::random_device r;
    std::mt19937 gen; 
    std::uniform_real_distribution<> get_real_between_one_and_zero;
    std::uniform_int_distribution<> get_int_ascii;


};
struct Trait {
	static constexpr const char* TARGET = "Hello, world!";
	static constexpr int POP_SIZE = 16;
	static constexpr long MAX_ITERATIONS = 1000000;
	static constexpr double CROSSOVER_RATE = 0.75;
	static constexpr double SELECTION_RATE = 0.70;
	static constexpr double MUTATION_RATE = 0.01;
	static constexpr int NB_LETTERS = 13;
	static constexpr int MAX_ERROR = NB_LETTERS;
	using Random = MyRandom;
	static Random random;

	struct Chromosome {
    	std::string word;
		double fitness;

		Chromosome() : word(getRandomWord(NB_LETTERS)), fitness(MAX_ERROR) {
		}

		void evaluateFitness() {
			assert(word != "");
			double error(0);
			for(int i=0; i<NB_LETTERS; ++i){
				if(word[i] != TARGET[i]){ ++error; }
			}
			fitness = error;
		}
		void init(){
			word = getRandomWord(NB_LETTERS);
		}
		std::string toString(){
			
			return word + " fitness : " + std::to_string(fitness);
		}
	};	
	/////////////////////////// RAND WORD FUNCTION
	static char getRandomLetter(){
		return random.getRealAsciiChar(); 
	}
	static std::string getRandomWord(const int len) {
	    std::string s;
	    for (int i = 0; i < len; ++i) {
	        s = s + getRandomLetter();
	    }
	    s[len] = 0;
	    return s;
	}

	static bool sort(Chromosome& c1, Chromosome& c2){
		return c1.fitness < c2.fitness ;
	}
	/////////////////////////// SELECTION
	// Take a sorted population and select one randomly using the SELECTION_RATE variable
	static std::pair<Chromosome, Chromosome> simpleSelection(std::vector<Chromosome> sortedPopulation){
		int rdm = random.getIntRange(0, POP_SIZE-1);
		int rdm2 = random.getIntRange(0, POP_SIZE-1);
		assert(rdm >= 0 && rdm < POP_SIZE);
		assert(rdm2 >= 0 && rdm < POP_SIZE);
		// std::cout << "rdm >>>>>" << rdm << "<" << std::endl;
		std::pair<Chromosome, Chromosome> pairToReturn(sortedPopulation[rdm], sortedPopulation[rdm2]);
		return pairToReturn;
	}
	static std::pair<Chromosome, Chromosome> simpleSelectionOnHalfOfBests(std::vector<Chromosome> sortedPopulation){
		int rdm = random.getIntRange(0, POP_SIZE*SELECTION_RATE-1);
		int rdm2 = random.getIntRange(0, POP_SIZE*SELECTION_RATE-1);
		std::pair<Chromosome, Chromosome> pairToReturn(sortedPopulation[rdm], sortedPopulation[rdm2]);
		return pairToReturn;
	}
	/////////////////////////// CROSSOVER
	static std::pair<Chromosome, Chromosome> singlePointCrossover(std::pair<Chromosome, Chromosome> parents){		
		// std::cout << "DEBUG >> parents 1 " << parents.first.toString() << std::endl;
		// std::cout << "DEBUG >> parents 2 " << parents.second.toString() << std::endl;
		std::pair<Chromosome, Chromosome> children;
		int randomCursor = rand() % (NB_LETTERS -1) + 1;
		for(int i=0; i<randomCursor; i++){
			children.first.word[i] = parents.first.word[i];
			children.second.word[i] = parents.second.word[i];
		}
		for(int i=randomCursor; i<NB_LETTERS; i++){
			children.first.word[i] = parents.second.word[i];
			children.second.word[i] = parents.first.word[i];
		}
		// std::cout << "DEBUG >> children 1 " << children.first.toString() << std::endl;
		// std::cout << "DEBUG >> children 2 " << children.second.toString() << std::endl;
			
			return children;
	}
	/////////////////////////// MUTATION
	static void simpleMutation(Chromosome& chromosome){
		// If the mutation rate is big enough
			int lettre = (int) rand()%NB_LETTERS;
			// Select one individual in the popultion, select one letter and generate a new one to replace it
			// std::cout << "individu n " << indiv << " lettre n " << lettre << " " << population[indiv].word[lettre] << std::endl;
			chromosome.word[lettre] = getRandomLetter();
			// std::cout << "individu n " << indiv << " lettre n " << lettre << " " << population[indiv].word[lettre] << std::endl;

	}
};
Trait::Random Trait::random ;

template<typename _Trait> 
class GeneticAlgo {
	using Chromosome = typename _Trait::Chromosome;
	using Random = typename _Trait::Random;
	public:
		GeneticAlgo() : population(_Trait::POP_SIZE), population2(), random() {

			// population.reserve(_Trait::POP_SIZE);
			population2.reserve(_Trait::POP_SIZE);
			initialisation();
		};

		void start(){

			do {
				/*
					1) evaluate every individual in population
					2) sort population by best score
					3) crossover
					4) mutation
				*/
				evaluate();
				// std::cout << "evaluate !" << std::endl;
				// printAllRes();

				// std::cout << "sort !" << std::endl;
				sort();
				// printAllRes();

				if(population[0].fitness == 0){
					break;
				}
				// std::cout << "crossover !" << std::endl;
				crossover();
				// printAllRes();


			} while(++currentIterationCount < _Trait::MAX_ITERATIONS);
			evaluate();
			sort();
			printRes();
		}
		void printRes() {
			std::cout << "Best answer found in " << currentIterationCount << " iterations : " << population[0].toString()  << std::endl;
		}
		void test(){
			for(int i=0;i<100;i++){
				std::cout << "rdm >>"<< random.getIntRange(0,5);
			}
		}
	private:
		void printAllRes(){
			for(int i=0; i<_Trait::POP_SIZE; i++){
				std::cout << "[" << i << "] >" << population[i].toString() << std::endl;
			}
		}
		Random random;
		std::vector<Chromosome> population;
		std::vector<Chromosome> population2;
		size_t currentIterationCount = -1;
		void initialisation(){
			for(int i=0; i<_Trait::POP_SIZE; ++i){
				population[i].init();
				population2[i].init();
			}
		}

		void evaluate(){
			for(int i=0; i<_Trait::POP_SIZE; ++i){
				population[i].evaluateFitness();
			}
		}
		void mutation(Chromosome& ch){
			if(rand() / RAND_MAX < _Trait::MUTATION_RATE){
				_Trait::simpleMutation(ch);
			}
		}
		void crossover(){
			// std::cout << "DEBUG >>>>>" << currentIterationCount << std::endl;
			assert(population2.size() == 0);
			assert(population.size() != 0);

			while(population2.size() <= _Trait::POP_SIZE*_Trait::CROSSOVER_RATE){
				std::pair<typename _Trait::Chromosome, typename _Trait::Chromosome> parents = _Trait::simpleSelectionOnHalfOfBests(population);
				std::pair<typename _Trait::Chromosome, typename _Trait::Chromosome> children = _Trait::singlePointCrossover(parents);
				// TODO Peut peut etre depassé le vector ici (si POP_SIZE est impair)
				mutation(children.first);
				mutation(children.second);
				population2.push_back(children.first);
				population2.push_back(children.second);

			}
			while(population2.size() <= _Trait::POP_SIZE){
				std::pair<typename _Trait::Chromosome, typename _Trait::Chromosome> nonChangeIndiv = _Trait::simpleSelection(population);
				// TODO Peut peut etre depassé le vector ici (si POP_SIZE est impair)
				population2.push_back(nonChangeIndiv.first);
				population2.push_back(nonChangeIndiv.second);

			}
			population.swap(population2);
			population2.clear();
		}

		void sort(){
			std::sort(population.begin(), population.end(), _Trait::sort);
		}

};
int main(int argc, char* argv[]){
	// seed = atoi(argv[1]);
	GeneticAlgo<Trait> gen;

	// gen.test();
	gen.start();

	// for(int i=0;i<10;i++){
	// 	std::cout << "test " << random.getBetweenOneAndZero() << std::endl;
	// }

}
