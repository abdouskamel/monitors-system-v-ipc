# Implementation of Hoare monitors using System V IPC (InterProcess Communication)

Hoare monitors are synchronization tools that prevent concurrent access to a shared object. They also allow threads to wait for a change of state in shared objects. 

- Checkout monitor.h and monitor.h to see how monitors are implemented using System V IPC. 

- Checkout main.c for a use case example with two concurrent processes.

- Compile and run the program:

```
gcc -o prog main.c monitor.c
./prog
```

The example proceeds as follow:

1- The program creates two processes P1 and P2, with a shared memory object stored in work_result. 

2- P1 starts and uses the monitor to wait for P2 to finish its work and assign a value to work_result. 

3- P2 takes 5 seconds to start, and then an extra 2 seconds to finish its work and uses the monitor to wake up P1. 

4- P1 wakes up, reads the value of the shared object work_result that P2 edited, and then terminates.

Note: if you have some issues, delete systemv_ipc_key and recreate it with:
```
touch systemv_ipc_key
```