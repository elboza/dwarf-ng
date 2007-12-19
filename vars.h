struct _gv
{
	char *name;
	int val;
	struct _gv *prev,*next;
} *gv_first,*gv_last,*gv_ptr;

void set_var(char *name,int val);
int get_var(char *name);
