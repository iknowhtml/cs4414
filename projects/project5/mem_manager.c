#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TLB_SIZE 4
#define PAGE_TABLE_SIZE 8
#define NUM_PAGES 16 
#define INIT_COUNTER_VAL 0x4000000000000;

typedef struct{
	int page, frame, valid;
	long counter;
} entry;

int findMin(entry table[], int size){
	int index;
	long min = INIT_COUNTER_VAL;
	for(int i = 0; i < size; i++){
		if(min > table[i].counter){
			min = table[i].counter;
			index = i;
		}
	}
	return index;
}

void incrementCounter(entry table[], int size){
	for(int i = 0; i < size; i++){
		table[i].counter >>= 1;
	}
}

entry Entry(int page, int frame){
	entry e;
	e.page = page;
	e.frame = frame;
	e.counter = INIT_COUNTER_VAL;
	e.valid = 1;

	return e; 
} 

int main(){
	int total = 0, faults = 0, hits = 0;

	entry tlb[TLB_SIZE] = { {0, 0, 0, 0} }, page_table[PAGE_TABLE_SIZE] = { {0, 0, 0, 0} };
	int memory[PAGE_TABLE_SIZE][64]; 
	
	int virtual_address;
	FILE *addresses = fopen("addresses.txt", "r");
	FILE *bin = fopen("BACKING_STORE.bin", "rb");

	while(fscanf(addresses, "%d", &virtual_address) == 1){
		total++;

		int page = (virtual_address & 0xF00) >> 8; 
		int offset = virtual_address & 0xFF;

		int i = 0;	
		
		while(tlb[i].page != page && i < TLB_SIZE){
			i++;
		}

		//case if page is not in tlb
		if(i == TLB_SIZE){
			printf("frame number for page %d is missing in the TLB\n", page);
			
			i = 0;

			while(page_table[i].page != page && i < PAGE_TABLE_SIZE){
				i++;
			}

			//case if page is not in page table
			if(i == PAGE_TABLE_SIZE){
				faults++;
				printf("virtual address %d contained in page %d causes a page fault\n", virtual_address, page);
				i = findMin(page_table, PAGE_TABLE_SIZE);

				page_table[i] = Entry(page, i);

				int data;

				//loads data from bin file into memory
				for(int j = 0; j < 64; j++){
					fseek(bin, page*256 + j*4, SEEK_SET);
					fread(&data, sizeof(data), 1, bin);
					memory[i][j]= data;
				}

				printf("page %d is loaded into frame %d\n", page, i);
			}

			//case if page is in page table
			else{
				page_table[i].counter |= INIT_COUNTER_VAL;  
				printf("page %d is contained in frame %d\n", page, i);
			}
			
			int j = findMin(tlb, TLB_SIZE);
				tlb[j] = Entry(page, i);
			printf("frame %d containing page %d is stored in entry %d of the TLB\n", i, page, j);
		}

		//case if page is in tlb
		else{
			hits++;
			tlb[i].counter |= INIT_COUNTER_VAL;  
			page_table[i].counter |= INIT_COUNTER_VAL;  
			printf("page %d is contained in frame %d\n", page, i);
			printf("page %d is stored in frame %d which is stored in entry %d of the TLB\n", tlb[i].page, tlb[i].frame, i);
		}

		incrementCounter(page_table, PAGE_TABLE_SIZE);
		incrementCounter(tlb, TLB_SIZE);
		printf("\n");
	}
	
	printf("The reference string generated %d page faults in memory and the TLB hit ratio is %d out of %d.\n\n", faults, hits, total);
	printf("The contents of page table after simulation:\n\n");
	for(int i = 0; i < NUM_PAGES; i++){
		for(int j = 0; j < PAGE_TABLE_SIZE; j++){
			if(i == page_table[j].page){
				printf("p%d in f%d\n", page_table[j].page, page_table[j].frame);
			}
		}
	}
	printf("\nThe contents of memory after simulation:\n\n");
	for(int i = 0; i < PAGE_TABLE_SIZE; i++){
		printf("f%d in p%d\n", i, page_table[i].page);
	}

	return 0;
}