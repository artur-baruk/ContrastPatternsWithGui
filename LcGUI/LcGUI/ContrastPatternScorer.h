#ifndef CONTRAST_PATTERN_SCORER_H
#define CONTRAST_PATTERN_SCORER_H

#include <vector>
#include <iostream>
#include "Candidate.h"
#include "Tuple.h"
#include "FixedHashTree.h"

#define HASH_DISTRIBUTE 371-1		//number used for hashing distribution

using namespace std;

class ContrastPatternScorer {
	private:
		vector<Candidate*>* contrastPatterns;
		vector<int>& classCardinalityTable;
		vector<Tuple*>& reducedTable;
		vector<int> compactSupportOfClassesCounter;

		//Assigns the number of transactions that are supported by contrast patterns
		void assignCompactSupportsToCandidatesFromAttrDense(FixedHashTree::HashTree* hashTree, vector<int>* compactSupportOfClassesCounter) {
			for(unsigned long i = 0; i < reducedTable.size(); i++) {
				reducedTable[i]->countCompactSupportFromAttrDense(compactSupportOfClassesCounter, hashTree);
			}
		}

	public:
		ContrastPatternScorer(vector<Tuple*>& tReducedTable, vector<Candidate*>* tContrastPatterns, vector<int>& tClassCardinalityTable): contrastPatterns(tContrastPatterns),  
						classCardinalityTable(tClassCardinalityTable), reducedTable(tReducedTable){ 
			for(int i = 0; i < classCardinalityTable.size(); i++) {
				compactSupportOfClassesCounter.push_back(0);
			}
		}

		//Gets decision class with the highest compactScore
		int chooseDecisionClass() {
			Timer t;
			t.start("Build hash tree");
			FixedHashTree::HashTree* hashTree = new FixedHashTree::HashTree(contrastPatterns, HASH_DISTRIBUTE);
			t.stop();
			t.start("Assign compact supports");
			assignCompactSupportsToCandidatesFromAttrDense(hashTree, &compactSupportOfClassesCounter);
			t.stop();
			cout << "========================================================" << "\r\n";

			int indexOfClass = 0;
			double maxCompactScore = 0.0;
			double currentComapctScore = 0.0;
			for(int i = 0; i < classCardinalityTable.size(); i++) {
				currentComapctScore = ((double)compactSupportOfClassesCounter[i]) / ((double)classCardinalityTable[i]);
				cout << "#INFO Compact score for class [" << i << "] = " << currentComapctScore << "\r\n";
				if(currentComapctScore > maxCompactScore) {
					maxCompactScore = currentComapctScore;
					indexOfClass = i;
				}
			}
			return indexOfClass;
		}

};

#endif
