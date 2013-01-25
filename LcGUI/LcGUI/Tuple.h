#ifndef TUPLE_H
#define TUPLE_H

#include "HashTree.h"
#include "FixedHashTree.h"



/**
 * Wrapper normalizing string data into transactinal (numbers instead of words).
 * Value 0.0 in a vector with attributes means not specified (null).
 * Value 0 of class field means that class is not yet specified.
 */
class Tuple {
	private:
		std::vector<float>* attributes;		//nominal attributes like Temperature{Low=1.0, Mild=2.0, High=3.0}
		int tupleClass;					//number of a class

	public:

        /**
         * Constructor initializing all attributes with -1
         */
		Tuple(int attrsCount, int tClass = -1) : tupleClass(tClass) {
		    attributes = new std::vector<float>(attrsCount);
		    for(int i = 0; i < attrsCount; ++i) {
		        (*attributes)[i] = -1;
		    }
		}

		~Tuple() {
		    if(attributes != NULL) {
                delete attributes;
		    }
		}

		int getTupleClass() { return tupleClass; }

		void setTupleClass(int tClass) { tupleClass = tClass; }

		std::vector<float>* getAttributes() { return attributes; }

		void setAttribute(int index, float value) { (*attributes)[index] = value; }

		float getAttribute(int index) { return (*attributes)[index]; }



		// generowanie podzbiorow k-elementowych z transakcji
		std::vector<std::vector<int>*>* getSubSets(int k)
		{
			std::vector<std::vector<int>*>* subsets = new std::vector<std::vector<int>*>();
			std::vector<int>* attributes_dense = new std::vector<int>();
			std::vector<int>* subset;

			for(int i=0; i<attributes->size(); i++)
			{
				if(attributes->at(i) !=  -1)
				{
					attributes_dense->push_back(i+1);
				}
			}

			int num_of_attributes = attributes_dense->size(); //ile elementow ma transakcja
			int subset_size = k;

			if (num_of_attributes < subset_size)
				return subsets;

			Combination* c = new Combination(num_of_attributes,subset_size);
			int num_of_subsets = c->Choose(num_of_attributes,subset_size);

			for (int i = 0 ; i < num_of_subsets; ++i)
			{
				subset = new std::vector<int>();
				for(int j = 0; j<subset_size; j++)
				{
					subset->push_back(attributes_dense->at(c->data[j]));
				}
				subsets->push_back(subset);
				c = c->Successor();
			}
			delete c;
			delete attributes_dense;
			return subsets;
		}

		// generowanie podzbiorow k-elementowych z transakcji
		std::vector<int>* getAttrDense()
		{
			std::vector<int>* attributes_dense = new std::vector<int>();

			for(int i=0; i<attributes->size(); i++)
			{
				if(attributes->at(i) !=  -1)
				{
					attributes_dense->push_back(i+1);
				}
			}
			return attributes_dense;
		}

		//bierze jedna transakcje, rozbija na podzbiory i jedzie każdym podzbiorem po drzewie az do liscia.
		//w lisciu sprawdza czy jest kandydat rowny temu podzbiorowi. Jezeli jest to podbija wsparcie kandydata o 1
		//przed uzyciem tej funkcji trzeba zbudowac drzewo k-elementowych kandydatow. HashTree* ht = new HashTree(candidates,k);
		void subset_and_count(HashTree* tree)
		{
			int k = tree->getMaxLevel();
			std::vector<std::vector<int>*>* subsets = getSubSets(k);
			for(int i=0; i<subsets->size();i++)
			{
				tree->countSupport(subsets->at(i),tupleClass);
			}
			for(int i = 0; i < subsets->size(); i++) {
				delete subsets->at(i);
			}
			subsets->clear();
			delete subsets;
		}

		void countSupportFromAttrDense(FixedHashTree::HashTree* tree)
		{
			std::vector<int>* attrDense = getAttrDense();
            tree->countSupport(attrDense, tupleClass);
            delete(attrDense);
		}

		void countCompactSupportFromAttrDense(vector<int>* compactSupportOfClassesCounter, FixedHashTree::HashTree* tree)
		{
			std::vector<int>* attrDense = getAttrDense();
            tree->countCompactSupport(compactSupportOfClassesCounter, attrDense, tupleClass);
            delete(attrDense);
		}

};

#endif
