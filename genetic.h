#include <ostream>
#include <sstream>
struct MyGene ;
struct MyRandom {
	static const int ASCII_DOWN_LIMIT;
	static const int ASCII_UP_LIMIT;

	MyRandom() : r(), gen(r()), get_real_between_one_and_zero(0, 1), get_int_ascii(ASCII_DOWN_LIMIT,ASCII_UP_LIMIT) {
	}

	MyRandom& operator=(const MyRandom& myRandom){
		// r();
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
        //int downlimit=ASCII_DOWN_LIMIT;
       // int uplimit=ASCII_UP_LIMIT;
       // return std::uniform_int_distribution<int>(ASCII_DOWN_LIMIT, ASCII_UP_LIMIT)(gen);
    	//return std::uniform_int_distribution<int>(32, 126)(gen);
    	return get_int_ascii(gen);
    	// return 0;
    }
    // Return a random int between downlimit and uplimit included
    int getIntRange(int downlimit, int uplimit){
    	return std::uniform_int_distribution<int>(downlimit, uplimit)(gen);
    }

	private:
 	// Seed with a real random value, if available
    std::random_device r;
    std::mt19937 gen; 
    std::uniform_real_distribution<double> get_real_between_one_and_zero;
    std::uniform_int_distribution<int> get_int_ascii;
};

const int MyRandom::ASCII_DOWN_LIMIT = 32;
const int MyRandom::ASCII_UP_LIMIT = 126;

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
		ss << "fitness = " << std::to_string(fitness) << " chromosome : ";
		for(size_t i=0; i<genes.size(); i++){
			ss << genes[i].toString();
		}
		return ss.str();;
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
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> selection(std::vector<Chromosome<_Trait>> sortedPopulation){
		MyRandom random;
		int rdm = random.getIntRange(0, _Trait::POP_SIZE-1);
		int rdm2 = random.getIntRange(0, _Trait::POP_SIZE-1);
		assert(rdm >= 0 && rdm < _Trait::POP_SIZE);
		assert(rdm2 >= 0 && rdm < _Trait::POP_SIZE);
		std::pair<Chromosome<_Trait>, Chromosome<_Trait>> pairToReturn(sortedPopulation[rdm], sortedPopulation[rdm2]);
		return pairToReturn;
	}
};
template<typename _Trait> 
struct SimpleSelectionOfBests{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> selection(std::vector<Chromosome<_Trait>> sortedPopulation){
		MyRandom random;
		int rdm = random.getIntRange(0, _Trait::POP_SIZE*_Trait::BEST_SELECTION_RATE-1);
		int rdm2 = random.getIntRange(0, _Trait::POP_SIZE*_Trait::BEST_SELECTION_RATE-1);
		std::pair<Chromosome<_Trait>, Chromosome<_Trait>> pairToReturn(sortedPopulation[rdm], sortedPopulation[rdm2]);
		return pairToReturn;
	}
};

