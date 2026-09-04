#include "smm.h"

int allocation_table[256][3] = {{0}};
int holes_created_count = 0;
struct Hole* head_hole = NULL;

int find_hole(int size){
	struct Hole* current = head_hole;
	while(current != NULL){
		if(current->size >= size){
			int base = current->base_address;

			current->base_address += size;
			current->size -= size;

			if(current->size == 0){
				remove_hole(base + size);
			}
			return base;
		}
		current = current->next;
	}
	return -1;
}

int allocate(int pid, int size){
    int row = find_empty_row();
    if(row == -1){
        printf("SMM Error: Allocation rejected - table full (requested %d units)\n", size);
        return 0;
    }

    int base = find_hole(size);
    if(base == -1){
        printf("SMM Error: Allocation rejected - no hole large enough for %d units\n", size);
        return 0;
    }

    allocation_table[row][0] = pid;
    allocation_table[row][1] = base;
    allocation_table[row][2] = size;
    return 1;
}

int is_allowed_address(int pid, int addr){
	if(pid == -1){
		return 1;
	}

	for(int i = 0; i < 256; i++){
		if(allocation_table[i][0] == pid && allocation_table[i][2] > 0){
			int base = allocation_table[i][1];
			int size = allocation_table[i][2];

			if(addr >= base && addr < (base + size)){
				return 1;
			}
		}
	}
	printf("SMM Error: Memory access violation by PID %d at address %d\n", pid, addr);
	return 0;
}

void deallocate(int pid){
	for(int i = 0; i < 256; i++){
		if(allocation_table[i][0] == pid && allocation_table[i][2] > 0){
			int base = allocation_table[i][1];
			int size = allocation_table[i][2];

			allocation_table[i][2] = 0;

			add_hole(base, size);

			merge_holes();
			return;
		}
	}
}

void add_hole(int base, int size){
	struct Hole* new_node = (struct Hole*)malloc(sizeof(struct Hole));
	new_node->base_address = base;
	new_node->size = size;
	new_node->next = NULL;

	holes_created_count++;

	if(head_hole == NULL || base < head_hole->base_address){
		new_node->next = head_hole;
		head_hole = new_node;
	}else{
		struct Hole* current = head_hole;
		while(current->next != NULL && current->next->base_address < base){
			current = current->next;
		}
		new_node->next = current->next;
		current->next = new_node;
	}
}

void merge_holes(){
	if(head_hole == NULL) return;

	struct Hole* current = head_hole;
	while(current->next != NULL){
		if(current->base_address + current->size == current->next->base_address){
			struct Hole* duplicate = current->next;
	
			current->size += duplicate->size;
		
			current->next = duplicate->next;

			free(duplicate);

			continue;
		}
		current = current->next;
	}
}

int get_base_address(int pid){
	for(int i = 0; i < 256; i++){
		if(allocation_table[i][0] == pid && allocation_table[i][2] > 0){
			return allocation_table[i][1];
		}
	}
	return -1;
}

int find_empty_row(){
	for(int i = 0; i < 256; i++){
		if(allocation_table[i][2] == 0){
			return i;
		}
	}
	return -1;
}

void remove_hole(int base){
	if(head_hole == NULL) return;

	if(head_hole->base_address == base){
		struct Hole* temp = head_hole;
		head_hole = head_hole->next;
		free(temp);
		return;
	}

	struct Hole* current = head_hole;
	while(current->next != NULL){
		if(current->next->base_address == base){
			struct Hole* temp = current->next;
			current->next = temp->next;
			free(temp);
			return;
		}
		current = current->next;
	}
}





















