# Semaphore-program
Objectives: Linux shared memory & semaphores

Background: 
A customer jointly owns 4 accounts (regular savings, regular checking, money market & IRA (retirement) accounts) & 4 loans (credit card, auto, equity line of credit & home mortgage) with his wife. Operations on each bank account include

deposit 1% of fund or interest in savings [37,003],
separately deposit 3% of fund or interest in IRA [37,003],
separately deposit (or wired) of $1500 into checking [37,003],
transfer $500 from equity line of credit into checking [100,003],
withdraw $100 from checking or clear $100 check [6,301],
deposit a small amount to IRA from a random account (atomically as a single transaction) [126,247,697].
Operations on each loan are
atomically transfer $200 from checking to auto loan [7001], 
atomically transfer $150 from checking to equity line of credit [7001], 
atomically transfer $300 from checking to home mortgage [3001]. 
Note: Allow the possibility for bank employees to access a loan balance to charge fees, clear fees, adjust balance, etc.
Problem statement:
As can be seen in the account balance example in lecture, it is conceivable that the balance of one or more accounts may be corrupted, if the balances are not updated correctly, i.e., lacking mutual exclusion. Hence, the assignment is to use semaphores properly to ensure correctness.

Simulation:
Six concurrent processes (the parent process & 5 child processes) will be used to simulate transactions in these accounts. One process may represent the husband, the 2nd would represent the wife, the 3rd could be a teller, & the remaining processes may be a bank computer or additional tellers. Each process would perform 500 transactions in these accounts, by executing the listed operations on these accounts.

Each process uses a random number that is divisible by a prime value [shown in the square bracket above] to determine which and when an operation on certain balance(s) are to be executed. For instance, when some process generates the random number that is divisible by 6,301, this process now deducts $100 from checking. Unspecified values can be hard coded or chosen as you wish. Disregard negative balances (or arithmetic overflow).

The 6 processes would need to be coordinated to ensure correctness. The coordination, however, needs to be done with the minimum amount of interference, in order to allow each process to execute its task as quickly as possible. Be careful with deadlock possibilities.

You should output the balances of these accounts periodically to verify & get some small assurance that the simulation and the semaphores are used correctly. You have permission to use the SEMAPHORE class for this assignment.
