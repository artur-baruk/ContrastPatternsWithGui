#ifndef FIXEDHASHTREE_H
#define FIXEDHASHTREE_H

#include <climits>



namespace FixedHashTree {


class HashTreeNode {
    private:
        int level; //roots level = 0
        int hash_arg;
        int candidateLenght;

        map<int,HashTreeNode*> children;
        vector<Candidate*> candidates;

	public:
        HashTreeNode(int hash_arg, int candidateLenght = 0, int level = 0) : hash_arg(hash_arg), candidateLenght(candidateLenght), level(level) {
            candidates.reserve(20);
        }

        ~HashTreeNode() 
		{
			for ( int i = 0; i < children.size(); i++ ) {
				delete children[i];
			}
			children.clear();
			candidates.clear();
		};

		//wrzuca wsk na kandydata do wektora
		void insertCandidate(Candidate* p_candidate)
		{
		    if(children.empty() && candidates.size() < 20 || level >= p_candidate->getAttributes()->size()) {
                candidates.push_back(p_candidate);
		    }
            else {
                map<int,HashTreeNode*>::iterator it;
                for(int i = 0; i < candidates.size(); ++i) {
                    int hash = candidates[i]->getAttributes()->at(level) % hash_arg;
                    it = children.find(hash);
                    if(it == children.end()) {
                        children.insert(pair<int,HashTreeNode*>(hash, new HashTreeNode(hash_arg, candidateLenght, level+1)));
                    }
                    children[hash]->insertCandidate(candidates[i]);

                }
                int hash = p_candidate->getAttributes()->at(level) % hash_arg;
                it = children.find(hash);
                if(it == children.end()) {
                    children.insert(pair<int,HashTreeNode*>(hash, new HashTreeNode(hash_arg, candidateLenght, level+1)));
                }

				children[hash]->insertCandidate(p_candidate);
                candidates.clear();
            }
		}

		void countSupport(vector<int>* p_attrDense, int tClass, int currentPosition = 0)
		{
			for(int i = 0; i < candidates.size(); ++i)
			{
				if(candidates[i]->isSubset(p_attrDense)) //jezeli kandydat == podzbior
				{
					candidates[i]->incrementSupport(tClass);
					candidates[i]->incrementTotalSupportCounterAllClasses();
					
				}
			}
			map<int,HashTreeNode*>::iterator it;
			//hashujemy od aktualnej pozycji do konca bez ostatnich (k - level - 1) pozycji
			int maxPosition = p_attrDense->size() - (candidateLenght - level - 1);
			if(maxPosition > p_attrDense->size())
                maxPosition = p_attrDense->size();
			for(int i = currentPosition; i < maxPosition; ++i) {
                it = children.find(p_attrDense->at(i) % hash_arg);
                if(it != children.end()) {
                    it->second->countSupport(p_attrDense, tClass, i + 1);
                }
			}

		}

		void countCompactSupport(vector<int>* classCompactSupports, vector<int>* p_attrDense, int tClass, int currentPosition = 0)
		{
			for(int i = 0; i < candidates.size(); ++i)
			{
				if(candidates[i]->isSubset(p_attrDense)) //jezeli kandydat == podzbior
				{
					(*classCompactSupports)[tClass]++;
					return;
				}
			}
			map<int,HashTreeNode*>::iterator it;
			//hashujemy po wszystkich pozycjach
			for(int i = currentPosition; i < p_attrDense->size(); ++i) {
                it = children.find(p_attrDense->at(i) % hash_arg);
                if(it != children.end()) {
                    it->second->countCompactSupport(classCompactSupports, p_attrDense, tClass, i + 1);
					return;
                }
			}

		}

		void print() {
			cout << "#######NODE:" << "\r\n";
		    for(map<int,HashTreeNode*>::iterator it = children.begin(); it != children.end(); ++it) {
                it->second->print();
		    }
		    std::cout << "Node level: " << level << "\r\n";
		    std::cout << "Candidates count: " << candidates.size() << "\r\n";
		    for(int i = 0; i < candidates.size(); ++i) {
                std::cout << "Candidate " << i << " attributes: ";
                std::vector<int>* attr = candidates[i]->getAttributes();
                for(int j = 0; j < attr->size(); ++j) {
                    std::cout << attr->at(j) << ", ";
                }
		    std::cout << "\r\n";
		    }
		}

};

class HashTree {
  private:
		HashTreeNode* root;
		int hash_arg;
		int maxLevel;

	public:
		HashTree(std::vector<Candidate*>* p_candidates, int k) //k to dlugosc wstawianych kandydatow
		{
			maxLevel = k; // wysokosc drzewa
			hash_arg = k+1; // bedziemy haszowac hash = argument MOD hash_arg

			root = new HashTreeNode(hash_arg, k);

			for( int i = 0; i < p_candidates->size(); i++ ) {
				root->insertCandidate(p_candidates->at(i));
			}
		}

		~HashTree()
		{
			delete root;
			int i = 1;
		};

		int getMaxLevel() {
			return maxLevel;
		}

		void countSupport(vector<int>* p_subset, int tClass)
		{
			root->countSupport(p_subset,tClass);
		}

		void countCompactSupport(vector<int>* classCompactSupports, vector<int>* p_subset, int tClass)
		{
			root->countCompactSupport(classCompactSupports, p_subset, tClass);
		}


		void printTree() {
			root->print();
		}
};


}

#endif // FIXEDHASHTREE_H_INCLUDED
