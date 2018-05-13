#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>
#include <chrono>
#include <ctime>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#include "cts.hpp"

double mmTime = 0;
using namespace std;

Sptree tempTrees[200];
int count = 0;

void copyMatrix(const double* srcMat, double* targetMat) {
    if(srcMat == NULL) {
        return;
    }
    cilk_for(int i = 0; i < B; i++) {
        cilk_for(int j = 0; j < B; j++) {
            targetMat[i*B + j] = srcMat[i*B + j];
        }
    }
}

void mergeMatrices(const double* srcMat1, const double* srcMat2, double* targetMat) {
    if(srcMat1 == NULL || srcMat2 == NULL) {
        return;
    }
    cilk_for(int i = 0; i < B; i++) {
        cilk_for(int j = 0; j < B; j++) {
            targetMat[i*B + j] = srcMat1[i*B + j] + srcMat2[i*B + j];
        }
    }
}

bool multiplyMatrices(const double* srcMat1, const double* srcMat2, double* targetMat) {
    if(srcMat1 == NULL || srcMat2 == NULL) {
        return false;
    }
    //auto start = chrono::system_clock::now();
    cilk_for (int k = 0; k < B; ++k) {
        cilk_for (int i = 0; i < B; ++i) {
            for (int j = 0; j < B; ++j) {
                targetMat[i*B + j] += srcMat1[i*B + k] * srcMat2[k*B + j];
            }
        }
    }
    bool notEmpty = false;
    for (int i = 0; i < B && !notEmpty; ++i) {
        for (int j = 0; j < B; ++j) {
            if(targetMat[i*B + j] != 0) {
                notEmpty = true;
                break;
            }
        }
    }
    //auto end = chrono::system_clock::now();
    //chrono::duration<double, milli>elapsed_seconds = end - start;
    //mmTime += elapsed_seconds.count();
    return notEmpty;
}

void Sptree::createCTS(Coo* M, int lenM, Base base) {

    Coo** Ms = new Coo*[ORTH];
    int lenMs[ORTH];
    int iMs[ORTH];
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }
    if (base.len <= B) {
        double* baseVal = new double[B * B]();
        for(int i = 0; i < lenM; i++) {
            baseVal[B*((M[i].x) % B) + (M[i].y) % B] = M[i].val;
        }
        Node node(base, baseVal, cPtr, -1);
        tree.emplace_back(node);
        delete[] baseVal;
        return;
    }

    Node root(base, NULL, cPtr, -1);
    tree.emplace_back(root);

    for (int i = 0; i < ORTH; i++) {
        Ms[i] = NULL;
        lenMs[i] = 0;
        iMs[i] = 0;
    }
    for (int i = 0; i < lenM; i++) {
        int iOrthant = base.getIOrthant(M[i].x, M[i].y);
        lenMs[iOrthant]++;
    }
    int nEmpty = 0;
    for (int i = 0; i < ORTH; i++) {
        if (lenMs[i] > 0) {
            nEmpty++;
            Ms[i] = (Coo*) malloc(lenMs[i] * sizeof(Coo));
        }
    }
    for (int i = 0; i < lenM; i++) {
        int iOrt = base.getIOrthant(M[i].x, M[i].y);
        Ms[iOrt][iMs[iOrt]++] = M[i];
    }
    bool has_sibling = (nEmpty != 1);
    for (int i = 0; i < ORTH; i++) {
        cPtr[i] = createSPTree(i, has_sibling, Ms[i], lenMs[i], base.getBase(i), 0);
    }

    for(int i = 0; i < ORTH; i++) {
        tree[0].cPtr[i] = cPtr[i];
    }
    for(int i = 0; i < ORTH; i++) {
        free(Ms[i]);
    }
    delete[] Ms;
    delete[] cPtr;
}

