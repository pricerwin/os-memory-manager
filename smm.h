#ifndef SMM_H
#define SMM_H

int get_base_address(int pid);
int find_empty_row();
void remove_hole(int base);

#include <stdio.h>
#include <stdlib.h>

struct Hole {
	int base_address;
	int size;
	struct Hole* next;
};

extern int allocation_table[256][3];
extern int holes_created_count;

int allocate(int pid, int size);
void deallocate(int pid);
int find_hole(int size);

void add_hole(int base, int size);
void remove_hole(int base);
void merge_holes();
void free_holes();

int get_base_address(int pid);
int find_empty_row();
int is_allowed_address(int pid, int addr);

#endif

