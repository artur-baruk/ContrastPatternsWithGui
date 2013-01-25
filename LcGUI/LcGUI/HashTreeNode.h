#ifndef HASH_TREE_NODE_H
#define HASH_TREE_NODE_H


using namespace std;

/*
 *
 */

class HashTreeNode {

    private:
		int level; //roots level = 0
		map<int,HashTreeNode*> children;
		vector<Candidate*> candidates;

    public:

		HashTreeNode(int p_level) {
			level = p_level;
		}

		~HashTreeNode() 
		{
			for (map<int,HashTreeNode*>::iterator it = children.begin(); it!=children.end();it++)
			{
				delete it->second;
			}
			children.clear();
			candidates.clear();
		};

		//zwraca wskaznik na potomka o odpowiednim haszu. W przypadku kiedy taki potomek nie istnieje to go tworzy.
		HashTreeNode* getChild(int hash, bool insertEnabled)
		{
			map<int,HashTreeNode*>::iterator it;
			pair<map<int,HashTreeNode*>::iterator,bool> ret;
			it = children.find(hash);
			if (it == children.end())
			{
				if (insertEnabled) {
					ret =	children.insert(pair<int,HashTreeNode*>(hash,new HashTreeNode(level+1)));
					it = ret.first;
				}
				else {
					return 0;
				}
			}
			return it->second;
		}

		//wrzuca wsk na kandydata do wektora
		void insertCandidate(Candidate* p_candidate)
		{
			candidates.push_back(p_candidate);
		}


		//do testow rekurencyjne drukowanie wezlow
		void print()
		{
			vector<int>* attributes;
			cout << "\r\n" << "level: " << level << "\r\n";
			cout << "num of children: " << children.size() << " " << "\r\n";
			cout << "candidates:" << "\r\n";
			for (int i=0; i<candidates.size(); i++)
			{
				cout << "candidate " << i << ": [";
				attributes = candidates[i]->getAttributes();
				for(int j=0; j<attributes->size();j++)
				{
					cout << attributes->at(j);
				}
				cout << "]";
			}
			map<int,HashTreeNode*>::iterator it;
			for ( it = children.begin(); it!=children.end();it++)
			{
				it->second->print();
			}

		}

		void countCandidatesSupport(vector<int>* p_subset, int tClass)
		{
			for(int i=0; i<candidates.size();i++)
			{
				if(candidates[i]->attributesEquals(p_subset)) //jezeli kandydat == podzbior
				{
					candidates[i]->incrementSupport(tClass);
					candidates[i]->incrementTotalSupportCounterAllClasses();
				}
			}
		}

};

#endif
