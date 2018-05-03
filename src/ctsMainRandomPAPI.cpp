#include <cstdlib>
#include <ctime>
#include <iostream>
#include <papi.h>
#include <sstream>

#include "cts.hpp"

using namespace std;

Sptree treeZ;
int main(int argc, char* argv[]) {

    int size = 1024;
    int factor = 100;
    istringstream iss1(argv[1]);
    if (!(iss1 >> size)) {
        cerr << "Invalid number: " << argv[1] << endl;
    }
    istringstream iss2(argv[2]);
    if (!(iss2 >> factor)) {
        cerr << "Invalid number: " << argv[2] << endl;
    }

    int *AX = new int[size*factor]();
    int *IAX = new int[size + 1]();
    int *JAX = new int[size*factor]();
    int *AY = new int[size*factor]();
    int *IAY = new int[size + 1]();
    int *JAY = new int[size*factor]();

    int k = 0;
    int a = 1;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < factor; j++) {
            int col = rand() % size;
            AX[k] = a++;
            JAX[k] = col;
            k++;
        }
        IAX[i+1] = IAX[i] + factor;
        a = 1;
    }
    srand(time(0));
    k = 0;
    a = 1;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < factor; j++) {
            int col = rand() % size;
            AY[k] = a++;
            JAY[k] = col;
            k++;
        }
        IAY[i+1] = IAY[i] + factor;
        a = 1;
    }

    long_long counters[3] = {0};
    int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_start_counters(PAPI_events, 3);

    Coo *mat1 = (Coo*) malloc(size * factor * sizeof(Coo));
    Coo *mat2 = (Coo*) malloc(size * factor * sizeof(Coo));
    k = 0;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < IAX[i+1] - IAX[i]; j++) {
            mat1[k].x = i;
            mat1[k].y = JAX[k];
            mat1[k].val = AX[k];
            k++;
        }
    }
    k = 0;
    srand(time(0));
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < IAY[i+1] - IAY[i]; j++) {
            mat2[k].x = i;
            mat2[k].y = JAY[k];
            mat2[k].val = AY[k];
            k++;
        }
    }

    Sptree treeX;
    Sptree treeY;
    Base base(0, 0, size);
    treeX.createCTS(mat1, size*factor, base);
    treeX.createCTS(mat2, size*factor, base);

    ::treeZ.multiply(treeX.getTree(), treeY.getTree());

    PAPI_read_counters(counters, 3);

    cout << "Size = " << size
    << " L1 misses = " << counters[0]
    << " L2 misses = " << counters[1]
    << " L3 misses = " << counters[2] << endl;

    delete[] AX;
    delete[] IAX;
    delete[] JAX;
    delete[] AY;
    delete[] IAY;
    delete[] JAY;
    delete[] mat1;
    delete[] mat2;
    return 0;
}