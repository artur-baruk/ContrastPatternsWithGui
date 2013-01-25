#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>

using namespace std;

enum Method
{
	Orlando,
	Agrawal,
	Michal2
};

enum OptimizationType {
	No,
	Generators,
	Closures
};

enum CounterType {
	AllClasses,
	Binary
};




class Configuration {
  
	private:
		
		string training_data_path;
		string data_description_path;
		string objects_to_clsf_path;
		OptimizationType optimizationType;
		Method method;
		CounterType counterType;

	public:
		Configuration() 
		{
			method = Agrawal;
			optimizationType = No;
			counterType = AllClasses;

		}

		string getTrainingDataPath() {return training_data_path;}

		string getDataDescriptionPath() {return data_description_path;}

		string getObjectsToClsfPath() {return objects_to_clsf_path;}

		void setTrainingDataPath(string p_str) {
				training_data_path = p_str;
		}

		void setDataDescriptionPath(string p_str) {
			data_description_path = p_str;
		}

		void setObjectsToClsfPath(string p_str) {
			objects_to_clsf_path = p_str;
		}

		void setOptimizationType(OptimizationType p_optimizationType) {optimizationType=p_optimizationType;}

		void setMethod(Method p_method) {method=p_method;}

		void setCounterType(CounterType p_counter_type) {counterType = p_counter_type;}

		OptimizationType getOptimizationType() {return optimizationType;}

		Method getMethod() {return method;}

		CounterType getCounterType() {return counterType;}


		

};

#endif
