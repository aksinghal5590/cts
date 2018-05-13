#ifndef CTS_HPP
#define CTS_HPP

#define ORTH 4
#define B 128

#include <vector>
#include <iostream>

using namespace std;

class Coo {

    public:
        int x, y;
        double val;
};

class Mtx {

    public:

        int x;
        int y;
        double val;

        Mtx(int x, int y, double val) : x(x), y(y), val(val) {
        }

        bool operator < (const Mtx& mtx) {
            if(x == mtx.x) {
                return(y < mtx.y);
            } else {
                return(x < mtx.x);
            }
        }
};

class Base {

    public:

    int x, y, len;

        Base() {
            x = 0;
            y = 0;
            len = 0;
        }

        Base(int x, int y, int len) : x(x), y(y), len(len) {
        }

        Base(const Base& base) {
            this->x = base.x;
            this->y = base.y;
            this->len = base.len;
        }

        void operator = (const Base& base) {
            this->x = base.x;
            this->y = base.y;
            this->len = base.len;
        }

        bool operator == (const Base& base) {
            if(x == base.x && y == base.y && len == base.len) {
               return true;
            }
            return false;
        }

        void printValues() {
            cout << "(" << x << ", " << y << ", " << len << ")";
        }

        int getIOrthant(const int x, const int y) {
            if ((x < this->x) || (x >= (this->x + this->len)) || (y < this->y) || (y >= (this->y + this->len))) {
                cout << "Input: " << x << ", " << y << " Base: ";
                printValues();
                cout << endl;
                return -1;
            }
            int iOrthant = 0;
            if (y >= this->y + this->len/2)
                iOrthant += 1;
            if (x >= this->x + this->len/2)
                iOrthant += 2;

            return iOrthant;
        }

        Base getBase(const int iOrt) {
            if (iOrt == 0)
                return {x, y, len/2};
            else if (iOrt == 1)
                return {x, y + len/2, len/2};
            else if (iOrt == 2)
                return {x + len/2, y, len/2};
            else if (iOrt == 3)
                return {x + len/2, y + len/2, len/2};
            else {
                cout << "Input: " << iOrt << " Base: ";
                printValues();
                cout << endl;
                exit(1);
            }
        }
};

class Node {

    public:

        Base base;
        double* val;
        int* cPtr;
        int parent;
        int offset;

        Node() {
            Base base;
            val = NULL;
            cPtr = NULL;
            parent = -1;
            offset = 0;
        }

        Node(const Node& node) {
            this->base = node.base;
            this->parent = node.parent;
            this->offset = node.offset;
            if(node.val != NULL) {
                this->val = new double[B * B];
                for(int i = 0; i < B * B; i++) {
                    this->val[i] = node.val[i];
                }
            } else {
                this->val = NULL;
            }
            this->cPtr = new int[ORTH];
            for(int i = 0; i < ORTH; i++) {
                this->cPtr[i] = node.cPtr[i];
            }
        }

        Node(Base base, double* val, int* cPtr, int parent) {
            this->base = base;
            this->parent = parent;
            this->offset = 0;
            if(val != NULL) {
                this->val = new double[B * B];
                for(int i = 0; i < B * B; i++) {
                    this->val[i] = val[i];
                }
            } else {
                this->val = NULL;
            }
            this->cPtr = new int[ORTH];
            for(int i = 0; i < ORTH; i++) {
                this->cPtr[i] = cPtr[i];
            }
        }

        Node(Base base, double* val, int* cPtr, int parent, int offset) {
            this->base = base;
            this->parent = parent;
            this->offset = offset;
            if(val != NULL) {
                this->val = new double[B * B];
                for(int i = 0; i < B * B; i++) {
                    this->val[i] = val[i];
                }
            } else {
                this->val = NULL;
            }
            this->cPtr = new int[ORTH];
            for(int i = 0; i < ORTH; i++) {
                this->cPtr[i] = cPtr[i];
            }
        }

        bool operator == (const Node& node) {
            if(base == node.base) {
               return true;
            }
            return false;
        }

        void operator = (const Node& node) {
            this->base = node.base;
            this->parent = node.parent;
            this->offset = node.offset;
            if(node.val != NULL) {
                this->val = new double[B * B];
                for(int i = 0; i < B * B; i++) {
                    this->val[i] = node.val[i];
                }
            } else {
                this->val = NULL;
            }
            this->cPtr = new int[ORTH];
            for(int i = 0; i < ORTH; i++) {
                this->cPtr[i] = node.cPtr[i];
            }
        }

        void printValues() {
            base.printValues();
            cout << "(";
            for(int i = 0; i < ORTH; i++) {
                cout << cPtr[i];
                if(i < ORTH-1) {
                    cout << ", ";
                }
            }
            cout << ")(" << parent << ")";
            cout << "(" << offset << ")";
            /*if(val != NULL) {
                cout << "(";
                for(int i = 0; i < B; i++) {
                    for(int j = 0; j < B; j++) {
                        cout << val[i*B + j] << ",";
                    }
                }
                cout << ")";
            }*/
            cout << endl;
        }

        ~Node() {
            if(val != NULL) {
                delete[] val;
            }
            if(cPtr != NULL) {
                delete[] cPtr;
            }
        }
};

class Sptree {

        vector<Node> tree;

        int createSPTree(int idx, bool has_sibling, Coo* M, int lenM, Base base, int parent);

        void mergeSptrees(const vector<Node>& tree1, const vector<Node>& tree2, const bool isMultiply, const int trueNodePos);

        int mergeTrees(const vector<Node>& tree1, const vector<Node>& tree2, const int pos1, const int pos2, const int parent);

        int appendNode(const Base& base, const int trueNodePos, const int parent);

        void multiplySptrees(Sptree& tempA, Sptree& tempB, const vector<Node>& tree1, const vector<Node>& tree2,
            const int pos1, const int pos2, const int parentPos, const int trueNodePos);

        void multiplyParts(const vector<Node>& tree1, const vector<Node>& tree2, const int pos1, const int pos2,
            const int parentPos, const int xPos, const int yPos, const int trueNodePos);

        int multiplyTrees(const vector<Node>& tree1, const vector<Node>& tree2, const int pos1, const int pos2,
            const int parent, const int orthant, const int trueNodePos);

    public:

        Sptree() {
        }

        Sptree(const vector<Node> tree) {
            this->tree = tree;
        }

        vector<Node>& getTree() {
            return tree;
        }

        void printValues() {
            cout << "(" << tree.size() << ") ";
            for(vector<Node>::iterator it = tree.begin(); it != tree.end(); it++) {
                it->printValues();
                cout << "  ";
            }
            cout << endl;
        }

        void multiply(const vector<Node>& tree1, const vector<Node>& tree2);

        void createCTS(Coo* M, int lenM, Base base);

        void merge(const vector<Node>& tree1, const vector<Node>& tree2) {
            mergeSptrees(tree1, tree2, false, 0);
        }

        void clear() {
            tree.clear();
        }
};

int getIOrthant(int x, int y, Base base);

Base getBase(Base base, int iOrt);


#endif
