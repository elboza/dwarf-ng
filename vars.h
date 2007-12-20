enum {TYPE_NULL,TYPE_STRING,TYPE_VAL};
struct _gv
{
	char *name;
	int val;
	struct _gv *prev,*next;
} *gv_first,*gv_last,*gv_ptr;
struct _s_var
{
	char *name;
	int type;
	int val;
	struct _s_var *prev,*next;
} *s_first,*s_last,*s_ptr;
void set_var(char *name,int val);
int get_var(char *name);
