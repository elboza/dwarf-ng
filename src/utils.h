typedef enum {comp_fixed,comp_var,comp_discardable} rl_comp;
struct comp_list
{
	char *s;
	int type;
	struct comp_list *prev,*next;
}*first_comp,*last_comp;

void file_probe(void);
void file_open(char *s);
void die(char *s);
char* rl_gets(char *prompt);
void execute(char *s);
void file_close(void);
void shell(void);
void load_headers(void);
void load_macho_hd(void);
void load_elf_hd(void);
void initialize_readline(void);
char ** dwarf_completion();
char* dwarf_command_generator();
void add_completion(char *path,char *item,int type);
void free_completion(void);
void add_cmds_completions(void);
