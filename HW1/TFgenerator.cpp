//Aug. 30, 2015. CYT.
#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;
int main()
{
	cout << "Enter output file name: ";
	char* filename = new char[80];
	cin.getline(filename,80);
	ofstream fout(filename);
	cout << "Enter rows and columns: ";
	int _r, _c;
	cin >> _r >> _c;
	srand(time(NULL));
	int val;
	for (int i=0;i<_r;i++)
	{
		for (int j=0;j<_c-1;j++)
		{
			val = rand()%300-99;
			if (val>100) fout << ",";
			else fout << val << ",";
		}
		
		val = rand()%300-100;
		if (val<=100) fout << val;
		if (i<_r-1) fout << "\r";
	}
}