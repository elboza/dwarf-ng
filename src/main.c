/*
 main.c : dwarf's main file.

 (c) 2007-2011-2013-2017 Fernando Iazeolla

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<getopt.h>
#include"libdwarf.h"
#include"repl.h"
#include "config.h"
#include "dw_readline_completion.h"
#include "sh_switchers.h"
#include "utils.h"


struct m_action{
	int file;
	int shell;
	int script;
	int exec;
	int stdin;
	int probe;
};
void usage()
{
  printf("dwarf-ng-%s (c) Fernando Iazeolla 2007-2013-2017.\n",VERSION);
	printf("\nUSAGE: ");
	printf("dwarf [options] [file]\n");
	printf("valid options:\n");
	printf("-s              --stdin                 file from stdin\n");
	printf("-i              --shell --interactive   interactive (shell mode)\n");
	printf("-h              --help                  show this help\n");
	printf("-c '<commands>' --command '<commands>'  command  mode (execute commands)\n");
	printf("-e '<commands>' --command '<commands>'  command  mode alias (execute commands)\n");
	printf("-x <file>       --execute <file>        execute file script\n");
	printf("-v              --version               show dwarf-ng's version number\n");
	printf("-f <file>       --file <file>           open file\n");
	printf("-p              --probe                 probe file and print file type and info\n");
	printf("-C [n]          --colors [n]            colored output (n=theme num)\n");
	printf("-B              --nocolors              nocolored output\n");
	printf("-T [n]          --theme [n]             theme 2 colored output (n=theme num)\n");
	printf("-W              --work-on-copy          work on copy file\n");
	printf("-P              --no-work-on-copy       no work on copy file\n");
	printf("-D <name>       --tmpdir <name>         set tmp dir path\n");
	printf("-N <name>       --tmpname <name>        set tmp filename\n");
	printf("-s <offs>       --seek <offs>           set seek offset\n");
	printf("-b <size>       --block <size>          set block size\n");
	exit(1);
}
void usage_b()
{
	//char *s="VERSION";
	printf("dwarf-ng-%s (c) Fernando Iazeolla 2007-2013-2017\n",VERSION);
	printf("for help type: dwarf --help\n");
}
off_t parse_num_arg(char *s){
	int x;
	if(*s=='0' && *(s+1)=='x'){
		sscanf(s+2,"%X",&x);
		return (off_t)x;
	}
	return (off_t) atoi(s);
}
void parse_args(int argc,char **argv,struct m_action *action,char *f2,char *cmds,char *filename)
{
	//char *s1;
	int c;
	action->shell=0;
	action->script=0;
	action->file=0;
	action->exec=0;
	action->stdin=0;
	action->probe=0;
	while (1)
	{
		static struct option long_options[] =
		{
			{"execute",required_argument,0,'x'},
			{"command",required_argument,0,'c'},
			{"command",required_argument,0,'e'},
			{"interactive",no_argument,0,'i'},
			{"shell",no_argument,0,'i'},
			{"help",no_argument,0,'h'},
			{"version",no_argument,0,'v'},
			{"stdin",no_argument,0,'s'},
			{"colors",optional_argument,0,'C'},
			{"nocolors",no_argument,0,'B'},
			{"theme",optional_argument,0,'T'},
			{"work-on-copy",no_argument,0,'W'},
			{"no-work-on-copy",no_argument,0,'P'},
			{"tmpdir",required_argument,0,'D'},
			{"tmpname",required_argument,0,'N'},
			{"seek",required_argument,0,'S'},
			{"block",required_argument,0,'b'},
			{"file",required_argument,0,'f'},
			{"probe",required_argument,0,'p'},
			{0,0,0,0,}
			
		};
		int option_index = 0;
		c = getopt_long (argc, argv, "pPT::C::Bsvhie:c:x:WD:N:S:b:f:",long_options, &option_index);
		if (c == -1) break;
		switch(c)
		{
			case 'i':
				action->shell=1;
				cfg.inshell=1;
				//shell();
				//strncpy(cmd,"quit;",1024);
				break;
			case 'x':
				//printf("input script file:%s\n",optarg);
				action->script=1;
				strncpy(f2,optarg,FILENAME_LEN);
				//run_script(optarg);
				//execute("quit");
				break;
			case 'v':
				usage_b();
				break;
			case 'c':
			case 'e':
				//printf("to execute:%s ....testing..\n",optarg);
				action->exec=1;
				strncpy(cmds,optarg,FILENAME_LEN);
				break;
			case 's':
				action->stdin=1;
				break;
			case 'f':
				strncpy(filename,optarg,FILENAME_LEN);
				action->file=1;
				break;
			case 'p':
				action->probe=1;
				break;
			case 'T':
				cfg.colors=true;
				cfg.theme=E_THEME_BCOLOR;
				set_colors(E_THEME_BCOLOR);
				if(optarg) set_colors(atoi(optarg));
				break;
			case 'W':
				cfg.work_on_copy=true;
				break;
			case 'P':
				cfg.work_on_copy=false;
				break;
			case 'S':
				cfg.seek=parse_num_arg(optarg);
				break;
			case 'b':
				cfg.block=parse_num_arg(optarg);
				break;
			case 'D':
				strncpy(cfg.copydir,optarg,FILENAME_LEN);
				break;
			case 'N':
				strncpy(cfg.copyname,optarg,FILENAME_LEN);
				break;
			case 'C':
				cfg.colors=true;
				cfg.theme=E_THEME_COLOR;
				set_colors(E_THEME_COLOR);
				if(optarg) set_colors(atoi(optarg));
				break;
			case 'B':
				cfg.colors=false;
				set_colors(cfg.colors);
				break;
			case 'h':
			case '?':
				usage();
				break;
			default:
				usage_b();
				break;
		}
		
	}
}
int main(int argc,char **argv)
{
	struct m_action action;
	char filename[FILENAME_LEN],filescript[FILENAME_LEN],cmd[FILENAME_LEN];
	dw_init();
	initialize_readline();
	add_cmds_completions();
	strncpy(filename,"<NULL>",FILENAME_LEN);
	action.file=0;
	parse_args(argc,argv,&action,&filescript[0],&cmd[0],&filename[0]);
	
	if(argc<2) usage_b();
	if(optind<argc) {strncpy(filename,argv[optind],FILENAME_LEN);action.file=1;}
	//else{strncpy(filename,"<NULL>",FILENAME_LEN);action.file=0;}
	//if(ilook_debug) look_debug();
	//execute(cmd);
	if(action.file)
	{
		file_open(NULL,filename,true);
		//free_completion();
		add_sh_completion();
	}
	if(action.stdin){
		//printf("from stdin...\n");
		open_stdin();
		reset_stdin();
		add_sh_completion();
	}
	if(action.probe){
		sw_do_info();
	}
	if(action.script)
	{
		run_script(filescript);
	}
	if(action.exec)
	{
		execute(cmd);
	}
	if(action.shell)
	{
		shell();
		printf("Bye.\n");
	}
	return 0;
}
