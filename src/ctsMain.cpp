#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <set>
#include <sstream>
#include <type_traits>
#include <vector>

#include "cts.hpp"

using namespace std;

int B;
int ORTH = 4;
double mmTime = 0;

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

    vector<Mtx> dupList, list;
    for(int i = 0; i < n * factor; i++) {
        dupList.emplace_back(Mtx(rand() % size, rand() % size, (rand() % 200) - 100));
    }
    sort(dupList.begin(), dupList.end());
    set<int> remove;
    for(int i = 1; i < dupList.size(); i++) {
        if((dupList[i].x == dupList[i-1].x) && (dupList[i].y == dupList[i-1].y)) {
            remove.insert(i);
        }
    }
    for(int i = 0; i < dupList.size(); i++) {
        if(remove.find(i) != remove.end()) {
        } else {
            list.emplace_back(dupList[i]);
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
        B = size/32;
    } else if(density > 0.0005 && density < 0.005) {
        B = size/64;
    } else {
        B = size/128;
    }
    B = (B <= 128 ? 128 : B);
    Base baseX(0, 0, size);

    Sptree treeX, treeY, treeZ;
    treeX.createCTS(mat1, elemCount, baseX);
    treeY.createCTS(mat2, list.size(), baseX);

    cout << "treeX node count = " << treeX.getTree().size() << endl;
    cout << "treeY node count = " << treeY.getTree().size() << endl;

    auto start = std::chrono::system_clock::now();

    treeZ.multiply(treeX, treeY, baseX);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli>total_ms = end - start;

    cout << "treeZ node count = " << treeZ.getTree().size() << endl;

    //cout << "Time taken by assembly: " << mmTime << " ms" << endl;
    cout << "Size = " << size
    << " Base = " << B
    << " Total Time taken: " << total_ms.count() << " ms" << endl;

    delete[] mat1;
    delete[] mat2;
    return 0;
}