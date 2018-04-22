#include "CentralStack.h"

using namespace std;

int main()
{

    CentralStack S(32);

    int a = 1;
    int b = 2;
    int c = 3;

    ThreadInfo *firstThread = S.genThreadInfo(1, 'U', &a);
    ThreadInfo *secondThread = S.genThreadInfo(2, 'O', &a);
    ThreadInfo *thirdThread = S.genThreadInfo(3, 'U', &b);
    ThreadInfo *fourthThread = S.genThreadInfo(4, 'O', &b);
    ThreadInfo *fifthThread = S.genThreadInfo(5, 'U', &c);
    ThreadInfo *sixthThread = S.genThreadInfo(6, 'O', &c);

    std::thread first (&CentralStack::StackOp, &S, firstThread);
    std::thread second (&CentralStack::StackOp, &S, secondThread);
    std::thread third (&CentralStack::StackOp, &S, thirdThread);
    std::thread fourth (&CentralStack::StackOp, &S, fourthThread);
    std::thread fifth (&CentralStack::StackOp, &S, fifthThread);
    std::thread sixth (&CentralStack::StackOp, &S, sixthThread);

    first.join();
    second.join();
    third.join();
    fourth.join();
    fifth.join();
    sixth.join();

    if(S.head.load() == nullptr)    {
        cout << "No head.\n";
    }
    else    {
        cout << *(int*) S.head.load()->data << "\n";
    }
}
