#include <stdio.h>
#include <string.h>

typedef unsigned long int u_long;
const char magic[4] = { 'Q', 'L', 'P', 0x00 };
const int addr_size = 4; //maybe use 4th byte of header to change this?

typedef struct {
	char	name[16];
	u_long	size;
	u_long	addr;
} QLPFILE;


int		QLPfileCount(u_long* qlp_ptr);
QLPFILE	QLPfile(u_long* qlp_ptr, int filenum);
u_long* QLPfilePtr(u_long* qlp_ptr, int filenum);


int QLPfileCount(u_long* qlp_ptr) {

	return *(u_long*)(qlp_ptr + 1);

}

QLPFILE QLPfile(u_long* qlp_ptr, int filenum) {

	return *((QLPFILE*)(qlp_ptr + 2) + filenum);

}

u_long* QLPfilePtr(u_long* qlp_ptr, int filenum) {

	return (qlp_ptr + ((QLPFILE*)(qlp_ptr + 2) + filenum)->addr);

}

int main() {

	FILE* inqlp = fopen("C:\\qlp\\graphics.qlp", "rb+");
	if (!inqlp) {
		perror("Can't open QLP");
		return 1;
	}
	char header[4];
	int filenum = 0;
	fread(header, 1, 4, inqlp);
	if (memcmp(magic, header, 4)) {
		printf("Wrong QLP Header!\n");
	}
	fread(&filenum, addr_size, 1, inqlp);
	printf("Files in QLP: %i\n", filenum);
	char filename[16];
	unsigned long int filesize = 0;
	unsigned long int fileaddr = 0;
	for (int i = 0; i < filenum; i++) {
		fread(filename, 1, 16, inqlp);
		fread(&filesize, addr_size, 1, inqlp);
		fread(&fileaddr, addr_size, 1, inqlp);
		printf("Name: %s\nSize: %i\nAddress Offset: %i\n", filename, filesize, fileaddr);
	}
}