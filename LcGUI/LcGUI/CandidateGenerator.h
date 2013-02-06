#ifndef CANDIDATE_GENERATOR_H
#define CANDIDATE_GENERATOR_H

#include <map>
#include <vector>
#include <list>
#include <iostream>
#include <math.h>

using namespace std;

/*
* Executes the porcess of candidates generation and collects contrast patterns;
*/
class CandidateGenerator {
private:
	vector<Tuple*>& reducedTable;
	vector<vector<Candidate*>*> candidates;
	vector<Candidate*> contrastPatterns;			
	vector<vector<int> > supportsOfCandidates;		//vector to store supports of candidates of length 1
	const int numberOfClasses;
	HashTree* hashTree;
	FixedHashTree::HashTree* fixedHashTree;
	map<unsigned long, vector<Candidate*>*> mapOfCandidatesForGenerators;
	OptimizationType generatorOptimization;
	int sum_of_candidates;

	void findOneLengthCandidates() {
		if(reducedTable.size() == 0) {
			cout << "Empty reduced table";
			return;
		}

		const int numberOfCandidates = reducedTable[0]->getAttributes()->size();

		//initializes support matrix (candidate x class)
		vector<vector<int>*>* supportsOfCandidatesPtr = new vector<vector<int>*>();
		for(int i = 0; i < numberOfCandidates; i++) {
			vector<int> internalSupports;
			vector<int>* supportsPtr = new vector<int>(numberOfClasses);
			for(int i = 0; i < numberOfClasses; i++) {
				internalSupports.push_back(0);
			}
			supportsOfCandidates.push_back(internalSupports);
			supportsOfCandidatesPtr->push_back(supportsPtr);
		}

		//full scan of reduced table
		for(unsigned long i = 0; i < reducedTable.size(); i++) {
			for(int j = 0; j < numberOfCandidates; j++) {
				if((*reducedTable[i]->getAttributes())[j] > 0.0f) {
					int tClass = reducedTable[i]->getTupleClass();
					supportsOfCandidates[j][tClass]++;			//increments support of attribute in appropriate class
					supportsOfCandidatesPtr->at(j)->at(tClass)++;
				}
			}
		}

		//creates and inserts a candidate
		vector<Candidate*>* candidatesOfLengthOne = new vector<Candidate*>();
		for(int i = 0; i < numberOfCandidates; i++) {
			int candidateId = i + 1;
			vector<int>* attributes = new vector<int>();
			attributes->push_back(candidateId);
			Candidate* candidate =  new Candidate(attributes, supportsOfCandidatesPtr->at(i));
			if(candidate->isContrastPattern()) {
				contrastPatterns.push_back(candidate);
			} else {
				candidatesOfLengthOne->push_back(candidate);
			}
		}
		supportsOfCandidatesPtr->clear();
		candidates.push_back(candidatesOfLengthOne);

		//if generator optimization, inserts into hashmap
		if(generatorOptimization == Generators) {
			insertCandidatesToHashMapForGenerators(candidatesOfLengthOne);
		}
	}

