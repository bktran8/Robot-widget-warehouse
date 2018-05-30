//VERSION ON EDORAS
/* Brenda Tran
* Professor Carroll
* Program 3 assignment
        * CS570
* SDSU
        * Spring 2018
*
* This is the ONLY file you are allowed to change. (In fact, the other
        * files should be symbolic links to
        *   ~cs570/Three/p3main.c
        *   ~cs570/Three/p3robot.c
        *   ~cs570/Three/p3.h
        *   ~cs570/Three/makefile
        *   ~cs570/Three/CHK.h    )
* The file ~cs570/Three/createlinks is a script that will create these for you.
*/
#include "p3.h"

#define COURSEID "570"

/* You may put declarations/definitions here.
   In particular, you will probably want access to information
   about the job (for details see the assignment and the documentation
   in p3robot.c):
     */
extern int nrRobots;
extern int quota;
extern int seed;
extern int width;
static sem_t *mutex_loc;
char semaphore[SEMNAMESIZE];
int robot_info;
int mutex_counter;
int temp_counter;
int total_widgets;


void initStudentStuff(void) {
    sprintf(semaphore, "btran%s%ldmutex", COURSEID, (long) getuid);
    //CREATE AND INITIALIZE SEMAPHORES
    if ((mutex_loc = sem_open(semaphore, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        mutex_loc = sem_open(semaphore, O_RDWR);
        CHK(robot_info = open("robotstuff", O_RDWR));
    } else {
        //DETERMINES WHHETHER ROBOT CAN ENTER MUTEX
        CHK(sem_wait(mutex_loc));
        CHK(robot_info = open("robotstuff", O_RDWR| O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
        mutex_counter = 0;
        //REPOSITIONS ROBOT_INFO OFFSET TO OFFSET BYTES
        CHK(lseek(robot_info, 0, SEEK_SET));
        //WRITING THE COUNTER AND THE ROW INTO ROBOT_INFO FOR OTHER ROBOTS TO HAVE
        assert(sizeof(mutex_counter) == write(robot_info, &mutex_counter, sizeof(mutex_counter)));
        //ALL FILES INITIALIZED, SEMAPHORE IS UNLOCKED FOR ROBOTS
        CHK(sem_post(mutex_loc));
    }

}

void placeWidget(int n) {
    //DETERMINES WHHETHER ROBOT CAN ENTER MUTEX
    sem_wait(mutex_loc);
    //ROBOTS READ COUNT AND ROW FROM ROBOT_INFO
    CHK(lseek(robot_info, 0, SEEK_SET));
    assert(sizeof(mutex_counter) == read(robot_info, &mutex_counter, sizeof(mutex_counter)));
    mutex_counter++;
    //ASSIGN COUNTER TO TEMP FOR LATER WRITE INTO ROBOT INFO
    temp_counter = mutex_counter;
    total_widgets = nrRobots * quota;
    if (temp_counter == total_widgets) {
        printeger(n);
        printf("F\n");
        fflush(stdout);
        CHK(close(robot_info));
        CHK(unlink("robotstuff"));
        CHK(sem_close(mutex_loc));
        CHK(sem_unlink(semaphore));
    } else {
        if (temp_counter % width == 0) {
            printeger(n);
            printf("N\n");
            fflush(stdout);
        } else {
            printeger(n);
            fflush(stdout);
        }
        CHK(lseek(robot_info, 0, SEEK_SET));
        assert(sizeof(mutex_counter) == write(robot_info, &mutex_counter, sizeof(mutex_counter)));
        CHK(sem_post(mutex_loc));
    }
}


