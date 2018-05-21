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

int B;
double mmTime;
Sptree treeZ;
int main(int argc, char *argv[]) {

    double fraction;
    istringstream iss1(argv[1]);
    if (!(iss1 >> fraction)) {
        cerr << "Invalid number: " << argv[1] << endl;
        return -1;
    }

    int n, m, elemCount;
    cin >> n >> m >> elemCount;
    n = (n >= m) ? n : m;
    int factor = (int)((double)n * fraction);
        int size = 0;
    int ceil2 = 2;
    while(1) {
        if(n <= ceil2) {
            size = ceil2;
            break;
        }
        ceil2 *= 2;
    }

    int x, y;
    double val;
    vector<Mtx> inList;
    for(int i = 0; i < elemCount; i++) {
        cin >> x >> y >> val;
        inList.emplace_back(Mtx(x, y, val));
    }
    sort(inList.begin(), inList.end());
    Coo *mat1 = new Coo[elemCount];
    for(int i = 0; i < inList.size(); i++) {
        mat1[i].x = inList[i].x - 1;
        mat1[i].y = inList[i].y - 1;
        mat1[i].val = inList[i].val;
    }

    vector<Mtx> list;
    for(int i = 0; i < n * factor; i++) {
        list.emplace_back(Mtx(rand() % size, rand() % size, (rand() % 200) - 100));
    }
    sort(list.begin(), list.end());
    for(int i = 0; i < list.size() - 1; i++) {
        if((list[i].x == list[i+1].x) && (list[i].y == list[i+1].y)) {
            list.erase(list.begin() + i + 1);
        }
    }
    Coo *mat2 = new Coo[list.size()];
    for(int i = 0; i < list.size(); i++) {
        mat2[i].x = list[i].x;
        mat2[i].y = list[i].y;
        mat2[i].val = list[i].val;
    }
    double density = (double)elemCount/((double)n*n);
    if(density < 0.0005) {
        B = size/64;
    } else if(density > 0.0005 && density < 0.005) {
        B = size/256;
    } else {
        B = size/512;
    }
    B = (B <= 128 ? 128 : B);
    Base base(0, 0, size);

    Sptree treeX;
    Sptree treeY;

    treeX.createCTS(mat1, elemCount, base);
    treeY.createCTS(mat2, list.size(), base);

    cout << "treeX node count = " << treeX.getTree().size() << endl;
    cout << "treeY node count = " << treeY.getTree().size() << endl;

    long_long counters[3] = {0};
    int PAPI_events[] = {PAPI_L1_TCM, PAPI_L2_TCM, PAPI_L3_TCM};
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_start_counters(PAPI_events, 3);

    ::treeZ.multiply(treeX.getTree(), treeY.getTree());

    PAPI_read_counters(counters, 3);

    cout << "treeZ node count = " << ::treeZ.getTree().size() << endl;

    cout << "Size = " << size
    << " Base = " << B
    << " L1 misses = " << counters[0]
    << " L2 misses = " << counters[1]
    << " L3 misses = " << counters[2] << endl;

    delete[] mat1;
    delete[] mat2;
    return 0;
}