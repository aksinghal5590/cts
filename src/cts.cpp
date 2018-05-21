#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>
#include <chrono>
#include <ctime>

#include "cts.hpp"

using namespace std;

extern int B;
extern double mmTime;
Sptree tempTrees[200];
int count = 0;

void mergeMatrices(const Csr& srcCsr1, const Csr& srcCsr2, Csr& targetCsr) {
    auto start = chrono::system_clock::now();

    for(int i = 0; i < B; i++) {
        int iCount1 = srcCsr1.iCount[i+1];
        int iCount2 = srcCsr2.iCount[i+1];
        int j = srcCsr1.iCount[i];
        int k = srcCsr2.iCount[i];
        int t = targetCsr.iCount[i];
        while(j < iCount1 && k < iCount2) {
            if(srcCsr1.idx[j] < srcCsr2.idx[k]) {
                targetCsr.idx[t] = srcCsr1.idx[j];
                targetCsr.vals[t] = srcCsr1.vals[j];
                j++;
            } else if(srcCsr1.idx[j] > srcCsr2.idx[k]) {
                targetCsr.idx[t] = srcCsr2.idx[k];
                targetCsr.vals[t] = srcCsr2.vals[k];
                k++;
            } else {
                targetCsr.idx[t] = srcCsr1.idx[j];
                targetCsr.vals[t] = srcCsr1.vals[j] + srcCsr2.vals[k];
                j++;
                k++;
            }
            t++;
        }
        while(j < iCount1) {
            targetCsr.idx[t] = srcCsr1.idx[j];
            targetCsr.vals[t] = srcCsr1.vals[j];
            j++;
            t++;
        }
        while(k < iCount2) {
            targetCsr.idx[t] = srcCsr2.idx[k];
            targetCsr.vals[t] = srcCsr2.vals[k];
            k++;
            t++;
        }
        targetCsr.iCount[i+1] = t;
    }

    targetCsr.vals.resize(targetCsr.iCount[B]);
    targetCsr.idx.resize(targetCsr.iCount[B]);

    auto end = chrono::system_clock::now();
    chrono::duration<double, milli>mm_ms = end - start;
    mmTime += mm_ms.count();
}

