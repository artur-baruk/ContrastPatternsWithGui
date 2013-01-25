#ifndef TUPLECREATOR_H
#define TUPLECREATOR_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include "stringtools.h"


const float null_value = -1.0f;
 
/**
 * Tuple attribute types
 */
enum Type
{
    AttrCategorical = 0,
    AttrInteger,
    AttrReal,
    AttrClass
};
 
class TupleCreator {
 
    bool tuplesReadOnce;
    std::map<int, std::pair<float, float> > minMaxAttributes;
 
        vector<Type> types;
 
        std::vector<std::string> keys;
        std::vector<std::vector<std::string> >* attrsKeys;
 
        int getKeyIndex(vector<std::string>* vector, const std::string& key)
        {
                for(int i = 0; i < vector->size(); ++i) {
                        if(vector->at(i) == key)
                                return i;
                }
                return -1;
        }
 
        public:
 
        TupleCreator() {
                tuplesReadOnce = false;
        }
 
        ~TupleCreator() {
                if(attrsKeys != NULL)
                        delete attrsKeys;
        }


		std::string getClassName(int class_number) {
			return keys[class_number];
		}

		void delKeys() {
			keys.clear();
		}
 
 
 
        void readDataDescription(string fileName)
        {
                std::string temp;
                int attr;
                std::vector<std::string> strs;
                std::vector<std::string>::iterator it;
                std::ifstream inputFile;
                inputFile.open(fileName.c_str());
 
                getline(inputFile, temp);
                split(temp, ',', strs);
 
                inputFile.close();
 
                for(it = strs.begin(); it != strs.end(); ++it) {
                        attr = strToInt(*it);
                        switch(attr) {
                                case 0:
                                        types.push_back(AttrCategorical);
                                        break;
                                case 1:
                                        types.push_back(AttrInteger);
                                        break;
                                case 2:
                                        types.push_back(AttrReal);
                                        break;
                                case 3:
                                        types.push_back(AttrClass);
                                        break;
                        }
                }
 
                attrsKeys = new std::vector<std::vector<std::string> >(types.size());
        }
 
        /**
         * Reads dataset from csv file
         * Returns class count
         */
        int readTuples(const std::string& fileName, std::vector<Tuple*>* tuples, std::vector<int>* objectsInClassesCount, std::vector<Tuple*>* originalTuples) {
 
        if(originalTuples == NULL)
            originalTuples = tuples;
 
                std::string temp;
                std::ifstream inputFile;
                inputFile.open(fileName.c_str());
                std::set<int> realAttributes;
 
                while(!inputFile.eof())
                {
                        getline(inputFile, temp);
                        std::vector<std::string> strs;
                        split(temp, ',', strs);
                        Tuple* t = new Tuple(strs.size() - 1);
                        int attrCount = 0;
                        int keyIndex;
                        for (int i = 0; i < strs.size(); ++i) {
                                switch(types[i])
                                {
                                        case AttrClass:
                                                keyIndex = getKeyIndex(&keys, strs[i]);
                                                if(keyIndex == -1) {
                                                        keys.push_back(strs[i]);
                                                        t->setTupleClass(keys.size() - 1);
                                                        if(objectsInClassesCount != NULL)
                                                                objectsInClassesCount->push_back(1);
                                                } else {
                                                        t->setTupleClass(keyIndex);
                                                        if(objectsInClassesCount != NULL)
                                                                objectsInClassesCount->at(keyIndex)++;
                                                }
                                                break;
                                        case AttrInteger:
                                                t->setAttribute(attrCount++, strToFloat(strs[i]));
                                                break;
                                        case AttrReal:
                                                realAttributes.insert(attrCount);
                                                t->setAttribute(attrCount++, strToFloat(strs[i]));
                                                break;
                                        case AttrCategorical:
                                                keyIndex = TupleCreator::getKeyIndex(&attrsKeys->at(i), strs[i]);
                                                if(keyIndex == -1) {
                                                        attrsKeys->at(i).push_back(strs[i]);
                                                        t->setAttribute(attrCount++, attrsKeys->at(i).size() - 1);
                                                } else {
                                                        t->setAttribute(attrCount++, keyIndex);
                                                }
                                                break;
                                }
                        }
                        tuples->push_back(t);
                }
 
                if(!realAttributes.empty()) {
 
                        for(std::set<int>::iterator it = realAttributes.begin(); it != realAttributes.end(); ++it) {
                                float max = INT_MIN;
                                float min = INT_MAX;
                                float v;
                                if(!tuplesReadOnce) {
                    for(int i = 0; i < originalTuples->size(); ++i) {
                        v = originalTuples->at(i)->getAttribute(*it);
                        if(v > max)
                            max = v;
                        if(v < min)
                            min = v;
                    }
                    minMaxAttributes[*it] = std::pair<float, float>(min, max);
                                } else {
                    min = minMaxAttributes[*it].first;
                    max = minMaxAttributes[*it].second;
                                }
                                std::cout << "min: " << min << std::endl;
                                std::cout << "max: " << max << std::endl;
                                float interval = max - min;
                                //TODO loop
                                if(interval > 100) {
                                        //do zakresu 0:100
                                        float part = interval / 100;
                                        for(int i = 0; i < tuples->size(); ++i) {
                                                v = tuples->at(i)->getAttribute(*it);
                                                v -= min;
                                                v /= part;
                                                tuples->at(i)->setAttribute(*it, v);
                                        }
                                } else if(min < 0) {
                                        //podnosimy wartosci o -min
                                        for(int i = 0; i < tuples->size(); ++i) {
                                                v = tuples->at(i)->getAttribute(*it);
                                                tuples->at(i)->setAttribute(*it, v - min);
                                        }
                                } else {
                                        //przedzial jest mniejszy niz 101 i sa to liczby dodatanie - nic nie robimy
                                }
 
                        }
 
                }
 
                inputFile.close();
 
                tuplesReadOnce = true;
 
                return keys.size();
        }
 
        void getReducedTable(std::vector<Tuple*>* table, Tuple* tuple) {
                int tupleAttributesSize = tuple->getAttributes()->size();
                for (int i = 0; i < table->size(); i++) {
                        for (int j = 0; j < tupleAttributesSize; j++) {
                                if((*tuple->getAttributes())[j] != (*(*table)[i]->getAttributes())[j]) {
                                        (*table)[i]->setAttribute(j, null_value);
                                }
                        }
                }
        }
 
 
        void getTuplesToBeClsf(const string& fileName, std::vector<Tuple*>* tuples_to_be_clsf, std::vector<Tuple*>* originalTuples)
        {
 
                readTuples(fileName, tuples_to_be_clsf, NULL, originalTuples);
                /*for (int i = 0; i < tuples_to_be_clsf->size(); i++) {
                        tuples_to_be_clsf->at(i).setTupleClass(-1);
                }*/
        }
};
 
 
#endif