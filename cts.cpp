#include "CTS2D.h"
#include "math.h"

int output2[32][32] = {0};
int output1[32][32] = {0};
CTS::~CTS() {
}
/*
dimension = length of a dimension like 16 for a 16*16  matrix
b = block size
nOrt = for a 2D matrix nOrt is 4
int n[2] = n[0] contains number of elements in the 1st input matrix.
COO* coos[2] = takes 2 input matrices in CTS format( an array of struct{x,y,val}); coos[0] contains 1st input matrix.
*/
CTS::CTS(int dimension, int b, int nOrt, int n[2], COO* coos[2]) {
    dim = dimension;
    B = b;
    nOrthants = nOrt;
    Base base = {0, 0, dim};
    height = log2(dimension) + 1;
    for (int i = 0; i < 2; i++) {
        num[i] = n[i];
        nodelist[i] = (Node *) malloc(n[i] * sizeof(Node));
        sizeNodelist[i] = n[i];
        iNodelist[i] = 0;
        createCTS(i, coos[i], num[i], base);
    }

    nodeZs = (Node**)malloc(height * sizeof(Node*));
    sizeZ = (int*)malloc(height * sizeof(int*));
    iZ = (int*)malloc(height * sizeof(int*));
    for (int iHeight = 0; iHeight < height; iHeight++) {
        nodeZs[iHeight] = (Node *) malloc(sizeof(Node));
        sizeZ[iHeight] = 1;
        iZ[iHeight] = 0;
    }
}

/*
M is list of input in COO format
iObj =0 means 1st input matrix
*/
CTS::Node CTS::createCTS(int iObj, COO* M, int lenM, Base base) {
    nodelist[iObj][iNodelist[iObj]++] = {base, 0, {-1, -1, -1, -1}};
    COO *Ms[nOrthants];
    int lenMs[nOrthants];
    int iMs[nOrthants];
    for (int i = 0; i < nOrthants; i++)
    {
        Ms[i] = NULL;
        lenMs[i] = 0;
        iMs[i] = 0;
    }
    for (int i = 0; i < lenM; i++)
    {
        int iOrthant = getIOrthant(M[i].x, M[i].y, base);
        lenMs[iOrthant]++;
    }
    int nEmpty = 0;
    for (int i = 0; i < nOrthants; i++) {
        if (lenMs[i] > 0) {
            nEmpty++;
            Ms[i] = (COO *) malloc(lenMs[i] * sizeof(COO));
        }
    }
    for (int i = 0; i < lenM; i++)
    {
        int iOrt = getIOrthant(M[i].x, M[i].y, base);
        Ms[iOrt][iMs[iOrt]++] = M[i];
    }
    bool has_sibling = (nEmpty != 1);
    for (int i = 0; i < nOrthants; i++) {
        createSPTree(iObj, 0, i, has_sibling, Ms[i], lenMs[i], getBase(base, i));
    }
    return nodelist[iObj][0];
}
void CTS::createSPTree(int iObj, int p, int index, bool has_sibling, COO* M, int lenM, Base base) {
    if (lenM == 0)
        return;

    if (base.len <= B)
    {
        nodelist[iObj][iNodelist[iObj]] = {base, M[0].val, {-1, -1, -1, -1}};
        nodelist[iObj][p].cPtr[index] = iNodelist[iObj]++;
        if (iNodelist[iObj] == sizeNodelist[iObj])
            doublingNodelist(iObj);
        return;
    }
    COO* Ms[nOrthants];
    int lenMs[nOrthants];
    int iMs[nOrthants];

    for (int iOrt = 0; iOrt < nOrthants; iOrt++) {
        Ms[iOrt] = NULL;
        lenMs[iOrt] = 0;
        iMs[iOrt] = 0;
    }
    for (int iM = 0; iM < lenM; iM++) {
        int iOrt = getIOrthant(M[iM].x, M[iM].y, base);
        lenMs[iOrt]++;
    }
    int nEmpty = 0;
    int iOrthant = -1;
    for (int i = 0; i < nOrthants; i++) {
        if (lenMs[i] > 0) {
            nEmpty++;
            iOrthant = i;
            Ms[i] = (COO *) malloc(lenMs[i] * sizeof(COO));
        }
    }
    for (int i = 0; i < lenM; i++)
    {
        int iOrt = getIOrthant(M[i].x, M[i].y, base);
        Ms[iOrt][iMs[iOrt]++] = M[i];
    }
    bool has_sibling_child = (nEmpty != 1);
    if (!has_sibling && !has_sibling_child)
        createSPTree(iObj, p, index, false, Ms[iOrthant], lenMs[iOrthant], getBase(base, iOrthant));
    else {
        nodelist[iObj][iNodelist[iObj]] = {base, 0, {-1, -1, -1, -1}};
        int currentI = iNodelist[iObj];
        nodelist[iObj][p].cPtr[index] = iNodelist[iObj]++;
        if (iNodelist[iObj] == sizeNodelist[iObj])
            doublingNodelist(iObj);
        for (int i = 0; i < nOrthants; i++)
            createSPTree(iObj, currentI, i, has_sibling_child, Ms[i], lenMs[i], getBase(base, i));
    }
}
/*
Return the root's index in the current level's array.
inodeX = root of the 1st tree.
nodeXl = the node to trace the base. always divides by 2
nodeZ = the parent node to be added at level array
level = current level of recursion.
*/