int Sptree::createSPTree(int idx, bool has_sibling, Coo* M, int lenM, Base base, int baseParent) {

    if (lenM == 0)
        return -1;

    Coo** Ms = new Coo*[ORTH];
    int lenMs[ORTH];
    int iMs[ORTH];
    int index = -1;
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }

    if (base.len <= B) {
        double* baseVal = new double[B * B]();
        for(int i = 0; i < lenM; i++) {
            baseVal[B*((M[i].x) % B) + (M[i].y) % B] = M[i].val;
        }
        Node node(base, baseVal, cPtr, baseParent);
        tree.emplace_back(node);
        delete[] baseVal;
        return tree.size() - 1;
    }

    for (int iOrt = 0; iOrt < ORTH; iOrt++) {
        Ms[iOrt] = NULL;
        lenMs[iOrt] = 0;
        iMs[iOrt] = 0;
    }
    for (int iM = 0; iM < lenM; iM++) {
        int iOrt = base.getIOrthant(M[iM].x, M[iM].y);
        lenMs[iOrt]++;
    }
    int nEmpty = 0;
    int iOrthant = -1;
    for (int i = 0; i < ORTH; i++) {
        if (lenMs[i] > 0) {
            nEmpty++;
            iOrthant = i;
            Ms[i] = new Coo[lenMs[i]];
        }
    }

    for (int i = 0; i < lenM; i++) {
        int iOrt = base.getIOrthant(M[i].x, M[i].y);
        Ms[iOrt][iMs[iOrt]++] = M[i];
    }

    bool has_sibling_child = (nEmpty != 1);
    if (!has_sibling && !has_sibling_child) {
        index = createSPTree(iOrthant, false, Ms[iOrthant], lenMs[iOrthant], base.getBase(iOrthant), baseParent);
    } else {
        Node node(base, NULL, cPtr, baseParent);
        tree.emplace_back(node);
        index = tree.size() - 1;
        for (int i = 0; i < ORTH; i++) {
            cPtr[i] = createSPTree(i, has_sibling_child, Ms[i], lenMs[i], base.getBase(i), index);
        }
        for(int i = 0; i < ORTH; i++) {
            tree[index].cPtr[i] = cPtr[i];
        }
    }

    for(int i = 0; i < ORTH; i++) {
        delete[] Ms[i];
    }
    delete[] Ms;
    delete[] cPtr;
    return index;
}

