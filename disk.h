//disk.h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memory.h"

static struct MemCell translate(char* buffer);

static int load_prog(char* fpath, int addr){
	FILE* fhandle;
	char buffer[256];
	int instruction_count =0;

	fhandle = fopen(fpath, "r");
	if(!fhandle) return 0;

	printf("%s\n", fpath);
	
	while(fgets(buffer, sizeof(buffer), fhandle)){
		struct MemCell cell =  translate(buffer);
		
		if (cell.op != -1) {
			mem_write(addr, cell);
			addr++;
			instruction_count++;
		}
	}
	
	fclose(fhandle);
	return instruction_count;
}


void load_programs(char* fname){
	FILE* fhandle = fopen(fname, "r");
	if(!fhandle){
		printf("Error: Could not open program list %s\n", fname);
		return;
	}
	
	int requested_size;
	char prog_name[256];
	
	while(fscanf(fhandle, "%d %s", &requested_size, prog_name) == 2){
		int pid = process_count;
		
		if(allocate(pid, requested_size) == 1){
			int assigned_base = get_base_address(pid);

			int actual_instr_count = load_prog(prog_name, assigned_base);

			new_process(assigned_base, requested_size);
		
			printf("Successfully loaded %s for PID %d at Address %d\n", prog_name, pid, assigned_base);
		}else{
			continue;
		}
	}
	fclose(fhandle);
}

static struct MemCell translate(char* buffer){
	struct MemCell cell;
	cell.op = -1;
	cell.arg = 0;

	if (buffer[0] == '/' || buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == ' '){
	return cell;
	}

	char* chunk0 = strtok(buffer, " \n\r");
	char* chunk1 = strtok(NULL, " \n\r");

	if (chunk0 == NULL)
		 return cell;

	if (strcmp(chunk0, "exit") == 0){
		cell.op = 0;
	}
	else if (strcmp(chunk0, "load_const") == 0){
		cell.op = 1;
		if (chunk1){
			cell.arg = atoi(chunk1);
		}
	}
	else if (strcmp(chunk0, "move_from_mbr") == 0){
		cell.op = 2;
	}
	else if (strcmp(chunk0, "move_from_mar") == 0){
		cell.op = 3;
	}
	else if (strcmp(chunk0, "move_to_mbr") == 0){
		cell.op = 4;
	}
	else if (strcmp(chunk0, "move_to_mar") == 0){
		cell.op = 5;
	}
	else if (strcmp(chunk0, "load_at_addr") == 0){
		cell.op = 6;
	}
	else if (strcmp(chunk0, "write_at_addr") == 0){
		cell.op = 7;
	}
	else if (strcmp(chunk0, "add") == 0){
		cell.op = 8;
	}
	else if (strcmp(chunk0, "multiply") == 0){
		cell.op = 9;
	}
	else if (strcmp(chunk0, "and") == 0){
		cell.op = 10;
	}
	else if (strcmp(chunk0, "or") == 0){
		cell.op = 11;
	}
	else if (strcmp(chunk0, "ifgo") == 0){
		cell.op = 12;
		if (chunk1) cell.arg = atoi(chunk1);
	}
	else if (strcmp(chunk0, "sleep") == 0){
		cell.op = 13;
	}

	if (cell.op != 1 && cell.op != 12 && chunk1 != NULL){
		cell.arg = atoi(chunk1);
	}
	
	return cell;
}