int CTS::tensorProduct(int iNodeX, Node nodeXl, int iNodeY, Node nodeYl, Node nodeZ, int level) {
    int res = -1;

    if (iNodeX == -1 || iNodeY == -1)
        return -1;

    int indexes[2] = {0, 0};
    int xs[nOrthants]; // child pointers of x matrix
    int ys[nOrthants];
    int zs[nOrthants];
    for (int i = 0; i < nOrthants; i++)
        xs[i] = nodelist[0][iNodeX].cPtr[i];
    for (int i = 0; i < nOrthants; i++)
        ys[i] = nodelist[1][iNodeY].cPtr[i];
    for (int i = 0; i < nOrthants; i++)
        zs[i] = -1;
    nodeZ.cPtr[0] = -1;
    nodeZ.cPtr[1] = -1;
    nodeZ.cPtr[2] = -1;
    nodeZ.cPtr[3] = -1;
    Node nodeZChildren[nOrthants] = {getNode(nodeZ, 0), getNode(nodeZ, 1), getNode(nodeZ, 2), getNode(nodeZ, 3)};

    if (nodelist[0][iNodeX].base.len == nodeXl.base.len && nodelist[1][iNodeY].base.len == nodeYl.base.len) {
        if (isLeaf(nodelist[0][iNodeX]) && isLeaf(nodelist[1][iNodeY])) {
            nodeZ.val = nodelist[0][iNodeX].val * nodelist[1][iNodeY].val;
//            std::cout << nodeZ.val << " multiplied from: " << iNodeX << ", " << iNodeY <<" at level " << level << '\n';
        }
        indexes[0] = (xs[0] == -1 || ys[0] == -1) ? -1 : tensorProduct(xs[0], getNextBox(0, xs[0], nodeXl), ys[0], getNextBox(1, ys[0], nodeYl), nodeZChildren[0], level + 1);
        indexes[1] = (xs[1] == -1 || ys[2] == -1) ? -1 : tensorProduct(xs[1], getNextBox(0, xs[1], nodeXl), ys[2], getNextBox(1, ys[2], nodeYl), nodeZChildren[0], level + 1);
        nodeZ.cPtr[0] = mergeWrapper(indexes[0], indexes[1], nodeZChildren[0], level);
        indexes[0] = (xs[0] == -1 || ys[1] == -1) ? -1 : tensorProduct(xs[0], getNextBox(0, xs[0], nodeXl), ys[1], getNextBox(1, ys[1], nodeYl), nodeZChildren[1], level + 1),
        indexes[1] = (xs[1] == -1 || ys[3] == -1) ? -1 : tensorProduct(xs[1], getNextBox(0, xs[1], nodeXl), ys[3], getNextBox(1, ys[3], nodeYl), nodeZChildren[1], level + 1),
        nodeZ.cPtr[1] = mergeWrapper(indexes[0], indexes[1], nodeZChildren[1], level);
        indexes[0] = (xs[2] == -1 || ys[0] == -1) ? -1 : tensorProduct(xs[2], getNextBox(0, xs[2], nodeXl), ys[0], getNextBox(1, ys[0], nodeYl), nodeZChildren[2], level + 1);
        indexes[1] = (xs[3] == -1 || ys[2] == -1) ? -1 : tensorProduct(xs[3], getNextBox(0, xs[3], nodeXl), ys[2], getNextBox(1, ys[2], nodeYl), nodeZChildren[2], level + 1);
        nodeZ.cPtr[2] = mergeWrapper(indexes[0], indexes[1], nodeZChildren[2], level);
        indexes[0] = (xs[2] == -1 || ys[1] == -1) ? -1 : tensorProduct(xs[2], getNextBox(0, xs[2], nodeXl), ys[1], getNextBox(1, ys[1], nodeYl), nodeZChildren[3], level + 1);
        indexes[1] = (xs[3] == -1 || ys[3] == -1) ? -1 : tensorProduct(xs[3], getNextBox(0, xs[3], nodeXl), ys[3], getNextBox(1, ys[3], nodeYl), nodeZChildren[3], level + 1);
        nodeZ.cPtr[3] = mergeWrapper(indexes[0], indexes[1], nodeZChildren[3], level);
    }
    else if (nodelist[0][iNodeX].base.len == nodeXl.base.len) {
        int iOrthant = getIOrthant(nodelist[1][iNodeY].base.x, nodelist[1][iNodeY].base.y, nodeYl.base);
            if (iOrthant == 0) {
                nodeZ.cPtr[0] = mergeWrapper((xs[0] == -1) ? -1 : tensorProduct(xs[0], getNextBox(0, xs[0], nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[0], level + 1), -1, nodeZChildren[0], level);
                nodeZ.cPtr[2] = mergeWrapper((xs[2] == -1) ? -1 : tensorProduct(xs[2], getNextBox(0, xs[2], nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[2], level + 1), -1, nodeZChildren[2], level);
            }
            else if (iOrthant == 1)
            {
                nodeZ.cPtr[1] = mergeWrapper((xs[0] == -1) ? -1 : tensorProduct(xs[0], getNextBox(0, xs[0], nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[1], level + 1), -1, nodeZChildren[1], level);
                nodeZ.cPtr[3] = mergeWrapper((xs[2] == -1) ? -1 : tensorProduct(xs[2], getNextBox(0, xs[2], nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[3], level + 1), -1, nodeZChildren[3], level);
            }
            else if (iOrthant == 2)
            {
                nodeZ.cPtr[0] = mergeWrapper((xs[1] == -1) ? -1 : tensorProduct(xs[1], getNextBox(0, xs[1], nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[0], level + 1), -1, nodeZChildren[0], level);
                nodeZ.cPtr[2] = mergeWrapper((xs[3] == -1) ? -1 : tensorProduct(xs[3], getNextBox(0, xs[3], nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[2], level + 1), -1, nodeZChildren[2], level);
            }
            else if (iOrthant == 3)
            {
                nodeZ.cPtr[1] = mergeWrapper((xs[1] == -1) ? -1 : tensorProduct(xs[1], getNextBox(0, xs[1], nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[1], level + 1), -1, nodeZChildren[1], level);
                nodeZ.cPtr[3] = mergeWrapper((xs[3] == -1) ? -1 : tensorProduct(xs[3], getNextBox(0, xs[3], nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[3], level + 1), -1, nodeZChildren[3], level);
            }
    }
    else if (nodelist[1][iNodeY].base.len == nodeYl.base.len) {
        int iOrthant = getIOrthant(nodelist[0][iNodeX].base.x, nodelist[0][iNodeX].base.y, nodeXl.base);
            if (iOrthant == 0) {
                nodeZ.cPtr[0] = mergeWrapper((ys[0] == -1) ? -1 : tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), ys[0], getNextBox(1, ys[0], nodeYl), nodeZChildren[0], level + 1), -1, nodeZChildren[0], level);
                nodeZ.cPtr[1] = mergeWrapper((ys[1] == -1) ? -1 : tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), ys[1], getNextBox(1, ys[1], nodeYl), nodeZChildren[1], level + 1), -1, nodeZChildren[1], level);
            }
            else if (iOrthant == 1)
            {
                nodeZ.cPtr[0] = mergeWrapper((ys[2] == -1) ? -1 : tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), ys[2], getNextBox(1, ys[2], nodeYl), nodeZChildren[0], level + 1), -1, nodeZChildren[0], level);
                nodeZ.cPtr[1] = mergeWrapper((ys[3] == -1) ? -1 : tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), ys[3], getNextBox(1, ys[3], nodeYl), nodeZChildren[1], level + 1), -1, nodeZChildren[1], level);
            }
            else if (iOrthant == 2)
            {
                nodeZ.cPtr[2] = mergeWrapper((ys[0] == -1) ? -1 : tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), ys[0], getNextBox(1, ys[0], nodeYl), nodeZChildren[2], level + 1), -1, nodeZChildren[2], level);
                nodeZ.cPtr[3] = mergeWrapper((ys[1] == -1) ? -1 : tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), ys[1], getNextBox(1, ys[1], nodeYl), nodeZChildren[3], level + 1), -1, nodeZChildren[3], level);
            }
            else if (iOrthant == 3)
            {
                nodeZ.cPtr[2] = mergeWrapper((ys[2] == -1) ? -1 : tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), ys[2], getNextBox(1, ys[2], nodeYl), nodeZChildren[2], level + 1), -1, nodeZChildren[2], level);
                nodeZ.cPtr[3] = mergeWrapper((ys[3] == -1) ? -1 : tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), ys[3], getNextBox(1, ys[3], nodeYl), nodeZChildren[3], level + 1), -1, nodeZChildren[3], level);
            }

    }
    else{
        Node xl = getNextBox(0, iNodeX, nodeXl);
        int iOrthantX = getIOrthant(nodelist[0][iNodeX].base.x, nodelist[0][iNodeX].base.y, nodeXl.base);
        Node yl = getNextBox(1, iNodeY, nodeYl);
        int iOrthantY = getIOrthant(nodelist[1][iNodeY].base.x, nodelist[1][iNodeY].base.y, nodeYl.base);
        if ((iOrthantX == 0 && iOrthantY == 0) || (iOrthantX == 1 && iOrthantY == 2))
            nodeZ.cPtr[0] = mergeWrapper(tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[0], level + 1), -1, nodeZChildren[0], level);
        else if ((iOrthantX == 0 && iOrthantY == 1) || (iOrthantX == 1 && iOrthantY == 3))
            nodeZ.cPtr[1] = mergeWrapper(tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[1], level + 1), -1, nodeZChildren[1], level);
        else if ((iOrthantX == 2 && iOrthantY == 0) || (iOrthantX == 3 && iOrthantY == 2))
            nodeZ.cPtr[2] = mergeWrapper(tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[2], level + 1), -1, nodeZChildren[2], level);
        else if ((iOrthantX == 2 && iOrthantY == 1) || (iOrthantX == 3 && iOrthantY == 3))
            nodeZ.cPtr[3] = mergeWrapper(tensorProduct(iNodeX, getNextBox(0, iNodeX, nodeXl), iNodeY, getNextBox(1, iNodeY, nodeYl), nodeZChildren[3], level + 1), -1, nodeZChildren[3], level);
    }

    int numNonNull = nOrthants;
    int iNonNull = -1;
    for (int iChild = 0; iChild < nOrthants; iChild++)
    {
        if (nodeZ.cPtr[iChild] == -1)
            numNonNull--;
        else
            iNonNull = iChild;
    }
    if (numNonNull == 0 && nodeZ.val == 0)
        return -1;
    else if (numNonNull == 1) {
        int indexOfOnlyOneNonNullChild = getIndexIfOnlyOneNonNullChild(&nodeZs[level][nodeZ.cPtr[iNonNull]]);
        if (indexOfOnlyOneNonNullChild > -1)
            nodeZ.cPtr[iNonNull] = indexOfOnlyOneNonNullChild; // iNonNull can be either 1st, 2nd, 3rd or 4th only one non-null child. NodeZ is returned at this level.
    }
    if (iZ[level] == sizeZ[level])
        doublingArray(&sizeZ[level], &nodeZs[level]);
    res = iZ[level];
    nodeZs[level][iZ[level]++] = nodeZ;
    return res;
}

