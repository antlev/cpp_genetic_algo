#ifndef GENETIC_H
#define GENETIC_H

#include <ostream>
#include <sstream>
#include <random>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <unistd.h>
// #include "irq.h"
// #include "genetic.h"

long pingpong();
struct MyGene ;
struct MyRandom {
	static constexpr int ASCII_DOWN_LIMIT = 32;
	static constexpr int ASCII_UP_LIMIT = 126;

	MyRandom() : r(), gen(r()), get_real_between_one_and_zero(0, 1), get_int_ascii(ASCII_DOWN_LIMIT,ASCII_UP_LIMIT), get_int_between_one_and_zero(0,1) {
	}

	MyRandom& operator=(const MyRandom& myRandom){
		gen = myRandom.gen;
		get_real_between_one_and_zero = myRandom.get_real_between_one_and_zero;
		get_int_ascii = myRandom.get_int_ascii;
		return *this; 
	} 

	// Return a real between 1 and 0
    double getRealBetweenOneAndZero() {
    	return get_real_between_one_and_zero(gen);
    }
    // Return a char between the 2 limits of ascii char
    char getIntAsciiChar() {
    	return get_int_ascii(gen);
    }
    // Return a random int between downlimit and uplimit included
    int getIntRange(int downlimit, int uplimit){
    	return std::uniform_int_distribution<int>(downlimit, uplimit)(gen);
    }
   	// Return a real between 1 and 0
    int getIntBetweenOneAndZero() {
    	return get_int_between_one_and_zero(gen);
    }

	private:
    std::random_device r;
    std::mt19937 gen; 
    std::uniform_real_distribution<double> get_real_between_one_and_zero;
    std::uniform_int_distribution<int> get_int_ascii;
    std::uniform_int_distribution<int> get_int_between_one_and_zero;
};

template<typename _Trait> 
struct Chromosome : _Trait::Evaluate {
	
	typename std::vector<typename _Trait::Gene> genes;
	double fitness;

	Chromosome() : genes(_Trait::NB_GENES), fitness(_Trait::MAX_ERROR) {
	}

	void evaluateFitness() {
		fitness = _Trait::Evaluate::evaluate(*this);
	}
	std::string toString() const {

		std::stringstream ss;
		ss << "fitness >" << std::to_string(fitness) << "< chromosome : ";
		for(size_t i=0; i<genes.size(); i++){
			ss << genes[i].toString() << " ";
		}
		return ss.str();;
	}
	std::string toSaveForm() const {

		std::stringstream ss;
		for(size_t i=0; i<genes.size(); i++){
			ss << genes[i].toString() << " ";
		}
		return ss.str();;
	}
	void saveState(){
		std::ofstream myfile;
		myfile.open (_Trait::SAVE_FILE_PATH, std::ios_base::app);
		myfile << (*this).toSaveForm() << "\n";
		myfile.close();
	}
};
////////////////////////////////////////// EVALUATION //////////////////////////////////////////
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
////////////////////////////////////////// SORT //////////////////////////////////////////
template<typename _Trait> 
struct Minimise
{	
	static bool sort(Chromosome<_Trait>& c1, Chromosome<_Trait>& c2){
		return c1.fitness < c2.fitness ;
	}
};
template<typename _Trait> 
struct Maximise
{	
	static bool sort(Chromosome<_Trait>& c1, Chromosome<_Trait>& c2){
		return c1.fitness > c2.fitness ;
	}
};