	/*
	* Generates candidates of length k+1 based on candidates of length k.
	* Returns generated candidates that will be used to generate longer candidates (k+1).
	*/
	vector<Candidate*>* generateCandidatesLengthKPlusOne(Method method, vector<Candidate*>* candidatesLenkthK) {
		vector<Candidate*>* candidatesLengthKPlusOne =  new vector<Candidate*>();			//it can contain contrast patterns as well
		vector<Candidate*>* candidatesLengthKPlusOneWithoutContrastPatterns =  new vector<Candidate*>();
						Timer t;
		t.start("Generate candidates");
		for(int i = 0; i < candidatesLenkthK->size(); i++) {
			for(int j = i + 1; j < candidatesLenkthK->size(); j++) {
				if((*candidatesLenkthK)[i]->isJoinable((*candidatesLenkthK)[j])) {
					vector<int>* attributes =  new vector<int>();
					joinCandidates(attributes, (*candidatesLenkthK)[i], (*candidatesLenkthK)[j]);
					vector<int>* supports = new vector<int>(numberOfClasses);
					Candidate* candidate = new Candidate(attributes,supports);
					candidatesLengthKPlusOne->push_back(candidate);
				}
			}
		}
		t.stop();
		t.start("Build hash tree");
		switch(method) {			//creates hash tree based on the optimization method
		case Orlando:
			hashTree = new HashTree(candidatesLengthKPlusOne, candidates.size() + 1);
			break;
		case Agrawal: case Michal2:
			fixedHashTree = new FixedHashTree::HashTree(candidatesLengthKPlusOne, candidates.size() + 1);
			break;
		}
						t.stop();
						t.start("Subset and count support");
		switch(method) {			//assigns supports based on the optimization method
		case Orlando:
			assignSupportsToCandidates(hashTree);
			break;
		case Agrawal:
			assignSupportsToCandidatesFromAttrDense(fixedHashTree);
			break;
		}
						t.stop();
						t.start("Collect Contrast Pattern");
		collectContrastPattern(candidatesLengthKPlusOne, candidatesLengthKPlusOneWithoutContrastPatterns);	//puts into candidatesLengthKPlusOneWithoutContrastPatterns
						t.stop();
						t.start("Delete tree");
		if(fixedHashTree != NULL) {
			delete fixedHashTree;
			fixedHashTree = NULL;
		}
		if(hashTree != NULL) {
			delete hashTree;
			hashTree = NULL;
		}
						t.stop();
						cout << "#INFO Number of candidates = " << candidatesLengthKPlusOne->size() << "\r\n";
						sum_of_candidates += candidatesLengthKPlusOne->size(); 
						cout << "#INFO Number of candidates without contrast patterns = " << candidatesLengthKPlusOneWithoutContrastPatterns->size() << "\r\n";
		
		//if generators optimisation 
		if(generatorOptimization == Generators) {
			vector<Candidate*>* candidatesLengthPlusOneMinusGeneratorSupersets = new vector<Candidate*>();
			candidatesLengthPlusOneMinusGeneratorSupersets = performGeneratorOptimisation(candidatesLengthKPlusOneWithoutContrastPatterns);
			insertCandidatesToHashMapForGenerators(candidatesLengthPlusOneMinusGeneratorSupersets);	
			candidates.push_back(candidatesLengthPlusOneMinusGeneratorSupersets);
			cout << "#INFO Number of candidates after Generator optimization = " << candidatesLengthPlusOneMinusGeneratorSupersets->size() << "\r\n";
			candidatesLengthKPlusOne->clear();
			delete candidatesLengthKPlusOne;
			candidatesLengthKPlusOneWithoutContrastPatterns->clear();
			delete candidatesLengthKPlusOneWithoutContrastPatterns;
			return candidatesLengthPlusOneMinusGeneratorSupersets;
		}

		candidates.push_back(candidatesLengthKPlusOneWithoutContrastPatterns);
		candidatesLengthKPlusOne->clear();
		delete candidatesLengthKPlusOne;
		return candidatesLengthKPlusOneWithoutContrastPatterns;
	}

	void collectContrastPattern(vector<Candidate*>* candidatesLenkthKPlusOne, vector<Candidate*>* candidatesLenkthKPlusOneWithoutContrastPatterns) {
		for(int i = 0; i < candidatesLenkthKPlusOne->size(); i++) {
			if((*candidatesLenkthKPlusOne)[i]->isContrastPattern()) {
				contrastPatterns.push_back((*candidatesLenkthKPlusOne)[i]);
			} else {
				candidatesLenkthKPlusOneWithoutContrastPatterns->push_back((*candidatesLenkthKPlusOne)[i]);
			}
		}
	}


	//does one scan of database and determine supports
	void assignSupportsToCandidates(HashTree* hashTree) {
		for(unsigned long i = 0; i < reducedTable.size(); i++) {
			reducedTable[i]->subset_and_count(hashTree);
		}
	}

	void assignSupportsToCandidatesFromAttrDense(FixedHashTree::HashTree* hashTree) {
		for(unsigned long i = 0; i < reducedTable.size(); i++) {
			reducedTable[i]->countSupportFromAttrDense(hashTree);
		}
	}

	void joinCandidates(vector<int>* attributes, Candidate* first, Candidate* second) {
		for(int i = 0; i < first->getAttributes()->size(); i++) {
			attributes->push_back(first->getAttributes()->at(i));
		}
		attributes->push_back(second->getAttributes()->back());
	}

	vector<Candidate*>* performGeneratorOptimisation(vector<Candidate*>* candidatesWithoutContrastPatterns) {
		//we create a list from a vector to increase the speed of candidates removal
		vector<Candidate*>* newSetOfCandidatesWithoutCP = new vector<Candidate*>();
		std::vector<Candidate*>::iterator it;
		for(it = candidatesWithoutContrastPatterns->begin(); it != candidatesWithoutContrastPatterns->end(); ++it) {
			if(!isForDeletionByGeneratorsOptimization(*it)) {
				newSetOfCandidatesWithoutCP->push_back(*it);
			}
		}
		return newSetOfCandidatesWithoutCP;
	}

