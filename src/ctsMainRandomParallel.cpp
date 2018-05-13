#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <vector>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#include "cts.hpp"

using namespace std;

Sptree treeZ;
int main(int argc, char *argv[]) {

    __cilkrts_set_param("nworkers", argv[2]);

    double fraction;
    istringstream iss2(argv[1]);
    if (!(iss2 >> fraction)) {
        cerr << "Invalid number: " << argv[1] << endl;
        return -1;
    }

    int n, m, elemCount;
    cin >> n >> m >> elemCount;
    n = (n >= m) ? n : m;
    int factor = (int)((double)n * fraction);
    Coo *mat1 = new Coo[elemCount];
    Coo *mat2 = new Coo[n * factor];

    int x, y;
    double val;
    vector<Mtx> inList;
    for(int i = 0; i < elemCount; i++) {
        cin >> x >> y >> val;
        inList.emplace_back(Mtx(x, y, val));
    }
    sort(inList.begin(), inList.end());
    for(int i = 0; i < inList.size(); i++) {
        mat1[i].x = inList[i].x - 1;
        mat1[i].y = inList[i].y - 1;
        mat1[i].val = inList[i].val;
    }

    int p = 0;
    int* col = new int[factor]();
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < factor; j++) {
            col[j] = rand() % n;
        }
        sort(col, col + factor);
        for(int j = 0; j < factor; j++) {
            mat2[p].x = i;
            mat2[p].y = col[j];
            mat2[p].val = (rand() % 200) - 100;
            p++;
        }
    }

    int size = 0;
    int ceil2 = 2;
    while(1) {
        if(n <= ceil2) {
            size = ceil2;
            break;
        }
        ceil2 *= 2;
    }
    Base base(0, 0, size);

    auto start = std::chrono::system_clock::now();

    Sptree treeX;
    Sptree treeY;

    treeX.createCTS(mat1, elemCount, base);
    //treeY.createCTS(mat1, elemCount, base);
    treeY.createCTS(mat2, n * factor, base);

    cout << "treeX size = " << treeX.getTree().size() << endl;
    cout << "treeY size = " << treeY.getTree().size() << endl;

    ::treeZ.multiply(treeX.getTree(), treeY.getTree());

    cout << "treeZ size = " << ::treeZ.getTree().size() << endl;

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double>elapsed_seconds = end - start;
    cout << "Size = " << size
    << " Base = " << B
    << " Time taken by parallel execution: " << elapsed_seconds.count() << "s" << endl;

    delete[] col;
    delete[] mat1;
    delete[] mat2;
    return 0;
}