void Sptree::mergeSptrees(const vector<Node>& tree1, const vector<Node>& tree2, const bool isMultiply, const int trueNodePos) {

    if(tree1.empty() && tree2.empty()) {
        return;
    }
    if(tree1.empty() && tree.empty()) {
        tree.assign(tree2.begin(), tree2.end());
        return;
    }
    if(tree2.empty() && tree.empty()) {
        tree.assign(tree1.begin(), tree1.end());
        return;
    }

    bool isOffset = false;
    bool isEmpty = true;
    Base newBase(0, 0, 0);
    double* newVal = NULL;
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }
    Node node1, node2, emptyNode;
    if(!tree1.empty()) {
        node1 = tree1[0];
    }
    if(!tree2.empty()) {
        node2 = tree2[0];
    }

    int initTreeSize = tree.size();
    if(!(node1 == emptyNode) && ((node2 == emptyNode) || (node1.base.len >= node2.base.len && node2.base.len > B))) {
        Node root(node1.base, NULL, cPtr, node1.parent, trueNodePos);
        tree.emplace_back(root);
        if(isMultiply && (initTreeSize > 0)) {
            int iOrth = tree[0].base.getIOrthant(root.base.x, root.base.y);
            tree[0].cPtr[iOrth] = tree.size() - 1 + tree[0].offset;
            tree[tree.size() - 1].offset = tree[0].offset;
        }
        isOffset = true;
    } else if(!(node2 == emptyNode) && ((node1 == emptyNode) || (node2.base.len >= node1.base.len && node1.base.len > B))) {
        Node root(node2.base, NULL, cPtr, node2.parent, trueNodePos);
        tree.emplace_back(root);
        if(isMultiply && (initTreeSize > 0)) {
            int iOrth = tree[0].base.getIOrthant(root.base.x, root.base.y);
            tree[0].cPtr[iOrth] = tree.size() - 1 + tree[0].offset;
            tree[tree.size() - 1].offset = tree[0].offset;
        }
        isOffset = true;
    } else {
        mergeTrees(tree1, tree2, 0, 0, trueNodePos);
        isOffset = false;
    }

    if(isOffset) {
        for(int i = 0; i < ORTH; i++) {
            if(!(node1 == emptyNode) && !(node2 == emptyNode) && (node1.cPtr[i] > -1) && node2.cPtr[i] > -1) {
                if(node1.base == node2.base) {
                    cPtr[i] = mergeTrees(tree1, tree2, node1.cPtr[i] - node1.offset, node2.cPtr[i] - node2.offset, trueNodePos);
                } else if(node1.base.len > node2.base.len) {
                    if(i == node1.base.getIOrthant(node2.base.x, node2.base.y)) {
                        cPtr[i] = mergeTrees(tree1, tree2, node1.cPtr[i] - node1.offset, 0, trueNodePos);
                    } else {
                        cPtr[i] = mergeTrees(tree1, tree2, node1.cPtr[i] - node1.offset, -1, trueNodePos);
                    }
                } else {
                    if(i == node2.base.getIOrthant(node1.base.x, node1.base.y)) {
                        cPtr[i] = mergeTrees(tree1, tree2, 0, node2.cPtr[i] - node2.offset, trueNodePos);
                    } else {
                        cPtr[i] = mergeTrees(tree1, tree2, -1, node2.cPtr[i] - node2.offset, trueNodePos);
                    }
                }
            } else if((!(node1 == emptyNode) && node1.cPtr[i] > -1) && ((node2 == emptyNode) || node2.cPtr[i] == -1)) {
                cPtr[i] = mergeTrees(tree1, tree2, node1.cPtr[i] - node1.offset, -1, trueNodePos);
            } else if(((node1 == emptyNode) || node1.cPtr[i] == -1) && (!(node2 == emptyNode) && node2.cPtr[i] > -1)) {
                cPtr[i] = mergeTrees(tree1, tree2, -1, node2.cPtr[i] - node2.offset, trueNodePos);
            } else {
                //do nothing
            }
            if(cPtr[i] != -1) {
                isEmpty = false;
            }
        }
        if(isEmpty) {
            clear();
        } else {
            for(int i = 0; i < ORTH; i++) {
                tree[trueNodePos - tree[0].offset].cPtr[i] = cPtr[i];
            }
        }
    }
    delete[] cPtr;
}