////////////////////////////////////////// SELECTION //////////////////////////////////////////
template<typename _Trait> 
struct SimpleSelection{
	// Take a sorted population and select one randomly using the BEST_SELECTION_RATE variable
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> selection(std::vector<Chromosome<_Trait>> sortedPopulation, MyRandom* random){
		int rdm = random->getIntRange(0, _Trait::POP_SIZE-1);
		int rdm2 = random->getIntRange(0, _Trait::POP_SIZE-1);
		assert(rdm >= 0 && rdm < _Trait::POP_SIZE);
		assert(rdm2 >= 0 && rdm < _Trait::POP_SIZE);
		std::pair<Chromosome<_Trait>, Chromosome<_Trait>> pairToReturn(sortedPopulation[rdm], sortedPopulation[rdm2]);
		return pairToReturn;
	}
};
template<typename _Trait> 
struct SimpleSelectionOfBests{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> selection(std::vector<Chromosome<_Trait>> sortedPopulation, MyRandom* random){
		int rdm = random->getIntRange(0, _Trait::POP_SIZE*_Trait::BEST_SELECTION_RATE-1);
		int rdm2 = random->getIntRange(0, _Trait::POP_SIZE*_Trait::BEST_SELECTION_RATE-1);
		std::pair<Chromosome<_Trait>, Chromosome<_Trait>> pairToReturn(sortedPopulation[rdm], sortedPopulation[rdm2]);
		return pairToReturn;
	}
};
template<typename _Trait> 
struct SelectionOfFirstBests{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> selection(std::vector<Chromosome<_Trait>> sortedPopulation, int i){
		std::pair<Chromosome<_Trait>, Chromosome<_Trait>> pairToReturn(sortedPopulation[i], sortedPopulation[i+1]);
		return pairToReturn;
	}
};

template<typename _Trait> 
struct TournamentSelection{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> selection(std::vector<Chromosome<_Trait>> sortedPopulation, MyRandom* random){
		// TODO
	}
};
template<typename _Trait> 
struct RouletteSelection{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> selection(std::vector<Chromosome<_Trait>> sortedPopulation, MyRandom* random){
		// TODO
	}
};
////////////////////////////////////////// CROSSOVER ////////////////////////////////////////// 
template<typename _Trait> 
struct SinglePointCrossover{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> crossover(const std::pair<Chromosome<_Trait>, Chromosome<_Trait>>& parents, MyRandom* random){		
		std::pair<Chromosome<_Trait>, Chromosome<_Trait>> children;
		int randomCursor = random->getIntRange(1, _Trait::NB_GENES - 2);
		for(int i=0; i<randomCursor; i++){
			children.first.genes[i] = parents.first.genes[i];
			children.second.genes[i] = parents.second.genes[i];
		}
		for(int i=randomCursor; i<_Trait::NB_GENES; i++){
			children.first.genes[i] = parents.second.genes[i];
			children.second.genes[i] = parents.first.genes[i];
		}			
		return std::move(children);
	}
};
template<typename _Trait> 
struct TwoPointCrossover{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> crossover(std::pair<Chromosome<_Trait>, Chromosome<_Trait>> parents, MyRandom* random){		
		std::pair<Chromosome<_Trait>, Chromosome<_Trait>> children;
		int randomCursor = *random->getIntRange(1, _Trait::NB_GENES - 3);
		int randomCursor2 = *random->getIntRange(randomCursor+1, _Trait::NB_GENES - 2);