	bool isForDeletionByGeneratorsOptimization(Candidate* candidate) {
		//generate subsets k-1 length
		for(int i = 0; i < candidate->getAttributes()->size(); i++) {
			vector<int> subset;
			subset.reserve(candidate->getAttributes()->size()-1);
			for(int k = 0; k < candidate->getAttributes()->size(); k++) {
				if(k != i) {
					subset.push_back((*candidate->getAttributes())[k]);
				}
			}
			unsigned long hashCode = calculateHashCode(&subset);
			//checks if in hashMap and   
			if(mapOfCandidatesForGenerators.find(hashCode) != mapOfCandidatesForGenerators.end()) {
				vector<Candidate*>* candidatesOfHashCode = mapOfCandidatesForGenerators[hashCode];
				for(int k = 0; k < candidatesOfHashCode->size(); k++) {
					if((*candidatesOfHashCode)[k]->attributesEquals(&subset) && (*candidatesOfHashCode)[k]->getTotalSupportCounterAllClasses() == (candidate->getTotalSupportCounterAllClasses())) {
						return true;
					}
				}
			}
		}
		return false;
	}

	void insertCandidatesToHashMapForGenerators(vector<Candidate*>* candidates) {
		unsigned long curentIndexInHashMap = 0;
		for(int i = 0; i < candidates->size(); i++) {
			Candidate* candidate = (*candidates)[i];
			curentIndexInHashMap = calculateHashCode(candidate);

			if(mapOfCandidatesForGenerators.find(curentIndexInHashMap) == mapOfCandidatesForGenerators.end()) {
				vector<Candidate*>* candidatesOfHashCode = new vector<Candidate*>();
				candidatesOfHashCode->push_back(candidate);
				mapOfCandidatesForGenerators.insert(pair<unsigned long,vector<Candidate*>*>(curentIndexInHashMap,candidatesOfHashCode));
			} else {
				mapOfCandidatesForGenerators[curentIndexInHashMap]->push_back(candidate);
			}
		}
	}

	unsigned long calculateHashCode(Candidate* candidate) {
		vector<int>* attributes = candidate->getAttributes();
		return calculateHashCode(attributes);
	}

	unsigned long calculateHashCode(vector<int>* attributes) {
		unsigned long sumOfHashCodesOfAttributes = 0;
		const unsigned long one = 1;
		for(int i = 0; i < attributes->size(); i++) {
			sumOfHashCodesOfAttributes += one << ( i % 32) + i + one << ((*attributes)[i] % 32) + (*attributes)[i] + 1; 
		}
		return sumOfHashCodesOfAttributes;
	}

public:
	CandidateGenerator(vector<Tuple*>& tReducedTable, const int tNumberOfClasses, OptimizationType tGeneratorOptimisation): reducedTable(tReducedTable), numberOfClasses(tNumberOfClasses) {
		generatorOptimization = tGeneratorOptimisation;
		hashTree = NULL;
		fixedHashTree = NULL;
		sum_of_candidates=0;
	}

	~CandidateGenerator() 
	{
		for(int i=0;i<candidates.size();i++)
		{
			for(int j=0; j<candidates[i]->size();j++)
			{
				delete candidates[i]->at(j);
			}
		}
		candidates.clear();
		supportsOfCandidates.clear();
		contrastPatterns.clear();
		for(map<unsigned long, vector<Candidate*>*>::iterator it = mapOfCandidatesForGenerators.begin(); it != mapOfCandidatesForGenerators.end(); ++it) {
				it->second->clear();
				delete it->second;
		    }
		mapOfCandidatesForGenerators.clear();

	}

	void execute(Method method) {
		Timer total;
		total.start("Total time of finding minimal contrast patterns");
		Timer t;
		t.start("Candidates of length [1] generation & support count");
		findOneLengthCandidates();
		t.stop();
		if(candidates[0]->size() >= 2) {
			int k = 1;		//initial length of candidates that will be incremented
			while(1) {
				t.start("Total time");
				cout << "========================================================" << "\r\n";
				cout << "#BEGIN: Candidates of length [" << k +1  << "]" << "\r\n";
				if(generateCandidatesLengthKPlusOne(method, candidates[k-1])->size() <= 1) {
					t.stop();
					cout << "#END: Candidates of length [" << k +1  << "]" << "\r\n";
					cout << "========================================================" << "\r\n";
					break;	//the process of generating patterns has finished
				}
				t.stop();
				cout << "#END: Candidates of length [" << k +1  << "]" << "\r\n";
				k++;
			}
		}
		cout << "#INFO Total number of generated candidates = " << sum_of_candidates << "\r\n";	
		cout << "========================================================" << "\r\n";
		cout << "#INFO Total number of minimal contrast patterns = " << contrastPatterns.size() << "\r\n";	
		cout << "========================================================" << "\r\n";
		total.stop();
		cout << "========================================================" << "\r\n";
	}

	vector<vector<Candidate*>*>& getCandidates() { return candidates; }

	vector<Candidate*>* getContrastPatterns() { return &contrastPatterns; }

};

#endif
