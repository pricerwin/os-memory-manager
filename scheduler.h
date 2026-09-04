#ifndef SCHEDULER_H
#define SCHEDULER_H

int loading_mode = 1;

int get_running_process_pid();
void remove_from_ready_queue(int pid);

#include "cpu.h"

struct PCB {
	int pid;
	int size;
	register_struct regs;
	struct PCB* next;
};

struct PCB process_table[1024];
int process_count = 0;

struct PCB* head = NULL;
struct PCB* tail = NULL;

void new_process(int base, int size){
	int pid = process_count++;
	process_table[pid].pid = pid;
	process_table[pid].size = size;
	process_table[pid].regs.Base = base;
	process_table[pid].regs.PC = 0;
	process_table[pid].regs.AC = 0;
	process_table[pid].next = NULL;

	if(head == NULL){
		head = &process_table[pid];
		tail = &process_table[pid];
	}else{
		tail->next = &process_table[pid];
		tail = &process_table[pid];
	}
}

void next_process() {
	if (head == NULL || head->next == NULL) return;
	
	struct PCB* temp = head;
	head = head->next;
    
	tail->next = temp;
	tail = temp;
	tail->next = NULL;
}

int schedule(int cycle_num, int process_status) {
	if (head == NULL) return 0;
	
	if (process_status == 0) {
		int finished_pid = head->pid;

		deallocate(finished_pid);

		head = head->next;
        	if (head == NULL) return 0;

		context_switch(head->regs);
		return 1;
	}
	
	if (cycle_num > 0 && cycle_num % 10 == 0 && head->next != NULL){
		head->regs = context_switch(head->next->regs);

		next_process();
	}

	return 1;
}

int get_running_process_pid(){
	if(loading_mode == 1 || head == NULL) return -1;
	return head->pid;
}

void remove_from_ready_queue(int pid){
	if(head == NULL) return;
	
	if(head->pid == pid){
		head = head->next;
		if(head == NULL) tail = NULL;
		return;
	}

	struct PCB* current = head;
	while(current->next != NULL){
		if(current->next->pid == pid){
			struct PCB* temp = current->next;
			current->next = temp->next;
			if(current->next == NULL) tail = current;
			return;
		}
		current = current->next;
	}
}






#endif

