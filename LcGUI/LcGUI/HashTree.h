#ifndef HASH_TREE_H
#define HASH_TREE_H

#include "HashTreeNode.h"


using namespace std;

/*
 *
 */

class HashTree {
  private:
		HashTreeNode* root;
		int hash_arg;
		int maxLevel;

	public:
		HashTree(vector<Candidate*>* p_candidates, int k) //k to dlugosc wstawianych kandydatow
		{
			maxLevel = k; // wysokosc drzewa
			hash_arg = k+1; // bedziemy haszowac hash = argument MOD hash_arg

			root = new HashTreeNode(0);

			for ( int i = 0; i < p_candidates->size(); i++ ) {
				insertCandidate((*p_candidates)[i]);
			}
		}

		~HashTree()
		{
			delete root;
		};

		//uzywana w konstruktorze, umieszcza jednego kandydata w odpowiednim lisciu
		void insertCandidate(Candidate* p_candidate)
		{
			HashTreeNode* current_node = root;
			int hash;
			vector<int>* attributes;
			for(int i=0; i<maxLevel; i++)
			{
				attributes = p_candidate->getAttributes();
				hash = (int)attributes->at(i) % hash_arg;
				current_node = current_node->getChild(hash,true);
			}
			current_node->insertCandidate(p_candidate);
		}

		//do testow drukowanie wezlow
		void printHashTree()
		{
			HashTreeNode* current_node = root;
			root->print();
		}

		int getMaxLevel() {
			return maxLevel;
		}

		//bierze podzbior transakcji i jedzie nim do liscia
		//jak w lisciu jest taki sam kandydat jak podzbior transakcji to podbija jego wsparcie
		void countSupport(vector<int>* p_subset, int tClass)
		{
			HashTreeNode* current_node = root;
			int hash;
			for(int i=0; i<maxLevel; i++)
			{
				hash = p_subset->at(i) % hash_arg;
				current_node = current_node->getChild(hash,false);
				if(current_node == 0) {
					return;
				}
			}
			//sprawdzamy i podbijamy supporty
			current_node->countCandidatesSupport(p_subset,tClass);
		}

};

#endif
