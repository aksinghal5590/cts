#include <algorithm>
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

    vector<Mtx> list1;
    vector<Mtx> list2;

    for(int i = 0; i < size * factor; i++) {
        list1.emplace_back(Mtx(rand() % size, rand() % size, (rand() % 200) - 100));
    }

    for(int i = 0; i < size * factor; i++) {
        list2.emplace_back(Mtx(rand() % size, rand() % size, (rand() % 200) - 100));
    }
    sort(list1.begin(), list1.end());
    sort(list2.begin(), list2.end());
    for(int i = 0; i < list1.size() - 1; i++) {
        if((list1[i].x == list1[i+1].x) && (list1[i].y == list1[i+1].y)) {
            list1.erase(list1.begin() + i + 1);
        }
    }
    for(int i = 0; i < list2.size() - 1; i++) {
        if((list2[i].x == list2[i+1].x) && (list2[i].y == list2[i+1].y)) {
            list2.erase(list2.begin() + i + 1);
        }
    }
    cout << size << " " << size << " " << list1.size() << endl;
    for(int i = 0; i < list1.size(); i++) {
        cout << list1[i].x << " " << list1[i].y << " " << list1[i].val << endl;
    }
    cout << size << " " << size << " " << list2.size() << endl;
    for(int i = 0; i < list2.size(); i++) {
        cout << list2[i].x << " " << list2[i].y << " " << list2[i].val << endl;
    }

    return 0;
}