int Sptree::mergeTrees(const vector<Node>& tree1, const vector<Node>& tree2, const int pos1, const int pos2, const int parent) {

    int index = -1;
    int count = 0;
    bool isAdded = false;
    const int offset = tree[0].offset;
    const int trueNodePos = parent - offset;
    Base newBase(0, 0, 0);
    double* newVal = NULL;
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }

    Node node1(newBase, newVal, cPtr, -1), node2(newBase, newVal, cPtr, -1), emptyNode(newBase, newVal, cPtr, -1);
    if(pos1 > -1) {
        node1 = tree1[pos1];
    }
    if(pos2 > -1) {
        node2 = tree2[pos2];
    }

    if(node1.base.len <= B && node2.base.len <= B) {
        count = ORTH;
    } else {
        for(int i = 0; i < ORTH; i++) {
            if(node1.cPtr[i] > -1 || node2.cPtr[i] > -1) {
                if(node1.base == node2.base) {
                    if(!isAdded) {
                        isAdded = true;
                        index = appendNode(node1.base, trueNodePos, parent);
                    }
                    //cout << "c1 - node1.base = "; node1.base.printValues(); cout << endl;
                    cPtr[i] = mergeTrees(tree1, tree2, node1.cPtr[i] - node1.offset, node2.cPtr[i] - node2.offset, index);
                } else if(node1.base.len > node2.base.len) {
                    if(!isAdded) {
                        isAdded = true;
                        index = appendNode(node1.base, trueNodePos, parent);
                    }
                    //cout << "c2 - node1.base = "; node1.base.printValues(); cout << endl;
                    if(node2.cPtr[i] == -1) {
                        cPtr[i] = mergeTrees(tree1, tree2, node1.cPtr[i] - node1.offset, -1, index);
                    } else {
                        cPtr[i] = mergeTrees(tree1, tree2, node1.cPtr[i] - node1.offset, pos2, index);
                    }
                } else if(node1.base.len < node2.base.len) {
                    if(!isAdded) {
                        isAdded = true;
                        index = appendNode(node2.base, trueNodePos, parent);
                    }
                    //cout << "c3 - node2.base = "; node2.base.printValues(); cout << endl;
                    if(node1.cPtr[i] == -1) {
                        cPtr[i] = mergeTrees(tree1, tree2, -1, node2.cPtr[i] - node2.offset, index);
                    } else {
                        cPtr[i] = mergeTrees(tree1, tree2, pos1, node2.cPtr[i] - node2.offset, index);
                    }
                } else if(node1.base.len == node2.base.len && node1.base.len > B) {
                    //cout << "c4 - node1.base = "; node1.base.printValues();
                    //cout << " node2.base = "; node2.base.printValues(); cout << endl;
                    Base virtualBase = tree[trueNodePos].base.getBase(i);
                    int iOrth1 = virtualBase.getIOrthant(node1.base.x, node1.base.y);
                    int iOrth2 = virtualBase.getIOrthant(node2.base.x, node2.base.y);
                    while(iOrth1 == iOrth2 && iOrth1 > -1) {
                        virtualBase = virtualBase.getBase(i);
                        iOrth1 = virtualBase.getIOrthant(node1.base.x, node1.base.y);
                        iOrth2 = virtualBase.getIOrthant(node2.base.x, node2.base.y);
                    }
                    if(!isAdded) {
                        isAdded = true;
                        index = appendNode(virtualBase, trueNodePos, parent);
                    }
                    cPtr[iOrth1] = mergeTrees(tree1, tree2, pos1, -1, index);
                    cPtr[iOrth2] = mergeTrees(tree1, tree2, -1, pos2, index);
                } else {
                    //cout << "c5 - node1.base = "; node1.base.printValues();
                    //cout << " node2.base = "; node2.base.printValues(); cout << endl;
                }
            } else {
                count++;
            }
        }
    }
    if(pos1 > -1 || pos2 > -1) {
        if(count == ORTH) {
            if(node2 == emptyNode) {
                newBase = node1.base;
                newVal = new double[B * B]();
                copyMatrix(node1.val, newVal);
            } else if(node1 == emptyNode) {
                newBase = node2.base;
                newVal = new double[B * B]();
                copyMatrix(node2.val, newVal);
            } else if(!(node2 == emptyNode) && !(node1 == emptyNode)) {
                newBase = node1.base;
                newVal = new double[B * B]();
                mergeMatrices(node1.val, node2.val, newVal);
            } else {
                cerr << "Something terrible happened!!\r";
                exit(1);
            }
            int orth = tree[trueNodePos].base.getIOrthant(newBase.x, newBase.y);
            if(orth == -1) {
                cout << "pos1 = " << pos1 << " pos2 = " << pos2 <<  endl;
                newBase.printValues();
                cout << endl;
                node1.printValues();
                node2.printValues();
            }
            if(parent + orth + 1 >= tree.size()) {
                Node newNode(newBase, newVal, cPtr, parent, offset);
                tree.emplace_back(newNode);
                index = tree.size() - 1 + offset;
            } else {
                if(trueNodePos >= 0 && orth >=0 && tree[trueNodePos].cPtr[orth] != -1) {
                    index = tree[trueNodePos].cPtr[orth];
                }
                index += offset;
            }
        } else {
            for(int i = 0; i < ORTH; i++) {
                tree[index - offset].cPtr[i] = cPtr[i];
            }
        }
    }
    if(newVal != NULL) {
        delete[] newVal;
    }
    delete[] cPtr;
    return index;
}

