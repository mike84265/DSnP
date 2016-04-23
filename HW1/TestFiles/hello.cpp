#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
int main()
{
	ifstream fin("input4.csv");
	fin.seekg(0,fin.end);
	int length = fin.tellg();
	fin.seekg(0,fin.beg);
	char* buffer = new char[length];
	fin.read(buffer,length);
	cout << buffer << endl;
	for (int i=0;i<length;i++) cout << (int)buffer[i] << ", ";
	/*int count=1;
	char* tok = new char[20];
	tok = strtok(buffer, "\n");
	cout << tok << endl;
	while (tok!=NULL)
	{
		tok = strtok(NULL, "\n");
		cout << tok << endl;
		count++;
		
	}
	cout << "count = " << count << endl;
	*/
}