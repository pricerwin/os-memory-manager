#include <stdio.h>
#include <stdlib.h>
#include "smm.h"
#include "cpu.h"
#include "scheduler.h"
#include "memory.h"
#include "disk.h"

struct MemCell memory[1024];

int main(int argc, char* argv[]){
    char* list = (argc > 1) ? argv[1] : "program_list_valid.txt";
    add_hole(0, 1024);
    load_programs(list);
		
	int total_cycles = 0;
	int status = 1;
	
	if(head != NULL) {
		context_switch(head->regs);
	}
	
	loading_mode = 0;
	
	while(schedule(total_cycles, status)){
		status = clock_cycle();
		total_cycles++;
	}

	printf("\n--- Simulation Complete ---\n");
	printf("Total holes created during runtime: %d\n", holes_created_count);

	printf("\nRequired Memory Dump:\n");
	int locations[] = {30, 150, 230};

	printf("Addr | OP | Arg\n");
	printf("----------------\n");
	for(int i = 0; i < 3; i++){
		int addr = locations[i];
		struct MemCell cell = memory[addr];
		printf("%4d | %2d | %3d\n", addr, cell.op, cell.arg);
	}
	
	free_holes();
	return 0;
}
