#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include"stdint.h"
#include"main.h"
#include"macho.h"

void look_debug()
{
	int a;
	a=0xf;
	printf("look_debug function !!!!!!!!!!!!!!!!!!!!!!!!\n");
	file_probe();
	printf("%d %d\n",file_type,a);
}
