#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <vector>

#include "cts.hpp"

using namespace std;

int main(int argc, char *argv[]) {

    double fraction;
    istringstream iss2(argv[1]);
    if (!(iss2 >> fraction)) {
        cerr << "Invalid number: " << argv[1] << endl;
        return -1;
    }
    int size;
    int n, m, elemCount;
    cin >> n >> m >> elemCount;
    n = (n >= m) ? n : m;
    int ceil2 = 2;
    while(1) {
        if(n <= ceil2) {
            size = ceil2;
            break;
        }
        ceil2 *= 2;
    }
    int factor = n * fraction;
    Coo *mat1 = new Coo[elemCount];
    Coo *mat2 = new Coo[n * factor];

    int k;
    int i, j;
    double val;
    for(k = 0; k < elemCount; k++) {
        cin >> i >> j >> val;
        mat1[k].x = i - 1;
        mat1[k].y = j - 1;
        mat1[k].val = val;
    }
    k = 0;
    srand(time(NULL));
    int a = 1;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < factor; j++) {
            mat2[k].x = i;
            mat2[k].y = rand() % n;
            mat2[k].val = a++;
            k++;
        }
        a = 1;
    }

    auto start = std::chrono::system_clock::now();

    Sptree treeX;
    Sptree treeY;
    Sptree treeZ;
    Base base(0, 0, size);
    cout << "Creating Trees" << endl;
    treeX.createCTS(mat1, elemCount, base);
    treeY.createCTS(mat2, n * factor, base);

    cout << "Trees created" << endl;
    treeZ.merge(treeX.getTree(), treeY.getTree());
    //treeZ.multiply(treeX.getTree(), treeY.getTree());

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double>elapsed_seconds = end - start;
    cout << "Size = " << size
    << " Time taken: " << elapsed_seconds.count() << "s" << endl;

    delete[] mat1;
    delete[] mat2;
    return 0;
}