template<typename _Trait> 
struct TournamentSelection{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> selection(std::vector<Chromosome<_Trait>> sortedPopulation){
		// TODO
	}
};
template<typename _Trait> 
struct RouletteSelection{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> selection(std::vector<Chromosome<_Trait>> sortedPopulation){
		// TODO
	}
};
////////////////////////////////////////// CROSSOVER ////////////////////////////////////////// vc
template<typename _Trait> 
struct SinglePointCrossover{
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> crossover(const std::pair<Chromosome<_Trait>, Chromosome<_Trait>>& parents){		
		MyRandom random;
		std::pair<Chromosome<_Trait>, Chromosome<_Trait>> children;
		int randomCursor = random.getIntRange(1, _Trait::NB_GENES - 2);
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
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> crossover(std::pair<Chromosome<_Trait>, Chromosome<_Trait>> parents){		
		MyRandom random;
		std::pair<Chromosome<_Trait>, Chromosome<_Trait>> children;
		int randomCursor = random.getIntRange(1, _Trait::NB_GENES - 3);
		int randomCursor2 = random.getIntRange(randomCursor+1, _Trait::NB_GENES - 2);

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
	static std::pair<Chromosome<_Trait>, Chromosome<_Trait>> crossover(std::pair<Chromosome<_Trait>, Chromosome<_Trait>> parents){		
		// TODO
	}
};
////////////////////////////////////////// MUTATION //////////////////////////////////////////
template<typename _Trait> 
struct SimpleMutation{
	static void mutation(Chromosome<_Trait>& chromosome){
		MyRandom random;
		int indexGene = random.getIntRange(0, _Trait::NB_GENES-1);
		chromosome.genes[indexGene].mutation();
	}

};

template<typename _Trait> 
struct Eval
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
class GeneticAlgo : _Trait::Selection, _Trait::Crossover, _Trait::Mutation {
	using Random = typename _Trait::Random;
		public:
		GeneticAlgo() : population(_Trait::POP_SIZE), population2(), random() {
			population2.reserve(_Trait::POP_SIZE);
			// initialisation();
		};

		void start(){
			do {
				printAllRes();
				std::cout << "evaluation - sort" << std::endl;
				evaluate();
				sort();
				printAllRes();
				if(population[0].fitness == 0){
					break;
				}
				std::cout << "crossover" << std::endl;
				crossover();
				printAllRes();
			} while(++currentIterationCount < (_Trait::MAX_ITERATIONS-1));
			std::cout << "evaluation - sort" << std::endl;
			evaluate();
			sort();
			printAllRes();
		}
	private:
		void printRes() {
			std::cout << "Best answer found in " << currentIterationCount << " iterations : " << population[0]  << std::endl;
		}
		void printAllRes(){
			for(int i=0; i<_Trait::POP_SIZE; i++){
				std::cout << "[" << i << "] >" << population[i].toString() << std::endl;
			}
		}
		std::vector<Chromosome<_Trait>> population;
		std::vector<Chromosome<_Trait>> population2;
		Random random;
		size_t currentIterationCount = -1;
		// void initialisation(){
		// 	for(int i=0; i<_Trait::POP_SIZE; ++i){
		// 		population[i].init();
		// 		population2[i].init();
		// 	}
		// }

		void evaluate(){
			for(int i=0; i<_Trait::POP_SIZE; ++i){
				population[i].evaluateFitness();
			}
		}
		void mutation(Chromosome<_Trait>& ch){
			if( random.getRealBetweenOneAndZero() < _Trait::MUTATION_RATE){
				std::cout << "MUTATION !!! " <<  std::endl;
				_Trait::Mutation::mutation(ch);
			}
		}
		void crossover(){
			// std::cout << "DEBUG >>>>>" << currentIterationCount << std::endl;
			assert(population2.size() == 0);
			assert(population.size() != 0);

			while(population2.size() <= _Trait::POP_SIZE*_Trait::CROSSOVER_RATE){
				std::pair< Chromosome<_Trait>,  Chromosome<_Trait>> parents = _Trait::Selection::selection(population);
				std::cout << "parent 1 : " << parents.first.toString() << " parent 2 : " << parents.second.toString() << std::endl;
				std::pair< Chromosome<_Trait>,  Chromosome<_Trait>> children = _Trait::Crossover::crossover(parents);
				std::cout << "Children produced by crossover  : child 1 : " << children.first.toString() << " child 2 : " << children.second.toString() << std::endl;
				// TODO Peut peut etre depassé le vector ici (si POP_SIZE est impair)
				mutation(children.first);
				mutation(children.second);
				std::cout << "        Children after mutation : child 1 : " << children.first.toString() << " child 2 : " << children.second.toString() << std::endl;
				population2.push_back(children.first);
				population2.push_back(children.second);

			}
			while(population2.size() <= _Trait::POP_SIZE){
				std::pair< Chromosome<_Trait>, Chromosome<_Trait>> nonChangeIndiv = _Trait::Selection::selection(population);
				// TODO Peut peut etre depassé le vector ici (si POP_SIZE est impair)
				population2.push_back(nonChangeIndiv.first);
				population2.push_back(nonChangeIndiv.second);

			}
			population.swap(population2);
			population2.clear();
		}
		void sort(){
			std::sort(population.begin(), population.end(), _Trait::Sort::sort);
		}
};

