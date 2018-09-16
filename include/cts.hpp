#ifndef CTS_HPP
#define CTS_HPP

#include <vector>
#include <iostream>

using namespace std;

extern int B;
extern int ORTH;

// coordinate format
class Coo {

    public:
        int x, y;
        double val;
};

//Matrix format to process input
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

//Compressed Sparse Row format
class Csr {

    public:

        vector<double> vals;
        vector<int> idx;
        vector<int> iCount;

        Csr() {}

        /*
        TODO: the initialization step can be removed from the constructor, only defining the size is enough
        but this would also require change in the multiplication step
        */
        Csr(int valSize, int idxSize, int iCountSize) : vals(valSize, 0), idx(idxSize, 0), iCount(iCountSize, 0) {
        }

        void operator = (const Csr& csr) {
            this->vals = csr.vals;
            this->idx = csr.idx;
            this->iCount = csr.iCount;
        }
};

//A base object defines a quadrant of a matrix
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

        //Returns the quadrant which the point(x, y) belongs to inside the current base
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

        //Returns a sub-base of the current base as per the orthant(iOrt) value
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
        Csr csr;
        int* cPtr;

        Node() {
            Base base;
            cPtr = NULL;
        }

        Node(const Node& node) {
            this->base = node.base;
            this->csr = node.csr;
            this->cPtr = new int[ORTH];
            for(int i = 0; i < ORTH; i++) {
                this->cPtr[i] = node.cPtr[i];
            }
        }

        Node(Base base, int* cPtr) {
            this->base = base;
            this->cPtr = new int[ORTH];
            for(int i = 0; i < ORTH; i++) {
                this->cPtr[i] = cPtr[i];
            }
        }

        Node(Base base, Csr csr, int* cPtr) {
            this->base = base;
            this->csr = csr;
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
            this->csr = node.csr;
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
            if(base.len <= B)
                cout << ")(" << csr.iCount[B] << ")";
            else
                cout << ")";
            cout << endl;
        }

        ~Node() {
            if(cPtr != NULL) {
                delete[] cPtr;
            }
        }
};

class Sptree {

        bool isAssemble;

        vector<Node> tree;

        /*
        Creates a list of the distinct non-zero positions(x,y) of the resultant tree.
        srcCsr1 - CSR of input tree 1
        srcCSR2 - CSR of input tree 2
        x - the current quadrant's x-coordinate
        y - the current quadrant's y-coordinate
        */
        void assemble(const Csr& srcCsr1, const Csr& srcCsr2, int x, int y);

        /*
        Creates the tree from the given parameters, it is assumed the tree is empty before creation
        idx - index of the current node in the tree
        has_sibling - true if the current node has a sibling; false otherwise
        M - base pointer of an array of the non-zero elements
        lenM - size of M
        base - size of the square matrix to be created
        parent - index of the parent node of the new node which is being created, -1 if root node
        */
        int createSPTree(int idx, bool has_sibling, Coo* M, int lenM, Base base, int parent);

        /*
        wrapper function to call multParts with all positions as 0
        */
        void multVectors(const vector<Node>& tree1, const vector<Node>& tree2);

        /*
        wrapper function to make 2 calls to multNodes
        tree1, tree2 - input trees as vectors
        pos1 - position of the first tree's node to be multiplied
        pos1 - position of the second tree's node to be multiplied
        pos - position of the new node in the resultant tree
        */
        void multParts(const vector<Node>& tree1, const vector<Node>& tree2,
            const int pos1, const int pos2, const int pos);

        /*
        Checks which child nodes of the unput nodes are to be multiplied
        tree1, tree2 - input trees as vectors
        pos1 - position of the first tree's node to be multiplied
        pos1 - position of the second tree's node to be multiplied
        xpos, ypos - values to identify which child nodes to multiply
        pos - position of the new node in the resultant tree
        */
        void multNodes(const vector<Node>& tree1, const vector<Node>& tree2, const int pos1, const int pos2,
            const int xPos, const int yPos, const int pos);

        /*
        Identifies if the input nodes are leaf nodes; if they are actually multiply them, else call multParts
        tree1, tree2 - input trees as vectors
        pos1 - position of the first tree's node to be multiplied
        pos1 - position of the second tree's node to be multiplied
        pos - position of the new node in the resultant tree
        */
        void multLeaves(const vector<Node>& tree1, const vector<Node>& tree2, const int pos1, const int pos2,
            const int pos);

    public:

        Sptree() {
        }

        Sptree(const vector<Node> tree) {
            this->tree = tree;
        }

        vector<Node> getTree() const {
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

        /*
        Function to be called publicly to multiply two trees
        spTree1, spTree2 - input tree
        base - to provide size of the resultant matrix
        */
        void multiply(const Sptree& spTree1, const Sptree& spTree2, Base base);

        /*
        Recursive function to create a tree from input coo array
        */
        void createCTS(Coo* M, int lenM, Base base);

        void clear() {
            tree.clear();
        }
};

int getIOrthant(int x, int y, Base base);

Base getBase(Base base, int iOrt);


#endif
