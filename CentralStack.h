#ifndef CENTRALSTACK_H
#define CENTRALSTACK_H

#include <atomic>
#include <thread>

// Used structures
struct Node {
    Node *next; // Perform CAS
    void *data;

    Node()  {
        next = nullptr;
        data = nullptr;
    }
};

struct AdaptParams  {
    int width;
    float factor;

    AdaptParams()   {
        width = 0;
        factor = 0.5;
    }
};

struct ThreadInfo   {
    int id;
    char op;
    Node *node;
    AdaptParams *adapt;

    ThreadInfo()    {
        id = 0;
        op = 'U';
        node = nullptr;
        adapt = nullptr;
    }
};

class CentralStack {
    public:
        std::atomic<Node*> head;
        std::atomic<ThreadInfo*> *location;
        std::atomic<int> *collision;
        int stackLength;

        // Constructor and Destructor
        CentralStack(int stackSize);
        ~CentralStack();

        //Generate a ThreadInfo Struct
        ThreadInfo *genThreadInfo(int threadID, char operation, void *nodeData)   {
            ThreadInfo *newThread = new ThreadInfo();

            newThread->id = threadID;
            newThread->op = operation;

            if(operation == 'U')    {
                Node *newNode = new Node();
                newNode->data = nodeData;
                newThread->node = newNode;
            }

            AdaptParams *newAdapt = new AdaptParams();
            newThread->adapt = newAdapt;

            return newThread;
        };

        // The function that is called when trying to perform an operation
        void StackOp(ThreadInfo *p) {
            if(TryPerformStackOp(p) == false)
                ElimOp(p);
            return;

        };


        // Try to perform operation on the main stack
        bool TryPerformStackOp(ThreadInfo *p)   {
            Node *top, *next;

            if(p->op == 'U')    {
                top = head.load();
                if(head.compare_exchange_weak(top, p->node)) {
                    return true;
                }
                else
                    return false;
            }

            if(p->op == 'O')    {
                top = head.load();
                if(top == nullptr)  {
                    p->node = nullptr;
                    return true;
                }
                next = top->next;
                if(head.compare_exchange_weak(top, next))   {
                    p->node = top;
                    return true;
                }
                else
                    return false;
            }
        };


        // Try to use the elimination array
        // NEED TO FINISH
        void ElimOp(ThreadInfo *p)  {

            location[p->id].store(p);
            // NEED TO MAKE A RANDOM Function for this
            int pos = 5;

            int him = collision[pos].load();

            while(!collision[pos].compare_exchange_weak(him, p->id))    {
                him = collision[pos].load();
            }

        };


        // Attempt a collision
        // NEED TO FINISH
        void TryCollision(ThreadInfo *p, ThreadInfo *q) {

        };


        // Finish a collision
        void FinishCollisions(ThreadInfo *p)    {
            if(p->op == 'O')    {
                p->node = location[p->id].load()->node;
                location[p->id] = nullptr;
            }
        };


};

CentralStack::CentralStack(int stackSize)    {
    head = nullptr;
    location = new std::atomic<ThreadInfo*>[stackSize];
    collision = new std::atomic<int>[stackSize];
    stackLength = stackSize;
}

CentralStack::~CentralStack(void)    {
    delete[] location;
    delete[] collision;
}


#endif // CENTRALSTACK_H
