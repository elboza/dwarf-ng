/*
 libdwarf.c : functions implementation.
 
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
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<math.h>
#include"libdwarf.h"

extern FILE *cfgyyin;
extern int errno,cfgyydebug,cfgyyparse(void);

void dw_init()
{
	char *envstr,str[FILENAME_LEN];
	cfg_reset();
	
	filecfg_first=NULL;
	filecfg_last=NULL;
	fc_ptr=NULL;
	gv_first=NULL;
	gv_last=NULL;
	
	envstr=getenv("TMPDIR");
	if(envstr) strncpy(cfg.copydir,envstr,FILENAME_LEN);
	readcfg("/etc/dwarfrc");
	envstr=getenv("HOME");
	if(envstr)
	{
		sprintf(str,"%s/.dwarfrc",envstr);
		readcfg(str);
	}
	if(cfg.colors) set_colors(true); else set_colors(false);
}
void readcfg(char *s)
{
	FILE *fp;
	//int x;
	fp=fopen(s,"r");
	if(fp==NULL) return;
	cfgyyin=fp;
	//cfgyydebug=1;   /*use -t parameter in bison*/
	cfgyyparse();
	fclose(fp);
}
void cfg_reset(void)
{
	cfg.writable=false;
	cfg.can_grow=false;
	cfg.work_on_copy=false;
	cfg.changed_altered=false;
	cfg.fd_type=FD_REGULAR_FILE;
	cfg.verbose=0;
	cfg.fd=0;
	cfg.cpu_endian=probe_cpu_endian();
	cfg.file_endian=little_endian;
	cfg.file_bit_class=0;
	cfg.file_cpu_type=0;
	cfg.file_type=0;
	cfg.filesize=0;
	cfg.faddr=NULL;
	cfg.seek=0;
	cfg.block=0x100;
	cfg.colors=false;
	cfg.theme=E_THEME_NONE;
	strcpy(cfg.copydir,"/tmp");
	strcpy(cfg.copyname,"dw_temp_file");
	cfg.next=NULL;
	cfg.prev=NULL;
}
struct _cfg* newfilecfg(void)
{
	struct _cfg *ptr;
	ptr=malloc(sizeof(struct _cfg));
	if(!ptr) {warn("error allocating new filecfg struct"); return NULL;}
	ptr->writable=cfg.writable;
	ptr->can_grow=cfg.can_grow;
	ptr->work_on_copy=cfg.work_on_copy;
	ptr->changed_altered=cfg.changed_altered;
	ptr->fd_type=cfg.fd_type;
	ptr->verbose=cfg.verbose;
	ptr->fd=cfg.fd;
	ptr->cpu_endian=cfg.cpu_endian;
	ptr->file_endian=cfg.file_endian;
	ptr->file_bit_class=cfg.file_bit_class;
	ptr->file_cpu_type=cfg.file_cpu_type;
	ptr->file_type=cfg.file_type;
	ptr->filesize=cfg.filesize;
	ptr->faddr=cfg.faddr;
	ptr->seek=cfg.seek;
	ptr->block=cfg.block;
	ptr->colors=cfg.colors;
	ptr->theme=cfg.theme;
	strcpy(ptr->copydir,cfg.copydir);
	strcpy(ptr->copyname,cfg.copyname);
	ptr->prev=NULL;
	ptr->next=NULL;
	