bool multiplyMatrices(const Csr& srcCsr1, const Csr& srcCsr2, Csr& targetCsr) {

    auto start = chrono::system_clock::now();
    //assemble
    bool* workspace = new bool[B]();
    int* wlist = new int[B]();
    int w_size = 0;

    targetCsr.iCount[0] = 0;
    for (int i = 0; i < B; i++) {
        for (int B2_pos = srcCsr1.iCount[i]; B2_pos < srcCsr1.iCount[i+1]; B2_pos++) {
            int k = srcCsr1.idx[B2_pos];
            for (int C2_pos = srcCsr2.iCount[k]; C2_pos < srcCsr2.iCount[k+1]; C2_pos++) {
                int j = srcCsr2.idx[C2_pos];
                if (!workspace[j]) {
                    wlist[w_size++] = j;
                    workspace[j] = true;
                }
            }
        }
        targetCsr.iCount[i+1] = targetCsr.iCount[i] + w_size;
        for (int w_pos = 0; w_pos < w_size; w_pos++) {
            int j = wlist[w_pos];
            targetCsr.idx[targetCsr.iCount[i] + w_pos] = j;
            workspace[j] = false;
        }
        w_size = 0;
    }

    // compute
    double* tempVals = new double[B]();

    for (int i = 0; i < B; i++) {
        for (int B2_pos = srcCsr1.iCount[i]; B2_pos < srcCsr1.iCount[i+1]; B2_pos++) {
            int k = srcCsr1.idx[B2_pos];
            for (int C2_pos = srcCsr2.iCount[k]; C2_pos < srcCsr2.iCount[k+1]; C2_pos++) {
                int j = srcCsr2.idx[C2_pos];
                tempVals[j] += srcCsr1.vals[B2_pos] * srcCsr2.vals[C2_pos];
            }
        }
        for (int A2_pos = targetCsr.iCount[i]; A2_pos < targetCsr.iCount[i+1]; A2_pos++) {
            int j = targetCsr.idx[A2_pos];
            targetCsr.vals[A2_pos] = tempVals[j];
            tempVals[j] = 0;
        }
    }

    targetCsr.vals.resize(targetCsr.iCount[B]);
    targetCsr.idx.resize(targetCsr.iCount[B]);

    auto end = chrono::system_clock::now();
    chrono::duration<double, milli>mm_ms = end - start;
    mmTime += mm_ms.count();

    delete[] workspace;
    delete[] wlist;
    delete[] tempVals;
    return (targetCsr.iCount[targetCsr.iCount.size() - 1] > 0 ? true : false);
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
        Csr csr(lenM, lenM, B + 1);
        for(int i = 0; i < lenM; i++) {
            csr.vals[i] = M[i].val;
            csr.idx[i] = M[i].y;
            csr.iCount[M[i].x + 1]++;
        }
        for(int i = 0; i < csr.iCount.size() - 1; i++) {
            csr.iCount[i+1] += csr.iCount[i];
        }
        Node node(base, csr, cPtr, -1);
        tree.emplace_back(node);
        return;
    }

    Node root(base, cPtr, -1);
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
        Csr csr(lenM, lenM, B + 1);
        for(int i = 0; i < lenM; i++) {
            csr.vals[i] = M[i].val;
            csr.idx[i] = (M[i].y % B);
            csr.iCount[(M[i].x % B) + 1]++;
        }
        for(int i = 0; i < csr.iCount.size() - 1; i++) {
            csr.iCount[i+1] += csr.iCount[i];
        }
        Node node(base, csr, cPtr, baseParent);
        tree.emplace_back(node);
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
        Node node(base, cPtr, baseParent);
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
        Node root(node1.base, cPtr, node1.parent, trueNodePos);
        tree.emplace_back(root);
        if(isMultiply && (initTreeSize > 0)) {
            int iOrth = tree[0].base.getIOrthant(root.base.x, root.base.y);
            tree[0].cPtr[iOrth] = tree.size() - 1 + tree[0].offset;
            tree[tree.size() - 1].offset = tree[0].offset;
        }
        isOffset = true;
    } else if(!(node2 == emptyNode) && ((node1 == emptyNode) || (node2.base.len >= node1.base.len && node1.base.len > B))) {
        Node root(node2.base, cPtr, node2.parent, trueNodePos);
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
    int* cPtr = new int[ORTH];
    for(int i = 0; i < ORTH; i++) {
        cPtr[i] = -1;
    }

    Node node1(newBase, cPtr, -1), node2(newBase, cPtr, -1), emptyNode(newBase, cPtr, -1);
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
            Csr newCsr(B*B, B*B, B+1);
            if(node2 == emptyNode) {
                newBase = node1.base;
                newCsr = node1.csr;
            } else if(node1 == emptyNode) {
                newBase = node2.base;
                newCsr = node2.csr;
            } else if(!(node2 == emptyNode) && !(node1 == emptyNode)) {
                newBase = node1.base;
                mergeMatrices(node1.csr, node2.csr, newCsr);
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
                Node newNode(newBase, newCsr, cPtr, parent, offset);
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
        Node newNode(base, cPtr, parent, tree[0].offset);
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
        Node root(newBase, cPtr, parentPos, trueNodePos);
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
            bool notEmpty = false;
            Csr newCsr(B*B, B*B, B+1);
            notEmpty = multiplyMatrices(node1.csr, node2.csr, newCsr);
            if(notEmpty) {
                newBase.x = node1.base.x;
                newBase.y = node2.base.y;
                newBase.len = B;
                Node newNode(newBase, newCsr, cPtr, parent, trueNodePos);
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
    return index;
}
