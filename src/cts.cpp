#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>

#include "cts.hpp"

#define ORTH 4
#define B 64

using namespace std;

double endM = 0.0;

Sptree tempTrees[200];
int count = 0;

void copyMatrix(int* srcMat, int* targetMat) {
    if(srcMat == NULL) {
        return;
    }
    for(int i = 0; i < B; i++) {
        for(int j = 0; j < B; j++) {
            targetMat[i*B + j] = srcMat[i*B + j];
        }
    }
}

void mergeMatrices(int* srcMat1, int* srcMat2, int* targetMat) {
    if(srcMat1 == NULL || srcMat2 == NULL) {
        return;
    }
    for(int i = 0; i < B; i++) {
        for(int j = 0; j < B; j++) {
            targetMat[i*B + j] = srcMat1[i*B + j] + srcMat2[i*B + j];
        }
    }
}

void multiplyMatrices(int* srcMat1, int* srcMat2, int* targetMat) {
  for (int i = 0; i < B; ++i) {
    for (int k = 0; k < B; ++k) {
      for (int j = 0; j < B; ++j) {
        targetMat[i*B + j] += srcMat1[i*B + k] * srcMat2[k*B + j];
      }
    }
  }
}

void Sptree::createCTS(Coo* M, int lenM, Base base) {

    Coo* Ms[ORTH];
    int lenMs[ORTH];
    int iMs[ORTH];
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }
    if (base.len <= B) {
        cout << "In base case - lenM = " << lenM << endl;
        int* baseVal = new int[B * B];
        for(int i = 0; i < lenM; i++) {
            baseVal[B*((M[i].x) % B) + (M[i].y) % B] = M[i].val;
        }
        Node node(base, baseVal, cPtr, -1);
        tree.emplace_back(node);
        delete[] baseVal;
        return;
    }

    Node root(base, NULL, cPtr);
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
    delete[] cPtr;
}


int Sptree::createSPTree(int idx, bool has_sibling, Coo* M, int lenM, Base base, int baseParent) {

    Coo* Ms[ORTH];
    int lenMs[ORTH];
    int iMs[ORTH];
    int index = -1;
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }

    if (lenM == 0)
        return -1;

    if (base.len <= B) {
        int* baseVal = new int[B * B];
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
        cPtr[idx] = createSPTree(idx, false, Ms[iOrthant], lenMs[iOrthant], base.getBase(iOrthant), baseParent);
    } else {
        Node node(base, NULL, cPtr);
        tree.emplace_back(node);
        index = tree.size() - 1;
        for (int i = 0; i < ORTH; i++) {
            cPtr[i] = createSPTree(i, has_sibling_child, Ms[i], lenMs[i], base.getBase(i), index);
        }
    }
    for(int i = 0; i < ORTH; i++) {
        tree[index].cPtr[i] = cPtr[i];
    }
    tree[index].parent = baseParent;

    if(Ms != NULL) {
        for(int i = 0; i < ORTH; i++) {
            delete[] Ms[i];
        }
    }
    delete[] cPtr;
    return index;
}