		children.first.genes = parents.first.genes;
		children.second.genes = parents.second.genes;
		assert(randomCursor < randomCursor2);
		for(int i=randomCursor; i<randomCursor2; i++){
			children.first.genes[i] = parents.second.genes[i];
			children.second.genes[i] = parents.first.genes[i];
		}
		return children;
	}
};
template<typename _Trait> 
struct MultiPointsCrossover{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> crossover(std::pair<Chromosome<_Trait>, Chromosome<_Trait>> parents, MyRandom* random){		
		// TODO
	}
};
////////////////////////////////////////// MUTATION //////////////////////////////////////////
template<typename _Trait> 
struct SimpleMutation{
	static void mutation(Chromosome<_Trait>& chromosome, MyRandom* random){
		int indexGene = random->getIntRange(0, _Trait::NB_GENES-1);
		chromosome.genes[indexGene].mutation();
	}

};
template<typename _Trait> 
struct BuildMyHeader{
	static void buildHeader(){

		truncate(_Trait::SAVE_FILE_PATH, 0);
		std::ofstream saveStateFile;
		saveStateFile.open (_Trait::SAVE_FILE_PATH);
		saveStateFile << _Trait::NB_GENES << " " << _Trait::POP_SIZE << " " << _Trait::MAX_ITERATIONS << " " << _Trait::CROSSOVER_RATE << " " << _Trait::BEST_SELECTION_RATE << " " << _Trait::MUTATION_RATE << " "  << _Trait::LIMIT_STAGNATION << "\n";
		saveStateFile.close();
	}
};
template<typename _Trait> 
class GeneticAlgo : _Trait::Selection, _Trait::Crossover, _Trait::Mutation {
	using Random = typename _Trait::Random;
	public:
		GeneticAlgo() : population(_Trait::POP_SIZE), population2(), random() {
			population2.reserve(_Trait::POP_SIZE);
		};
		GeneticAlgo(int i) : population(_Trait::POP_SIZE), population2(), random() {
			population2.reserve(_Trait::POP_SIZE);
		};
		void start(){
		#ifndef NDEBUG
			printAllRes();
		#endif
			do {
				#ifndef NDEBUG
					printAllRes();
					std::cout << "evaluation - sort" << std::endl;
				#endif
				evaluate();
				sort();
				#ifndef NDEBUG
					printAllRes();
				#endif
				// if(population[0].fitness == 0){
				// 	std::cout << "Breaking !!" << std::endl;
				// 	break;
				// }
				if(stagnation > _Trait::LIMIT_STAGNATION){
					std::cout << "Breaking !!" << std::endl;
					break;
				}
				crossover();
				#ifndef NDEBUG
					std::cout << "crossover" << std::endl;
					printAllRes();
				#endif
				if( population[0].fitness >= bestAnswer ){
					stagnation++;
				}else{
					stagnation=0;
					bestAnswer = population[0].fitness;
				}
				saveState();
			} while(++currentIterationCount < (_Trait::MAX_ITERATIONS-1) && stagnation < _Trait::LIMIT_STAGNATION);
			evaluate();
			sort();
			printAllRes();
		}
	private:
		void printRes() {
			std::cout << "Best answer found in " << currentIterationCount << " iterations : " << population[0]  << std::endl;
		}
		void printAllRes(){
			std::cout << "Best answer found in " << currentIterationCount << " iterations : " << population[0]  << std::endl;
			for(int i=0; i<_Trait::POP_SIZE; i++){
				std::cout << "[" << i << "] >" << population[i].toString() << std::endl;
			}
		}
		std::vector<Chromosome<_Trait>> population;
		std::vector<Chromosome<_Trait>> population2;
		Random random;
		size_t currentIterationCount = 0;
		long bestAnswer = _Trait::MAX_ERROR;
		int stagnation = 0;

		void evaluate(){
			for(int i=0; i<_Trait::POP_SIZE; ++i){
				population[i].evaluateFitness();
			}
		}
		void mutation(Chromosome<_Trait>& ch){
			if( random.getRealBetweenOneAndZero() < _Trait::MUTATION_RATE){
				#ifndef NDEBUG
					std::cout << "MUTATION !!! " <<  std::endl;
				#endif		
				_Trait::Mutation::mutation(ch, &random);
			}
		}
		void crossover(){
			assert(population2.size() == 0);
			assert(population.size() != 0);

			while(population2.size() <= _Trait::POP_SIZE*_Trait::CROSSOVER_RATE){
				std::pair< Chromosome<_Trait>,  Chromosome<_Trait>> parents = _Trait::Selection::selection(population, &random);
				std::pair< Chromosome<_Trait>,  Chromosome<_Trait>> children = _Trait::Crossover::crossover(parents, &random);
				#ifndef NDEBUG
					std::cout << "Children produced by crossover  : child 1 : " << children.first.toString() << " child 2 : " << children.second.toString() << std::endl;
				#endif		
				mutation(children.first);
				mutation(children.second);
				#ifndef NDEBUG
					std::cout << "        Children after mutation : child 1 : " << children.first.toString() << " child 2 : " << children.second.toString() << std::endl;
				#endif		
				population2.push_back(children.first);
				population2.push_back(children.second);

			}
			int i=0;
			while(population2.size() <= _Trait::POP_SIZE){
				std::pair< Chromosome<_Trait>, Chromosome<_Trait>> nonChangeIndiv = _Trait::SelectionOfBests::selection(population, i);
				i+=2;
				population2.push_back(nonChangeIndiv.first);
				population2.push_back(nonChangeIndiv.second);

			}
			population.swap(population2);
			population2.clear();
		}
		void sort(){
			std::sort(population.begin(), population.end(), _Trait::Sort::sort);
		}
		void saveState(){
			// TODO

			// Build header 
			_Trait::BuildHeader::buildHeader();

			for(size_t i=0;i<_Trait::POP_SIZE;i++){
				population[i].saveState();
			}		
		}
};
#endif // GENETIC_H