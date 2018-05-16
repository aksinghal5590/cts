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


void mmIKJ(int *mZ, int row0, int col0, int *mX, int row1, int col1, int *mY,
       int row2, int col2, int n, int size) {
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            for (int j = 0; j < n; ++j) {
                mZ[(i + row0) * size + col0 + j] += mX[(row1 + i) * size + col1 + k] * mY[(row2 + k) * size + col2 + j];
            }
        }
    }
}

void printMatrix(int *m, int row, int col) {
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            cout << m[i * col + j] << " ";
        }
        cout << endl;
    }
}

int main(int argc, char *argv[]) {

    int size;
    int factor;

    istringstream iss2(argv[1]);
    if (!(iss2 >> B)) {
        cerr << "Invalid number: " << argv[1] << endl;
        return -1;
    }

    cin >> size >> size >> factor;

    int *mX = new int[size*size]();
    int *mY = new int[size*size]();
    int *mZ = new int[size*size]();
    Coo *mat1 = (Coo*) malloc(size * factor * sizeof(Coo));
    Coo *mat2 = (Coo*) malloc(size * factor * sizeof(Coo));
    vector<Mtx> list1;
    vector<Mtx> list2;

    int x, y;
    double val;
    for(int i = 0; i < size*factor; i++) {
        cin >> x >> y >> val;
        list1.emplace_back(Mtx(x, y ,val));
    }
    for(int i = 0; i < size*factor; i++) {
        cin >> x >> y >> val;
        list2.emplace_back(Mtx(x, y ,val));
    }
    sort(list1.begin(), list1.end());
    sort(list2.begin(), list2.end());

    for(int i = 0; i < size * factor; i++) {
        mat1[i].x = list1[i].x;
        mat1[i].y = list1[i].y;
        mat1[i].val = list1[i].val;
        mX[mat1[i].x * size + mat1[i].y] = mat1[i].val;
    }
    for(int i = 0; i < size * factor; i++) {
        mat2[i].x = list2[i].x;
        mat2[i].y = list2[i].y;
        mat2[i].val = list2[i].val;
        mY[mat2[i].x * size + mat2[i].y] = mat2[i].val;
    }
    Sptree treeX;
    Sptree treeY;

    Base baseX(0, 0, size);

    auto start = std::chrono::system_clock::now();

    treeX.createCTS(mat1, size*factor, baseX);
    treeY.createCTS(mat2, size*factor, baseX);

    cout << "treeX node count = " << treeX.getTree().size() << endl;
    cout << "treeY node count = " << treeY.getTree().size() << endl;
    //treeX.printValues();
    //treeY.printValues();

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double, std::milli>create_ms = end - start;
    cout << " Time taken in tree creation: " << create_ms.count() << " ms" << endl;

    ::treeZ.multiply(treeX.getTree(), treeY.getTree());

    cout << "treeZ node count = " << ::treeZ.getTree().size() << endl;
    //::treeZ.printValues();

    end = std::chrono::system_clock::now();
    std::chrono::duration<double>total_ms = end - start;
    cout << "Size = " << size
    << " Base = " << B
    << " Total Time taken: " << total_ms.count() << " ms" << endl;

    cout << "Multiplication time: " << mmTime << " ms" << endl;

    int *mT = new int[size*size]();
    /*for(int i = 0; i < ::treeZ.getTree().size(); i++) {
        Node node = ::treeZ.getTree()[i];
        if(node.base.len <= B) {
            int row = node.base.x;
            int col = node.base.y;
            for(int k = 0; k < B; k++) {
                for(int l = 0; l < B; l++) {
                     mT[(row + k)*size + col + l] = (int)node.val[k*B + l];
                }
            }
        }
    }
    mmIKJ(mZ, 0, 0, mX, 0, 0, mY, 0, 0, size, size);
    //cout << endl << endl;
    //printMatrix(mZ, size, size);
    //cout << endl << endl;
    //printMatrix(mT, size, size);

    int resultCount = 0;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if(mZ[i*size + j] > 0) {
                resultCount++;
            }
        }
    }
    Coo *mat3 = (Coo*) malloc(resultCount * sizeof(Coo));
    int k = 0;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if(mZ[i*size + j] > 0) {
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
    delete[] mat3;*/

    delete[] mat1;
    delete[] mat2;
    delete[] mX;
    delete[] mY;
    delete[] mZ;
    delete[] mT;
    return 0;
}