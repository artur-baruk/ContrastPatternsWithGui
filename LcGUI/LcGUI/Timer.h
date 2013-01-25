#ifndef TIMER_H
#define TIMER_H

#include <ctime>
#include <string>



using namespace std;

class Timer {
  
	private:
		clock_t begin;
		clock_t end;
		string subject;

	public:

		Timer() {}
		
		void start(string str) 
		{
			this->subject=str;
			this->begin = clock();
		}

		void stop()
		{
			this->end = clock();
			double elapsed_secs = double(this->end - this->begin) / CLOCKS_PER_SEC;
			cout << "#TIME: " << this->subject << " = " << elapsed_secs << "\r\n";
		}

};

#endif
