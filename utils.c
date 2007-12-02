#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/mman.h>
#include<fcntl.h>
#include"stdint.h"
#include"main.h"
#include"macho.h"
#include"utils.h"

void file_open(char *s)
{
	off_t len;
	fd=open(filename,O_RDWR);
	if(fd==-1)
	{
		printf("error opening %s file.\nexiting...\n",s);
		exit(1);
	}
	len=lseek(fd,(off_t)0,SEEK_END);
	faddr=(char*)mmap(NULL,(size_t)len,PROT_READ|PROT_WRITE,MAP_FILE,fd,(off_t)0);
	if(faddr==-1) die("error on mmap(ing) the file");
}
char* file_probe()
{
	struct mach_header *mac;
	
	
}
void die(char *s)
{
	printf("%s\n",s);
	exit(1);
}
