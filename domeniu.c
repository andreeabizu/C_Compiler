	#include <stdio.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <string.h>
	#include <ctype.h>
	#include "alex.h"
	#include "domeniu.h"
	#include "mv.h"
	#include "gc.h"

	void initSymbols(Symbols *symbols)
	{
		symbols->begin=NULL;
		symbols->end=NULL;
		symbols->after=NULL;
	}


	Symbol *addSymbol(Symbols *symbols,const char *name,int cls)
	{
		Symbol *s;
		if(symbols->end==symbols->after){ // create more room
		int count=symbols->after-symbols->begin;
		int n=count*2; // double the room
		if(n==0)n=1; // needed for the initial case
		symbols->begin=(Symbol**)realloc(symbols->begin, n*sizeof(Symbol*));
		if(symbols->begin==NULL)err("not enough memory");
		symbols->end=symbols->begin+count;
		symbols->after=symbols->begin+n;
		}
		SAFEALLOC(s,Symbol)
		*symbols->end++=s;
		s->name=name;
		s->cls=cls;
		s->depth=crtDepth;
		return s;
	}

	Symbol *findSymbol(Symbols *symbols, const char *name)//cauta de la dreapta la stanga
	{
	  int count=symbols->end -symbols->begin-1;
	  
	  for(int i=count;i>=0;i--)
	    if(strcmp(symbols->begin[i]->name,name)==0)
	      {	return symbols->begin[i];
	      }
	  return NULL;
	}

	Symbol *requireSymbol(Symbols *symbols, const char *name)
	{
	  int count=symbols->end -symbols->begin-1;
	  
	  for(int i=count;i>=0;i--)
	    if(strcmp(symbols->begin[i]->name,name)==0)
	      {	return symbols->begin[i];
	      }
	  err("symbol not found");
	  return NULL;
	}


	void deleteSymbolsAfter(Symbols *symbols, Symbol *start)//sterge toate simbolurile dupa start
	{
	  int count = symbols->end-symbols->begin;
	 
	  for(int i=count-1;i>=0;i--)
	   { if(strcmp(symbols->begin[i]->name,start->name)==0)
	       {
		for (int j=i+1;j<count;j++)
	     	{free(symbols->begin[j]);
		 symbols->begin[j]=NULL;
		 symbols->end--;
		}
		return;
	       }
	   }
	   return;
	}

	void printSymbols(Symbols *symbols)
	{int count;
		count = symbols->end - symbols->begin;
		for (int i=0;i<count; i++)
		{
			printf("%s ", symbols->begin[i]->name);

			switch(symbols->begin[i]->cls){
			case 0: printf("cls: CLS_VAR"); break;
			case 1: printf("cls: CLS_FUNC"); break;
			case 2: printf("cls: CLS_EXTFUNC"); break;
			case 3: printf("cls: CLS_STRUCT"); break;}

			switch(symbols->begin[i]->mem){
			case 0: printf(" mem: MEM_GLOBAL"); break;
			case 1: printf(" mem: MEM_ARG"); break;
			case 2: printf(" mem: MEM_LOCAL"); break;}

			
			switch(symbols->begin[i]->type.typeBase){
			case 0: printf(" type: TB_INT"); break;
			case 1: printf(" type: TB_DOUBLE"); break;
			case 2: printf(" type: TB_CHAR"); break;
			case 3: printf(" type: TB_STRUCT"); break;
			case 4: printf(" type: TB_VOID"); break;}
			printf("\n");
		}
	}


	Type createType(int typeBase,int nElements)
	{
	Type t;
	t.typeBase=typeBase;
	t.nElements=nElements;
	return t;
	}

	void put_s()
	{printf("#%s\n",(char*)popa());
	}

	void get_s()
	{
	  fgets(popa(),100,stdin); 
	}

	void put_i()
	{
	printf("#%d\n",popi());
	}

	void get_i()
	{int nr;
	  scanf("%d",&nr);
	  pushi(nr);
	}

	void put_d()
	{
	  printf("#%g\n",popd());
	}

	void get_d()
	{
	  double d;
	  scanf("%lg",&d);
	  pushd(d);
	}

	void put_c()
	{
	  printf("#%c\n",popc());
	}

	void get_c()
	{
	  char c;
	  c=getchar();
	  pushc(c);
	}

	Symbol *addExtFunc(const char *name,Type type, void *addr)
	{
	Symbol *s=addSymbol(&symbols,name,CLS_EXTFUNC);
	s->type=type;
	s->addr=addr;
	initSymbols(&s->args);
	return s;
	}

	Symbol *addFuncArg(Symbol *func,const char *name,Type type)
	{
	Symbol *a=addSymbol(&func->args,name,CLS_VAR);
	a->type=type;
	return a;
	}

	void addExtFuncs() {
	  Symbol *s,*a;

	  s=addExtFunc("put_s",createType(TB_VOID,-1),put_s);
	  a=addSymbol(&s->args,"s",CLS_VAR);
	  a->type=createType(TB_CHAR,0);
	  
	  s = addExtFunc("get_s", createType(TB_VOID, -1), get_s);
	  a = addSymbol(&s->args, "s", CLS_VAR);
	  a->type = createType(TB_CHAR, 0);
	  
	  s=addExtFunc("put_i",createType(TB_VOID,-1),put_i);
	  a=addSymbol(&s->args,"i",CLS_VAR);
	  a->type=createType(TB_INT,-1);
	  
	  s = addExtFunc("get_i", createType(TB_INT, -1), get_i);
	  
	  s=addExtFunc("put_d",createType(TB_VOID,-1),put_d);
	  a=addSymbol(&s->args,"d",CLS_VAR);
	  a->type=createType(TB_DOUBLE,-1);
	  
	  s = addExtFunc("get_d", createType(TB_DOUBLE, -1), get_d);
	  
	  s = addExtFunc("put_c", createType(TB_VOID, -1), put_c);
	  a = addSymbol(&s->args, "c", CLS_VAR);
	  a->type = createType(TB_CHAR, -1);
	  
	  s = addExtFunc("get_c", createType(TB_CHAR, -1), get_c);
	 
	}

	void cast(Type *dst,Type *src)
	{
	  if(src->nElements>-1){
	    if(dst->nElements>-1){
	      if(src->typeBase!=dst->typeBase) 
		 tkerr(crtTk,"an array cannot be converted to an array of another type");
	      }else{
	      tkerr(crtTk,"an array cannot be converted to a non-array");
	      }
	      }else{
	      if(dst->nElements>-1){
		 tkerr(crtTk,"a non-array cannot be converted to an array");
		 }
	      }
	  switch(src->typeBase){
	     case TB_CHAR:
	     case TB_INT:
	     case TB_DOUBLE:
	      switch(dst->typeBase){
		case TB_CHAR:
		case TB_INT:
		case TB_DOUBLE:
		return;
	      }
	  case TB_STRUCT:
	  if(dst->typeBase==TB_STRUCT){
	    if(src->s!=dst->s)
	      tkerr(crtTk,"a structure cannot be converted to another one");
	    return;
	    }
	   }
	   tkerr(crtTk,"incompatible types");
	}


	void  addVar(Token *tkName,Type *t)
	{
	    Symbol      *s;
	    if(crtStruct){
		if(findSymbol(&crtStruct->members,tkName->text))
		    tkerr(crtTk,"symbol redefinition: %s",tkName->text);
		s=addSymbol(&crtStruct->members,tkName->text,CLS_VAR);
		}
	    else if(crtFunc){
		s=findSymbol(&symbols,tkName->text);
		if(s&&s->depth==crtDepth)
		    tkerr(crtTk,"symbol redefinition: %s",tkName->text);
		s=addSymbol(&symbols,tkName->text,CLS_VAR);
		s->mem=MEM_LOCAL;
		}
	    else{
		if(findSymbol(&symbols,tkName->text))
		    tkerr(crtTk,"symbol redefinition: %s",tkName->text);
		s=addSymbol(&symbols,tkName->text,CLS_VAR);   
		s->mem=MEM_GLOBAL;
		}
	    s->type=*t;
	     if(crtStruct||crtFunc){
		    s->offset=offset;
		    }else{
		    s->addr=allocGlobal(typeFullSize(&s->type));
		    }
	    offset+=typeFullSize(&s->type);
	}

	Type getArithType(Type *s1, Type *s2)
	{
	  switch (s1->typeBase)
	   {case TB_INT:
	      switch (s2->typeBase)
	      {case TB_INT: return *s1;
	       case TB_DOUBLE:return *s2;
	       case TB_CHAR:return *s1;
	       default:tkerr(crtTk,"incompatible types");
	       }
	       break;
	    case TB_CHAR:
	      switch (s2->typeBase)
	      {case TB_INT: return *s2;
	       case TB_DOUBLE: return *s2;
	       case TB_CHAR: return *s1;
	       default: tkerr(crtTk,"incompatible types");
	      }break;
	    case TB_DOUBLE:
	      switch (s2->typeBase)
	      {case TB_INT:return *s1;
	       case TB_DOUBLE:return *s1;
	       case TB_CHAR:return *s1;
	       default:tkerr(crtTk,"incompatible types");
	      }break;
	   }
		tkerr(crtTk, "incompatible types");
	}