void Sptree::merge(vector<Node>& tree1, vector<Node>& tree2, const bool isMultiply, const int trueNodePos) {

    if(tree1.empty() && tree2.empty()) {
        return;
    }
    if(tree1.empty()) {
        tree.assign(tree2.begin(), tree2.end());
        return;
    }
    if(tree2.empty()) {
        tree.assign(tree1.begin(), tree1.end());
        return;
    }

    Node node1 = tree1[0];
    Node node2 = tree2[0];
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }

    int initTreeSize = tree.size();
    if(node1.base.len >= node2.base.len && node2.base.len > B) {
        Node root(node1.base, NULL, cPtr, node1.parent, trueNodePos);
        tree.emplace_back(root);
        if(isMultiply && (initTreeSize > 0)) {
            int iOrth = tree[0].base.getIOrthant(root.base.x, root.base.y);
            tree[0].cPtr[iOrth] = tree.size() - 1 + tree[0].offset;
            tree[tree.size() - 1].offset = tree[0].offset;
        }
    } else if(node2.base.len >= node1.base.len && node1.base.len > B) {
        Node root(node2.base, NULL, cPtr, node2.parent, trueNodePos);
        tree.emplace_back(root);
        if(isMultiply && (initTreeSize > 0)) {
            int iOrth = tree[0].base.getIOrthant(root.base.x, root.base.y);
            tree[0].cPtr[iOrth] = tree.size() - 1 + tree[0].offset;
            tree[tree.size() - 1].offset = tree[0].offset;
        }
    } else {
        mergeNodes(tree1, tree2, 0, 0, trueNodePos);
        return;
    }

    for(int i = 0; i < ORTH; i++) {
        if((node1.cPtr[i] > -1) && node2.cPtr[i] > -1) {
            if(node1.base == node2.base) {
                cPtr[i] = mergeNodes(tree1, tree2, node1.cPtr[i] - node1.offset, node2.cPtr[i] - node2.offset, trueNodePos);
            } else if(node1.base.len > node2.base.len) {
                if(i == node1.base.getIOrthant(node2.base.x, node2.base.y)) {
                    cPtr[i] = mergeNodes(tree1, tree2, node1.cPtr[i] - node1.offset, 0, trueNodePos);
                } else {
                    cPtr[i] = mergeNodes(tree1, tree2, node1.cPtr[i] - node1.offset, -1, trueNodePos);
                }
            } else {
                if(i == node2.base.getIOrthant(node1.base.x, node1.base.y)) {
                    cPtr[i] = mergeNodes(tree1, tree2, 0, node2.cPtr[i] - node2.offset, trueNodePos);
                } else {
                    cPtr[i] = mergeNodes(tree1, tree2, -1, node2.cPtr[i] - node2.offset, trueNodePos);
                }
            }
        } else if(node1.cPtr[i] > -1 && node2.cPtr[i] == -1) {
            cPtr[i] = mergeNodes(tree1, tree2, node1.cPtr[i] - node1.offset, -1, trueNodePos);
        } else if(node1.cPtr[i] == -1 && node2.cPtr[i] > -1) {
            cPtr[i] = mergeNodes(tree1, tree2, -1, node2.cPtr[i] - node2.offset, trueNodePos);
        } else {
            //cout << "Merge - case 4" << endl;
        }
    }
    for(int i = 0; i < ORTH; i++) {
        tree[trueNodePos - tree[0].offset].cPtr[i] = cPtr[i];
    }
    delete[] cPtr;
}

