#include <iostream>
#include <papi.h>

#include "cts.hpp"

using namespace std;

int main() {

    int size1 = 1024;
    int factor1 = 1024;
    int size2 = 1024;
    int factor2 = 1024;

    Coo *mat1 = new Coo[size1*factor1];
    Coo *mat2 = new Coo[size2*factor2];

    Sptree treeX, treeY;
    Base baseX = {0, 0, size1};
    Base baseY = {0, 0, size2};

    long_long counters[2] = {0};
    int PAPI_events[] = {PAPI_L2_TCM, PAPI_L2_TCA};
    //int PAPI_events[] = {PAPI_L1_TCM, PAPI_L1_TCA, PAPI_L2_TCM, PAPI_L2_TCA, PAPI_L3_TCM, PAPI_L3_TCA};

    PAPI_library_init(PAPI_VER_CURRENT);

    int k = 0;
    int a = 1;
    for(int i = 0; i < factor1; i++) {
        for(int j = 0; j < size1; j++) {
            mat1[k].x = i;
            mat1[k].y = j;
            mat1[k].val = a;
            k++;
        }
    }
    k = 0;
    for(int i = 0; i < factor2; i++) {
        for(int j = 0; j < size2; j++) {
            mat2[k].x = i;
            mat2[k].y = j;
            mat2[k].val = a;
            k++;
        }
    }

    PAPI_start_counters(PAPI_events, 2);

    treeX.createCTS(mat1, size1*factor1, baseX);
    treeY.createCTS(mat2, size2*factor2, baseY);

    //treeX.printValues();
    //treeY.printValues();

    Sptree treeZ;
    //treeZ.merge(treeX.getTree(), treeY.getTree());

    treeZ.multiply(treeX.getTree(), treeY.getTree());

    PAPI_read_counters(counters, 2);

    cout << "Cache Misses:" << endl
    << " L2 : " << "Misses = " << counters[2] << " Accesses = " << counters[3] << endl;

    //treeZ.printValues();
    delete[] mat1;
    delete[] mat2;
    return 0;
}