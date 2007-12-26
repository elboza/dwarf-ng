enum {TYPE_NULL,TYPE_STRING,TYPE_VAL};
enum {S_MAIN,S_PH,S_SH,S_LC};
struct _var
{
	char *name;
	int type;
	union{
		int val;
		char *s;
	};
};
struct _gv
{
	struct _var v;
	struct _gv *prev,*next;
} *gv_first,*gv_last,*gv_ptr,*s_first,*s_last,*s_ptr;
void set_var(char *name,int type,void *val);
struct _gv* get_var(char *name);
