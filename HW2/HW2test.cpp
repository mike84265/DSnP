#include <iostream>
using namespace std;
int main()
{
	char t[15] = "abcdefghijklmn";
	cout << *(t+6) << endl;
	string s(t+3,t+7);
	cout << s << endl;
	for (int i=0;i<5;i++) cout << (void*)(t+i) << endl;
	for (int i=0;i<5;i++) cout << (char)0;

}

