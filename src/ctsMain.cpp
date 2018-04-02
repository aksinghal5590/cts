#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <type_traits>
#include <vector>

#include "cts.hpp"

using namespace std;

Sptree treeZ;
int main() {

    int size1 = 4096;
    int factor1 = 4096;
    int size2 = 4096;
    int factor2 = 4096;

    Coo *mat1 = (Coo*) malloc(size1 * factor1 * sizeof(Coo));
    Coo *mat2 = (Coo*) malloc(size2 * factor2 * sizeof(Coo));
    int k = 0;
    int a = 1;
    for(int i = 0; i < factor1; i++) {
        for(int j = 0; j < size1; j++) {
            mat1[k].x = i;
            mat1[k].y = j;
            mat1[k].val = a++;
            k++;
        }
        a = 1;
    }
    k = 0;
    for(int i = 0; i < factor2; i++) {
        for(int j = 0; j < size2; j++) {
            mat2[k].x = i;
            mat2[k].y = j;
            mat2[k].val = a++;
            k++;
        }
        a = 1;
    }

    Sptree treeX, treeY;

    Base baseX = {0, 0, size1};
    Base baseY = {0, 0, size2};

    auto start = std::chrono::system_clock::now();
    treeX.createCTS(mat1, size1*factor1, baseX);
    treeY.createCTS(mat2, size2*factor2, baseY);

    //treeX.printValues();
    //treeY.printValues();

    //treeZ.merge(treeX.getTree(), treeY.getTree());

    ::treeZ.multiply(treeX.getTree(), treeY.getTree());

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double>elapsed_seconds = end - start;
    cout << "Time taken with size = " << size1 << " : " << elapsed_seconds.count() << "s" << endl;

    //treeZ.printValues();
    ::treeZ.getTree()[7].printValues();

    delete[] mat1;
    delete[] mat2;
    return 0;
}