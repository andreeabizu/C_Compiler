	#include <stdio.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include <string.h>
	#include <ctype.h>
	#include "alex.h"
	#include "domeniu.h"
	#include "mv.h"
	#include "gc.h"
	

	Token *consumedTk, *crtTk;//atomul consumat, atomul curent
	Symbol *crtFunc = NULL, *crtStruct = NULL;

	int sizeArgs, offset;
	Instr *crtLoopEnd;
	
		
	int unit();
	int declStruct();
	int declVar();
	int typeBase();
	int arrayDecl();
	int typeName();
	int declFunc();
	int funcArg();
	int stm();
	int stmCompound();
	int expr();
	int exprAssign();
	int exprOr();
	int exprAnd();
	int exprEq();
	int exprRel();
	int exprAdd();
	int exprMul();
	int exprCast();
	int exprUnary();
	int exprPostfix();
	int exprPrimary();

	int consume(int code)//consuma atomul
	{ printf("consume(%s)",codeName(crtTk->code));
	  if(crtTk->code==code)
	  {  printf("=>consumat\n");
	     consumedTk=crtTk;
	     crtTk=crtTk->next;
	     return 1;
	  }
	  printf("=>altceva(%s)\n",codeName(crtTk->code));
	  return 0;
	}


	int unit()
	{
	  Instr *labelMain=addInstr(O_CALL);
	  addInstr(O_HALT);
	  printf("@decUnit %d\n",crtTk->code);
	  Token *startTk=crtTk;
	  for(;;){
	    if(declStruct()){
	    }
	    else 
	      if(declFunc()){
	      }
	      else
		if(declVar()){
		}
		else break;
	  }
	  labelMain->args[0].addr=requireSymbol(&symbols,"main")->addr;
	  if(consume(END))
	    {return 1;}
	  else tkerr(crtTk,"syntax error");

	  crtTk=startTk;
	  return 0;
	}

	void declStructAD(Token *tkName)
	{if(findSymbol(&symbols,tkName->text))
	    tkerr(crtTk,"symbol redefinition: %s",tkName->text);
	  crtStruct=addSymbol(&symbols,tkName->text,CLS_STRUCT);
	  initSymbols(&crtStruct->members);
	}

	int declStruct()
	{printf("@decStruct %d\n",crtTk->code);
	  Token *startTk=crtTk;
	  Token *tkName;
	 
	  if(consume(STRUCT)){
	    if(consume(ID)){
	      tkName = consumedTk;
	      if(consume(LACC)){
		offset=0;
		declStructAD(tkName);
		for(;;)
		  {if(declVar()){
		    }
		    else break;
		  }
		if(consume(RACC)){
		  if(consume(SEMICOLON)){
		    crtStruct = NULL;
		    return 1;
		  } else tkerr(crtTk,"missing ; after }");
		}else tkerr(crtTk,"invalid variable declaration in STRUCT or missing }");
	      }
	    } else tkerr(crtTk,"missing name after STRUCT");
	  }
	 
	  crtTk=startTk;
	  return 0;    
	}

	int declVar(){
	  printf("@decVar %d\n",crtTk->code);
	  Token *startTk=crtTk;
	  int isDV;
	  Type t;
	  Token *tkName;
	 
	  if(typeBase(&t)){
	    if(consume(ID)){
	      tkName=consumedTk;
	      isDV=arrayDecl(&t);
	    
	      if(!isDV)
		t.nElements=-1;
	      addVar(tkName,&t);
	    
	      for(;;){
		if(consume(COMMA)){
		  isDV=1;
		  if(consume(ID)){
		    tkName=consumedTk;
		    if(!arrayDecl(&t))
		      t.nElements=-1;
		    addVar(tkName,&t);
		  }else tkerr(crtTk,"missing the variable name after ,");
		}else break;
	      }
	      if(consume(SEMICOLON)){
		return 1;
	      }else{
		// if(isDV)
		tkerr(crtTk,"missing ; after the declaration of %s",tkName->text);}
	    }else tkerr(crtTk,"the name is missing after the type declaration");
	  }
	  crtTk=startTk;
	  return 0;
	}


	void typeBaseAD(Token *tkName,Type *ret)
	{Symbol *s=findSymbol(&symbols,tkName->text);
	  if(s==NULL)tkerr(crtTk,"undefined symbol: %s",tkName->text);
	  if(s->cls!=CLS_STRUCT)tkerr(crtTk,"%s is not a struct",tkName->text);
	  ret->typeBase=TB_STRUCT;
	  ret->s=s;
	}

	int typeBase(Type *ret)
	{printf("@dectypeBase %d\n",crtTk->code);
	  Token *startTk=crtTk;
	  Token *tkName;
	  if(consume(INT))
	    {ret->typeBase=TB_INT;
	      return 1;}
	  if(consume(DOUBLE))
	    {ret->typeBase=TB_DOUBLE;
	      return 1;}
	  if(consume(CHAR))
	    {ret->typeBase=TB_CHAR;
	      return 1;
	    }
	  if(consume(STRUCT)){
	    if(consume(ID)){
	      tkName=consumedTk;
	      typeBaseAD(tkName,ret);
	      return 1;
	    }else tkerr(crtTk,"invalid type or missing the name after STRUCT");
	  }
	  crtTk=startTk;
	  return 0;       
	}

	int arrayDecl(Type *ret)
	{ Instr *instrBeforeExpr;
	  printf("@decarrayDecl %d\n",crtTk->code);
	  Token *startTk=crtTk;

	  if(consume(LBRACKET)){
	    instrBeforeExpr=lastInstruction;
	    RetVal rv;
	    if(expr(&rv))
	      { if(!rv.isCtVal)
		  tkerr(crtTk,"the array size is not a constant");
		if(rv.type.typeBase!=TB_INT)
		  tkerr(crtTk,"the array size is not an integer");
		ret->nElements=rv.ctVal.i;
		//the expr needs only to providean array size and no code for it
		deleteInstructionsAfter(instrBeforeExpr);}
	    else
	      ret->nElements=0;
	   
	    if(consume(RBRACKET)){
	      return 1;
	    }else tkerr(crtTk,"invalid expression or missing ]");
	  }
	  crtTk=startTk;
	  return 0;
	}


	int typeName(Type *ret)
	{printf("@dectypeName %d\n",crtTk->code);
	  Token *startTk=crtTk;
	  if(typeBase(ret))
	    {
	      if(!arrayDecl(ret))
		ret->nElements=-1;
	      return 1;}
	  crtTk=startTk;
	  return 0;
	}


	void declFuncAD(Token *tkName,Type t)
	{
	  if(findSymbol(&symbols,tkName->text))
	    tkerr(crtTk,"symbol redefinition: %s",tkName->text);
	  crtFunc=addSymbol(&symbols,tkName->text,CLS_FUNC);
	  initSymbols(&crtFunc->args);
	  crtFunc->type=t;
	  crtDepth++;
	}

	int declFunc()
	{printf("@decFunc %d\n",crtTk->code);
	  Symbol **ps;
	  int exist=0;
	  Token *startTk=crtTk;
	  int isDF=0;
	  Type t;
	  Token *tkName;
	  if(typeBase(&t)){
	    isDF=consume(MUL);
	    if(isDF)
	      t.nElements=0;
	    else
	      t.nElements=-1;
	    exist=1;
	  }
	  else 
	    if(consume(VOID))
	      {t.typeBase=TB_VOID;
		exist=1;isDF=1;}

	  if(exist==1)
	    {if(consume(ID)){
		tkName=consumedTk;
		sizeArgs=offset=0;
		if(consume(LPAR)){
		  declFuncAD(tkName,t);
		  if(funcArg()){
		    while(1){
		      if(consume(COMMA)){
			if(funcArg()){
			}else 
			  tkerr(crtTk,"missing the argument name after ,");
		      }else break;
		    }
		  }
		  if(consume(RPAR)){
		    crtDepth--;
		    crtFunc->addr=addInstr(O_ENTER);
		    sizeArgs=offset;
		    //update args offsets for correct FP indexing
		    for(ps=symbols.begin;ps!=symbols.end;ps++){
		      if((*ps)->mem==MEM_ARG){
		        //2*sizeof(void*) == sizeof(retAddr)+sizeof(FP)
		        (*ps)->offset-=sizeArgs+2*sizeof(void*);
		      }
		    }
		    offset=0;
	    
		    if(stmCompound()){
		      deleteSymbolsAfter(&symbols,crtFunc);
		      ((Instr*)crtFunc->addr)->args[0].i=offset;  // setup the ENTER argument 
		      if(crtFunc->type.typeBase==TB_VOID){
			addInstrII(O_RET,sizeArgs,0);
		      }
		      crtFunc=NULL;
		      return 1;
		    }else tkerr(crtTk,"error in the body of the function or it is missing");
		  }else tkerr(crtTk,"Invalid function argument or missing )");
		}else if(isDF) tkerr(crtTk,"missing ( after function name");
	      }else if(isDF) tkerr(crtTk,"missing function name");
	    }
	  crtTk=startTk;
	  return 0;
	}


	void funcArgAD(Token *tkName, Type t)
	{Symbol *s=addSymbol(&symbols, tkName->text, CLS_VAR);
	  s->mem=MEM_ARG;
	  s->type=t;
	  s->offset=offset;
	  s=addSymbol(&crtFunc->args, tkName->text, CLS_VAR);
	  s->mem=MEM_ARG;
	  s->type=t;
	  s->offset=offset;
	  offset+=typeArgSize(&s->type);
	}

	int funcArg()
	{ Token *startTk=crtTk;
	  printf("@decfuncArg %d\n", crtTk->code);
	  Type t;
	  Token *tkName;
	  
	  if(typeBase(&t)){
	    if(consume(ID))
	      {tkName=consumedTk;
		if(!arrayDecl(&t))
		  t.nElements=-1;
	     
		funcArgAD(tkName, t);
		return 1;
	      }
	    else
	      tkerr(crtTk,"missing name of the argument");
	  }
	  
	  crtTk = startTk;
	  return 0;
	}



	int stm()
	{printf("@decstm %d\n",crtTk->code);
	  Token *startTk=crtTk;
	  Instr *i,*i1,*i2,*i3,*i4,*is,*ib3,*ibs;
	  if(stmCompound())
	    return 1;
	 
	  if(consume(IF)){
	    if(consume(LPAR)){
	      RetVal rv;
	      if(expr(&rv)){

		if(rv.type.typeBase==TB_STRUCT)
		  tkerr(crtTk,"a structure cannot be logically tested");
		if(consume(RPAR)){
		  i1=createCondJmp(&rv);
		  if(stm()){
		    if(consume(ELSE)){
		      i2=addInstr(O_JMP);
		      if(stm())
			{i1->args[0].addr=i2->next;
			 i1=i2;}
		      else
			{tkerr(crtTk,"error in the body of else or it is missing");
			}
		    }  
		    i1->args[0].addr=addInstr(O_NOP);
		    return 1;
		  }else tkerr(crtTk,"error in the body of if or it is missing");
		}else tkerr(crtTk,"invalid condition in if or missing )");
	      }else tkerr(crtTk,"invalid condition for if");
	    }else tkerr(crtTk,"missing ( after if");
	  }
	  
	  else
	    
	    if(consume(WHILE)){
	      Instr *oldLoopEnd=crtLoopEnd;
	      crtLoopEnd=createInstr(O_NOP);
	      i1=lastInstruction;
	      if(consume(LPAR)){
		RetVal rv;
		if(expr(&rv)){
		  if(rv.type.typeBase==TB_STRUCT)
		    tkerr(crtTk,"a structure cannot be logically tested");
		  if(consume(RPAR)){
		    i2=createCondJmp(&rv);
		    if(stm()){
		      addInstrA(O_JMP,i1->next);
		      appendInstr(crtLoopEnd);
		      i2->args[0].addr=crtLoopEnd;
		      crtLoopEnd=oldLoopEnd;
		      return 1;}else tkerr(crtTk,"error in the body of while or it is missing");
		  }else tkerr(crtTk,"invalid condition in while or ) is missing");
		}else tkerr(crtTk,"invalid condition for while");
	      }else tkerr(crtTk,"missing ( after while");
	    }
	    else
	      if(consume(FOR)){
		Instr *oldLoopEnd=crtLoopEnd;
		crtLoopEnd=createInstr(O_NOP);	
		if(consume(LPAR)){
		  RetVal rv1;
		  if(expr(&rv1))
		    {if(typeArgSize(&rv1.type)) //daca nu e void
			addInstrI(O_DROP,typeArgSize(&rv1.type));
		    }
		  if(consume(SEMICOLON)){
		    i2=lastInstruction; 
		    RetVal rv2;
		    if(expr(&rv2))
		      { i4=createCondJmp(&rv2);
			if(rv2.type.typeBase==TB_STRUCT)
			  tkerr(crtTk,"a structure cannot be logically tested");
		      }
		    else
		      {
		     i4=NULL;
		      }
		    if(consume(SEMICOLON)){
		     ib3=lastInstruction; 
		      RetVal rv3;
		      if(expr(&rv3))
		      {
		                    if(typeArgSize(&rv3.type))
		                    addInstrI(O_DROP,typeArgSize(&rv3.type));
		      }
		      if(consume(RPAR)){
		       ibs=lastInstruction; 
			if(stm()){
		             if(ib3!=ibs){
		                i3=ib3->next;
		                is=ibs->next;
		                ib3->next=is;
		                is->last=ib3;
		                lastInstruction->next=i3;
		                i3->last=lastInstruction;
		                ibs->next=NULL;
		                lastInstruction=ibs;
			     }
		        addInstrA(O_JMP,i2->next);
		        appendInstr(crtLoopEnd);
		        if(i4)
			  i4->args[0].addr=crtLoopEnd;
		        crtLoopEnd=oldLoopEnd;
			  return 1;}else tkerr(crtTk,"error in the body of for or it is missing");
		      }else tkerr(crtTk,"invalid expression in for or missing )");
		    }else tkerr(crtTk,"invalid condition in for or missing ;");
		  }else tkerr(crtTk,"invalid initizlization in for or missing ;");
		}else tkerr(crtTk,"missing ( after for");
	      }
	      else
		if(consume(BREAK)){
		  if(consume(SEMICOLON)){
		    if(!crtLoopEnd)tkerr(crtTk,"break without for or while");
		    addInstrA(O_JMP,crtLoopEnd);
		    return 1;}
		  else tkerr(crtTk,"missing ; after break");
		}
		else
		  if(consume(RETURN)){
		    RetVal rv;
		    if(expr(&rv)){	
		      if(crtFunc->type.typeBase==TB_VOID)
			tkerr(crtTk,"a void function cannot return a value");
		      cast(&crtFunc->type,&rv.type);
			 i=getRVal(&rv);
		        addCastInstr(i,&rv.type,&crtFunc->type);
		    }
		    if(consume(SEMICOLON)){
		       if(crtFunc->type.typeBase==TB_VOID){
		            addInstrII(O_RET,sizeArgs,0);
		       }else{
			 addInstrII(O_RET,sizeArgs,typeArgSize(&crtFunc->type));}
		       return 1;
			 }
			   else tkerr(crtTk,"invalid expression or missing ; after return");
		  }
		  else{RetVal rv;
		    int is=expr(&rv);
		    if(is)
		      {if(typeArgSize(&rv.type))
			  addInstrI(O_DROP,typeArgSize(&rv.type));
		      }
		    if(consume(SEMICOLON)){
		      return 1;}
		    else if(is) tkerr(crtTk,"invalid expression or missing ;");
		  }

	  crtTk=startTk;
	  return 0;
	}



	int stmCompound()
	{printf("@decstmCompound %d\n",crtTk->code);
	 Token *startTk=crtTk;
	 Symbol *start=symbols.end[-1];
	 
	  if(consume(LACC)){
	    crtDepth++;
	       while(1){
	       if(declVar()){
	       }
	       else if(stm()){
	       }
	       else break;
	       }
	       if(consume(RACC)){
		 crtDepth--;
		 deleteSymbolsAfter(&symbols,start);
		 return 1;}else tkerr(crtTk,"syntax error or missing }");
	       }
	      
	  crtTk=startTk;
	  return 0;
	}


	int expr(RetVal *rv)
	{printf("@decexpr %d",crtTk->code);
	  Token *startTk=crtTk;
	if(exprAssign(rv))
	   return 1;

	 crtTk=startTk;
	 return 0;
	}

	int exprAssign(RetVal *rv)
	{Instr *i,*oldLastInstr=lastInstruction;
	 Token *startTk=crtTk;
	 printf("@decAssign %d\n",crtTk->code);

	 
	 if(exprUnary(rv)){
	  if(consume(ASSIGN)){
	   RetVal rve;
	   if(exprAssign(&rve))
	     { if(rv->isLVal ==0 && rv->isCtVal==0 )
	      tkerr(crtTk,"a function cannot be used as a variable");
	      if(!rv->isLVal)tkerr(crtTk,"cannot assign to a non-lval");
	      if(rv->type.nElements>-1||rve.type.nElements>-1)
		  tkerr(crtTk,"the arrays cannot be assigned");
	      cast(&rv->type,&rve.type);
	      i=getRVal(&rve);
	      addCastInstr(i,&rve.type,&rv->type);
	      addInstrII(O_INSERT,
		     sizeof(void*)+typeArgSize(&rv->type),
		     typeArgSize(&rv->type));
	      addInstrI(O_STORE,typeArgSize(&rv->type));
	      rv->isCtVal=rv->isLVal=0;
	     return 1; 
	     }
	   else tkerr(crtTk,"invalid expression after =");
	  }
	   deleteInstructionsAfter(oldLastInstr); 
	   crtTk=startTk;
	  }
	  
	if(exprOr(rv))
	  return 1;
	crtTk=startTk;
	return 0;
	}


	int exprOrPrim(RetVal *rv)
	{Token *tkopr;
	Instr *i1,*i2;
	Type t,t1,t2;
	if(consume(OR)){
	i1=rv->type.nElements<0?getRVal(rv):lastInstruction;
	t1=rv->type;

	tkopr=consumedTk;
	RetVal rve;
	if(exprAnd(&rve)){
	if(rv->type.typeBase==TB_STRUCT||rve.type.typeBase==TB_STRUCT)
	  tkerr(crtTk,"a structure cannot be logically tested");

	if(rv->type.nElements>=0){      // vectors
	addInstr(O_OR_A);
	}else{  // non-vectors
	i2=getRVal(&rve);t2=rve.type;
	t=getArithType(&t1,&t2);
	addCastInstr(i1,&t1,&t);
	addCastInstr(i2,&t2,&t);
	switch(t.typeBase){
	case TB_INT:addInstr(O_OR_I);break;
	case TB_DOUBLE:addInstr(O_OR_D);break;
	case TB_CHAR:addInstr(O_OR_C);break;
	}
	}

	rv->type=createType(TB_INT,-1);
	rv->isCtVal=rv->isLVal=0;
	if(exprOrPrim(rv)){
	return 1;
	}
	}else tkerr(crtTk,"invalid expression after %s",codeName(tkopr->code));
	}
	return 1;
	}

	int exprOr(RetVal *rv)
	{printf("@decOr %d\n",crtTk->code);
	Token *startTk=crtTk;
	 
	if(exprAnd(rv)){
	if(exprOrPrim(rv)){
	return 1;}
	}
	  
	crtTk=startTk;
	return 0;
	}

	int exprAndPrim(RetVal *rv)
	{Token *tkopr;
	Instr *i1,*i2;
	Type t,t1,t2;
	if(consume(AND)){
	i1=rv->type.nElements<0?getRVal(rv):lastInstruction;
	t1=rv->type;
	tkopr=consumedTk;
	RetVal rve;
	if(exprEq(&rve)){
	if(rv->type.typeBase==TB_STRUCT||rve.type.typeBase==TB_STRUCT)
	  tkerr(crtTk,"a structure cannot be logically tested");
	if(rv->type.nElements>=0){      // vectors
	addInstr(O_AND_A);
	}else{  // non-vectors
	i2=getRVal(&rve);t2=rve.type;
	t=getArithType(&t1,&t2);
	addCastInstr(i1,&t1,&t);
	addCastInstr(i2,&t2,&t);
	switch(t.typeBase){
	case TB_INT:addInstr(O_AND_I);break;
	case TB_DOUBLE:addInstr(O_AND_D);break;
	case TB_CHAR:addInstr(O_AND_C);break;
	}
	}
	rv->type=createType(TB_INT,-1);
	rv->isCtVal=rv->isLVal=0;
	if(exprAndPrim(rv)){
	return 1;
	}
	}else tkerr(crtTk,"invalid expression after %s",codeName(tkopr->code));
	}
	return 1;
	}

	int exprAnd(RetVal *rv)
	{printf("@decexprAnd %d\n",crtTk->code);
	Token *startTk=crtTk;
	if(exprEq(rv)){
	if(exprAndPrim(rv)){
	return 1;
	}
	}
	crtTk=startTk;
	return 0;
	}

	int exprEqPrim(RetVal *rv)
	{Token *tkopr;
	Instr *i1,*i2;
	Type t,t1,t2;
	if(consume(EQUAL)|| consume(NOTEQ))
	  {RetVal rve;
	   i1=rv->type.nElements<0?getRVal(rv):lastInstruction;
	   t1=rv->type;
	tkopr=consumedTk;
	if(exprRel(&rve))
	  {if(rv->type.typeBase==TB_STRUCT||rve.type.typeBase==TB_STRUCT)
	     tkerr(crtTk,"a structure cannot be compared");
	 if(rv->type.nElements>=0){      // vectors
		        addInstr(tkopr->code==EQUAL?O_EQ_A:O_NOTEQ_A);
		        }else{  // non-vectors
		        i2=getRVal(&rve);t2=rve.type;
		        t=getArithType(&t1,&t2);
		        addCastInstr(i1,&t1,&t);
		        addCastInstr(i2,&t2,&t);
		        if(tkopr->code==EQUAL){
		                switch(t.typeBase){
		                        case TB_INT:addInstr(O_EQ_I);break;
		                        case TB_DOUBLE:addInstr(O_EQ_D);break;
		                        case TB_CHAR:addInstr(O_EQ_C);break;
		                        }
		                }else{
		                switch(t.typeBase){
		                        case TB_INT:addInstr(O_NOTEQ_I);break;
		                        case TB_DOUBLE:addInstr(O_NOTEQ_D);break;
		                        case TB_CHAR:addInstr(O_NOTEQ_C);break;
		                        }
		                }
		        } 
	rv->type=createType(TB_INT,-1);
	rv->isCtVal=rv->isLVal=0;

	if(exprEqPrim(rv))
	  return 1;
	}else tkerr(crtTk,"invalid expression after %s",codeName(tkopr->code));
	}
	return 1;
	}

	int exprEq(RetVal *rv)
	{printf("@decEq %d\n",crtTk->code);
	Token *startTk=crtTk;

	if(exprRel(rv)){
	if(exprEqPrim(rv)){ 
	return 1;
	}
	}
	crtTk=startTk;
	return 0;
	}


	int exprRelPrim(RetVal *rv)
	{int exist=0;
	 Instr *i1,*i2;
	 Type t,t1,t2;
	 Token *tkopr;
	  if(consume(LESS)|| consume(LESSEQ) || consume(GREATER) ||consume(GREATEREQ))
	    {tkopr=consumedTk;
	     i1=getRVal(rv);
	     t1=rv->type;
	     RetVal rve;
	 if(exprAdd(&rve)){
	    if(rv->type.nElements>-1||rve.type.nElements>-1)
		        tkerr(crtTk,"an array cannot be compared");
		if(rv->type.typeBase==TB_STRUCT||rve.type.typeBase==TB_STRUCT)
		        tkerr(crtTk,"a structure cannot be compared");
		i2=getRVal(&rve);t2=rve.type;
		t=getArithType(&t1,&t2);
		addCastInstr(i1,&t1,&t);
		addCastInstr(i2,&t2,&t);
		switch(tkopr->code){
		        case LESS:
		                switch(t.typeBase){
		                        case TB_INT:addInstr(O_LESS_I);break;
		                        case TB_DOUBLE:addInstr(O_LESS_D);break;
		                        case TB_CHAR:addInstr(O_LESS_C);break;
		                        }
		                break;
		        case LESSEQ:
		                switch(t.typeBase){
		                        case TB_INT:addInstr(O_LESSEQ_I);break;
		                        case TB_DOUBLE:addInstr(O_LESSEQ_D);break;
		                        case TB_CHAR:addInstr(O_LESSEQ_C);break;
		                        }
		                break;
		        case GREATER:
		                switch(t.typeBase){
		                        case TB_INT:addInstr(O_GREATER_I);break;
		                        case TB_DOUBLE:addInstr(O_GREATER_D);break;
		                        case TB_CHAR:addInstr(O_GREATER_C);break;
		                        }
		                break;
		        case GREATEREQ:
		                switch(t.typeBase){
		                        case TB_INT:addInstr(O_GREATEREQ_I);break;
		                        case TB_DOUBLE:addInstr(O_GREATEREQ_D);break;
		                        case TB_CHAR:addInstr(O_GREATEREQ_C);break;
		                        }
		                break;
		        }
		rv->type=createType(TB_INT,-1);
		rv->isCtVal=rv->isLVal=0;
	  if(exprRelPrim(rv)){
	   return 1;}
	 }else tkerr(crtTk,"invalid expression after %s",codeName(tkopr->code));
	 }

	return 1;
	}

	int exprRel(RetVal *rv)
	{printf("@decRel %d\n",crtTk->code);
	Token *startTk=crtTk;

	 if(exprAdd(rv)){
	  if(exprRelPrim(rv)){ 
	   return 1;
	  }
	 }
	crtTk=startTk;
	return 0;
	}


	int exprAddPrim(RetVal *rv)
	{Token *tkopr;
	 Instr *i1,*i2;
	 Type t1,t2;
	 char c;
	 if(consume(ADD)|| consume(SUB))
	   {RetVal rve;
	    i1=getRVal(rv);
	    t1=rv->type;
	     tkopr=consumedTk;
	    if(exprMul(&rve)){
	    if(rv->type.nElements>-1||rve.type.nElements>-1)
		        tkerr(crtTk,"an array cannot be added or subtracted");
		if(rv->type.typeBase==TB_STRUCT||rve.type.typeBase==TB_STRUCT)
		        tkerr(crtTk,"a structure cannot be added or subtracted");
		rv->type=getArithType(&rv->type,&rve.type);
		i2=getRVal(&rve);
		t2=rve.type;
		addCastInstr(i1,&t1,&rv->type);
		addCastInstr(i2,&t2,&rv->type);
		if(tkopr->code==ADD){
		        switch(rv->type.typeBase){
		                case TB_INT:addInstr(O_ADD_I);break;
		                case TB_DOUBLE:addInstr(O_ADD_D);break;
		                case TB_CHAR:addInstr(O_ADD_C);break;
		                }
		        }else{
		        switch(rv->type.typeBase){
		                case TB_INT:addInstr(O_SUB_I);break;
		                case TB_DOUBLE:addInstr(O_SUB_D);break;
		                case TB_CHAR:addInstr(O_SUB_C);break;
		                }
		}
		rv->isCtVal=rv->isLVal=0;       
	    if(exprAddPrim(rv))
	      return 1;
	    }else tkerr(crtTk,"invalid expression after %s",codeName(tkopr->code));
	  }
	return 1;
	}

	int exprAdd(RetVal *rv)
	{printf("@decAdd %d\n",crtTk->code);
	Token *startTk=crtTk;

	 if(exprMul(rv)){
	  if(exprAddPrim(rv)){ 
	   return 1;
	  }
	 }
	crtTk=startTk;
	return 0;
	}

	int exprMulPrim(RetVal *rv)
	{ Token *tkopr;
	  Instr *i1,*i2;
	  Type t1,t2;
	  if(consume(MUL) || consume(DIV))
	  { i1=getRVal(rv);
	    t1=rv->type;
	    tkopr=consumedTk;
	    RetVal rve;
	    if(exprCast(&rve)){
		if(rv->type.nElements>-1||rve.type.nElements>-1)
		        tkerr(crtTk,"an array cannot be multiplied or divided");
		if(rv->type.typeBase==TB_STRUCT||rve.type.typeBase==TB_STRUCT)
		        tkerr(crtTk,"a structure cannot be multiplied or divided");
		rv->type=getArithType(&rv->type,&rve.type);
		i2=getRVal(&rve);
		t2=rve.type;
		addCastInstr(i1,&t1,&rv->type);
		addCastInstr(i2,&t2,&rv->type);
		if(tkopr->code==MUL){
		        switch(rv->type.typeBase){
		                case TB_INT:addInstr(O_MUL_I);break;
		                case TB_DOUBLE:addInstr(O_MUL_D);break;
		                case TB_CHAR:addInstr(O_MUL_C);break;
		                }
		        }else{
		        switch(rv->type.typeBase){
		                case TB_INT:addInstr(O_DIV_I);break;
		                case TB_DOUBLE:addInstr(O_DIV_D);break;
		                case TB_CHAR:addInstr(O_DIV_C);break;
		                }
		        }
		rv->isCtVal=rv->isLVal=0;
	   if(exprMulPrim(rv)){
	     return 1;
	   }
	    }else tkerr(crtTk,"invalid expression after %s",codeName(tkopr->code));
	 }
	return 1;
	}

	int exprMul(RetVal *rv)
	{printf("@decMul %d\n",crtTk->code);
	Token *startTk=crtTk;

	 if(exprCast(rv)){
	  if(exprMulPrim(rv)){ 
	   return 1;
	  }
	 }
	crtTk=startTk;
	return 0;
	}


	int exprCast(RetVal *rv)
	{Token *startTk=crtTk;
	 Instr *oldLastInstr=lastInstruction;
	 printf("@decCast %d",crtTk->code);
	 Type t;
	  if(consume(LPAR)){
	    if(typeName(&t)){
	      if(consume(RPAR)){
		RetVal rve;
		if(exprCast(&rve)){
		    cast(&t,&rve.type);
		     if(rv->type.nElements<0&&rv->type.typeBase!=TB_STRUCT){
		        switch(rve.type.typeBase){
		                case TB_CHAR:
		                        switch(t.typeBase){
		                                case TB_INT:addInstr(O_CAST_C_I);break;
		                                case TB_DOUBLE:addInstr(O_CAST_C_D);break;
		                                }
		                        break;
		                case TB_DOUBLE:
		                        switch(t.typeBase){
		                                case TB_CHAR:addInstr(O_CAST_D_C);break;
		                                case TB_INT:addInstr(O_CAST_D_I);break;
		                                }
		                        break;
		                case TB_INT:
		                        switch(t.typeBase){
		                                case TB_CHAR:addInstr(O_CAST_I_C);break;
		                                case TB_DOUBLE:addInstr(O_CAST_I_D);break;
		                                }
		                        break;
		                }
		        }
		    rv->type=t;
		    rv->isCtVal=rv->isLVal=0;
		     return 1;
		}else tkerr(crtTk,"invalid expression or expression is missing");
	      }else tkerr(crtTk,"invalid type or missing )");
	    }
	   crtTk=startTk;
	   deleteInstructionsAfter(oldLastInstr);

	  }
	 
	 if(exprUnary(rv))
	   return 1;

	crtTk=startTk;
	  return 0;
	}

	int exprUnary(RetVal *rv)
	{ printf("@decUnary %d\n",crtTk->code);
	  Token *startTk=crtTk;
	  int ok=0; char c;
	  Token *tkop;
	  if(consume(SUB) || consume(NOT))
	    { tkop=consumedTk;
	   if(exprUnary(rv))
	     {
		 if(tkop->code==SUB)
		 {
		    if(rv->type.nElements>=0)tkerr(crtTk,"unary '-' cannot be applied to an array");
		    if(rv->type.typeBase==TB_STRUCT)
		        tkerr(crtTk,"unary '-' cannot be applied to a struct");
		     getRVal(rv);
		     switch(rv->type.typeBase){
		        case TB_CHAR:addInstr(O_NEG_C);break;
		        case TB_INT:addInstr(O_NEG_I);break;
		        case TB_DOUBLE:addInstr(O_NEG_D);break;
		     }
		 }
		 else
		 {  // NOT
		    if(rv->type.typeBase==TB_STRUCT)tkerr(crtTk,"'!' cannot be applied to a struct");
		    if(rv->type.nElements<0){
		        getRVal(rv);
		        switch(rv->type.typeBase){
		                case TB_CHAR:addInstr(O_NOT_C);break;
		                case TB_INT:addInstr(O_NOT_I);break;
		                case TB_DOUBLE:addInstr(O_NOT_D);break;
		                }
		        }else{
		        addInstr(O_NOT_A);      
		        }
		    rv->type=createType(TB_INT,-1);
		 }
		 rv->isCtVal=rv->isLVal=0;
	      return 1;

	      }
	   else tkerr(crtTk,"invalid expression after %s",codeName(tkop->code));
	  }
	  else
	    if(exprPostfix(rv))
	      return 1;
	    
	  
	  crtTk=startTk;
	  return 0;
	}

	int exprPostfixPrim(RetVal *rv)
	{
	if(consume(LBRACKET)){
	  RetVal rve;
	  if(expr(&rve))
	  {
	     if(rv->type.nElements<0) tkerr(crtTk,"only an array can be indexed");
		    Type typeInt=createType(TB_INT,-1);
		    cast(&typeInt,&rve.type);
		    rv->type=rv->type;
		    rv->type.nElements=-1;
		    rv->isLVal=1;
		    rv->isCtVal=0;
	      if(consume(RBRACKET))
	      {         addCastInstr(lastInstruction,&rve.type,&typeInt);
		        getRVal(&rve);
		        if(typeBaseSize(&rv->type)!=1){
		                addInstrI(O_PUSHCT_I,typeBaseSize(&rv->type));
		                addInstr(O_MUL_I);
		                }
		        addInstr(O_OFFSET);
		 if(exprPostfixPrim(rv))
		 return 1;
	      }
	      else tkerr(crtTk,"invalid expression or missing ]");
	  }else
		tkerr(crtTk,"invalid expression after [");
	 }
	 else
	 {
	 if(consume(DOT))
	 {
	   if(consume(ID))
	     {
	       Token *tkName=consumedTk;
		Symbol  *sStruct=rv->type.s;
		if(rv->type.typeBase!=TB_STRUCT)
		  tkerr(crtTk,"is not a struct,it does not have member %s",tkName->text);
		Symbol  *sMember=findSymbol(&sStruct->members,tkName->text);
		if(!sMember)
		        tkerr(crtTk,"struct %s does not have a member %s",sStruct->name,tkName->text);
		rv->type=sMember->type;
		rv->isLVal=1;
		rv->isCtVal=0;
		 if(sMember->offset){
		                addInstrI(O_PUSHCT_I,sMember->offset);
		                addInstr(O_OFFSET);
		                }
		if(exprPostfixPrim(rv))
		   return 1;
	      }else tkerr(crtTk,"missing field name after .");
	 }
	}
	return 1;
	}

	int exprPostfix(RetVal *rv)
	{printf("@decexprPostfix %d\n",crtTk->code); 
	Token *startTk=crtTk;
	if(exprPrimary(rv)){
	 if(exprPostfixPrim(rv)){
	   return 1;
	   }
	  }
	crtTk=startTk;
	return 0;
	}


	int exprPrimary(RetVal *rv)
	{printf("@decPrimary %d\n",crtTk->code);
	 int isF=0;
	 Instr *i;
	Token *startTk=crtTk;
	 Token *tkName;
	 
	 if(consume(ID)){
	   tkName=consumedTk;
	   Symbol *s=findSymbol(&symbols,tkName->text);
	   if(!s)tkerr(crtTk,"undefined symbol %s",tkName->text);
	   rv->type=s->type;
	   rv->isCtVal=0;
	   rv->isLVal=1;
	   
	      if(s->cls==CLS_FUNC || s->cls==CLS_EXTFUNC)
	     { rv->isLVal=0;
	       isF=1;
	     }
	      
	  if(consume(LPAR))
	  {
	     Symbol **crtDefArg=s->args.begin;
	     if(s->cls!=CLS_FUNC&&s->cls!=CLS_EXTFUNC)
	     tkerr(crtTk,"call of the non-function %s",tkName->text);

	     RetVal arg;
	    if(expr(&arg))
	    {
		if(crtDefArg==s->args.end)tkerr(crtTk,"too many arguments in call");
		cast(&(*crtDefArg)->type,&arg.type);
		if((*crtDefArg)->type.nElements<0){  //only arrays are passed by addr
		                    i=getRVal(&arg);
		                    }else{
		                    i=lastInstruction;
		                    }
		addCastInstr(i,&arg.type,&(*crtDefArg)->type);
		crtDefArg++;
		for(;;)
		{
		   if(consume(COMMA))
		   {
		     RetVal arg;
		     if(expr(&arg))
		     {
		       if(crtDefArg==s->args.end)tkerr(crtTk,"too many arguments in call");
		       cast(&(*crtDefArg)->type,&arg.type);
		       if((*crtDefArg)->type.nElements<0){
		                    i=getRVal(&arg);
		                    }else{
		                    i=lastInstruction;
		                    }
		       addCastInstr(i,&arg.type,&(*crtDefArg)->type);
		       crtDefArg++;
		     }else tkerr(crtTk,"invalid expression after , or it is missing");
		    }else break;
		}
	    }
	    if(consume(RPAR))
	    {
	      if(crtDefArg!=s->args.end)tkerr(crtTk,"too few arguments in call");
	      rv->type=s->type;
	      rv->isCtVal=rv->isLVal=0;
	       // function call
		      i=addInstr(s->cls==CLS_FUNC?O_CALL:O_CALLEXT);
		      i->args[0].addr=s->addr;
	    }
	    else {
	       if(s->cls==CLS_FUNC||s->cls==CLS_EXTFUNC)
		 tkerr(crtTk,"missing call for function %s",tkName->text);
	      // tkerr(crtTk,"invalid argument or missing )");
		   
	    }
	  } else{
	    if(isF) tkerr(crtTk,"missing call for function %s", tkName->text);
	    //variable
	     if(s->depth)
	       { addInstrI(O_PUSHFPADDR,s->offset);
		}else{
		 addInstrA(O_PUSHCT_A,s->addr);
		}
	  }
	  return 1;}

	  if(consume(CT_INT))
	    {Token *tki=consumedTk;
	     rv->type=createType(TB_INT,-1);
	     rv->ctVal.i=tki->i;
	     rv->isCtVal=1;
	     rv->isLVal=0;
	     addInstrI(O_PUSHCT_I,tki->i);
	    return 1;
	    }

	  if(consume(CT_REAL))
	    {Token *tkr=consumedTk;
	     rv->type=createType(TB_DOUBLE,-1);
	     rv->ctVal.d=tkr->r;
	     rv->isCtVal=1;
	     rv->isLVal=0;
	     i=addInstr(O_PUSHCT_D);
	     i->args[0].d=tkr->r;
	    return 1;
	    }
	  if(consume(CT_CHAR))
	    {Token *tkc=consumedTk;
	     rv->type=createType(TB_CHAR,-1);
	     rv->ctVal.i=tkc->i;
	     rv->isCtVal=1;
	     rv->isLVal=0;
	     addInstrI(O_PUSHCT_C,tkc->i);
	    return 1;
	    }
	  if(consume(CT_STRING))
	    {Token *tks=consumedTk;
	     rv->type=createType(TB_CHAR,0);
	     rv->ctVal.str=tks->text;
	     rv->isCtVal=1;
	     rv->isLVal=0;
	     addInstrA(O_PUSHCT_A,tks->text);
	    return 1;
	    }
	  if(consume(LPAR)){
	   if(expr(rv)){
	    if(consume(RPAR)){
	     return 1;
	    }else tkerr(crtTk,"invalid expression or missing )");
	   }
	  }
	       
	  
	 crtTk=startTk;
	 return 0;
	 }


	int main(int argc, char *argv[])
	{  if(argc!=2)
	    {perror("file name is missing");
	      exit(-1);
	    }
	    read(argv[1]);
	   /* crtTk=getTokens();//lista de atomi
	     addExtFuncs();  
	    if(unit()){
	      printf("syntax ok\n");
	    }else{
	  tkerr(crtTk,"syntax error");
	    }
	   run(instructions);
	    terminare();*/
	    return 0;
	}













