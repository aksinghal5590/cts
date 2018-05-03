#ifndef CTS_HPP
#define CTS_HPP

#define ORTH 4
#define B 64

#include <vector>
#include <iostream>

using namespace std;

class Base {

    public:

	   int x, y, len;

        Base() {
            x = 0;
            y = 0;
            len = 0;
        }

        Base(int x, int y, int len) {
            this->x = x;
            this->y = y;
            this->len = len;
        }

        bool operator==(const Base& base) {
            if(x == base.x && y == base.y && len == base.len) {
               return true;
            }
            return false;
        }

        void printValues() {
            cout << "(" << x << ", " << y << ", " << len << ")";
        }

        int getIOrthant(int x, int y) {
            int iOrthant = 0;
            if (y >= this->y + this->len / 2)
                iOrthant += 1;
            if (x >= this->x + this->len / 2)
                iOrthant += 2;
            if (x < this->x || x > this->x + this->len || y < this->y || y > this->y + this->len)
                return -1;
            else
                return iOrthant;
        }

        Base getBase(int iOrt) {
            Base newBase;
            if (iOrt == 0) {
                newBase.x = x;
                newBase.y = y;
                newBase.len = len/2;
            } else if (iOrt == 1) {
                newBase.x = x;
                newBase.y = y + len/2;
                newBase.len = len/2;
            } else if (iOrt == 2) {
                newBase.x = x + len/2;
                newBase.y = y;
                newBase.len = len/2;
            } else if (iOrt == 3) {
                newBase.x = x + len/2;
                newBase.y = y + len/2;
                newBase.len = len/2;
            }
            return newBase;
        }
};

class Coo {

    public:

	int x, y;
    double val;
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

        Node(Base base, double* val, int* cPtr) {
            this->base = base;
            this->parent = -1;
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

        bool operator==(const Node& node) {
            if(base == node.base) {
               return true;
            }
            return false;
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
            if(val != NULL) {
                cout << "(";
                for(int i = 0; i < B; i++) {
                    for(int j = 0; j < B; j++) {
                        cout << val[i*B + j] << ",";
                    }
                }
                cout << ")";
            }
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

        int createSPTree(int idx, bool has_sibling, Coo* M, int lenM, Base& base, int parent);

        void merge(vector<Node>& tree1, vector<Node>& tree2, const bool isMultiply, const int trueNodePos);

        int mergeNodes(vector<Node>& tree1, vector<Node>& tree2, const int pos1, const int pos2, const int parent);

        void multiply(Sptree& tempA, Sptree& tempB, vector<Node>& tree1, vector<Node>& tree2,
            const int pos1, const int pos2, const int parentPos, const int trueNodePos);

        void multiplyParts(vector<Node>& tree1, vector<Node>& tree2, const int pos1, const int pos2,
            const int parentPos, const int xPos, const int yPos, const int trueNodePos);

        int multiplyNodes(vector<Node>& tree1, vector<Node>& tree2, const int pos1, const int pos2,
            const int parent, const int orthant, const int trueNodePos);

    public:

        Sptree() {
        }

        Sptree(vector<Node> tree) {
            this->tree = tree;
        }

        vector<Node>& getTree() {
            return tree;
        }

        void printValues() {
            cout << "(" << tree.size() << ") ";
            for(vector<Node>::iterator it = tree.begin(); it != tree.end(); it++) {
                it->printValues();
                //cout << it->val;
                cout << "  ";
                /*cout << "(";
                //cout << it->parent << ")(";
                for(int i = 0; i < ORTH; i++) {
                    cout << it->cPtr[i];
                    if(i < ORTH-1)
                        cout << ",";
                }
                cout << ")  ";*/
            }
            cout << endl;
        }

        void multiply(vector<Node>& tree1, vector<Node>& tree2);

        void createCTS(Coo* M, int lenM, Base& base);

        void merge(vector<Node>& tree1, vector<Node>& tree2) {
            merge(tree1, tree2, false, 0);
        }

        void clear() {
            tree.clear();
        }
};

int getIOrthant(int x, int y, Base base);

Base getBase(Base base, int iOrt);


#endif
