#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semaphore.h"
 
using namespace std;
 
const int BUFFSIZE = 8; // array of 8 elements, they are correlative to 8 accounts
 
// 8 semaphores is used for 8 accounts
enum {SAVING, CHECKING, MONEY_MARKET, IRA, CREDIT_CARD, AUTO, EQUITY_LINE, HOME_MORTGAGE}; 
 
// function declaration
void bankOperation(SEMAPHORE &sem, float *shmBUF, int &countOperation);
void depositToSavingItself(SEMAPHORE &sem, float *shmBUF, int &countOperation);
void depositToIRAItself(SEMAPHORE &sem, float *shmBUF, int &countOperation);
void depositToChecking(SEMAPHORE &sem, float *shmBUF, int &countOperation);
void transferEquitylineToChecking(SEMAPHORE &sem, float *shmBUF, int &countOperation);
void withdrawFromChecking(SEMAPHORE &sem, float *shmBUF, int &countOperation);
void depositToIRA(SEMAPHORE &sem, float *shmBUF, int &countOperation);
void transferCheckingToAuto(SEMAPHORE &sem, float *shmBUF, int &countOperation);
void transferCheckingToEquityLine(SEMAPHORE &sem, float *shmBUF, int &countOperation);
void transferCheckingToHomeMortgage(SEMAPHORE &sem, float *shmBUF, int &countOperation);
void parent_cleanup(SEMAPHORE &sem, int shmid);
 
int main(){
    int shmid;
    float *shmBUF;      // pointer points to shared memory segment
    long childPid;
    int countOperation = 1; // how many transaction of a process
 
    // 8 semaphores, initialize value to 1
    SEMAPHORE sem(8);
    sem.V(SAVING);
    sem.V(CHECKING);
    sem.V(MONEY_MARKET);
    sem.V(IRA);
    sem.V(CREDIT_CARD);
    sem.V(AUTO);
    sem.V(EQUITY_LINE);
    sem.V(HOME_MORTGAGE);
 
    shmid = shmget(IPC_PRIVATE, BUFFSIZE*sizeof(float &), PERMS); // allocates a shared memory segment
    shmBUF = (float *)shmat(shmid, 0, SHM_RND); // point pointer to shared memory segment
 
    // initalize values for account
    *(shmBUF + 0) = 5000;   // saving
    *(shmBUF + 1) = 3000;   // checking
    *(shmBUF + 2) = 4000;   // money market
    *(shmBUF + 3) = 5000;   // ira
    *(shmBUF + 4) = 3000;   // credit card
    *(shmBUF + 5) = 6000;   // auto
    *(shmBUF + 6) = 7000;   // equity line
    *(shmBUF + 7) = 2000;   // home mortgage
 
    // create 5 child processes
    for (int i = 0; i < 5; i++)
    {
        childPid = fork();
        if (childPid == -1) 
        {   //error handling
            cout << "Child process failed to spawn." << endl;
            exit(1);
        } 
        else if (childPid == 0) 
        {   //child proceees                
            bankOperation(sem, shmBUF, countOperation);     
            exit(0);
        } 
        else
        {   //parent process
            cout << "Child processs " << i+1 << " created(" << childPid << ")" << endl;     
        }
    }
     
    bankOperation(sem, shmBUF, countOperation); // parent process execute transaction
 
    for (int i = 0; i < 3; i++)  // parent process waits
        wait(NULL);
    cout << "Stop all transaction." << endl;
 
    parent_cleanup(sem, shmid); // clean up semaphore and shared memory
    exit(0);
} 
 
// execuse transaction
void bankOperation(SEMAPHORE &sem, float *shmBUF, int &countOperation)
{
    srand(time(NULL)); // run exactly once to intialise the PRNG
 
    while(countOperation < 500)
    {
        int randomNumber = rand() % 2147483647 + 1; // random number from 1 to 2147483647
        if (randomNumber % 37003 == 0)
        {
            depositToSavingItself(sem, shmBUF, countOperation);
            depositToIRAItself(sem, shmBUF, countOperation);
            depositToChecking(sem, shmBUF, countOperation);
        }
        else if (randomNumber % 100003 == 0)
        {
            transferEquitylineToChecking(sem, shmBUF, countOperation);
        }
        else if (randomNumber % 6301 == 0)
        {
            withdrawFromChecking(sem, shmBUF, countOperation);
        }
        else if (randomNumber % 126247697 == 0)
        {
            depositToIRA(sem, shmBUF, countOperation);
        }
        else if (randomNumber % 7001 == 0)
        {
            transferCheckingToAuto(sem, shmBUF, countOperation);
            transferCheckingToEquityLine(sem, shmBUF, countOperation);
        }
        else if (randomNumber % 3001 == 0)
        {
            transferCheckingToHomeMortgage(sem, shmBUF, countOperation);
        }
    } 
}
 
// deposit 1% of fund or interest in savings 
void depositToSavingItself(SEMAPHORE &sem, float *shmBUF, int &countOperation)
{
    sem.P(SAVING);  // wait until SAVING semaphore is avaiable
    *(shmBUF + 0) += *(shmBUF + 0) / 100;
    cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "saving(+3%)" << "\t\t\t" << *(shmBUF + 0) <<  endl;
    countOperation++;
    sem.V(SAVING);  // resume the SAVING semaphore
}
 