int Sptree::mergeNodes(vector<Node>& tree1, vector<Node>& tree2, const int pos1, const int pos2, const int parent) {

    Base newBase = {0, 0, 0};
    int* newVal = NULL;
    int index = -1;
    int count = 0;
    int offset = tree[0].offset;
    int trueNodePos = parent - offset;
    bool isAdded = false;
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }

    Node node1(newBase, newVal, cPtr), node2(newBase, newVal, cPtr);
    Node emptyNode(newBase, newVal, cPtr);
    if(pos1 > -1) {
        node1 = tree1[pos1];
    }
    if(pos2 > -1) {
        node2 = tree2[pos2];
    }

    for(int i = 0; i < ORTH; i++) {
        if((!(node1 == emptyNode) && node1.cPtr[i] > -1) && (!(node2 == emptyNode) && node2.cPtr[i] > -1)) {
            if(node1.base == node2.base) {
                //cout << "c1 - node1.base.len = " << node1.base.len << endl;
                if(!isAdded) {
                    isAdded = true;
                    int orth = tree[trueNodePos].base.getIOrthant(node1.base.x, node1.base.y);
                    if(tree[trueNodePos].cPtr[orth] != -1) {
                        index = tree[trueNodePos].cPtr[orth];
                    } else {
                        newBase = node1.base;
                        Node newNode(newBase, newVal, cPtr, parent, offset);
                        tree.emplace_back(newNode);
                        index = tree.size() - 1 + offset;
                    }
                }
                cPtr[i] = mergeNodes(tree1, tree2, node1.cPtr[i] - node1.offset, node2.cPtr[i] - node2.offset, index);
            } else {
                if(node1.base.len > node2.base.len) {
                    //cout << "c2 - node1.base.len = " << node1.base.len << endl;
                    if(!isAdded) {
                        isAdded = true;
                        int orth = tree[trueNodePos].base.getIOrthant(node1.base.x, node1.base.y);
                        if(tree[trueNodePos].cPtr[orth] != -1) {
                            index = tree[trueNodePos].cPtr[orth];
                        } else {
                            newBase = node1.base;
                            Node newNode(newBase, newVal, cPtr, parent, offset);
                            tree.emplace_back(newNode);
                            index = tree.size() - 1 + offset;
                        }
                    }
                    cPtr[i] = mergeNodes(tree1, tree2, node1.cPtr[i], pos2, index);
                } else {
                    //cout << "c3 - node2.base.len = " << node2.base.len << endl;
                    if(!isAdded) {
                        isAdded = true;
                        int orth = tree[trueNodePos].base.getIOrthant(node2.base.x, node2.base.y);
                        if(tree[trueNodePos].cPtr[orth] != -1) {
                            index = tree[trueNodePos].cPtr[orth];
                        } else {
                            newBase = node2.base;
                            Node newNode(newBase, newVal, cPtr, parent, offset);
                            tree.emplace_back(newNode);
                            index = tree.size() - 1 + offset;
                        }
                    }
                    cPtr[i] = mergeNodes(tree1, tree2, pos1, node2.cPtr[i] - node2.offset, index);
                }
            }
        } else if((!(node1 == emptyNode) && node1.cPtr[i] > -1) && node2.cPtr[i] == -1) {
            //cout << "c4 - node1.base.len = " << node1.base.len << endl;
            if(!isAdded) {
                isAdded = true;
                int orth = tree[trueNodePos].base.getIOrthant(node1.base.x, node1.base.y);
                if(tree[trueNodePos].cPtr[orth] != -1) {
                    index = tree[trueNodePos].cPtr[orth];
                } else {
                    newBase = node1.base;
                    Node newNode(newBase, newVal, cPtr, parent, offset);
                    tree.emplace_back(newNode);
                    index = tree.size() - 1 + offset;
                }
            }
            cPtr[i] = mergeNodes(tree1, tree2, node1.cPtr[i] - node1.offset, pos2, index);
        } else if(node2.cPtr[i] == -1 && (!(node2 == emptyNode) && node2.cPtr[i] > -1)) {
            //cout << "c5 - node2.base.len = " << node2.base.len << endl;
            if(!isAdded) {
                isAdded = true;
                int orth = tree[trueNodePos].base.getIOrthant(node2.base.x, node2.base.y);
                if(tree[trueNodePos].cPtr[orth] != -1) {
                    index = tree[trueNodePos].cPtr[orth];
                } else {
                    newBase = node2.base;
                    Node newNode(newBase, newVal, cPtr, parent, offset);
                    tree.emplace_back(newNode);
                    index = tree.size() - 1 + offset;
                }
            }
            cPtr[i] = mergeNodes(tree1, tree2, pos1, node2.cPtr[i] - node2.offset, index);
        } else {
           count++;
        }
    }

    if(count == ORTH) {
        if(node2 == emptyNode) {
            newBase = node1.base;
            newVal = new int[B * B];
            copyMatrix(node1.val, newVal);
        } else if(node1 == emptyNode){
            newBase = node2.base;
            newVal = new int[B * B];
            copyMatrix(node2.val, newVal);
        } else if(!(node2 == emptyNode) && !(node1 == emptyNode)) {
            newBase = node1.base;
            newVal = new int[B * B];
            mergeMatrices(node1.val, node2.val, newVal);
        } else {
            cerr << "Something terrible happened!!\r";
            exit(1);
        }
        int orth = tree[trueNodePos].base.getIOrthant(newBase.x, newBase.y);
        //cout << "Parent = " << parent << " orth = " << orth << " size = "<< tree.size() << endl;
        if(parent + orth + 1 >= tree.size()) {
            Node newNode(newBase, newVal, cPtr, parent, offset);
            tree.emplace_back(newNode);
            index = tree.size() - 1 + offset;
        } else {
            if(tree[trueNodePos].cPtr[orth] != 1) {
                index = tree[trueNodePos].cPtr[orth];
            }
            index += offset;
        }
    } else {
        for(int i = 0; i < ORTH; i++) {
            tree[index - offset].cPtr[i] = cPtr[i];
        }
    }
    if(newVal != NULL) {
        delete[] newVal;
    }
    delete[] cPtr;
    return index;
}

void Sptree::multiply(vector<Node>& tree1, vector<Node>& tree2) {
    Sptree& tempA = ::tempTrees[::count++];
    Sptree& tempB = ::tempTrees[::count++];
    multiply(tempA, tempB, tree1, tree2, 0, 0, -1, 0);
}

