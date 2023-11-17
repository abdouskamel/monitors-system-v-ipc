#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include <string.h>
#include <errno.h>

#include "monitor.h"

#define NB_CONDS 1
#define IPC_KEY_NAME "systemv_ipc_key"

// Shared memory object
typedef struct
{
    int work_result;

} shm_mem;

int main()
{
    pid_t pid1, pid2;
    monitor mtor;

    int err_code = create_monitor(IPC_KEY_NAME, 1, 2, NB_CONDS, sizeof(shm_mem), &mtor);
    if (err_code != 0)
    {
        perror("Cannot create monitor");
        return (EXIT_FAILURE);
    }

    // Create first process
    pid1 = fork();
    if (!pid1)
    {
        printf("P1 starts.\n");
        init_monitor(IPC_KEY_NAME, 1, 2, &mtor);

        sleep(1);
        enter_monitor(&mtor);

        shm_mem *shm_ptr = mtor_shmat(&mtor);

        // Stop and wait for P2 to finish
        if (!shm_ptr->work_result)
        {
            printf("P1 waits for P2.\n");
            mtor_wait(&mtor, 0);
        }

        // Here, P1 has been woken up by P1
        printf("P1 continues.\n");
        
        //  Wait for 2 seconds before terminating
        sleep(2);
        printf("P1 finished, work_result = %d\n", shm_ptr->work_result);
        mtor_shmdt(shm_ptr);

        exit_monitor(&mtor);
        exit(EXIT_SUCCESS);
    }

    else
    {
        // Create second process
        pid2 = fork();
        if (!pid2)
        {
            init_monitor(IPC_KEY_NAME, 1, 2, &mtor);

            // Wait for 5 seconds before entering the monitor (to simulate some workload)
            sleep(5);
            printf("P2 starts.\n");
            enter_monitor(&mtor);

            // Wait for 2 seconds before accessing shared memory
            sleep(2);

            // Access the shared memory object and edit it with a random number
            shm_mem *shm_ptr = mtor_shmat(&mtor);
            shm_ptr->work_result = 42;
            mtor_shmdt(shm_ptr);

            printf("P2 finished. P2 wakes P1.\n");
            mtor_signal(&mtor, 0);

            exit_monitor(&mtor);
            exit(EXIT_SUCCESS);
        }

        else
        {
            // We are in parent process, just wait
            while (wait(NULL) != -1) ;
            free_monitor(&mtor);
        }
    }

    return (EXIT_FAILURE);
}
