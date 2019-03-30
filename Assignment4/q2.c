
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <memory.h>
#include <assert.h>
#include <math.h>

#define EMax 200
#define RMax 200
typedef int EVMId;
typedef int RobotId;
//static const int RMax = 200;
//static const int EMax = 200;
const int EVM_TRANSITION_TIME = 0;
struct voting_Bth;
typedef struct voting_Bth voting_Bth;

typedef struct ElectronicVotingMachine {
    int uniqID;
    int number_of_slots;
    int number_occupied;
    voting_Bth *single_booth;
} ElectronicVotingMachine;
ElectronicVotingMachine get_new_evm(int uniqID, int number_of_slots, voting_Bth *single_booth) {
    ElectronicVotingMachine evm;
    evm.uniqID = uniqID;
    evm.number_of_slots = number_of_slots;
    evm.number_occupied = 0;
    evm.single_booth = single_booth;
    return evm;
}
typedef struct Robot {
    int uniqID;
    int voterID;
    ElectronicVotingMachine *assigned_evm;
    voting_Bth *single_booth;
} Robot;
typedef enum Bool {
    False = 0,
    True = 1,
} Bool;
struct voting_Bth {
    pthread_mutex_t mutex;
    ElectronicVotingMachine evms[EMax];
    int number_of_evms;
    ElectronicVotingMachine *free_evm;

    Robot robots[RMax];
    int number_of_robots;

    int number_of_unassigned_voters;
    int n_votes_to_cast;
};
Robot robot_new(int uniqID, voting_Bth *single_booth) {
    Robot r;
    r.uniqID = uniqID;
    r.single_booth = single_booth;
    r.assigned_evm = NULL;
    r.voterID = -1;
    return r;
}
// All the Function Declarations are Here.
voting_Bth booth_new(int number_of_evms, int number_of_robots, int number_of_unassigned_voters);
void booth_lock(voting_Bth *single_booth) {   pthread_mutex_lock(&single_booth->mutex);  }
void booth_unlock(voting_Bth *single_booth) { pthread_mutex_unlock(&single_booth->mutex); }
int booth_get_voter(voting_Bth *single_booth);
void polling_ready_evm(voting_Bth *single_booth, ElectronicVotingMachine *evm);
ElectronicVotingMachine* voter_wait_for_evm(voting_Bth *single_booth);
void voter_in_slot(voting_Bth *single_booth, Robot *r);
void* robot_fn(void *data);
void* evm_fn(void *data);

int main() {
    int number_of_evms = 100;
    int number_of_robots = 100;
    int number_of_unassigned_voters = 100;

    assert (number_of_evms < EMax);
    assert (number_of_robots < RMax);

    voting_Bth single_booth = booth_new(number_of_evms, number_of_robots, number_of_unassigned_voters);
    
    const int number_of_slots = 1;
    for(int i = 0; i < number_of_evms; i++) {
        single_booth.evms[i] = get_new_evm(i, number_of_slots, &single_booth);
        pthread_t thread;
        pthread_create(&thread, NULL, &evm_fn, &single_booth.evms[i]);
    }

    for(int i = 0; i < number_of_robots; i++) {
        single_booth.robots[i] = robot_new(i, &single_booth);
        pthread_t thread;
        pthread_create(&thread, NULL, &robot_fn, &single_booth.robots[i]);
    }

    while(single_booth.n_votes_to_cast > 0) {};
}

void* evm_fn(void *data) {
    ElectronicVotingMachine *instanceEVM = (ElectronicVotingMachine *)data;

    while(instanceEVM->single_booth->n_votes_to_cast > 0) {

        assert(instanceEVM->number_occupied >= 0 && instanceEVM->number_occupied <= instanceEVM->number_of_slots);

        if(instanceEVM->number_occupied < instanceEVM->number_of_slots) {
            polling_ready_evm(instanceEVM->single_booth, instanceEVM);
            printf("EVM: %d  | free\n", instanceEVM->uniqID);
        } else {
            //printf("EVM: %d | fully occupied\n", instanceEVM->uniqID);
        }

    }
    return NULL;
};
void* robot_fn(void *data) {
    Robot *r = (Robot *)data;

    voting_Bth *single_booth = r->single_booth;

    while(1) {
        r->voterID = booth_get_voter(single_booth);
        if (r->voterID <= -1) {
            return NULL;
        }
 //       printf("ROBOT %d <= voter %d\n", r->uniqID, r->voterID);
        r->assigned_evm = voter_wait_for_evm(single_booth);
        voter_in_slot(single_booth, r);
    }

    return NULL;
}
void voter_in_slot(voting_Bth *single_booth, Robot *r) {
    booth_lock(single_booth);

    assert(r->assigned_evm != NULL);
    printf("VOTE: robot %d | voter  %d | evm %d\n", r->uniqID,
            r->voterID, r->assigned_evm->uniqID);
    r->assigned_evm->number_occupied--;
    r->assigned_evm = NULL;
    
    single_booth->n_votes_to_cast--;
    booth_unlock(single_booth);
    return;
}
ElectronicVotingMachine* voter_wait_for_evm(voting_Bth *single_booth) {
    while(1) { 
        booth_lock(single_booth);

        //allocate a slot and return
        ElectronicVotingMachine *free_evm = single_booth->free_evm;
        if (free_evm != NULL) {
            assert(free_evm->number_occupied >= 0);
            assert(free_evm->number_occupied < free_evm->number_of_slots);

            free_evm->number_occupied++;

            if (free_evm->number_occupied == free_evm->number_of_slots) {
                single_booth->free_evm = NULL;
            }

            booth_unlock(single_booth);
            return free_evm;
        }

        booth_unlock(single_booth);
    }

    assert(False && "should never reach here");
}
void polling_ready_evm(voting_Bth *single_booth, ElectronicVotingMachine *evm) {
    assert(single_booth != NULL);

    while(1) {
        booth_lock(single_booth);

        if (evm->number_occupied == evm->number_of_slots) {
            booth_unlock(single_booth);
            return;
        }

        if (single_booth->free_evm == NULL) {
            single_booth->free_evm = evm;
            booth_unlock(single_booth);
            return;
        }

        booth_unlock(single_booth);
        usleep(EVM_TRANSITION_TIME);
    }
}
int booth_get_voter(voting_Bth *single_booth) {
   // assert (single_booth->number_of_unassigned_voters >= 0);
    if (single_booth->number_of_unassigned_voters == 0) {
        return -1;
    }

    booth_lock(single_booth);
    int voterID = single_booth->number_of_unassigned_voters;
    single_booth->number_of_unassigned_voters--;
    booth_unlock(single_booth);
    return voterID;
}
voting_Bth booth_new(int number_of_evms, int number_of_robots, int number_of_unassigned_voters) {
    voting_Bth single_booth;
    pthread_mutex_init(&single_booth.mutex, NULL);
    single_booth.free_evm = 0;
    single_booth.number_of_evms = number_of_evms;
    single_booth.number_of_robots = number_of_robots;
    single_booth.number_of_unassigned_voters = number_of_unassigned_voters;
    single_booth.n_votes_to_cast = number_of_unassigned_voters;

    return single_booth;
}