	return ptr;
}
int addfilecfg(struct _cfg *ptr)
{
	if(ptr==NULL) return -1;
	if(filecfg_first==NULL)
	{
		filecfg_first=ptr;
	}
	else
	{
		filecfg_last->next=ptr;
		ptr->prev=filecfg_last;
	}
	filecfg_last=ptr;
	return 0;
}
void rmfilecfg(struct _cfg *ptr)
{
	if(ptr==NULL) return;
	if(ptr->prev) ptr->prev->next=ptr->next;
	if(ptr->next) ptr->next->prev=ptr->prev;
	if(ptr==filecfg_first) filecfg_first=ptr->next;
	if(ptr==filecfg_last) filecfg_last=ptr->prev;
	free(ptr);
	fc_ptr=filecfg_first;
}
struct _cfg* getnewfilecfg(void)
{
	struct _cfg *ptr;
	ptr=newfilecfg();
	addfilecfg(ptr);
	return ptr;
}
void deleteallfilecfg(void)
{
	struct _cfg *ptr;
	ptr=filecfg_first;
	while(ptr)
	{
		if(ptr->next)
		{
			ptr=ptr->next;
			if(ptr) free(ptr->prev);
		}
		else
		{
			free(ptr);
			ptr=NULL;
		}
	}
	filecfg_first=NULL;
	filecfg_last=NULL;
	fc_ptr=NULL;
}
void getcopyname(char *s)
{
	static int num=0;
	char tmp[FILENAME_LEN];
	strncpy(tmp,s,FILENAME_LEN);
	sprintf(s,"%s/%s_%d",fc_ptr->copydir,tmp,num++);
}
void file_open(char *s,int probeb)
{
	enum{opencpspec,opencpnorm,opennorm,noopen};
	struct stat st;
	int x,opentype;
	char openstr[1024],execstr[4096];
	off_t filesize;
	x=stat(s,&st);
	if(x==-1) {warn("error obtaining stat info");return;}
	fc_ptr=getnewfilecfg();
	if(!fc_ptr) {warn("error getting new file cfg");return;}
	switch (st.st_mode & S_IFMT) {
		case S_IFBLK:
		case S_IFCHR:
			opentype=opencpspec;
			break;
		case S_IFDIR:  warn("cannot open directory files\n"); opentype=noopen;break;
		case S_IFIFO:  warn("cannot open FIFO/pipe files\n"); opentype=noopen;break;
		case S_IFLNK:  warn("cannot open symlink files\n");   opentype=noopen;break;
		case S_IFSOCK: warn("cannot open socket files\n");    opentype=noopen;break;
		case S_IFREG:  if(fc_ptr->work_on_copy) opentype=opencpnorm; else opentype=opennorm;            
			break;
		default:       warn("unknown file\n");                opentype=noopen;break;
	}
	if(opentype==noopen) {rmfilecfg(fc_ptr); return;}
	if((opentype==opencpspec)||(opentype==opencpnorm))
	{
		fc_ptr->work_on_copy=true;
		getcopyname(fc_ptr->copyname);
		x=512*10;
		if(opentype==opencpspec)
		{
			sprintf(openstr,"bs=%d count=1",x);
			fc_ptr->can_grow=false;
			fc_ptr->writable=false;
		}
		else 
		{
			strcpy(openstr,"");
			fc_ptr->can_grow=true;
			fc_ptr->writable=true;
		}
		sprintf(execstr,"dd if=%s of=%s %s",s,fc_ptr->copyname,openstr);
		x=system(execstr);
		if(x==-1) {warn("error making copyfile");rmfilecfg(fc_ptr);return;}
		fc_ptr->fd=open(fc_ptr->copyname,O_RDWR);
	}
	if(opentype==opennorm)
	{
		fc_ptr->can_grow=true;
		fc_ptr->writable=true;
		fc_ptr->fd=open(s,O_RDWR);
	}
	if(fc_ptr->fd==-1)
	{
		printf("error opening file.\n");
		if(errno==EACCES) printf("file access not allowed, check permissions.\n");
		rmfilecfg(fc_ptr);
		return;
	}
	strncpy(fc_ptr->name,s,FILENAME_LEN);
	filesize=lseek(fc_ptr->fd,(off_t)0,SEEK_END);
	fc_ptr->filesize=filesize;
	fc_ptr->fd_type=FD_REGULAR_FILE;
	fc_ptr->faddr=(char*)mmap(NULL,(size_t)filesize,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,fc_ptr->fd,(off_t)0);
	if(fc_ptr->faddr==MAP_FAILED) {warn("error on mmap(ing) the file");close(fc_ptr->fd);rmfilecfg(fc_ptr);return;}
	if(probeb) file_probe();
	printf("%s opened\n",s);
}
void open_stdin(void){
	size_t buffsize=sizeof(char)*4096;
	off_t len;
	fc_ptr=getnewfilecfg();
	if(!fc_ptr) {warn("error getting new file cfg");return;}
	fc_ptr->can_grow=false;
	fc_ptr->writable=false;
	fc_ptr->faddr=(char*)malloc(buffsize);
	if(!fc_ptr->faddr){
		warn("error allocating file buffer");
		rmfilecfg(fc_ptr);
		return;
	}
	len=read(STDIN_FILENO,fc_ptr->faddr,buffsize);
	fc_ptr->filesize=len;
	fc_ptr->fd_type=FD_STDIN; // stdin fd file...
	file_probe();
}
void close_stdin(void){
	if(!fc_ptr) {printf("no file opened!\n");return;}
	if(fc_ptr->faddr) free(fc_ptr->faddr);
	rmfilecfg(fc_ptr);
	fc_ptr=NULL;
}
void file_close(void)
{
	int x;
	char cmd[4096];
	off_t filesize;
	if(!fc_ptr) {printf("no file opened!\n");return;}
	if(fc_ptr->fd_type==FD_STDIN) return close_stdin();
	if(fc_ptr->fd)
	{
		filesize=lseek(fc_ptr->fd,(off_t)0,SEEK_END);
		x=munmap(fc_ptr->faddr,(size_t)filesize);
		close(fc_ptr->fd);
		if(fc_ptr->work_on_copy)
		{
			sprintf(cmd,"rm %s",fc_ptr->copyname);
			x=system(cmd);
			if(x==-1) warn("could not remove the temp file !");
		}
		rmfilecfg(fc_ptr);
		fc_ptr=NULL;
	}
	
}
void file_close_all(void){
	while(filecfg_first){
		file_close();
		fc_ptr=filecfg_first;
	}
}
void dw_quit(void){
	file_close_all();
}
void file_save(char *s)
{
	int x;
	char cmd[4096];
	x=0;
	if(!fc_ptr) {printf("no file opened.\n"); return;}
	if(!s) s=strdup(fc_ptr->name);
	if(!s) {printf("error saving file.\n"); return;}
	if(!fc_ptr->writable) {warn("file not writable"); return;}
	if(fc_ptr->work_on_copy)
	{
		sprintf(cmd,"cp %s %s",fc_ptr->copyname,s);
		x=system(cmd);
		if(x==-1) warn("error saving file!");
	}
	if(x!=-1) printf("file saved.\n");
}
off_t filesize(int fd)
{
	off_t len;
	if(!fc_ptr) {printf("no file opened!\n");return -1;}
	if(fc_ptr->fd==FD_STDIN){
		len=fc_ptr->filesize;
	}else{
		if(fd<1) {printf("no file opened.\n");return -1;}
		len=lseek(fd,(off_t)0,SEEK_END);
	}
	return len;
}
void prettybyte(char *s,off_t num)
{
	off_t Gb,Mb,kB,b;
	off_t x;
	char a[255];
	Gb=1000000000;
	Mb=1000000;
	kB=1000;
	b=1;
	if(num>=Gb)
	{
			x=(off_t)(lround(num/Gb));
			dottedbyte(a,x);
			sprintf(s,"%s Gb",a);
	}
	if(num>=Mb)
	{
			x=(off_t)(round(num/Mb));
			dottedbyte(a,x);
			sprintf(s,"%s Mb",a);
	}
	if(num>=kB)
	{
			x=(off_t)(round(num/kB));
			dottedbyte(a,x);
			sprintf(s,"%s kB",a);
			return;
	}
	if(num>=b)
	{
			x=num;
			dottedbyte(a,x);
			sprintf(s,"%s bytes",a);
	}
}
void dottedbyte(char *s,off_t num)
{
	int x,y,z;
	char aux[255];
	sprintf(s,"%lld",(long long)num);
	x=strlen(s)-1;
	y=0;
	z=0;
	while(x>=0)
	{
			aux[y++]=s[x--];
			z++;
			if((z % 3)==0) aux[y++]='.';
	}
	aux[y]='\0';
	x=strlen(aux)-1;
	y=0;
	while(x>=0)
	{
			s[y++]=aux[x--];
	}
	s[y]='\0';
}
int growth(off_t len)
{
	off_t offset;
	//int n;
	char *x;
	if(!fc_ptr) {printf("no file opened!\n");return false;}
	if(!fc_ptr->can_grow) {printf("this file cannot change its size\n"); return false;}
	if(!fc_ptr->fd) {printf("no file opened!\n");return false;}
	x=(char*)malloc(len);
	if(x==NULL) {warn("error allocating mem");return false;}
	offset=lseek(fc_ptr->fd,(off_t)0,SEEK_END);
	write(fc_ptr->fd,x,(size_t) len);
	free(x);
	fc_ptr->changed_altered=true;
	#if HAVE_MREMAP
	fc_ptr->faddr=(char*)mremap(fc_ptr->faddr,(size_t) offset,(size_t) (offset+len),MAP_FILE|MAP_SHARED);
	#else
	munmap(fc_ptr->faddr,(size_t) offset);
	fc_ptr->faddr=(char*)mmap(NULL,(size_t) (offset+len),PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,fc_ptr->fd,(off_t)0);
	#endif
	if(fc_ptr->faddr==MAP_FAILED) die("error on mmap(ing) the file");
	return true;
}
int shrink(off_t len)
{
	off_t offset,new_offset;
	int n;
	if(!fc_ptr) {printf("no file opened!\n");return false;}
	if(!fc_ptr->can_grow) {printf("this file cannot change its size\n"); return false;}
	if(!fc_ptr->fd) {warn("no file opened!\n");return false;}
	offset=lseek(fc_ptr->fd,(off_t)0,SEEK_END);
	new_offset=offset-len;
	n=ftruncate(fc_ptr->fd,new_offset);
	fc_ptr->changed_altered=true;
	if(n==-1) die("error shrinking file");
	#if HAVE_MREMAP
	fc_ptr->faddr=(char*)mremap(fc_ptr->faddr,(size_t) offset,(size_t) new_offset,MAP_FILE|MAP_SHARED);
	#else
	munmap(fc_ptr->faddr,(size_t) offset);
	fc_ptr->faddr=(char*)mmap(NULL,(size_t)new_offset,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,fc_ptr->fd,(off_t)0);
	#endif
	if(fc_ptr->faddr==MAP_FAILED) die("error on mmap(ing) the file");
	return true;
}
void extract(off_t from,off_t len,char *file)
{
	char *mem;
	off_t i;
	FILE *fp;
	if(!fc_ptr) {printf("no file opened!\n");return;}
	mem=(char*)fc_ptr->faddr;
	mem+=from;
	fp=fopen(file,"a");
	if(!fp) {printf("error opening %s file.\n",file); return;}
	for(i=0;i<len;i++)
	{
		fputc((int)(*(mem++)),fp);
	}
	fclose(fp);
}
void move(off_t from,off_t end,off_t to)
{
	#define reversed 1
	#define normal 0
	off_t to_end,tmp,i;
	int direction;
	char *mem;
	//printf("move from:%d end:%d to%d\n",from,end,to);
	if(!fc_ptr) {printf("no file opened!\n");return;}
	if(!fc_ptr->faddr) {printf("no files is opened!\n");return;}
	if(end<from)
	{
		tmp=from;
		from=end;
		end=tmp;
	}
	to_end=end-from+to;
	//printf("* from:%d end:%d to:%d to_end:%d\n",from,end,to,to_end);
	if((from<to) && (to<end)) direction=reversed; else direction=normal;
	mem=(char*)fc_ptr->faddr;
	if(direction==reversed)
	{
		//printf("reversed\n");
		for(i=end;i>=from;i--)
		{
			mem[to_end]=mem[i];
			to_end--;
		}
	}
	else
	{
		//printf("normal\n");
		for(i=from;i<=end;i++)
		{
			mem[to]=mem[i];
			to++;
		}
	}
	fc_ptr->changed_altered=true;
}
int mod_len(off_t len)
{
	if(len<0) return (shrink(-len)); else return (growth(len));
}
void move_r_pos(off_t from,off_t len,off_t to)
{
	move(from,from+len,to);
}
void inject_byte(int data,off_t from,off_t len,int shift)
{
	off_t i;
	char *mem,cdata;
	if(!fc_ptr) {printf("no file opened!\n");return;}
	if(!fc_ptr->fd) {printf("no file opened!\n");return;}
	//printf("inject byte...%d %d %d %d\n",data,from,len,shift);
	cdata=(char)data;
	mem=(char*)fc_ptr->faddr;
	if(shift) {if(!(mod_len(len))){return;};move(from,filesize(fc_ptr->fd),from+len); }
	mem+=from;
	for(i=0;i<len;i++,mem++) *mem=cdata;
	fc_ptr->changed_altered=true;
}
void inject_file(char *file,off_t from,off_t len,int shift)
{
	off_t i;
	char *mem,cdata;
	FILE *fp;
	if(!fc_ptr) {printf("no file opened!\n");return;}
	if(!fc_ptr->fd) {printf("no file opened!\n");return;}
	//printf("inject from file...%s %d %d %d\n",file,from,len,shift);
	mem=(char*)fc_ptr->faddr;
	fp=fopen(file,"r");
	if(fp==NULL) {printf("error opening file to inject.\n"); return;}
	if(len==-1) if((fseek(fp,0L,SEEK_END))==0) len=ftell(fp);
	rewind(fp);
	if(shift) {if(!(mod_len(len))){fclose(fp);return;};move(from,filesize(fc_ptr->fd),from+len); }
	mem+=from;
	for(i=0;i<len;i++,mem++)
	{
		cdata=(char)fgetc(fp);
		*mem=cdata;
		//to code: check if mem goes out of bound !!!
	}
	fclose(fp);
	fc_ptr->changed_altered=true;
}
void set_colors(int b){
	static char* _colors[]={
		"\e[0m", //reset
		"\e[1;33m", // prompt - bold yellow
		"\e[1;35m", // bold purple
		"\e[1;34m", // bold blue
		"\e[40m", // black
		"\e[1m", // bold
		"\e[1;31m", // bold red
		"\e[1;32m", // green
		"\e[0;33m", //  yellow
		"\e[0;35m", // purple
		"\e[0;34m", // blue
		"\e[0;31m", // red
		"\e[0;32m", // green
		NULL
	};
	static char* _bcolors[]={
		"\e[0m", //reset
		"\e[1;33m", // prompt - bold yellow
		"\e[1;35m", // bold purple
		"\e[1;34m", // bold blue
		"\e[40m", // black
		"\e[1m", // bold
		"\e[1;31m", // bold red
		"\e[1;32m", // bold green
		"\e[1;33m", //  bold yellow
		"\e[1;35m", // bold purple
		"\e[1;34m", // bold blue
		"\e[1;31m", // bold red
		"\e[1;32m", // bold green
		NULL
	};
	static char* _nocolors[]={
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		NULL
	};
	
	if (b==E_THEME_COLOR) {ptr_colors=_colors;return;}
	if (b==E_THEME_BCOLOR) {ptr_colors=_bcolors;return;}
	ptr_colors=_nocolors;
}
void block_func(int set,off_t offs){
	if(!fc_ptr) return;
	if(!set){
		printf("0x%llx\n",(long long unsigned)fc_ptr->block);
		return;
	}
	fc_ptr->block=offs;
}
void block_inc_func(off_t x){
	fc_ptr->block+=x;
}
void block_dec_func(off_t x){
	fc_ptr->block-=x;
}
void show_current_seek(void){
	if(fc_ptr){
		printf("0x%llx\n",(long long unsigned)fc_ptr->seek);
	}
}
void set_current_seek(off_t x){
	if(fc_ptr){
		fc_ptr->seek=x;
	}
}
void seek_inc(off_t x){
	if(fc_ptr){
		fc_ptr->seek+=x;
	}
}
void seek_dec(off_t x){
	if(fc_ptr){
		fc_ptr->seek-=x;
	}
}
void seek_block_inc(void){
	if(fc_ptr){
		seek_inc(fc_ptr->block);
	}
}
void seek_block_dec(void){
	if(fc_ptr){
		seek_dec(fc_ptr->block);
	}
}
void seek_data(char *s){
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	printf("seek (%zu bytes) data...%s from address 0x%llx\n",strlen(s),s,(long long unsigned)fc_ptr->seek);
	seek_data_in_file(s,strlen(s));
}
void seek_hex_data(char *s){
	int len;
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	len=strlen(s);
	if(len&1) len++;
	printf("seek (%d bytes) hex data...%s form address 0x%llx\n",len/2,s,(long long unsigned)fc_ptr->seek);
	convert_string_hex(s);
	seek_data_in_file(s,len/2);
}
int cmp_mem(char *m1,char *m2,int len){
	int x=0;
	while(*m1++==*m2++) {x++;}
	//if(x>1) printf("%d %d\n",len,x);
	if(x>=len) return true;
	return false;
}
void seek_data_in_file(char *s,int len){
	char *f;
	f=(char*)(fc_ptr->seek+fc_ptr->faddr);
	char *end=(char*)(f+fc_ptr->filesize);
	while(f<end){
		if(cmp_mem(s,f,len)){
			printf("found at 0x%llx\n",(long long unsigned)(f-fc_ptr->faddr));
		}
		f++;
	}
}
void inject(off_t x,off_t size){
	//inject zero at offset x growing file...
	off_t e1;
	//off_t e2;
	char *c;
	e1=filesize(fc_ptr->fd);
	if(growth(size)){
		//e2=filesize(fc_ptr->fd);
		move(x,x+e1,x+size);
		c=fc_ptr->faddr+x;
		while(size-->0){
			*c++='\0';
		}
	}
}
void dw_write_hex(struct _fmt *fmt,char *s,off_t x,int xb,int gb){
	int len,count=1;
	char *c,*ps;
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	len=strlen(s);
	convert_string_hex(s);
	len=strlen(s);
	ps=&s[0];
	if(!xb) x=fc_ptr->seek;
	if(gb) inject(x,(off_t)len);
	c=fc_ptr->faddr+x;
	if(fmt->rep==1 && fmt->type=='d') fmt->rep=1024;
	while(len-->0){
		*c++=*ps++;
		if(count>=fmt->rep) break;
		count++;
	}
}
void dw_write_string(struct _fmt *fmt,char *s,off_t x,int xb,int gb,int zb){
	int len,count=1;
	char *c,*ps;
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	len=strlen(s);
	ps=&s[0];
	if(!xb) x=fc_ptr->seek;
	if(gb) inject(x,(off_t)len);
	c=fc_ptr->faddr+x;
	if(fmt->rep==1 && fmt->type=='d') fmt->rep=1024;
	while(len-->0){
		*c++=*ps++;
		if(count>=fmt->rep-zb) break;
		count++;
	}
	if(zb) *c='\0';
}
void dw_write_pattern(struct _fmt *fmt,char *s,off_t x,int xb,int gb,int pattern_type){
	int len,count=0,strcount=0,inject_len;
	char *c,*ps;
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	len=strlen(s);
	if(pattern_type==DW_PATTERN_HEX){
		convert_string_hex(s);
		len=strlen(s);
	}
	ps=&s[0];
	if(fmt->rep==1 && fmt->type=='d') {fmt->rep=1;fmt->type='p';}
	if(fmt->type!='b') fmt->type='p';
	if(!xb) x=fc_ptr->seek;
	if(gb){
		if(fmt->type=='p'){
			inject_len=len*fmt->rep;
		}
		else{
			inject_len=fmt->rep;
		}
		inject(x,(off_t)inject_len);
	}
	c=fc_ptr->faddr+x;
	while(count<fmt->rep){
		*c++=*ps++;
		if(--len==0) {len=strlen(s);ps=&s[0];strcount++;}
		if(fmt->type=='p'){
			count=strcount;
		}
		else{
			count++;
		}
	}
	
}
void dw_write_file(struct _fmt *fmt,char *s,off_t x,int xb,int gb){
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	FILE *fp;
	off_t len;
	char *c;
	fp=fopen(s,"r");
	if(!fp){fprintf(stderr,"error opening %s file.\n",s); return;}
	fseek(fp,0L,SEEK_END);
	len=ftello(fp);
	rewind(fp);
	if(fmt->rep!=1 && fmt->type!='d') len=(off_t)fmt->rep;
	if(!xb) x=fc_ptr->seek;
	c=fc_ptr->faddr+x;
	if(gb) inject(x,len);
	while(len-->0){
		*c++=(char)fgetc(fp);
	}
}
void dw_write_random(struct _fmt *fmt,off_t x,int xb,int gb){
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	off_t len;
	char *c;
	len=(off_t)fmt->rep;
	if(!xb) x=fc_ptr->seek;
	c=fc_ptr->faddr+x;
	if(gb) inject(x,len);
	while(len-->0){
		//*c++=(char)arc4random();
		*c++=(char)random();
	}
}
void dw_write_le(struct _fmt *fmt,off_t num,off_t x,int xb,int gb){
	switch(fmt->rep){
		case 1:
			fmt->type='c';
			break;
		case 2:
			fmt->type='s';
			break;
		case 4:
			fmt->type='w';
			break;
		case 8:
			fmt->type='q';
			break;
		default:
			fprintf(stderr,"invalid number type.\n");
			return;
			break;
	}
	dw_write_number(fmt,num,x,xb,gb);
}
void dw_write_be(struct _fmt *fmt,off_t num,off_t x,int xb,int gb){
	switch(fmt->rep){
		case 1:
			fmt->type='C';
			break;
		case 2:
			fmt->type='S';
			break;
		case 4:
			fmt->type='W';
			break;
		case 8:
			fmt->type='Q';
			break;
		default:
			fprintf(stderr,"invalid number type.\n");
			return;
			break;
	}
	dw_write_number(fmt,num,x,xb,gb);
}
void dw_write_number(struct _fmt *fmt,off_t num,off_t x,int xb,int gb){
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	char *c;
	off_t len;
	uint16_t *mem16;
	uint32_t *mem32;
	uint64_t *mem64;
	switch(fmt->type){
		case 'c':
		case 'C':
			fmt->rep=1;
			break;
		case 's':
		case 'S':
			fmt->rep=2;
			break;
		case 'w':
		case 'W':
			fmt->rep=4;
			break;
		case 'q':
		case 'Q':
			fmt->rep=8;
			break;
		default:
			fprintf(stderr,"invalid number type.\n");
			return;
			break;
	}
	if(!xb) x=fc_ptr->seek;
	c=fc_ptr->faddr+x;
	len=(off_t)fmt->rep;
	if(gb) inject(x,len);
	if(fmt->type=='c' || fmt->type=='C'){
		*c=(char)num;
	}
	if(fmt->type=='s' || fmt->type=='S'){
		uint16_t num16;
		num16=(uint16_t)num;
		if(fmt->type=='s'){
			if(fc_ptr->cpu_endian==big_endian) endian_swap_16(&num16);
		}else{
			if(fc_ptr->cpu_endian==little_endian) endian_swap_16(&num16);
		}
		mem16=(uint16_t*)c;
		*mem16=num16;
	}
	if(fmt->type=='w' || fmt->type=='W'){
		uint32_t num32;
		num32=(uint32_t)num;
		if(fmt->type=='w'){
			if(fc_ptr->cpu_endian==big_endian) endian_swap_32(&num32);
		}else{
			if(fc_ptr->cpu_endian==little_endian) endian_swap_32(&num32);
		}
		mem32=(uint32_t*)c;
		*mem32=num32;
	}
	if(fmt->type=='q' || fmt->type=='Q'){
		uint64_t num64;
		num64=(uint64_t)num;
		if(fmt->type=='q'){
			if(fc_ptr->cpu_endian==big_endian) endian_swap_64(&num64);
		}else{
			if(fc_ptr->cpu_endian==little_endian) endian_swap_64(&num64);
		}
		mem64=(uint64_t*)c;
		*mem64=num64;
	}
}
void dw_write_over(struct _fmt *fmt,off_t num,off_t x,int xb,int wo_op){
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	off_t len;
	char *c,n;
	len=(off_t)fmt->rep;
	if(!xb) x=fc_ptr->seek;
	c=fc_ptr->faddr+x;
	n=(char)num;
	while(len-->0){
		switch(wo_op){
			case E_WO_ADD:
			*c++ += n;
			break;
			case E_WO_AND:
			*c++ &= n;
			break;
			case E_WO_SUB:
			*c++ -= n;
			break;
			case E_WO_MUL:
			*c++ *= n;
			break;
			case E_WO_OR:
			*c++ |= n;
			break;
			case E_WO_XOR:
			*c++ ^= n;
			break;
			case E_WO_RSHIFT:
			*c++ >>= n;
			break;
			case E_WO_LSHIFT:
			*c++ <<= n;
			break;
			default:
			break;
		}
	}
}
void dw_write_over_2swap(off_t x,int xb){
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	uint16_t *c,n;
	if(!xb) x=fc_ptr->seek;
	c=(uint16_t*)(fc_ptr->faddr+x);
	n=*c;
	endian_swap_16(&n);
	*c=n;
}
void dw_write_over_4swap(off_t x,int xb){
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	uint32_t *c,n;
	if(!xb) x=fc_ptr->seek;
	c=(uint32_t*)(fc_ptr->faddr+x);
	n=*c;
	endian_swap_32(&n);
	*c=n;
}
void dw_write_over_8swap(off_t x,int xb){
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	uint64_t *c,n;
	if(!xb) x=fc_ptr->seek;
	c=(uint64_t*)(fc_ptr->faddr+x);
	n=*c;
	endian_swap_64(&n);
	*c=n;
}
void dw_open_create(struct _fmt *fmt,char *filename,char *type){
	fc_ptr=getnewfilecfg();
	if(!fc_ptr) {warn("error getting new file cfg");return;}
	if(filename) strncpy(fc_ptr->name,filename,FILENAME_LEN);
	if(type) fc_ptr->file_type=decode_file_type2(type);
	fc_ptr->filesize=0;
	if(fmt->type=='w') fc_ptr->work_on_copy=true;
	if(fmt->type=='f') fc_ptr->work_on_copy=false;
	getcopyname(fc_ptr->copyname);
	if(!filename) filename=fc_ptr->copyname;
	if(fc_ptr->work_on_copy) filename=fc_ptr->copyname;
	fc_ptr->can_grow=true;
	fc_ptr->writable=true;
	//do_printcfg(fc_ptr,"");
	fc_ptr->fd=open(filename,O_CREAT|O_RDWR,0644);
	if(fc_ptr->fd==-1)
	{
		fprintf(stderr,"error opening file.\n");
		if(errno==EACCES) fprintf(stderr,"file access not allowed, check permissions.\n");
		rmfilecfg(fc_ptr);
		return;
	}
	write(fc_ptr->fd,"  ",2);
	fc_ptr->filesize=2;
	fc_ptr->fd_type=FD_REGULAR_FILE;
	fc_ptr->faddr=(char*)mmap(NULL,(size_t)2,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,fc_ptr->fd,(off_t)0);
	if(fc_ptr->faddr==MAP_FAILED) {warn("error on mmap(ing) the file");fprintf(stderr," : %d\n",errno);close(fc_ptr->fd);rmfilecfg(fc_ptr);return;}
	printf("new created\n");
}
int decode_file_type2(char *s){
	if(strcmp(s,"elf")==0) return 3;
	if(strcmp(s,"macho")==0) return 4;
	if(strcmp(s,"pe")==0) return 2;
	if(strcmp(s,"mz")==0) return 1;
	if(strcmp(s,"fatmacho")==0) return 5;
	if(strcmp(s,"fat")==0) return 6;
	if(strcmp(s,"fat16")==0) return 7;
	if(strcmp(s,"fat32")==0) return 8;
	if(strcmp(s,"ext")==0) return 9;
	if(strcmp(s,"mbr")==0) return 10;
	if(strcmp(s,"gpt")==0) return 11;
	return 0;
}
void dw_open_type(char *s){
	if(!s) return;
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	fc_ptr->file_type=decode_file_type2(s);
}