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
double mmTime;
int zeros = 0;

void mmIKJ(double *mZ, int row0, int col0, double *mX, int row1, int col1, double *mY, int row2, int col2, int n, int size) {
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            for (int j = 0; j < n; ++j) {
                mZ[(i + row0) * size + col0 + j] += mX[(row1 + i) * size + col1 + k] * mY[(row2 + k) * size + col2 + j];
            }
        }
    }
}

void printMatrix(double *m, int row, int col) {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            cout << m[i * col + j] << " ";
        }
        cout << endl;
    }
}

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


    double *mX = new double[size * size]();
    double *mY = new double[size * size]();
    double *mZ = new double[size * size]();

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
        mX[mat1[i].x * size + mat1[i].y] = mat1[i].val;
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
        mY[mat2[i].x * size + mat2[i].y] = mat2[i].val;
    }

    Sptree treeX, treeY, treeZ;
    B = 8;
    Base baseX(0, 0, size);

    treeX.createCTS(mat1, elemCount, baseX);
    treeY.createCTS(mat2, list.size(), baseX);

    cout << "treeX node count = " << treeX.getTree().size() << endl;
    cout << "treeY node count = " << treeY.getTree().size() << endl;

    mmTime = 0;
    auto start = std::chrono::system_clock::now();

    treeZ.multiply(treeX, treeY, baseX);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli>total_ms = end - start;

    cout << "treeZ node count = " << treeZ.getTree().size() << endl;

    //cout << "Time taken by assembly: " << mmTime << " ms" << endl;
    cout << "Size = " << size
    << " Base = " << B
    << " Total Time taken: " << total_ms.count() << " ms" << endl;

    double *mT = new double[size*size]();
    for(int i = 0; i < treeZ.getTree().size(); i++) {
        Node node = treeZ.getTree()[i];
        if(node.base.len <= B) {
            int row = node.base.x;
            int col = node.base.y;
            for(int k = 0; k < B; k++) {
                for(int l = node.csr.iCount[k]; l < node.csr.iCount[k+1]; l++) {
                    mT[(row + k)*size + col + node.csr.idx[l]] = node.csr.vals[l];
                }
            }
        }
    }
    cout << "Zeros = " << zeros << endl;
    treeZ.printValues();
    mmIKJ(mZ, 0, 0, mX, 0, 0, mY, 0, 0, size, size);
    cout << endl << endl;
    printMatrix(mZ, size, size);
    cout << endl << endl;
    printMatrix(mT, size, size);

    int resultCount = 0;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if(mZ[i*size + j] != 0) {
                resultCount++;
            }
        }
    }
    Coo *mat3 = (Coo*) malloc(resultCount * sizeof(Coo));
    int k = 0;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if(mZ[i*size + j] != 0) {
                mat3[k].x = i;
                mat3[k].y = j;
                mat3[k].val = mZ[i*size + j];
                k++;
            }
        }
    }
    Sptree treeTest;
    treeTest.createCTS(mat3, resultCount, baseX);
    treeTest.printValues();
    delete[] mat3;

    delete[] mat1;
    delete[] mat2;
    delete[] mX;
    delete[] mY;
    delete[] mZ;
    delete[] mT;
    return 0;
}