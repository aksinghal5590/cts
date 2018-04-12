#include <iostream>
#include <papi.h>
#include <sstream>

#include "cts.hpp"

using namespace std;

int main(int argc, char* argv[]) {

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

    long_long counters[3] = {0};
    int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_start_counters(PAPI_events, 3);

    treeX.createCTS(mat1, size*factor, baseX);

    Sptree treeZ;

    treeZ.multiply(treeX.getTree(), treeX.getTree());

    PAPI_read_counters(counters, 3);

    cout << "Size = " << size
    << " L1 misses = " << counters[0]
    << " L2 misses = " << counters[1]
    << " L3 misses = " << counters[2] << endl;

    delete[] mat1;
    return 0;
}