//DSnP HW1.  Aug. 2015
//B03901078 CYT
#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstring> 
#include <algorithm>
#include <string>
using namespace std;
class Data 
{
public:
	const int operator[] (size_t i) const;
	int& operator[] (size_t i);
	void setCol(size_t col) {_cols = new int [col];}
	void print();
	Data(const int*);
	Data();
private:
	int *_cols;
};

class SortData
{
public:
	bool operator() (const Data&, const Data&);
	SortData(vector<size_t> InVec) : sortOrder(InVec) {}
private:
	vector<size_t> sortOrder;
};

int stoi (const char* arr, int beg=-1, int end=-1);
size_t rows, columns;
int main()
{
	char filename[50];
	cout << "Please enter the file name: " ;
	cin.getline(filename,50);
	ifstream fin(filename);
		
	//Get rows and columns for the table:
	cout << "Please enter the numbers of rows and columns: ";
	cin >> rows >> columns;
	vector<Data> table(rows);
	for (int i=0;i<rows;i++) table[i].setCol(columns);
	
	//Reading the data from the file:
	fin.seekg(0,fin.end);
	int length = fin.tellg();
	fin.seekg(0,fin.beg);
	char* buffer = new char[length];
	fin.read(buffer,length);
	fin.close();
	
	//Analyze and read the data from the buffer:
	size_t m=0, n=0;
	const size_t size = (columns*5) + 6;
	size_t pos=0,len=0;
	for (m=0;m<rows;m++)
	{
		while (buffer[pos+len]!='\r' && buffer[pos+len]!='\n' && buffer[pos+len]!='\0') len++;
		char* substr = new char[len+1];
		for (size_t i=0;i<len+1;i++) substr[i] = buffer[pos+i];
		substr[len] = ',';
		int index=0, beg=0, end=0;
		for (n=0;n<columns;n++)
		{
			while (substr[index] != ',') index++;
			end = index-1;
			if (beg<=end) table[m][n] = stoi(substr, beg, end);
			else table[m][n] = 200;
			beg = index+1;
			index++;
		}
		pos = pos+len+1;
		len = 0;
	}//end of for
	//end of reading file.
	
	//successful message
	printf("File \"%s\" was read in successfully.\n", filename);
	cin.ignore();		//flush the buffer.
	
	while(1)
	{
		//Reading the commands:
		char* line = new char[size];
		cout << "Input command: ";
		cin.getline(line,size);
		
		if (strlen(line)==0) 
		{
			cout << "input is NULL\n";
			continue;
		}	//Error input handling.
		
		char* command = strtok(line," ");
		if (strcmp(command,"PRINT")==0)		//using strcmp function rather than the operator==
		{
			for (int i=0;i<table.size();i++) table[i].print();
		}
		
		else if (strcmp(command,"ADD")==0)
		{
			int* NewRow = new int [columns];	//Read the values into an integer array first.
			char* temp = new char[10];
			for (int i=0;i<columns;i++) 
			{
				temp = strtok(NULL," ");
				if (strcmp(temp,"-")==0) NewRow[i] = 200;
				else NewRow[i] = stoi(temp);
			}
			table.push_back(Data(NewRow));
			rows++;
		}
			
		else if (strcmp(command,"SUM")==0 || strcmp(command, "AVE") ==0)	//Similar operation, handle them together.
		{
			int sum=0;
			m = stoi(strtok(NULL," "));
			size_t count=0;
			for (int i=0;i<rows;i++)
			{
				if (table[i][m] == 200) continue;
				else 
				{
					sum+=table[i][m];
					count++;
				}
			}
			if (strcmp(command, "SUM")==0) printf("The summation of data in column #%zu is %d.\n",m,sum);
			else
			{
				double avg = (double)sum/count;
				printf("The average of integers in column#%zu is %f.\n",m,avg);
			}
		}
			
		else if (strcmp(command,"MAX")==0)
		{
			m = stoi(strtok(NULL," "));
			int max = -200;		
			for (int i=0;i<rows;i++)
			{
				if (table[i][m] > max && table[i][m] != 200) max = table[i][m];
			}
			printf("The max value in column#%zu is %d.\n", m, max);
		}
			
		else if (strcmp(command,"MIN")==0)
		{
			m = stoi(strtok(NULL," "));
			int min = 150;
			for (int i=0;i<rows;i++)
			{
				if ((table[i][m]<min) && (table[i][m]!=200)) min = table[i][m];
			}
			printf("The min value in column#%zu is %d.\n", m, min);
		}	
		
		else if (strcmp(command,"COUNT")==0)
		{
			m = stoi(strtok(NULL," "));
			
			vector<int> temp(rows);
			for (int i=0;i<rows;i++) temp[i] = table[i][m];
			sort(temp.begin(),temp.end());
			
			size_t count = 1;
			for (int i=1;i<rows;i++)
				if (temp[i] != temp[i-1]) count++;	
			if (temp[rows-1] == 200) count--;	//the blank should not increase the count.
			printf("The distinct count of data in column#%zu is %zu.\n",m,count);
			
		}
		
		else if (strcmp(command,"SORT")==0)
		{
			vector<size_t> order;
			char* frg = strtok(NULL," ");
			while (frg != NULL)
			{
				order.push_back(stoi(frg));
				frg = strtok(NULL," ");
			}
			sort(table.begin(), table.end(), SortData(order));	//using constructor to push the sortOrder.
		}
		
		else if (strcmp(command,"END")==0)
			break;
		
		else
		{
			cout << "The received command is \"" << command << "\", no command is executed." << endl;
		}	
	}
}//end main

const int Data::operator[] (size_t i) const
{
	return _cols[i];
}

int& Data::operator[] (size_t i)
{
	return _cols[i];
}

int stoi(const char* arr, int beg, int end)
{
	if (beg == -1 && end == -1) 		//Default value
	{
		beg = 0;
		end = strlen(arr)-1;
	}
	int num_i = 0;
	int index=end;
	int base=1;
	if (arr[beg] == '-')
	{
		while (index>=beg+1)
		{
			num_i -= (int)(arr[index] - '0') * base;
			base*=10;
			index--;
		}
	}
	else
	{
		while (index>=beg)
		{
			num_i += (int)(arr[index] - '0') * base;
			base*=10;
			index--;
		}
	}
	return num_i;
}

void Data::print()
{
	for (int i=0;i<columns;i++)
	{
		if (_cols[i] == 200) cout << setw(4) << right << " ";
		else cout << setw(4) << right << _cols[i];
	}
	cout << endl;
}

Data::Data()
{
	_cols = NULL;
}

Data::Data(const int* arr)
{
	_cols = new int [columns];
	for (int i=0;i<columns;i++) _cols[i] = arr[i];
}

bool SortData::operator() (const Data& d1, const Data& d2)
{
	size_t od = 0;
	while (d1[sortOrder[od]] == d2[sortOrder[od]] && od<sortOrder.size()-1 ) od++;
	return (d1[sortOrder[od]] < d2[sortOrder[od]]);
}
