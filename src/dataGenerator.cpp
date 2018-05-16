#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <vector>

#include "cts.hpp"

using namespace std;

extern int B;
extern double mmTime;
int main(int argc, char *argv[]) {

	int size;
	int factor;

	istringstream iss1(argv[1]);
    if (!(iss1 >> size)) {
        cerr << "Invalid number: " << argv[1] << endl;
        return -1;
    }
    istringstream iss2(argv[2]);
    if (!(iss2 >> factor)) {
        cerr << "Invalid number: " << argv[2] << endl;
        return -1;
    }

    cout << size << " " << size << " " << factor << endl;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < factor; j++) {
        	cout << i << " " << (rand() % size) << " " << 1 << endl;
        }
    }

    for(int i = 0; i < size; i++) {
        for(int j = 0; j < factor; j++) {
        	cout << i << " " << (rand() % size) << " " << 1 << endl;
        }
    }

    return 0;
}