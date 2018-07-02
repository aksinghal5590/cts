#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>
#include <chrono>
#include <ctime>
#include <set>

#include "cts.hpp"

using namespace std;

extern int B;
extern int ORTH;
extern double mmTime;
int count = 0;

vector<int> *assembledSet;

void Sptree::assemble(const Csr& srcCsr1, const Csr& srcCsr2, int x, int y) {

    auto start = chrono::system_clock::now();
    //assemble
    bool* workspace = new bool[B]();
    int* wlist = new int[B]();
    int w_size = 0;

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
        for (int w_pos = 0; w_pos < w_size; w_pos++) {
            int j = wlist[w_pos];
            assembledSet[x + i].emplace_back(y + j);
            workspace[j] = false;
        }
        w_size = 0;
    }

    delete[] workspace;
    delete[] wlist;

    auto end = chrono::system_clock::now();
    chrono::duration<double, milli>mm_ms = end - start;
    mmTime += mm_ms.count();
}

void multiplyMatrices(const Csr& srcCsr1, const Csr& srcCsr2, Csr& targetCsr) {

    auto start = chrono::system_clock::now();
    /* //assemble
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
    }*/

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
            targetCsr.vals[A2_pos] += tempVals[j];
            tempVals[j] = 0;
        }
    }

    //targetCsr.vals.resize(targetCsr.iCount[B]);
    //targetCsr.idx.resize(targetCsr.iCount[B]);

    auto end = chrono::system_clock::now();
    chrono::duration<double, milli>mm_ms = end - start;
    mmTime += mm_ms.count();

    delete[] tempVals;
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
            csr.idx[i] = M[i].y % B;
            csr.iCount[(M[i].x % B) + 1]++;
        }
        for(int i = 0; i < B; i++) {
            csr.iCount[i+1] += csr.iCount[i];
        }
        Node node(base, csr, cPtr);
        tree.emplace_back(node);
        return;
    }

    Node root(base, cPtr);
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
        for(int i = 0; i < B; i++) {
            csr.iCount[i+1] += csr.iCount[i];
        }
        Node node(base, csr, cPtr);
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
        Node node(base, cPtr);
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

void Sptree::multiply(const Sptree& spTree1, const Sptree& spTree2, Base base) {

    isAssemble = true;

    assembledSet = new vector<int>[base.len];

    multVectors(spTree1.getTree(), spTree2.getTree());

    int elemCount = 0;
    for(int i = 0; i < base.len; i++) {
        elemCount += assembledSet[i].size();
    }
    Coo* M  = new Coo[elemCount];
    int k = 0;
    for(int i = 0; i < base.len; i++) {
        sort( assembledSet[i].begin(), assembledSet[i].end() );
        assembledSet[i].erase(unique(assembledSet[i].begin(), assembledSet[i].end()), assembledSet[i].end());
        for(int j = 0; j < assembledSet[i].size(); j++) {
            M[k].x = i;
            M[k].y = assembledSet[i][j];
            M[k].val = 0.0;
            k++;
        }

    }
    createCTS(M, elemCount, base);

    isAssemble = false;
    multVectors(spTree1.getTree(), spTree2.getTree());

    delete[] assembledSet;
}

void Sptree::multVectors(const vector<Node>& tree1, const vector<Node>& tree2) {
    if(tree1.size() == 1 && tree2.size() == 1) {
        multLeaves(tree1, tree2, 0, 0, 0);
        return;
    }
    multParts(tree1, tree2, 0, 0, 0);
}

void Sptree::multParts(const vector<Node>& tree1, const vector<Node>& tree2, const int pos1, const int pos2, const int pos) {

    multNodes(tree1, tree2, pos1, pos2, 0, 0, pos);
    multNodes(tree1, tree2, pos1, pos2, 1, 2, pos);
}

void Sptree::multNodes(const vector<Node>& tree1, const vector<Node>& tree2,
    const int pos1, const int pos2, const int xPos, const int yPos, const int pos) {

    if(pos == -1 || tree1.empty() || tree2.empty()) {
        return;
    }

    Node node1 = tree1[pos1];
    Node node2 = tree2[pos2];

    if(isAssemble) {
        for(int i = 0; i < ORTH; i++) {
            int xi = ((i / 2) * 2) + xPos;
            int yi = (i % 2) + yPos;
            if(node1.cPtr[xi] > -1 && node2.cPtr[yi] > -1) {
                if(tree1[node1.cPtr[xi]].base.len == tree2[node2.cPtr[yi]].base.len) {
                    multLeaves(tree1, tree2, node1.cPtr[xi], node2.cPtr[yi], 0);
                } else if(tree1[node1.cPtr[xi]].base.len > tree2[node2.cPtr[yi]].base.len) {
                    multLeaves(tree1, tree2, node1.cPtr[xi], pos2, 0);
                } else {
                    multLeaves(tree1, tree2, pos1, node2.cPtr[yi], 0);
                }
            }
        }
    } else {
        Node node = tree[pos];
        for(int i = 0; i < ORTH; i++) {
            int xi = ((i / 2) * 2) + xPos;
            int yi = (i % 2) + yPos;
            if(node.cPtr[i] > -1 && node1.cPtr[xi] > -1 && node2.cPtr[yi] > -1) {
                if(tree1[node1.cPtr[xi]].base.len == tree2[node2.cPtr[yi]].base.len) {
                    multLeaves(tree1, tree2, node1.cPtr[xi], node2.cPtr[yi], node.cPtr[i]);
                } else if(tree1[node1.cPtr[xi]].base.len > tree2[node2.cPtr[yi]].base.len) {
                    multLeaves(tree1, tree2, node1.cPtr[xi], pos2, node.cPtr[i]);
                } else {
                    multLeaves(tree1, tree2, pos1, node2.cPtr[yi], node.cPtr[i]);
                }
            }
        }
    }
}

void Sptree::multLeaves(const vector<Node>& tree1, const vector<Node>& tree2, const int pos1, const int pos2, const int pos) {

    if(pos == -1) {
        return;
    }

    Node node, node1, node2, emptyNode;
    node1 = tree1[pos1];
    node2 = tree2[pos2];

    if(tree.empty() && !isAssemble) {
        Csr newCsr(B*B, B*B, B+1);
        multiplyMatrices(node1.csr, node2.csr, newCsr);
        Base newBase(0, 0, 0);
        int* cPtr = new int[ORTH];
        for(int i = 0; i < ORTH; i++) {
            cPtr[i] = -1;
        }
        newBase.x = node1.base.x;
        newBase.y = node2.base.y;
        newBase.len = B;
        Node newNode(newBase, newCsr, cPtr);
        tree.emplace_back(newNode);
        return;
    }

    if(!(node1 == emptyNode) && !(node2 == emptyNode)) {
        if(node1.base.len > B || node2.base.len > B) {
            multParts(tree1, tree2, pos1, pos2, pos);
        } else {
            if(isAssemble) {
                assemble(node1.csr, node2.csr, node1.base.x, node2.base.y);
            } else {
                multiplyMatrices(node1.csr, node2.csr, tree[pos].csr);
            }
        }
    }
}