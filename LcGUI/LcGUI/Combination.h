#ifndef COMBINATION_H
#define COMBINATION_H



class Combination
{
  public:  
		int n;
		int k;
		int* data;

	Combination(int n, int k)
	{
		this->n = n;
		this->k = k;
		this->data = new int[k];
		for (int i = 0; i < k; ++i)
			this->data[i] = i;
	} 

	~Combination()
	{
		delete [] data;
	}
		
  Combination* Successor()
  {
	if (this->data[0] == this->n - this->k)
	  return new Combination(0, 0);

	Combination* ans = new Combination(this->n, this->k);

	int i;
	for (i = 0; i < this->k; ++i)
	  ans->data[i] = this->data[i];
 
	for (i = this->k - 1; i > 0 && ans->data[i] == this->n - this->k + i; --i);
 
	++ans->data[i];

	for (int j = i; j < this->k - 1; ++j)
	  ans->data[j+1] = ans->data[j] + 1;

	return ans;
  } 

  int Choose(int n, int k)
  {
    if (n < k)
      return 0;

    if (n == k)
      return 1;

    int delta, iMax;

    if (k < n-k) 
    {
      delta = n-k;
      iMax = k;
    }
    else         
    {
      delta = k;
      iMax = n-k;
    }
    int ans = delta + 1;

    for (int i = 2; i <= iMax; ++i)
    {
      ans = (ans * (delta + i)) / i;
    }
    return ans;
  } 
};

#endif
