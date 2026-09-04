#ifndef MEMORY_H
#define MEMORY_H

int get_running_process_pid();
int is_allowed_address(int pid, int addr);
void deallocate(int pid);
void remove_from_ready_queue(int pid);

struct MemCell{
	int op;
	int arg;
};

extern struct MemCell memory[1024];

static struct MemCell mem_read(int addr){
	int current_pid = get_running_process_pid();

	if(is_allowed_address(current_pid, addr)){
		return memory[addr];
	}else{
		printf("Memory Error: PID %d attempted illegal read at %d\n", current_pid, addr);
		deallocate(current_pid);
		remove_from_ready_queue(current_pid);

		struct MemCell error_cell = {0, 0};
		return error_cell;
	}
}

static void mem_write(int addr, struct MemCell cell_val){
	int pid = get_running_process_pid();

	if(is_allowed_address(pid, addr)){
		memory[addr] = cell_val;
	}else{
		printf("SMM REJECTION: PID %d attempted illegal write at address %d\n", pid, addr);
		deallocate(pid);
		remove_from_ready_queue(pid);
	}
}

#endif
