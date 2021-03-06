#ifndef CENTRALSTACK_H
#define CENTRALSTACK_H

#include <iostream>
#include <atomic>
#include <thread>

using namespace std;

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
            if(TryPerformStackOp(p) == false)   {
                ElimOp(p);
            }

            return;

        };


        // Try to perform operation on the main stack
        bool TryPerformStackOp(ThreadInfo *p)   {
            Node *top, *next;

            if(p->op == 'U')    {
                top = head.load();
                p->node->next = top;
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
            return false;
        };


        void ElimOp(ThreadInfo *p)  {
            int myID = p->id;
            location[myID].store(p);

            int pos = getPos(p);

            int him = collision[pos].load();

            while(!collision[pos].compare_exchange_weak(him, myID))    {
                him = collision[pos].load();
            }

            if(him != 0)    {
                ThreadInfo *q = location[him].load();
                 if(q!=nullptr && q->id==him && q->op!=p->op) {
                    if(location[myID].compare_exchange_weak(p, nullptr))  {
                        if(TryCollision(p, q))  {
                            return;
                        }
                        else    {
                            StackOp(p);
                            return;
                        }
                    }
                    else    {
                        FinishCollisions(p);
                        return;
                    }

                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            //Adapt(p, 0);

            if(!(location[myID].compare_exchange_weak(p, nullptr))) {

                FinishCollisions(p);
                return;
            }
            StackOp(p);
            return;
        };


        // Attempt a collision
        // NEED TO FINISH
        bool TryCollision(ThreadInfo *p, ThreadInfo *q) {
            if(p->op == 'U')    {
                if(location[q->id].compare_exchange_weak(q,p))
                    return true;
                else    {
                    //Adapt(p, 1);
                    return false;
                }
            }
            if(p->op == 'O')    {
                if(location[q->id].compare_exchange_weak(q, nullptr)) {
                    p->node = q->node;
                    location[p->id].store(nullptr);
                    return true;
                }
                else{
                    //Adapt(p, 1);
                    return false;
                }
            }
            return false;
        };


        // Finish a collision
        void FinishCollisions(ThreadInfo *p)    {

            if(p->op == 'O')    {
                p->node = location[p->id].load()->node;
                location[p->id].store(nullptr);

            }
        };


        int getPos(ThreadInfo *p)   {

            int random = rand() % 32;
            return random;
        };


        void Adapt(ThreadInfo *p, int direction)    {
            if(direction == 0)   {
                if(p->adapt->width == 0)  {
                    p->adapt->width = 5;
                    p->adapt->factor = p->adapt->factor/2;
                    if(p->adapt->factor < 0.125)
                        p->adapt->factor = 0.125;
                }
                else
                    p->adapt->width--;
            }
            if(direction == 1)  {
                if(p->adapt->width == 5)  {
                    p->adapt->width = 0;
                    p->adapt->factor = p->adapt->factor*2;
                    if(p->adapt->factor > 1)
                        p->adapt->factor = 1;
                }
            }

            return;
        };


};

CentralStack::CentralStack(int stackSize)    {
    head = nullptr;
    location = new std::atomic<ThreadInfo*>[stackSize];
    for(int i = 0; i < stackSize; i++)  {
        location[i].store(nullptr);
    }
    collision = new std::atomic<int>[stackSize];
    for(int i = 0; i < stackSize; i++)  {
        collision[i].store(0);
    }

    stackLength = stackSize;
}

CentralStack::~CentralStack(void)    {
    delete[] location;
    delete[] collision;
}


#endif // CENTRALSTACK_H
