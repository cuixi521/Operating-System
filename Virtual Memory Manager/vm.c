#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define OFFP 0xFF			// get the rightest 8bit
#define PAGEP 0xFF00
#define PAGE_SIZE 256 // Page size, in bytes.
#define PAGE_ENTRIES 256 // Max page table entries.
#define PAGE_NUM_BITS 8 // Page number size, in bits.
#define FRAME_SIZE 256 // Frame size, in bytes.
#define FRAME_ENTRIES 256 // Number of frames in physical memory.
#define MEM_SIZE (FRAME_SIZE * FRAME_ENTRIES) // Mem size, in bytes.
#define TLB_ENTRIES 16 // Max TLB entries.

int FNo = 0; 
char Frame[MEM_SIZE];

typedef struct {
	int Pno;
	int Fno;
	int v;
} TForm;

TForm TLB[TLB_ENTRIES];


int get_offset(int vir) {
	return (vir & OFFP);
}

int get_pagenumber(int vir) {
	int p = vir & PAGEP;
	return (p >> 8);
}


void initialPT(int page_table[PAGE_SIZE]) {
	for (int i = 0; i < PAGE_SIZE; i++) {
		page_table[i] = -1;
	}
	return;
}

int getFrameNo(int page_table[PAGE_SIZE], int PNo) {
	int result;
	if (page_table[PNo] == -1) {
		page_table[PNo] = FNo;
		result = FNo;
		FNo++;
	}
	else {
		result = page_table[PNo];
	}
	return result;
}


int PysicalA(int fn, int offset) {
	int result = fn << 8;
	result = result + offset;
	return result;
}

void initialFrame() {
	for (int i = 0; i < MEM_SIZE; i++) {
		Frame[i] = -1;
	}
	return;
}
void LoadFrame(FILE *bp, int fn, int pn) {
	fseek(bp, pn*FRAME_SIZE, SEEK_SET);
	fread(&Frame[fn*FRAME_SIZE], 1, FRAME_SIZE, bp);	
	//fread( Frame + fn*FRAME_SIZE, 1, FRAME_SIZE, bp);
}

char getValue(int off, int fn) {
	char result = Frame[fn*FRAME_SIZE + off];
	return result;
}

void initialTLB() {
	for (int i = 0; i < TLB_ENTRIES; i++) {
		TLB[i].Pno = 0;
		TLB[i].Fno = -1;
	}
	return;
}

int TLB_seek(int pn) {
	int result;
	for (int i = 0; i < TLB_ENTRIES; i++) {
		if (TLB[i].Pno == pn) {
			result = TLB[i].Fno;
			return result;
		}
		else {
			result = -1;
		}
	}
	return result;
}

void TLB_update(int pn, int count, int fn) {
	int i = count % 16;
	TLB[i].Pno = pn;
	TLB[i].Fno = fn;
	return; 
}

int main()
{
	FILE *ptr, *back;
	back = fopen("BACKING_STORE.bin", "r");
	ptr = fopen("addresses.txt", "r");

	int virtualA;	//Store virtual address

	int page_table[PAGE_SIZE];
	int count = 0;
	int sum = 0;

	initialPT(page_table);
	initialTLB();
	initialFrame();

	while (feof(ptr)==0) {
		fscanf(ptr, "%d", &virtualA);			//get virtual address 
		int off = get_offset(virtualA);
		int pn = get_pagenumber(virtualA);

		int fn = TLB_seek(pn);

		if (fn == -1) {
			fn = getFrameNo(page_table, pn);
			TLB_update(pn, count, fn);
			count++;
		}

		char value = getValue(off, fn);

		if (value == -1) {
			LoadFrame(back, fn, pn);
			value = getValue(off, fn);
		}

		printf("%d\n", value);
		//printf("%d\n", PysicalA(fn, off));
		sum++;
	}
	 
	//printf("%d \n", count, sum);
	return 0;
}



