#include <iostream>
#include <vector>
#include <fstream>



#include "util/testData.h"

void selectSort(std::vector<int> &a) {
	for(unsigned int i = 0; i < a.size(); i++) {
		int min_index = i;
		for(unsigned int j = i+1; j < a.size(); j++) {
			if(a[j] < a[min_index]) {
				min_index = j;
			}
		}
		int tmp = a[i];
		a[i] = a[min_index];
		a[min_index] = tmp;
	}
}

int main() {
    int N=2000;
/*	TestData<int> ta(N);
	std::cout << "hello fang." <<std::endl;

	int * p = new int[N];
	for(int i = 0; i < N; i++) {
		p[i] = (int)(2*i + 1);
	}
	ta.readFromBuf(p);
	ta.show("readFromBuf");
	ta.generateSequence(2,1);
	ta.show("generateSequence");
	ta.generateRandom(-100,100);
	ta.show("generateRandom");

	ta.writeToBuf(p);
	for(int i = 0; i < N; i++) {
		std::cout << p[i] << " ";
	}
	std::cout << std::endl;

	ta.writeToDisk("./abc.txt");
	ta.readFromDisk("./abc.txt");
	ta.show("readFromDisk");

	delete p;*/

	TestData<int> ta(20);
	ta.readFromDisk("./abc.txt");
	ta.show();

	std::vector<int> abc;
	ta.writeToBuf(abc);
	selectSort(abc);

	ta.readFromBuf(abc);
	ta.show();
	return 0;
}
