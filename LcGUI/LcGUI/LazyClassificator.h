#ifndef LAZY_CLASSIFICATOR_H
#define LAZY_CLASSIFICATOR_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>
#include "Timer.h"
#include "Combination.h"
#include "Candidate.h"
#include "Tuple.h"
#include "TupleCreator.h"
#include "CandidateGenerator.h"
#include "ContrastPatternScorer.h"


using namespace std;

/*
 * The class that execute the process of classification.
 */
class LazyClassificator {
	private:
		std::vector<Tuple*>& reducedTable;			//table with reduced classified records
		Method method;
		OptimizationType optimizationType;
		std::vector<int>& objectsInClassesCount;

	public:
		LazyClassificator(vector<Tuple*>& tReducedTable, Method tMethod, OptimizationType tOptimizationType, vector<int>& tObjectsInClassesCount) : reducedTable(tReducedTable), objectsInClassesCount(tObjectsInClassesCount) {
			optimizationType = tOptimizationType;
			method = tMethod;
		}

		void executeParallelWithTwoLengthCounter() {
			
		}

		int executeWithNCounters() {
			cout << "========================================================" << "\r\n";
			cout << "#INFO Number of objects in training data = " << reducedTable.size() << "\r\n";
			cout << "#INFO Number of attributes = " << reducedTable.at(0)->getAttributes()->size() << "\r\n";
			cout << "#INFO Number of classes = " << objectsInClassesCount.size() << "\r\n";
			cout << "========================================================" << "\r\n";
			CandidateGenerator candidateGenerator = CandidateGenerator(reducedTable, objectsInClassesCount.size(), optimizationType);
			candidateGenerator.execute(method);
			Timer t;
			t.start("Total time of scorer execution");
			ContrastPatternScorer scorer = ContrastPatternScorer(reducedTable, candidateGenerator.getContrastPatterns(), objectsInClassesCount);
			int decision_class = scorer.chooseDecisionClass();
			cout << "========================================================" << "\r\n";
			t.stop();
			return decision_class;
		}


};

#endif
