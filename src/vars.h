enum {TYPE_NULL,TYPE_STRING,TYPE_VAL,TYPE_STRUCT,TYPE_NODE_STRUCT};
enum {S_MAIN,S_PH,S_SH,S_LC};
struct _var
{
	char *name;
	int type;
	int len;
	union{
		int val;
		char *s;
		struct {
			struct _gv *first,*last;
		}p;
	};
};
struct _p
{
	struct _gv *first,*last;
}*bookmark;
struct _pp
{
	struct _gv **f,**l;
};
struct _gv
{
	struct _var v;
	struct _gv *prev,*next;
} *gv_first,*gv_last,*gv_ptr,*s_first,*s_last,*s_ptr;
void set_var(char *name,int type,void *val);
struct _var* get_s_var(char *name);
struct _var* get_var(char *name);
struct _stack_s
{
	char *name;
	struct _stack_s *prev;
}*last_stack;
void free_bookmark(void);
void set_bookmark(struct _p *p);
struct _p* get_bookmark(void);
//void push(char *s);
//struct _var* peek(void);
//struct _var* pop(void);
//void s_remove(void);