// separately deposit 3% of fund or interest in IRA
void depositToIRAItself(SEMAPHORE &sem, float *shmBUF, int &countOperation)
{
    sem.P(IRA);
    *(shmBUF + 3) += *(shmBUF + 3) * 3 / 100;
    cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "IRA(+3%)" << "\t\t\t" << *(shmBUF + 3) <<  endl;
    countOperation++;
    sem.V(IRA);
}
 
// separately deposit (or wired) of $1500 into checking
void depositToChecking(SEMAPHORE &sem, float *shmBUF, int &countOperation)
{
    sem.P(CHECKING);
    *(shmBUF + 1) += 1500;
    cout << "[" << getpid() << "]-" << countOperation<< "\t\t" << "checking(+1500)" << "\t\t\t" << *(shmBUF + 1) <<  endl;
    countOperation++;
    sem.V(CHECKING);
}
 
// transfer $500 from equity line of credit into checking
void transferEquitylineToChecking(SEMAPHORE &sem, float *shmBUF, int &countOperation)
{
    int check = 0;  
    sem.P(EQUITY_LINE); // wait until EQUITY_LINE is available
    if(*(shmBUF + 6) > 500)
    {
        *(shmBUF + 6) -= 500;
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "equity line(-500)" << "\t\t" << *(shmBUF + 6) <<  endl;
        check = 1;
    }
    sem.V(EQUITY_LINE);
 
    if(check == 1)
    {
        sem.P(CHECKING);    // wait until CHECKING semaphore is available   
        *(shmBUF + 1) += 500;
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "equity line(-500)" << "\t\t" << *(shmBUF + 6) <<  endl;
        countOperation++;
        sem.V(CHECKING);
    }
}
 
// withdraw $100 from checking or clear $100 check
void withdrawFromChecking(SEMAPHORE &sem, float *shmBUF, int &countOperation)
{
    sem.P(CHECKING);
    if(*(shmBUF + 1) > 100)
    {
        *(shmBUF + 1) -= 100;
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "checking(-100)" << "\t\t\t" << *(shmBUF + 1) <<  endl;
        countOperation++;
    }
    sem.V(CHECKING);
}
 
// deposit a small amount to IRA from a random account
void depositToIRA(SEMAPHORE &sem, float *shmBUF, int &countOperation)
{
    int check = 0; 
    sem.P(SAVING);
    if(*(shmBUF + 0) > 50)
    {
        *(shmBUF + 0) -= 50;            
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "saving(-50)" << "\t\t\t" << *(shmBUF + 0) <<  endl;
        check = 1;
    }
    sem.V(SAVING);
 
    if(check == 1)
    {
        sem.P(IRA);
        *(shmBUF + 3) += 50;    
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "IRA(+50)" << "\t\t\t" << *(shmBUF + 3) <<  endl;
        countOperation++;
        sem.V(IRA);
    }
}
 
// atomically transfer $200 from checking to auto loan
void transferCheckingToAuto(SEMAPHORE &sem, float *shmBUF, int &countOperation)
{
    int check = 0;
    sem.P(CHECKING);
    if(*(shmBUF + 1) > 200)
    {
        *(shmBUF + 1) -= 200;   
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "checking(-200)" << "\t\t\t" << *(shmBUF + 1) <<  endl;
        check = 1;
    }
    sem.V(CHECKING);
 
    if(check == 1)
    {
        sem.P(AUTO);
        *(shmBUF + 5) += 200;       
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "auto(+200)" << "\t\t\t" << *(shmBUF + 5) <<  endl;
        countOperation++;
        sem.V(AUTO);
    }
}
 
 
// atomically transfer $150 from checking to equity line of credit
void transferCheckingToEquityLine(SEMAPHORE &sem, float *shmBUF, int &countOperation)
{
    int check = 0;
    sem.P(CHECKING);
    if(*(shmBUF + 1) > 150)
    {
        *(shmBUF + 1) -= 150;   
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "checking(-150)" << "\t\t\t" << *(shmBUF + 1) <<  endl;        
        check = 1;
    }
    sem.V(CHECKING);
 
    if(check == 1)
    {
        sem.P(EQUITY_LINE);
        *(shmBUF + 6) += 150;           
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "equity line(+150)" << "\t\t" << *(shmBUF + 6) <<  endl;
        countOperation++;   
        sem.V(EQUITY_LINE);
    }
}
 
// atomically transfer $300 from checking to home mortgage
void transferCheckingToHomeMortgage(SEMAPHORE &sem, float *shmBUF, int &countOperation)
{
    int check = 0;
    sem.P(CHECKING);
    if(*(shmBUF + 1) > 300)
    {
        *(shmBUF + 1) -= 300;   
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "checking(-300)" << "\t\t\t" << *(shmBUF + 1) <<  endl;
        check = 1;
    }
    sem.V(CHECKING);
 
    if(check == 1)
    {
        sem.P(HOME_MORTGAGE);
        *(shmBUF + 7) += 300;           
        cout << "[" << getpid() << "]-" << countOperation << "\t\t" << "home mortgage(+300)" << "\t\t" << *(shmBUF + 7) <<  endl;
        countOperation++;       
        sem.V(HOME_MORTGAGE);
    }
}
 
// clean up semaphore and shared memory
void parent_cleanup (SEMAPHORE &sem, int shmid) {
    int status;         /* child status */
    wait(0);    /* wait for child to exit */
    shmctl(shmid, IPC_RMID, NULL);  /* cleaning up */
    sem.remove();
} // parent_cleanup
