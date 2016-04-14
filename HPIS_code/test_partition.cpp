#include "partition.h"

#include <vector>
#include <iostream>
#include <ctime>

using std::vector;
using std::cout;

int main() {
        clock_t t = clock();
	vector<int> mem = { 100, 120, 150, 200, 250, 300 };
        size_t sz = 256;
        vector<int> mem_alloc;
	//cout << optimal(sz, mem, mem_alloc) << "\n";
        for (int i = 0;i < mem_alloc.size(); ++i) {
          cout << mem_alloc[i] << ",";
        }
        cout << "\n";
        mem_alloc.clear();
        cout << "Time taken: " << (clock() - t) << "\n";
        t = clock();
        cout << proportional(sz, mem, mem_alloc) << "\n";
        for (int i = 0;i < mem_alloc.size(); ++i) {
          cout << mem_alloc[i] << ",";
        }
        cout << "\n";
        mem_alloc.clear();
        cout << "Time taken: " << (clock() - t) << "\n";
        t = clock();
        cout << greedy(sz, mem, mem_alloc) << "\n";
        for (int i = 0;i < mem_alloc.size(); ++i) {
          cout << mem_alloc[i] << ",";
        }
        cout << "\n";
        mem_alloc.clear();
        cout << "Time taken: " << (clock() - t) << "\n";
	return 0;
}
