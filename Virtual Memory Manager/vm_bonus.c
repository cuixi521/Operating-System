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
#define MEM_SIZE (FRAME_SIZE * SFRAME_ENTRIES) // Mem size, in bytes.
#define TLB_ENTRIES 16 // Max TLB entries.
#define SFRAME_ENTRIES 256

int FNo = 0;
char Frame[FRAME_SIZE *SFRAME_ENTRIES];

void LoadFrame(FILE *bp, int fn, int pn);

typedef struct {
	int Pno;
	int Fno;
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

int getFrameNo(int page_table[PAGE_SIZE], int PNo, FILE *bp) {
	int result;
	if (page_table[PNo] == -1) {
		result = FNo % SFRAME_ENTRIES;
		page_table[PNo] = result;
		LoadFrame(bp, result, PNo);
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
	return;
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

int TLB_update(int pn, int count, int fn) {
	for (int i = 0; i < TLB_ENTRIES; i++) {
		if (TLB[i].Fno == fn) {
			TLB[i].Pno = pn;
			return count;
		}
	}
	int i = count % 16;
	TLB[i].Pno = pn;
	TLB[i].Fno = fn;
	return (count + 1);
}


int main()
{
	FILE *ptr, *back;
	back = fopen("BACKING_STORE.bin", "r");
	ptr = fopen("addresses.txt", "r");

	int virtualA;	//Store virtual address

	int page_table[PAGE_SIZE];
	int count = 0;
	int pagef = 0;
	int sum = 0;

	initialPT(page_table);
	initialTLB();
	initialFrame();

	while (feof(ptr) == 0) {
		fscanf(ptr, "%d", &virtualA);			//get virtual address 
		int off = get_offset(virtualA);
		int pn = get_pagenumber(virtualA);

		int fn = TLB_seek(pn);

		if (fn == -1) {

			fn = getFrameNo(page_table, pn, back);
			count = TLB_update(pn, count, fn);

			if (FNo > SFRAME_ENTRIES - 1) {
				for (int i = 0; i < PAGE_SIZE; i++) {
					if (page_table[i] == fn && i != pn) {
						page_table[i] = -1;
						break;
					}
				}
			}
		}

		else {
			printf("*****************");
		}

		char value = getValue(off, fn);

		printf("%d ---%d\n", value, sum);
		//printf("%d\n", PysicalA(fn, off));
		sum++;
	}

	printf("%d %d\n", FNo, sum);
	//printf("%f \n", (double)pagef/(double)sum);
	return 0;
}




