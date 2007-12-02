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
	printf("this is look debug:%s\n",filename);
	exit(1);
}
