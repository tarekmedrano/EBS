#ifndef CENTRALSTACK_H
#define CENTRALSTACK_H

// Used structures
struct Node {
    Node *pnext;
    void *pdata;
};

struct AdaptParams  {
    int width;
    float factor;
};

struct ThreadInfo   {
    unsigned int id;
    // In the pseudo code they use char arrays, but
    //  a single char should suffice, U for push and O for pop.
    char op;
    Node *node;
    AdaptParams *adapt;
};

class CentralStack {
    public:
        Node *phead;
        ThreadInfo *location;
        int *collision;

        // Constructor and Destructor
        CentralStack(int stackSize);
        ~CentralStack();

        //Function declaration
        void StackOp(ThreadInfo *p);
        bool TryPerformStackOp(ThreadInfo *p);
        void ElimOp(ThreadInfo *p);
        void TryCollision(ThreadInfo *p, ThreadInfo *q);
        void FinishCollisions(ThreadInfo *p);


};

CentralStack::CentralStack(int stackSize)    {
    phead = nullptr;
    location = new ThreadInfo[stackSize];
    collision = new int[stackSize];
}

CentralStack::~CentralStack(void)    {
    delete phead;
    delete[] location;
    delete[] collision;
}

#endif // CENTRALSTACK_H