/*
returns child
*/

int CTS::merge(int iX, int iY, Node nodeZ, int level) {
//    std:: cout << "indexes: (" << iX << ", " << iY << ");" << '\n';
//    std:: cout << "nodes in level " << level << '\n';
//    printNodelist(nodeZs[level], iZ[level]);
//    if(level < height - 1) {
//        std::cout << "nodes in level " << level + 1 << '\n';
//        printNodelist(nodeZs[level + 1], iZ[level + 1]);
//    }

    int res = -1;
    int iBegin = iZ[level];
    if (iX == -1 && iY == -1) {
        res = -1;
    }
    else if (iX == -1) {
        copyTree(iY, level);
        res = iBegin;
    }
    else if (iY == -1) {
        copyTree(iX, level);
        res = iBegin;
    }
    else {
        int iXNew = iX;
        int iYNew = iY;
        Base baseX = nodeZs[level + 1][iX].base;
        Base baseY = nodeZs[level + 1][iY].base;
        Node nodeZNew = nodeZ;
        if (baseX.len > baseY.len) {
            Node iter = nodeZ;
            int iOrtY = getIOrthant(baseY.x, baseY.y, iter.base);
            while (iter.base.len > baseX.len) {
                iter = getNode(iter, iOrtY);
                iOrtY = getIOrthant(baseY.x, baseY.y, iter.base);
            }
            if (iZ[level + 1] == sizeZ[level + 1])
                doublingArray(&sizeZ[level + 1], &nodeZs[level + 1]);
            iter.cPtr[iOrtY] = iY;
            iYNew = iZ[level + 1];
            nodeZs[level + 1][iZ[level + 1]++] = iter;
        }
        else if (baseX.len < baseY.len) {
            Node iter = nodeZ;
            int iOrtX = getIOrthant(baseX.x, baseX.y, iter.base);
            while (iter.base.len > baseY.len)
            {
                iter = getNode(iter, iOrtX);
                iOrtX = getIOrthant(baseX.x, baseX.y, iter.base);
            }
            if (iZ[level + 1] == sizeZ[level + 1])
                doublingArray(&sizeZ[level + 1], &nodeZs[level + 1]);
            iter.cPtr[iOrtX] = iX;
            iXNew = iZ[level + 1];
            nodeZs[level + 1][iZ[level + 1]++] = iter;
        } else {
            Node iter = nodeZ;
            int iOrtX = getIOrthant(baseX.x, baseX.y, iter.base);
            int iOrtY = getIOrthant(baseY.x, baseY.y, iter.base);
            while (iter.base.len > baseX.len && iter.base.len > baseY.len && iOrtX == iOrtY) {
                iter = getNode(iter, iOrtX);
                iOrtX = getIOrthant(baseX.x, baseX.y, iter.base);
                iOrtY = getIOrthant(baseY.x, baseY.y, iter.base);
            }

            if (iter.base.len == baseX.len) {
                if (iter.base.len == 1)
                    iter.val = nodeZs[level + 1][iX].val + nodeZs[level + 1][iY].val;
                else {
                    for (int i = 0; i < nOrthants; i++) {
                        iter.cPtr[i] = merge(nodeZs[level + 1][iX].cPtr[i], nodeZs[level + 1][iY].cPtr[i],
                                             getNode(iter, i), level);
                    }
                }
                if (iZ[level] == sizeZ[level])
                    doublingArray(&sizeZ[level], &nodeZs[level]);
                res = iZ[level];
                nodeZs[level][iZ[level]++] = iter;
                return res;
            }
            if (iter.base.len > baseX.len) {
                nodeZNew = iter;
                if (iZ[level + 1] == sizeZ[level + 1])
                    doublingArray(&sizeZ[level + 1], &nodeZs[level + 1]);
                Node newNodeX = iter;
                newNodeX.cPtr[iOrtX] = iX;
                iXNew = iZ[level + 1];
                nodeZs[level + 1][iZ[level + 1]++] = newNodeX;

                if (iZ[level + 1] == sizeZ[level + 1])
                    doublingArray(&sizeZ[level + 1], &nodeZs[level + 1]);
                Node newNodeY = iter;
                newNodeY.cPtr[iOrtY] = iY;
                iYNew = iZ[level + 1];
                nodeZs[level + 1][iZ[level + 1]++] = newNodeY;

            }
        }
//recursively merge 2 nodes at level+1 and put the merged nodes in level's array.
            int numNonNull = nOrthants;
            int iNonNull = -1;
            for (int iOrt = 0; iOrt < nOrthants; iOrt++) {
                nodeZNew.cPtr[iOrt] = merge(nodeZs[level + 1][iXNew].cPtr[iOrt], nodeZs[level + 1][iYNew].cPtr[iOrt],
                                         getNode(nodeZNew, iOrt), level);
                if (nodeZNew.cPtr[iOrt] == -1)
                    numNonNull--;
                else
                    iNonNull = iOrt;
            }
            if (numNonNull == 1) {
                int indexOfOnlyOneNonNullChild = getIndexIfOnlyOneNonNullChild(&nodeZs[level][nodeZNew.cPtr[iNonNull]]);
                if (indexOfOnlyOneNonNullChild > -1)
                    nodeZNew.cPtr[iNonNull] = indexOfOnlyOneNonNullChild;
                return nodeZNew.cPtr[iNonNull];
            }

        if (iZ[level] == sizeZ[level])
            doublingArray(&sizeZ[level], &nodeZs[level]);
        res = iZ[level];
        nodeZs[level][iZ[level]++] = nodeZNew;

        if (nodeZNew.base.len < nodeZ.base.len)
        {
            if (iZ[level] == sizeZ[level])
                doublingArray(&sizeZ[level], &nodeZs[level]);
            res = iZ[level];
            nodeZ.cPtr[getIOrthant(nodeZNew.base.x, nodeZNew.base.y, nodeZ.base)] = res;
            nodeZs[level][iZ[level]++] = nodeZ;
        }
    }
    return res;
}

