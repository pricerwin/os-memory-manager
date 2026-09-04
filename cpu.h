#ifndef CPU_H
#define CPU_H

typedef struct {
	int Base;
	int PC;
	int IR0;
	int IR1;
	int AC;
	int MAR;
	int MBR;
} register_struct;

register_struct context_switch(register_struct new_vals);

#include "memory.h"
#include <stdio.h>

int Base = 0;
int PC = 0;
int IR0 = 0;
int IR1 = 0;
int AC = 0;
int MAR = 0;
int MBR = 0;

register_struct context_switch(register_struct new_vals) {
	register_struct old_vals;
	old_vals.Base = Base;
	old_vals.PC = PC;
	old_vals.IR0 = IR0;
	old_vals.IR1 = IR1;
	old_vals.AC = AC;
	old_vals.MAR = MAR;
	old_vals.MBR = MBR;

	Base = new_vals.Base;
	PC = new_vals.PC;
	IR0 = new_vals.IR0;
	IR1 = new_vals.IR1;
	AC = new_vals.AC;
	MAR = new_vals.MAR;
	MBR = new_vals.MBR;

	return old_vals;
}

static int mem_address(int i_addr) {
	return Base + i_addr;
}

static void fetch_instruction(int addr) {
	struct MemCell cell = mem_read(addr);
	IR0 = cell.op;
	IR1 = cell.arg;
}

static void execute_instruction(){
	switch (IR0) {
		case 0: //exit
			break;
		case 1: //load const
			AC = IR1;
			break;
		case 2: //move_from_mbr
			AC = MBR;
			break;
		case 3: //move_from_mar
			AC = MAR;
			break;
		case 4: //move_to_mbr
			MBR = AC;
			break;
		case 5: //move_to_mar
			MAR = AC;
			break;
		case 6: //load_at_addr
			MBR = mem_read(MAR).arg;
			break;
		case 7: //write_at_addr
		{
			struct MemCell temp;
			temp.op = 0;
			temp.arg = MBR;
			mem_write(MAR, temp);
			break;
		}
		case 8: //add
			AC = AC + MBR;
			break;
		case 9: //multiply
			AC = AC * MBR;
			break;
		case 12: //ifgo
			if(AC != 0){
				PC = IR1 - 1;
			}
			break;
		default:
			break;
	}
}

static int clock_cycle(){
	fetch_instruction(mem_address(PC));
	
	if (IR0 == 0) return 0;

	execute_instruction();
	PC++;
	return 1;
}

#endif
