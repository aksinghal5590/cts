#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <vector>

#include "cts.hpp"

#define B 64

using namespace std;

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

Sptree treeZ;
int main(int argc, char *argv[]) {

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

    int *mX = new int[size*size]();
    int *mY = new int[size*size]();
    int *mZ = new int[size*size]();
    Coo *mat1 = (Coo*) malloc(size * factor * sizeof(Coo));
    Coo *mat2 = (Coo*) malloc(size * factor * sizeof(Coo));

    int k = 0;
    int a = 1;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < factor; j++) {
            int col = rand() % size;
            mX[i * size + col] = a;
            mat1[k].x = i;
            mat1[k].y = col;
            mat1[k].val = a;
            k++;
            a++;
        }
        a = 1;
    }

    k = 0;
    a = 1;
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < factor; j++) {
            int col = rand() % size;
            mY[i * size + col] = a;
            mat2[k].x = i;
            mat2[k].y = col;
            mat2[k].val = a;
            k++;
            a++;
        }
        a = 1;
    }

    mmIKJ(mZ, 0, 0, mX, 0, 0, mY, 0, 0, size, size);

    Sptree treeX, treeY;
    Base base(0, 0, size);
    treeX.createCTS(mat1, size*factor, base);
    treeY.createCTS(mat2, size*factor, base);

    ::treeZ.multiply(treeX.getTree(), treeY.getTree());

    int *mT = new int[size*size]();
    for(int i = 0; i < ::treeZ.getTree().size(); i++) {
        Node node = ::treeZ.getTree()[i];
        if(node.base.len <= B) {
            int row = node.base.x;
            int col = node.base.y;
            for(int k = 0; k < B; k++) {
                for(int l = 0; l < B; l++) {
                    cout << node.val[k*B + l] << " ";
                }
                cout << endl;
            }
        }
    }
    //printMatrix(mX, size, size);
    //cout << endl << endl;
    //printMatrix(mY, size, size);
    cout << endl << endl;
    printMatrix(mZ, size, size);
    cout << endl << endl;
    //printMatrix(mT, size, size);

    delete[] mat1;
    delete[] mat2;
    delete[] mX;
    delete[] mY;
    delete[] mZ;
    delete[] mT;
    return 0;
}