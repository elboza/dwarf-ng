#include <stdio.h>
#include"main.h"
#include "lang.h"
#include "y.tab.h"
#include"vars.h"

#define ARITH v[0]=ex(p->opr.op[0]);\
        		if(v[0]->type!=TYPE_VAL) {printf("error! not an integer type!\n");break;}\
        		v[1]=ex(p->opr.op[1]);\
        		if(v[1]->type!=TYPE_VAL) {printf("error! not an integer type!\n");break;}\
        		v[2]=(struct _var*)malloc(sizeof(struct _var));\
        		if(v[2]==NULL) {die("error allocating space for var");}\
        		v[2]->type=TYPE_VAL;\
        		v[2]->name=NULL;\
        		v[2]->val=v[0]->val == v[1]->val;\
        		return v[2];

struct _val* ex(nodeType *p) {
	struct _val *v[3];
	char *name,*s;
	int type,val,i;
	if (!p) return NULL;
    switch(p->type) {
    case typeCon:       return p->con.value;
    case typeVar:
    	//printf("var is %s\n",p->id.s);
    	return get_var(p->id.s);;
    	break;
    case typeWord:
    	printf("word is %s\n",p->id.s);
    	//return sym[p->id.i];
    	return 0;
    	break;
    case typeString:
    	printf("string is %s\n",p->id.s);
    	return p->id.s;
    	break;
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:     while(ex(p->opr.op[0])) ex(p->opr.op[1]); return 0;
        case IF:        if (ex(p->opr.op[0]))
                            ex(p->opr.op[1]);
                        else if (p->opr.nops > 2)
                            ex(p->opr.op[2]);
                        return 0;
        case PRINT:     
        		v[0]=ex(p->opr.op[1]);
        		if(p->opr.op[0]==NULL)
        		{
        			//standard print
        			if(v[0]->type==TYPE_VAL) {printf("%d\n",v[0]->val);break;}
        			if(v[0]->type==TYPE_STRING) {printf("%s\n",v[0]->s);break;}
        		}
        		// else printf .....formatted print
        		
        		//printf("%d\n", ex(p->opr.op[0])); 
        		//return NULL;
        		break;
        case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
        case '=':
        	v[0]=ex(p->por.op[1]);
        	if(v[0]->type==TYPE_VAL) set_var(p->opr.op[0]->id.s,TYPE_VAL,v[0]->val);
        	if(v[0]->type==TYPE_STRING) set_var(p->opr.op[0]->id.s,TYPE_STRING,v[0]->s);
               	//set_var(p->opr.op[0]->id.s,.........);
               	//return sym[p->opr.op[0]->id.i] = ex(p->opr.op[1]);
               	//return NULL;
               	break;
        case UMINUS:    
        		v[0]=ex(p->opr.op[0]);
        		if(v[0]->type!=TYPE_VAL) {printf("error! not an integer type!\n");break;}
        		v[2]=(struct _var*)malloc(sizeof(struct _var));
        		if(v[2]==NULL) {die("error allocating space for var");}
        		v[2]->type=TYPE_VAL;
        		v[2]->name=NULL;
        		v[2]->val=-(v[0]->val);
        		return v[2];
        		//return -ex(p->opr.op[0]);
        case '+':       
        		ARITH
        		v[2]->val=v[0]->val + v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) + ex(p->opr.op[1]);
        case '-':       
        		ARITH
        		v[2]->val=v[0]->val - v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) - ex(p->opr.op[1]);
        case '*':       
        		ATIRH
        		v[2]->val=v[0]->val * v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) * ex(p->opr.op[1]);
        case '/':       
        		ARITH
        		if(v[1]->val==0) {printf("division by zero not permitted. operation will be skipped.\n");return NULL;}
        		v[2]->val=v[0]->val / v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) / ex(p->opr.op[1]);
        case '<':       
        		v[0]=ex(p->opr.op[0]);
        		if(v[0]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[1]=ex(p->opr.op[1]);
        		if(v[1]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[2]=(struct _var*)malloc(sizeof(struct _var));
        		if(v[2]==NULL) {die("error allocating space for var");}
        		v[2]->type=TYPE_VAL;
        		v[2]->name=NULL;
        		v[2]->val=v[0]->val < v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) < ex(p->opr.op[1]);
        case '>':       
        		v[0]=ex(p->opr.op[0]);
        		if(v[0]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[1]=ex(p->opr.op[1]);
        		if(v[1]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[2]=(struct _var*)malloc(sizeof(struct _var));
        		if(v[2]==NULL) {die("error allocating space for var");}
        		v[2]->type=TYPE_VAL;
        		v[2]->name=NULL;
        		v[2]->val=v[0]->val > v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) > ex(p->opr.op[1]);
        case GE:        
        		v[0]=ex(p->opr.op[0]);
        		if(v[0]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[1]=ex(p->opr.op[1]);
        		if(v[1]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[2]=(struct _var*)malloc(sizeof(struct _var));
        		if(v[2]==NULL) {die("error allocating space for var");}
        		v[2]->type=TYPE_VAL;
        		v[2]->name=NULL;
        		v[2]->val=v[0]->val >= v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) >= ex(p->opr.op[1]);
        case LE:        
        		v[0]=ex(p->opr.op[0]);
        		if(v[0]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[1]=ex(p->opr.op[1]);
        		if(v[1]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[2]=(struct _var*)malloc(sizeof(struct _var));
        		if(v[2]==NULL) {die("error allocating space for var");}
        		v[2]->type=TYPE_VAL;
        		v[2]->name=NULL;
        		v[2]->val=v[0]->val <= v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) <= ex(p->opr.op[1]);
        case NE:        
        		v[0]=ex(p->opr.op[0]);
        		if(v[0]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[1]=ex(p->opr.op[1]);
        		if(v[1]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[2]=(struct _var*)malloc(sizeof(struct _var));
        		if(v[2]==NULL) {die("error allocating space for var");}
        		v[2]->type=TYPE_VAL;
        		v[2]->name=NULL;
        		v[2]->val=v[0]->val != v[1]->val;
        		return v[2];
        		//return ex(p->opr.op[0]) != ex(p->opr.op[1]);
        case EQ:        
        		v[0]=ex(p->opr.op[0]);
        		if(v[0]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[1]=ex(p->opr.op[1]);
        		if(v[1]->type!=TYPE_VAL) {printf("error! not an integer type!\n");return NULL;}
        		v[2]=(struct _var*)malloc(sizeof(struct _var));
        		if(v[2]==NULL) {die("error allocating space for var");}
        		v[2]->type=TYPE_VAL;
        		v[2]->name=NULL;
        		v[2]->val=v[0]->val == v[1]->val;
        		return v[2];
        case QUIT:
        	//printf("Ok.Quitting... bye.\n");
        	forced=QUITTING;
        	break;
        case TYPE:
        	file_probe();
        	load_headers();
        	break;
        case LOAD:
        	printf("closing file\n");
        	file_close();
        	printf("file closed\n");
        	printf("opening file %s\n",p->opr.op[0]->id.s);
        	file_open(p->opr.op[0]->id.s);
        	file_probe();
        	load_headers();
        	break;;
        }
    }
    for(i=0;i<3;i++)
    {
    	if(v[i]!=NULL) 
    	{
    		if(v[i]->name==NULL) free(v[i]);
    	}
    }
    return NULL;
}
