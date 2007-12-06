int ilook_debug; /* if set perform the debug function only*/
#define MAX_FILENAME 255
#define MAX_STR 255
char filename[MAX_FILENAME],*cmd;
int forced;  /*if set force a file to be treat as a given kind reguardless of what really kind is*/
int file_type;
int fd;
void *faddr;

//file types
#define FT_NULL		0
#define FT_ELF		1
#define FT_MZ		2
#define FT_MACHO	3
#define FT_PE		4