int CTS::getIOrthant(int x, int y, Base base) {
    int iOrthant = 0;
    if (x >= base.x + base.len / 2)
        iOrthant += 1;
    if (y >= base.y + base.len / 2)
        iOrthant += 2;

    if (x < base.x || x > base.x + base.len || y < base.y || y > base.y + base.len)
        return -1;
    else
        return iOrthant;
}
void CTS::doublingNodelist(int iObj) {
    sizeNodelist[iObj] *= 2;
    Node* newNodelist = (Node*)realloc(nodelist[iObj], sizeNodelist[iObj] * sizeof(Node));
    if (newNodelist != NULL)
        nodelist[iObj] = newNodelist;
}
void CTS::doublingArray(int* size, Node** arr) {
    *size = *size * 2;
    Node* newNodelist = (Node*)realloc(*arr, *size * sizeof(Node));
    if (newNodelist != NULL)
        *arr = newNodelist;
}
CTS::Base CTS::getBase(Base base, int iOrt) {
    Base b = {};
    if (iOrt == 0)
        b = {base.x, base.y, base.len / 2};
    else if (iOrt == 1)
        b = {base.x + base.len / 2, base.y, base.len / 2};
    else if (iOrt == 2)
        b = {base.x, base.y + base.len / 2, base.len / 2};
    else if (iOrt == 3)
        b = {base.x + base.len / 2, base.y + base.len / 2, base.len / 2};
    return b;
}
CTS::Node CTS::getImmediateParent(int index1, int index2, int level, Node parent)
{
    Base baseX = nodeZs[level + 1][index1].base;
    Base baseY = nodeZs[level + 1][index2].base;
    Node iter = parent;
    Node res = parent;
    int iOrtX = getIOrthant(baseX.x, baseX.y, iter.base);
    int iOrtY = getIOrthant(baseY.x, baseY.y, iter.base);
    while (iter.base.len > baseX.len && iter.base.len > baseY.len && iOrtX == iOrtY)
    {
        iter = getNode(iter, iOrtX);
        if (!(iter.base.len > baseX.len && iter.base.len > baseY.len && iOrtX == iOrtY))
            return iter;
        iOrtX = getIOrthant(baseX.x, baseY.y, iter.base);
        iOrtY = getIOrthant(baseY.x, baseY.y, iter.base);
    }
    return iter;
}
CTS::Node CTS::getNextBox(int iObj, int iC, Node y) {
    if (iC == -1)
        return y;
    if (nodelist[iObj][iC].base.len == y.base.len / 2)
        return nodelist[iObj][iC];
    else
    {
        int iOrthant = getIOrthant(nodelist[iObj][iC].base.x, nodelist[iObj][iC].base.y, y.base);
        Node res = {getBase(y.base, iOrthant), 0, {-1, -1, -1, -1}};
        res.cPtr[iOrthant] = iC;
        return res;
    }
}
CTS::Node CTS::getNode(Node node, int child_idx) {
    Node res = {node.base, 0, {-1, -1, -1, -1}};
    if (child_idx == 0)
        res.base = {node.base.x, node.base.y, node.base.len / 2};
    else if (child_idx == 1)
        res.base = {node.base.x + node.base.len / 2, node.base.y, node.base.len / 2};
    else if (child_idx == 2)
        res.base = {node.base.x, node.base.y + node.base.len / 2, node.base.len / 2};
    else if (child_idx == 3)
        res.base = {node.base.x + node.base.len / 2, node.base.y + node.base.len / 2, node.base.len / 2};
    return res;
}
int CTS::getIndexIfOnlyOneNonNullChild(Node* node) {
    int num = nOrthants;
    int res = -1;
    for (int i = 0; i < nOrthants; i++)
    {
        if (node->cPtr[i] == -1)
            num--;
        else
            res = node->cPtr[i];
    }
    if (num == 1)
        return res;
    else
        return -1;
}
int CTS::mergeWrapper(int iX, int iY, Node nodeZ, int level) {
    int res = merge(iX, iY, nodeZ, level);
    iZ[level + 1] = 0; // iZ specifies the staring index of free space.
    return res;
};
void CTS::copyTree(int root, int level){
    if (iZ[level] == sizeZ[level])
        doublingArray(&sizeZ[level], &nodeZs[level]);
    nodeZs[level][iZ[level]] = nodeZs[level + 1][root];
    int indexRoot = iZ[level];
    iZ[level]++;

    for (int iChild = 0; iChild < nOrthants; iChild++){
        int indexChild = nodeZs[level][indexRoot].cPtr[iChild];
        if (indexChild != -1) {
            nodeZs[level][indexRoot].cPtr[iChild] = iZ[level];
            copyTree(indexChild, level);
        }
    }
}
bool CTS::isLeaf(Node node) {
    for (int i = 0; i < nOrthants; i++)
    {
        if (node.cPtr[i] > -1)
            return false;
    }
    return true;
}
void CTS::printNodelist(Node* nodelist, int len) {
    for (int i = 0; i < len; i++)
    {
        std::cout << i <<  ": (" << nodelist[i].base.x << ", " << nodelist[i].base.y << "， " << nodelist[i].base.len << "), " << nodelist[i].val << ", {" << nodelist[i].cPtr[0] << ", " << nodelist[i].cPtr[1] << ", " << nodelist[i].cPtr[2] << ", " << nodelist[i].cPtr[3] << "}" <<'\n';
        std::cout <<  '\n';
    }
}
void printMatrix(int matrix[32][32], int dimension) {
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++)
            std::cout << matrix[i][j] << '\t';
        std::cout <<  '\n';
    }
    std::cout <<  '\n';
}
void printCoo(CTS::COO* arr, int n){
    for (int i = 0; i < n; i++)
        std::cout << "{" << arr[i].x << ", " << arr[i].y << ", " << arr[i].val << "}," << '\n';
    std::cout <<  '\n';
}
void getOutput1(int matrix1[32][32], int matrix2[32][32], int dimension) {
    for (int i = 0; i < dimension; ++i) {
        for (int j = 0; j < dimension; ++j) {
            for (int k = 0; k < dimension; k++)
                output1[i][j] += matrix1[i][k] * matrix2[k][j];
        }
    }
}
void printOutput(int dimension)
{
    std::cout << "output1：" << '\n';
    for (int i = 0; i < dimension; ++i)
    {
        for (int j = 0; j < dimension; ++j)
            std::cout << output1[i][j] << '\t';
        std::cout <<  '\n';
    }
    std::cout <<  '\n';
    std::cout << "output2：" << '\n';
    for (int i = 0; i < dimension; i++)
    {
        for (int j = 0; j < dimension; j++) {
            if (output2[i][j] == output1[i][j])
                std::cout << output2[i][j] << '\t';
            else
                std::cout << "[" << output2[i][j] << "]" <<'\t';
        }
        std::cout <<  '\n';
    }
    std::cout <<  '\n';
}
void convertMatrix(CTS::Node* arr, int index) {
    if (index == -1)
        return;
    if (arr[index].cPtr[0] == -1 && arr[index].cPtr[1] == -1 && arr[index].cPtr[2] == -1 && arr[index].cPtr[3] == -1)
        output2[arr[index].base.y][arr[index].base.x] = arr[index].val;
    else {
        for (int i = 0; i < 4; i++) {
            if (arr[index].cPtr[i] > -1)
                convertMatrix(arr, arr[index].cPtr[i]);
        }
    }
}