int Sptree::appendNode(const Base& base, const int trueNodePos, const int parent) {
    int index = -1;
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }
    int orth = tree[trueNodePos].base.getIOrthant(base.x, base.y);
    if(tree[trueNodePos].cPtr[orth] != -1) {
        index = tree[trueNodePos].cPtr[orth];
    } else {
        Node newNode(base, NULL, cPtr, parent, tree[0].offset);
        tree.emplace_back(newNode);
        index = tree.size() - 1 + tree[0].offset;
    }
    delete[] cPtr;
    return index;
}

void Sptree::multiply(const vector<Node>& tree1, const vector<Node>& tree2) {
    if(tree1.size() == 1 && tree2.size() == 1) {
        multiplyTrees(tree1, tree2, 0, 0, -1, 0, 0);
        return;
    }
    Sptree& tempA = ::tempTrees[::count++];
    Sptree& tempB = ::tempTrees[::count++];
    multiplySptrees(tempA, tempB, tree1, tree2, 0, 0, -1, 0);
}

void Sptree::multiplySptrees(Sptree& tempA, Sptree& tempB, const vector<Node>& tree1, const vector<Node>& tree2,
    const int pos1, const int pos2, const int parentPos, const int trueNodePos) {

    tempA.multiplyParts(tree1, tree2, pos1, pos2, parentPos, 0, 0, trueNodePos);
    //cout << "TempA tree:- ";
    //tempA.printValues();

    tempB.multiplyParts(tree1, tree2, pos1, pos2, parentPos, 1, 2, trueNodePos);
    //cout << "TempB tree:- ";
    //tempB.printValues();

    //cout << "Non-merged tree:- ";
    //printValues();
    mergeSptrees(tempA.getTree(), tempB.getTree(), true, trueNodePos);
    //cout << "Merged tree:- ";
    //printValues();
    if(::count >= 2) {
        tempA.clear();
        tempB.clear();
        ::tempTrees[--::count] = tempA;
        ::tempTrees[--::count] = tempB;
    }
}

