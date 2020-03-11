//
// Created by vwslz on 12/4/2017.
//

#ifndef CTS_CTS2D_H
#define CTS_CTS2D_H

#include <stdio.h>
#include "stdlib.h"
#include <iostream>

class CTS
{
private:

public:
    typedef struct {
        int x;
        int y;
        int len;
    }Base;
    typedef struct {
        int x;
        int y;
        int val;
    }COO;
    typedef struct {
        Base base;
        double val;
        int cPtr[4];
    }Node;
    struct Tree {
        int nNode;
        int nVal;
        Node* matrix;
        double* matrix_val; // It can be array of base case CSR matrix
    };

    int dim;
    int B;
    int nOrthants;
    int num[2];
    Node* nodelist[2];
    int iNodelist[2];
    int sizeNodelist[2];
    int height;
    Node** nodeZs;
    int* sizeZ;
    int* iZ;

    CTS(int dimension, int b, int nOrt, int n[2], COO* coos[2]);
    ~CTS();

    Node createCTS(int i, COO* M, int lenM, Base base);
    void createSPTree(int iObj, int p, int i, bool has_sibling, COO* M, int lenM, Base base);
    int tensorProduct(int iNodeX, Node nodeXl, int iNodeY, Node nodeYl, Node nodeZ, int level);
    int merge(int tree1, int tree2, Node nodeZ, int level);

    int mergeWrapper(int iX, int iY, Node nodeZ, int level);
    void copyTree(int root, int level);
    void doublingNodelist(int iObj);
    void doublingArray(int* size, Node** arr);
    int getIOrthant(int x, int y, Base base);
    Base getBase(Base base, int iOrt);
    Node getNextBox(int iObj, int iX, Node y);
    Node getImmediateParent(int index1, int index2, int level, Node parent);
    Node getNode(Node node, int child_idx);
    bool isLeaf(Node node);
    int getIndexIfOnlyOneNonNullChild(Node* node);
    void printCooArray(COO* arr, int len);
    void printNodelist(Node* node, int len);
};

#endif //CTS_CTS2D_H
