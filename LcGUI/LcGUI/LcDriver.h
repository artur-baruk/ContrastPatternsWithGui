#ifndef LC_DRIVER_H
#define LC_DRIVER_H

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <map>
#include "Configuration.h"
#include "Timer.h"
#include "LazyClassificator.h"


class LcDriver : public CWinThread {
	private:
		
		

	public:

		Configuration configuration;

		
		void run() 
		{
			vector<Tuple*> table;

			vector<Tuple*> tuples_to_be_classified;

			vector<int> objectsInClassesCount;

			LazyClassificator* lazyClassificator;

			streambuf *psbuf, *backup;
			ofstream filestr;
			filestr.open ("report.txt");
			backup = cout.rdbuf();
			psbuf = filestr.rdbuf();
		    cout.rdbuf(psbuf);

			if(configuration.getDataDescriptionPath()=="" || configuration.getTrainingDataPath()=="" || configuration.getObjectsToClsfPath()=="") {
				cout << "Please select files.";
				filestr.close();
				return;
			}
			cout << "=====================Lazy Classification Report====================" << "\r\n";
			cout << "========================================================" << "\r\n";

			Timer total;
			total.start("Total execution time");

			Timer t;
			t.start("Training data read time");

			TupleCreator tc;

			tc.readDataDescription(configuration.getDataDescriptionPath()); //czytamy plik z formatem danych

			tc.readTuples(configuration.getTrainingDataPath(), &table, &objectsInClassesCount, NULL); // czytamy zbior danych

			t.stop();

			t.start("Objects to be classified read time");

			tc.getTuplesToBeClsf(configuration.getObjectsToClsfPath(),&tuples_to_be_classified, &table); //czytamy plik z obiektami ktore mamy klasyfikowac

			t.stop();

			int decision_class;
			int correctedClassifiedCounter = 0;

			for(int i = 0; i<tuples_to_be_classified.size();i++) 
			{
				cout << "========================================================" << "\r\n";
				cout << "#BEGIN Classification of object no [" << i << "]\r\n";
				cout << "========================================================" << "\r\n";
				if(table.empty())
				{
					t.start("Training data read time");
					tc.readTuples(configuration.getTrainingDataPath(), &table, &objectsInClassesCount, NULL); // czytamy zbior danych
					t.stop();
				}

				t.start("Reduced table creation");
				tc.getReducedTable(&table, tuples_to_be_classified[i]);
				t.stop();
				lazyClassificator = new LazyClassificator(table, configuration.getMethod(), configuration.getOptimizationType(), objectsInClassesCount);
				decision_class = lazyClassificator->executeWithNCounters();
				cout << "========================================================" << "\r\n";
				cout << "Object no [" << i << "] classified to class [" << tc.getClassName(decision_class) << "]\r\n";
				cout << "========================================================" << "\r\n";
				cout << "#END Classification of object no [" << i << "]\r\n";
				cout << "========================================================" << "\r\n";
				if(decision_class == tuples_to_be_classified[i]->getTupleClass()) {
					correctedClassifiedCounter++;
				}
				table.clear();
				objectsInClassesCount.clear();
				tc.delKeys();
			}
			cout << "Classification quality = " << ((double) correctedClassifiedCounter)/((double) tuples_to_be_classified.size()) << "\r\n";
			total.stop();

			filestr.close();

			//cleaning up
			objectsInClassesCount.clear();
			delete lazyClassificator;
		}
};

#endif