void Sptree::multiplyParts(const vector<Node>& tree1, const vector<Node>& tree2,
    const int pos1, const int pos2, const int parentPos, const int xPos, const int yPos, const int trueNodePos) {

    if(tree1.empty() || tree2.empty()) {
        return;
    }

    bool isAdded = false;
    bool isEmpty = true;
    int parent = -1;
    Base newBase(0, 0, 0), virtualBase(0, 0, 0);
    Node node1 = tree1[pos1];
    Node node2 = tree2[pos2];

    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }

    if(node1.base.len > node2.base.len) {
        for(int i = 0; i < ORTH; i++) {
            if(node1.cPtr[i] > -1) {
                virtualBase = node1.base.getBase(i);
                Base childBase = tree1[node1.cPtr[i] - node1.offset].base;
                int iOrth = virtualBase.getIOrthant(childBase.x, childBase.y);
                while(virtualBase.len > node2.base.len) {
                    virtualBase = virtualBase.getBase(iOrth);
                    iOrth = virtualBase.getIOrthant(childBase.x, childBase.y);
                }
                node1.base = virtualBase;
                int pos = node1.cPtr[i];
                node1.cPtr[i] = -1;
                node1.cPtr[iOrth] = pos;
                break;
            }
        }
    } else if(node1.base.len < node2.base.len) {
        for(int i = 0; i < ORTH; i++) {
            if(node2.cPtr[i] > -1) {
                virtualBase = node2.base.getBase(i);
                Base childBase = tree2[node2.cPtr[i] - node2.offset].base;
                int iOrth = virtualBase.getIOrthant(childBase.x, childBase.y);
                while(virtualBase.len > node1.base.len) {
                    virtualBase = virtualBase.getBase(iOrth);
                    iOrth = virtualBase.getIOrthant(childBase.x, childBase.y);
                }
                node2.base = virtualBase;
                int pos = node2.cPtr[i];
                node2.cPtr[i] = -1;
                node2.cPtr[iOrth] = pos;
                break;
            }
        }
    }
    if(node1.base.len > B && node2.base.len > B) {
        newBase.x = node1.base.x;
        newBase.y = node2.base.y;
        newBase.len = node1.base.len;
        Node root(newBase, NULL, cPtr, parentPos, trueNodePos);
        tree.emplace_back(root);
        parent = tree.size() - 1 + trueNodePos;
        //cout << "Node 1 = "; node1.printValues();
        //cout << "Node 2 = "; node2.printValues();
        //cout << "Root = "; root.printValues();
    }

    for(int i = 0; i < ORTH; i++) {
        int xi = ((i / 2) * 2) + xPos;
        int yi = (i % 2) + yPos;
        if(node1.cPtr[xi] > -1 && node2.cPtr[yi] > -1) {
            if(tree1[node1.cPtr[xi] - node1.offset].base.len == tree2[node2.cPtr[yi] - node2.offset].base.len) {
                cPtr[i] = multiplyTrees(tree1, tree2, node1.cPtr[xi] - node1.offset, node2.cPtr[yi] - node2.offset, parent, i, trueNodePos);
            } else if(tree1[node1.cPtr[xi] - node1.offset].base.len > tree2[node2.cPtr[yi] - node2.offset].base.len) {
                cPtr[i] = multiplyTrees(tree1, tree2, node1.cPtr[xi] - node1.offset, pos2, parent, i, trueNodePos);
            } else {
                cPtr[i] = multiplyTrees(tree1, tree2, pos1, node2.cPtr[yi] - node2.offset, parent, i, trueNodePos);
            }
        }
        if(cPtr[i] > -1) {
            isEmpty = false;
        }
    }
    if(isEmpty && tree.size() == 1) {
        clear();
    } else {
        if(!isEmpty) {
            for(int i = 0; i < ORTH; i++) {
                tree[parent - trueNodePos].cPtr[i] = cPtr[i];
            }
        }
    }
    delete[] cPtr;
}

int Sptree::multiplyTrees(const vector<Node>& tree1, const vector<Node>& tree2,
    const int pos1, const int pos2, const int parent, const int orthant, const int trueNodePos) {
    Base newBase(0, 0, 0);
    double* newVal = NULL;
    int index = -1;
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }
    int* cPtrLeaf = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtrLeaf[i] = -1;
    }
    Node node1, node2, emptyNode;
    node1 = tree1[pos1];
    node2 = tree2[pos2];
    if(!(node1 == emptyNode) && !(node2 == emptyNode)) {
        if(node1.base.len > B || node2.base.len > B) {
            Sptree& tempA = ::tempTrees[::count++];
            Sptree& tempB = ::tempTrees[::count++];
            multiplySptrees(tempA, tempB, tree1, tree2, pos1, pos2, parent, tree.size() + parent);
        } else {
            newVal = new double[B * B]();
            bool notEmpty = false;
            notEmpty = multiplyMatrices(node1.val, node2.val, newVal);
            if(notEmpty) {
                newBase.x = node1.base.x;
                newBase.y = node2.base.y;
                newBase.len = B;
                Node newNode(newBase, newVal, cPtr, parent, trueNodePos);
                tree.emplace_back(newNode);
                index = tree.size() - 1 + trueNodePos;
                //cout << "Node 1 = "; node1.printValues();
                //cout << "Node 2 = "; node2.printValues();
                //cout << "New Node = "; newNode.printValues();
            }
        }
    }
    delete[] cPtr;
    delete[] cPtrLeaf;
    if(newVal != NULL) {
        delete[] newVal;
    }
    return index;
}