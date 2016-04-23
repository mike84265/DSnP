#include <iostream>
using namespace std;
template <class T>		//or template <typename T>
T maximum (T val1, T val2, T val3)
{
	T max = val1;
	if (val2 > max) max = val2;
	if (val3 > max) max = val3;
	return max;
}
int main()
{
	int i1, i2, i3;
	cout << "Input 3 integers: " ;
	cin >> i1 >> i2 >> i3;
	cout << "The maximum integer value is " << maximum(i1,i2,i3) << endl;
	
	double d1, d2, d3;
	cout << "Input 3 double values: ";
	cin >> d1 >> d2 >> d3;
	cout << "The maximum double value is " << maximum(d1,d2,d3) << endl;
	
	char c1, c2, c3;
	cout << "Input 3 characters: ";
	cin >> c1 >> c2 >> c3;
	cout << "The maximum char value is " << maximum(c1, c2, c3) << endl;
	
}

