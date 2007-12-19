#include <stdio.h>
#include"main.h"
#include "lang.h"
#include "y.tab.h"
#include"vars.h"

int ex(nodeType *p) {
	if (!p) return 0;
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
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:     while(ex(p->opr.op[0])) ex(p->opr.op[1]); return 0;
        case IF:        if (ex(p->opr.op[0]))
                            ex(p->opr.op[1]);
                        else if (p->opr.nops > 2)
                            ex(p->opr.op[2]);
                        return 0;
        case PRINT:     printf("%d\n", ex(p->opr.op[0])); return 0;
        case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
        case '=':
               	set_var(p->opr.op[0]->id.s,ex(p->opr.op[1]));
               	//return sym[p->opr.op[0]->id.i] = ex(p->opr.op[1]);
               	return 0;
               	break;
        case UMINUS:    return -ex(p->opr.op[0]);
        case '+':       return ex(p->opr.op[0]) + ex(p->opr.op[1]);
        case '-':       return ex(p->opr.op[0]) - ex(p->opr.op[1]);
        case '*':       return ex(p->opr.op[0]) * ex(p->opr.op[1]);
        case '/':       return ex(p->opr.op[0]) / ex(p->opr.op[1]);
        case '<':       return ex(p->opr.op[0]) < ex(p->opr.op[1]);
        case '>':       return ex(p->opr.op[0]) > ex(p->opr.op[1]);
        case GE:        return ex(p->opr.op[0]) >= ex(p->opr.op[1]);
        case LE:        return ex(p->opr.op[0]) <= ex(p->opr.op[1]);
        case NE:        return ex(p->opr.op[0]) != ex(p->opr.op[1]);
        case EQ:        return ex(p->opr.op[0]) == ex(p->opr.op[1]);
        case QUIT:
        	//printf("Ok.Quitting... bye.\n");
        	forced=QUITTING;
        	return 0;
        	break;
        case TYPE:
        	file_probe();
        	load_headers();
        	return 0;
        	break;
        case LOAD:
        	printf("closing file\n");
        	file_close();
        	printf("file closed\n");
        	printf("opening file %s\n",p->opr.op[0]->id.s);
        	file_open(p->opr.op[0]->id.s);
        	return 0;
        }
    }
    return 0;
}
