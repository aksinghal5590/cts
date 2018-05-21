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

    istringstream iss(argv[1]);
    if (!(iss >> B)) {
        cerr << "Invalid number: " << argv[1] << endl;
        return -1;
    }

    int n, m, elemCount;
    cin >> n >> m >> elemCount;
    n = (n >= m) ? n : m;
    Coo *mat1 = new Coo[elemCount];

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

    int size = 0;
    int ceil2 = 2;
    while(1) {
        if(n <= ceil2) {
            size = ceil2;
            break;
        }
        ceil2 *= 2;
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
    treeY.createCTS(mat1, elemCount, base);
    cout << "treeX node count = " << treeX.getTree().size() << endl;
    cout << "treeY node count = " << treeY.getTree().size() << endl;

    auto start = std::chrono::system_clock::now();

    ::treeZ.multiply(treeX.getTree(), treeY.getTree());

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli>total_ms = end - start;

    cout << "treeZ node count = " << ::treeZ.getTree().size() << endl;

    cout << "Time taken by base case merge and multiply = " << mmTime << " ms" << endl;
    cout << "Size = " << size
    << " Base = " << B
    << " Total time taken: " << total_ms.count() << " ms" << endl;

    delete[] mat1;
    return 0;
}