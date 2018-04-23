#include "CentralStack.h"
#include <iostream>
#include <ctime>

using namespace std;

void runThreads(CentralStack *S, ThreadInfo **threadList, int listSize)   {

    for(int i = 0; i < listSize; i++)   {
        S->StackOp(threadList[i]);
    }

}

int main()
{
    CentralStack *S;
    S = new CentralStack(32);
    ThreadInfo **listOne;
    ThreadInfo **listTwo;
    ThreadInfo **listThree;
    ThreadInfo **listFour;
    ThreadInfo **listFive;
    ThreadInfo **listSix;
    ThreadInfo **listSeven;
    ThreadInfo **listEight;

    int listSize = 62500;
    int a = 5;

    listOne = new ThreadInfo*[listSize];
    listTwo = new ThreadInfo*[listSize];
    listThree = new ThreadInfo*[listSize];
    listFour = new ThreadInfo*[listSize];
    listFive = new ThreadInfo*[listSize];
    listSix = new ThreadInfo*[listSize];
    listSeven = new ThreadInfo*[listSize];
    listEight = new ThreadInfo*[listSize];

    for(int i = 0; i < listSize; i++)   {
        listOne[i] = S->genThreadInfo(1, 'U', &a);
        listTwo[i] = S->genThreadInfo(2, 'O', &a);
        listThree[i] = S->genThreadInfo(3, 'U', &a);
        listFour[i] = S->genThreadInfo(4, 'O', &a);
        listFive[i] = S->genThreadInfo(5, 'U', &a);
        listSix[i] = S->genThreadInfo(6, 'O', &a);
        listSeven[i] = S->genThreadInfo(7, 'U', &a);
        listEight[i] = S->genThreadInfo(8, 'O', &a);
    }


    clock_t t = clock();

    std::thread testOne(runThreads, S, listOne, listSize);
    std::thread testTwo(runThreads, S, listTwo, listSize);
    std::thread testThree(runThreads, S, listOne, listSize);
    std::thread testFour(runThreads, S, listTwo, listSize);
    std::thread testFive(runThreads, S, listFive, listSize);
    std::thread testSix(runThreads, S, listSix, listSize);
    std::thread testSeven(runThreads, S, listSeven, listSize);
    std::thread testEight(runThreads, S, listEight, listSize);

    testOne.join();
    testTwo.join();
    testThree.join();
    testFour.join();
    testFive.join();
    testSix.join();
    testSeven.join();
    testEight.join();

    t = clock() - t;
    cout << ((float)t)/CLOCKS_PER_SEC;
}