int main() {
    srand (time(NULL));

    int dimension = 32; // could change for 2D
    int nOrthant = 4;
    int matrix1[32][32] = {0};
    int matrix2[32][32] = {0};

    int nCoo1 = 100;
    int nCoo2 = 150;

    CTS::COO cooMatrix1[nCoo1];
    CTS::COO cooMatrix2[nCoo2];
    int i = 0;
    while (i < nCoo1){
        cooMatrix1[i] = {(int)(rand() % dimension), (int)rand() % dimension, i+1};
        if (matrix1[cooMatrix1[i].y][cooMatrix1[i].x] == 0)
        {
            matrix1[cooMatrix1[i].y][cooMatrix1[i].x] = cooMatrix1[i].val;
            i++;
        }
    }
    i = 0;
    while (i < nCoo2){
        cooMatrix2[i] = {rand() % dimension, rand() % dimension, i + 1};
        if (matrix2[cooMatrix2[i].y][cooMatrix2[i].x] == 0)
        {
            matrix2[cooMatrix2[i].y][cooMatrix2[i].x] = cooMatrix2[i].val;
            i++;
        }
    }

    printMatrix(matrix1, dimension);
    printMatrix(matrix2, dimension);

    int n[2] = {nCoo1, nCoo2};
    CTS::COO* cooMatrix[2] = {cooMatrix1, cooMatrix2};
    CTS::Base base = {0, 0, dimension};
    CTS::Node output = {base, 0, {-1, -1, -1, -1}};

    int output1[dimension][dimension] = { 0 };
    int output2[dimension][dimension] = { 0 };

    /**
     * @output: matrix mulplication
     */
    getOutput1(matrix1, matrix2, dimension);

    /**
     * @output: CTS
     */
    CTS cts(dimension, 1, nOrthant, n, cooMatrix);
    cts.tensorProduct(0, cts.nodelist[0][0], 0, cts.nodelist[1][0], output, 0);
    convertMatrix(cts.nodeZs[0], cts.iZ[0] - 1);

    printOutput(dimension);

    return 0;
}
