#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned long int u_long;
const char magic[4] = { 'Q', 'L', 'P', 0x00 };
const int addr_size = sizeof(long int); //maybe use 4th byte of header to change this?
const int buffer_max = 262144;
const char padder[4] = { 0x00,0x00,0x00,0x00 };

typedef struct {
	char	name[16];
	unsigned long int	size;
	unsigned long int	addr;
} QLPFILE;



int splitqlp(int c, char* v[]);
int makeqlp(int c, char* v[]);
int main(int argc, char* argv[]);


int splitqlp(int c, char* v[]) {


	FILE* inqlp = fopen(v[2], "rb+");
	if (!inqlp) {
		perror("Can't open QLP");
		return 1;
	}
	char header[4];
	long int filenum = 0;
	fread(header, 1, 4, inqlp);
	if (memcmp(magic, header, 4)) {
		printf("Wrong QLP Header!\n");
	}
	fread(&filenum, addr_size, 1, inqlp);
	//printf("Files in QLP: %i\n", filenum);
	QLPFILE* filelist = calloc(filenum, sizeof(QLPFILE));
	if (!filelist) {
		printf("Malloc error!\n");
	}
	QLPFILE* readtmp = filelist;
	for (int i = 0; i < filenum; i++) {
		fread(readtmp, sizeof(QLPFILE), 1, inqlp);
		readtmp++;
	}
	readtmp = filelist;
	int buffer_left = 0;
	char* buffer;
	for (int i = 0; i < filenum; i++) {
		buffer_left = readtmp->size;
		fseek(inqlp, readtmp->addr * addr_size, SEEK_SET);
		//printf("Saving file #%i %s (%i bytes)...\n", i + 1, readtmp->name, readtmp->size);
		FILE* outqlp = fopen(readtmp->name, "wb+");
		if (!outqlp) {
			perror("Can't open output file");
			return 1;
		}
		while (buffer_left > 0) {
			if (buffer_left > buffer_max) {
				buffer = malloc(buffer_max);
				fread(buffer, 1, buffer_max, inqlp);
				fwrite(buffer, 1, buffer_max, outqlp);
				buffer_left -= buffer_max;

			}
			else {
				buffer = malloc(buffer_left);
				fread(buffer, 1, buffer_left, inqlp);
				fwrite(buffer, 1, buffer_left, outqlp);
				buffer_left = 0;
			}
			free(buffer);
		}
		fclose(outqlp);
		readtmp++;
	}
	free(filelist);
	printf("QLP save completed!\n");
	return 0;
}

int makeqlp(int c, char* v[]) {
	int oldsize = 0;
	long int filenum = c - 2;
	QLPFILE* filelist = calloc(filenum, sizeof(QLPFILE));
	QLPFILE* writetmp = filelist;
	long int lastaddr = (filenum * (sizeof(QLPFILE) / addr_size)) + 2; //Header must be the same size as system pointer (4)!
	for (int i = 2; i < c; i++) {
		FILE* inqlp = fopen(v[i], "rb+");
		if (!inqlp) {
			perror("Couldn't open input file");
			printf("Failed file: %s\n", v[i]);
			return 1;
		}
		fseek(inqlp, 0, SEEK_END);
		writetmp->size = ftell(inqlp);
		int padbytes = addr_size - (writetmp->size % addr_size);
		if (padbytes == addr_size) {
			padbytes = 0;
		}
		if (i == 2) {
			writetmp->addr = lastaddr;
			oldsize = writetmp->size + padbytes;
		}
		else {
			
			writetmp->addr = (oldsize / addr_size) + lastaddr;
			oldsize = writetmp->size + padbytes;
		}
		
		lastaddr = writetmp->addr;
		int fnlen = strlen(v[i]);
		if (fnlen > 15) {
			for (int j = 0; j < 15; j++) {
				writetmp->name[j] = v[i][j + (fnlen - 15)];
			}
		}
		else {
			for (int j = 0; j < fnlen; j++) {
				writetmp->name[j] = v[i][j];
			}
		}
		//printf("file found: %s size:%i addr:%i\n", writetmp->name, writetmp->size, writetmp->addr);
		fclose(inqlp);
		writetmp++;
	}
	FILE* outqlp = fopen(v[1], "wb+");
	if (!outqlp) {
		perror("Couldn't open output QLP file");
		return 1;
	}
	fwrite(magic, addr_size, 1, outqlp);
	fwrite(&filenum, 1, addr_size, outqlp);
	fwrite(filelist, filenum, sizeof(QLPFILE), outqlp);
	writetmp = filelist;
	int buffer_left = 0;
	char* buffer;
	for (int i = 2; i < c; i++) {
		FILE* inqlp = fopen(v[i], "rb+");
		if (!inqlp) {
			perror("Couldn't open input file");
			printf("Failed file: %s\n", v[i]);
			return 1;
		}
		buffer_left = writetmp->size;
		while (buffer_left > 0) {
			//printf("%s has %i bytes left!\n", v[i], buffer_left);
			if (buffer_left > buffer_max) {
				buffer = malloc(buffer_max);
				fread(buffer, 1, buffer_max, inqlp);
				fwrite(buffer, 1, buffer_max, outqlp);
				buffer_left -= buffer_max;

			}
			else {
				buffer = malloc(buffer_left);
				fread(buffer, 1, buffer_left, inqlp);
				fwrite(buffer, 1, buffer_left, outqlp);
				buffer_left = 0;
			}
			free(buffer);
		}
		int padbytes = addr_size - (writetmp->size % addr_size);
		if (padbytes == addr_size) {
			padbytes = 0;
		}
		fwrite(padder, 1, padbytes, outqlp);
		writetmp++;

	}
	printf("Packing complete!\n");
	return 0;
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		printf("Usage: %s [options] outfile infile [infile2] [infile3...\n",argv[0]);
		printf("Option -x extracts all resources from the first file passed as an argument.\n");
		printf("Otherwise, QLPTool will pack all other specified files into the first file.\n");
		return 0;
	}
	else if (!strcmp(argv[1] , "-x")) {
		return splitqlp(argc,argv);
	}
	else {
		return makeqlp(argc, argv);
	}
	
}