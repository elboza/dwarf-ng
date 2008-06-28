/*
 main.c : dwarf's main file.

 (c) 2007-2008 Fernando Iazeolla

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
#include<stdlib.h>
#include<unistd.h>
#include<getopt.h>
#include<string.h>
#include<errno.h>
#include"main.h"
#include"dw_readline_completion.h"
#include"vars.h"


void usage()
{
	printf("dwarf-%s (c) Fernando Iazeolla <drugo@code.autistici.org>\n",VERSION);
	printf("\nUSAGE: ");
	printf("dwarf [options] [file]\n");
	printf("valid options:\n");
	printf("-i              --shell --interactive        interactive (shell mode)\n");
	printf("-h              --help                       show this help\n");
	printf("-c '<commands>' --command '<commands>'       command  mode (execute commands)\n");
	printf("-x <file>       --execute <file>             execute file script\n");
	printf("-t              --type                       file type\n");
//	printf("-s <all,sh,main>  section headers\n");
//	printf("-f type       force to treat as a <pe|elf|mach-o|mz> file\n");
//	printf("-r            reloc (relocate section offsets)\n");
//	printf("-S --size= ±val,val,±section\n");
//	printf("--section= ±pos,name\n");
//	printf("--rel= sectors, sectname,±offs      (relocate section(s) offset)\n");
//	printf("--ph=±pos,name\n");
//	printf("-m --move=init,[end,len,]init2       move block\n");
	exit(1);
}
void usage_b()
{
	printf("dwarf-%s (c) Fernando Iazeolla 2007-2008\n",VERSION);
	printf("for help type: dwarf --help\n");
}
void get_defaults_arg(struct m_action *action)
{
	ilook_debug=0;
	action->file=0;
	action->shell=0;
	action->script=0;
	action->exec=0;
	forced=FT_NULL;
	file_type=FT_NULL;
	cmd=(char*)malloc(MAX_CMD);
	fd=0;
	gv_first=NULL;
	gv_last=NULL;
	last_stack=NULL;
}
void parse_args(int argc,char **argv,struct m_action *action)
{
	char *s1;
	int c;
	while (1)
	{
		static struct option long_options[] =
		{
			{"execute",required_argument,0,'x'},
			{"command",required_argument,0,'c'},
			{"interactive",no_argument,0,'i'},
			{"shell",no_argument,0,'i'},
			{"type",required_argument,0,'t'},
			{"help",no_argument,0,'h'},
			{0,0,0,0,}
			
		};
		int option_index = 0;
		c = getopt_long (argc, argv, "hidtrS:m:s:c:x:f:",long_options, &option_index);
		if (c == -1) break;
		switch(c)
		{
			case 'i':
				printf("dwarf Copyright (C) 2007-2008 Fernando Iazeolla\n");
    				printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
    				printf("This is free software (GNU GPLv2), and you are welcome to redistribute it\n");
    				printf("under GNU GPL version 2 (see <http://www.gnu.org/licenses/>).\n");
				printf("entering shell-interactive mode....testing...\n");
				action->shell=1;
				//shell();
				//strncpy(cmd,"quit;",1024);
				break;
			case 't':
				//printf("type\n");
				strncpy(cmd,"type;",1024);
				action->exec=1;
				break;
			case 'x':
				//printf("input script file:%s\n",optarg);
				action->script=1;
				strncpy(scriptfile,optarg,MAX_FILENAME);
				//execute_script(optarg);
				//strncpy(cmd,"quit",1024);
				break;
			case 's':
				if((strcmp(optarg,"all"))==0) ;//section_type=SEC_ALL;
				if((strcmp(optarg,"main"))==0) ;
				if((strcmp(optarg,"sh"))==0) ;
				break;
			case'f':
				if((strcmp(optarg,"null"))==0) forced=FT_NULL;  //default
				if((strcmp(optarg,"pe"))==0) forced=FT_PE;
				if((strcmp(optarg,"elf"))==0) forced=FT_ELF;
				if((strcmp(optarg,"mach-o"))==0) forced=FT_MACHO;
				if((strcmp(optarg,"mz"))==0) forced=FT_MZ;
				file_type=forced;
				break;
			case 'r':
				
				break;
			case 'S':
				printf("size!...to develop.\n");
				break;
			case 'm':
				printf("move!....to develop.\n");
				break;
			case 'd':
				ilook_debug=1;
				break;
			case 'c':
				//printf("to execute:%s ....testing..\n",optarg);
				action->exec=1;
				strncpy(cmd,optarg,1024);
				break;
			case 'h':
			case '?':
				usage();
				break;
			default:
				usage();
				break;
		}
		
	}
}
void look_data_ok()
{
	
}
void config_parse()
{
	
}
int main(int argc,char **argv)
{
	struct m_action action;
	char fileopen[MAX_CMD];
	get_defaults_arg(&action);
	config_parse();
	initialize_readline();
	add_cmds_completions();
	cpu_endian=probe_endian();
	file_endian=little_endian;
	parse_args(argc,argv,&action);
	look_data_ok();
	if(argc<2) usage_b();
	if(optind<argc) {strncpy(filename,argv[optind],MAX_FILENAME);action.file=1;}
	else{strncpy(filename,"<NULL>",MAX_FILENAME);}
	//if(ilook_debug) look_debug();
	//execute(cmd);
	if(action.file)
	{
		sprintf(fileopen,"open %s",filename);
		execute(fileopen);
	}
	if(action.script)
	{
		execute_script(scriptfile);
	}
	if(action.exec)
	{
		execute(cmd);
	}
	if(action.shell)
	{
		shell();
	}
	
	printf("Bye.\n");
	free(cmd);
	file_close();
	return 0;
}