void Sptree::multiply(Sptree& tempA, Sptree& tempB, vector<Node>& tree1, vector<Node>& tree2,
    const int pos1, const int pos2, const int parentPos, const int trueNodePos) {

    tempA.multiplyParts(tree1, tree2, pos1, pos2, parentPos, 0, 0, trueNodePos);
    //cout << "TempA tree:- ";
    //tempA.printValues();

    tempB.multiplyParts(tree1, tree2, pos1, pos2, parentPos, 1, 2, trueNodePos);
    //cout << "TempB tree:- ";
    //tempB.printValues();

    merge(tempA.getTree(), tempB.getTree(), true, trueNodePos);
    //cout << "Merged tree:- ";
    //printValues();
    if(::count >= 2) {
        tempA.clear();
        tempB.clear();
        ::tempTrees[--::count] = tempA;
        ::tempTrees[--::count] = tempB;
    }
}

void Sptree::multiplyParts(vector<Node>& tree1, vector<Node>& tree2,
    const int pos1, const int pos2, const int parentPos, const int xPos, const int yPos, const int trueNodePos) {

    if(tree1.empty() || tree2.empty()) {
        return;
    }

    bool isEmpty = true;
    int parent = -1;
    Base newBase = {0, 0, 0};
    Node node1 = tree1[pos1];
    Node node2 = tree2[pos2];
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }

    newBase.x = node1.base.x;
    newBase.y = node2.base.y;
    newBase.len = node1.base.len;
    Node root(newBase, NULL, cPtr, parentPos, trueNodePos);
    tree.emplace_back(root);
    parent = tree.size() - 1 + trueNodePos;

    for(int i = 0; i < ORTH; i++) {
        int xi = ((i / 2) * 2) + xPos;
        int yi = (i % 2) + yPos;
        if(node1.cPtr[xi] > -1 && node2.cPtr[yi] > -1) {
            if(tree1[node1.cPtr[xi] - node1.offset].base.len == tree2[node2.cPtr[yi] - node2.offset].base.len) {
                cPtr[i] = multiplyNodes(tree1, tree2, node1.cPtr[xi] - node1.offset, node2.cPtr[yi] - node2.offset, parent, i, trueNodePos);
            } else if(tree1[node1.cPtr[xi] - node1.offset].base.len > tree2[node2.cPtr[yi] - node2.offset].base.len) {
                cPtr[i] = multiplyNodes(tree1, tree2, node1.cPtr[xi] - node1.offset, pos2, parent, i, trueNodePos);
            } else {
                cPtr[i] = multiplyNodes(tree1, tree2, pos1, node2.cPtr[yi] - node2.offset, parent, i, trueNodePos);
            }
        }
        if(cPtr[i] > -1) {
            isEmpty = false;
        }
    }
    if(isEmpty) {
    } else {
        for(int i = 0; i < ORTH; i++) {
            tree[parent - trueNodePos].cPtr[i] = cPtr[i];
        }
    }
    delete[] cPtr;
}

int Sptree::multiplyNodes(vector<Node>& tree1, vector<Node>& tree2,
    const int pos1, const int pos2, const int parent, const int orthant, const int trueNodePos) {
    Base newBase = {0, 0, 0};
    int* newVal = NULL;
    int index = -1;
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }
    int* cPtrLeaf = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtrLeaf[i] = -1;
    }
    Node node1(newBase, newVal, cPtr), node2(newBase, newVal, cPtr);
    Node emptyNode(newBase, newVal, cPtr);
    node1 = tree1[pos1];
    node2 = tree2[pos2];

    //cout << "In MN - pos1 = " << pos1 << " pos2 = " << pos2 << endl;
    if(!(node1 == emptyNode) && !(node2 == emptyNode)) {
        if(node1.base.len > B || node2.base.len > B) {
            Sptree& tempA = ::tempTrees[::count++];
            Sptree& tempB = ::tempTrees[::count++];
            multiply(tempA, tempB, tree1, tree2, pos1, pos2, parent, tree.size() + parent);
        }
    } else {
        return -1;
    }
    //cout << "Base Case - pos1 = " << pos1 << " pos2 = " << pos2 << endl;
    if(!(node1 == emptyNode) && !(node2 == emptyNode) && node1.base.len <= B && node2.base.len <= B) {
        newVal = new int[B * B]();
        multiplyMatrices(node1.val, node2.val, newVal);
        newBase = tree[parent - trueNodePos].base.getBase(orthant);
        Node newNode(newBase, newVal, cPtr, parent, trueNodePos);
        tree.emplace_back(newNode);
        index = tree.size() - 1 + trueNodePos;
    }

    delete[] cPtr;
    delete[] cPtrLeaf;
    if(newVal != NULL) {
        delete[] newVal;
    }
    return index;
}
