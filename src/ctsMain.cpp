#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <vector>

#include "cts.hpp"

using namespace std;

Sptree treeZ;
int main(int argc, char *argv[]) {

    int size = 1024;
    istringstream iss1(argv[1]);
    if (!(iss1 >> size)) {
        cerr << "Invalid number: " << argv[1] << endl;
        return -1;
    }
    int factor = size;

    Coo *mat1 = (Coo*) malloc(size * factor * sizeof(Coo));
    int k = 0;
    int a = 1;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < factor; j++) {
            mat1[k].x = i;
            mat1[k].y = j;
            mat1[k].val = a++;
            k++;
        }
        a = 1;
    }
    Sptree treeX;

    Base baseX(0, 0, size);

    auto start = std::chrono::system_clock::now();
    treeX.createCTS(mat1, size*factor, baseX);

    ::treeZ.multiply(treeX.getTree(), treeX.getTree());

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double>elapsed_seconds = end - start;
    cout << "Size = " << size
    << "Time taken: " << elapsed_seconds.count() << "s" << endl;

    delete[] mat1;
    return 0;
}