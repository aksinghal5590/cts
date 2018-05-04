#ifndef CTS_HPP
#define CTS_HPP

#define ORTH 4
#define B 64

#include <vector>
#include <iostream>

class Base {

    public:

	   int x, y, len;

        Base() {
            x = 0;
            y = 0;
            len = 0;
        }

        Base(const Base& base) {
            x = base.x;
            y = base.y;
            len = base.len;
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
            std::cout << "(" << x << ", " << y << ", " << len << ")";
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
            val = NULL;
            cPtr = NULL;
            parent = -1;
            offset = 0;
        }

        Node(const Node& node) {
            base = node.base;
            parent = node.parent;
            offset = node.offset;
            if(node.val != NULL) {
                val = new double[B * B];
                for(int i = 0; i < B * B; i++) {
                    val[i] = node.val[i];
                }
            } else {
                val = NULL;
            }
            cPtr = new int[ORTH];
            for(int i = 0; i < ORTH; i++) {
                cPtr[i] = node.cPtr[i];
            }
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
            std::cout << "(";
            for(int i = 0; i < ORTH; i++) {
                std::cout << cPtr[i];
                if(i < ORTH-1) {
                    std::cout << ", ";
                }
            }
            std::cout << ")(" << parent << ")";
            std::cout << "(" << offset << ")";
            if(val != NULL) {
                std::cout << "(";
                for(int i = 0; i < B; i++) {
                    for(int j = 0; j < B; j++) {
                        std::cout << val[i*B + j] << ",";
                    }
                }
                std::cout << ")";
            }
            std::cout << std::endl;
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

        std::vector<Node> tree;

        int createSPTree(int idx, bool has_sibling, Coo* M, int lenM, Base& base, int parent);

        void merge(std::vector<Node>& tree1, std::vector<Node>& tree2, const bool isMultiply, const int trueNodePos);

        int mergeNodes(std::vector<Node>& tree1, std::vector<Node>& tree2, const int pos1, const int pos2, const int parent);

        void multiply(Sptree& tempA, Sptree& tempB, std::vector<Node>& tree1, std::vector<Node>& tree2,
            const int pos1, const int pos2, const int parentPos, const int trueNodePos);

        void multiplyParts(std::vector<Node>& tree1, std::vector<Node>& tree2, const int pos1, const int pos2,
            const int parentPos, const int xPos, const int yPos, const int trueNodePos);

        int multiplyNodes(std::vector<Node>& tree1, std::vector<Node>& tree2, const int pos1, const int pos2,
            const int parent, const int orthant, const int trueNodePos);

    public:

        Sptree() {
        }

        Sptree(std::vector<Node> tree) {
            this->tree = tree;
        }

        std::vector<Node>& getTree() {
            return tree;
        }

        void printValues() {
            std::cout << "(" << tree.size() << ") ";
            for(std::vector<Node>::iterator it = tree.begin(); it != tree.end(); it++) {
                it->printValues();
                std::cout << "  ";
            }
            std::cout << std::endl;
        }

        void multiply(std::vector<Node>& tree1, std::vector<Node>& tree2);

        void createCTS(Coo* M, int lenM, Base& base);

        void merge(std::vector<Node>& tree1, std::vector<Node>& tree2) {
            merge(tree1, tree2, false, 0);
        }

        void clear() {
            tree.clear();
        }
};

int getIOrthant(int x, int y, Base base);

Base getBase(Base base, int iOrt);


#endif
