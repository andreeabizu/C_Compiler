#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "alex.h"


    int line=1;
    Token *tokens = NULL, *lastToken = NULL;

    char buf[30000], *pCrtCh;


    void err(const char *fmt,...)
    {
        va_list va;
        va_start(va,fmt);
        fprintf(stderr,"error: ");
        vfprintf(stderr,fmt,va);
        fputc('\n',stderr);
        va_end(va);
        exit(-1);
    }




    void tkerr(const Token *tk,const char *fmt,...)
    {
        va_list va;
        va_start(va,fmt);
        fprintf(stderr,"error in line %d: ",tk->line);
        vfprintf(stderr,fmt,va);
        fputc('\n',stderr);
        va_end(va);
        exit(-1);
    }

    Token *addTk(int code)
    {//aloca memorie pentru atom si il adauga in lista
        Token *tk;
        SAFEALLOC(tk,Token)
        tk->code=code;
        tk->line=line;
        tk->next=NULL;
        if(lastToken){
            lastToken->next=tk;
        }else{
            tokens=tk;
        }
        lastToken=tk;
        return tk;
    }



    void showAtoms()
    {//afisare atomi din lista
    Token *p=tokens;
    while(p!=NULL)
        {
        printf("%d ",p->line);
                switch(p->code)
                    {
                case ID:
                    printf(" ID:%s",p->text);
                    break;
                case END:
                        printf(" END");
                    break;
                    case BREAK:
                    printf(" BREAK");
                    break;
                case CHAR:
                    printf(" CHAR");
                    break;
                case DOUBLE:
                    printf(" DOUBLE");
                    break;
                case ELSE:
                    printf(" ELSE");
                    break;
                case FOR:
                    printf(" FOR");
                    break;
                case IF:
                    printf(" IF");
                    break;
                case INT:
                    printf(" INT");
                    break;
                case RETURN:
                    printf(" RETURN");
                    break;
                case STRUCT:
                    printf(" STRUCT");
                    break;
                case VOID:
                    printf(" VOID");
                    break;
                case WHILE:
                    printf(" WHILE");
                    break;
                case COMMA:
                    printf(" COMMA");
                    break;
                case SEMICOLON:
                    printf(" SEMICOLON");
                    break;
                case LPAR:
                    printf(" LPAR");
                    break;
                case RPAR:
                    printf(" RPAR");
                    break;
                case LBRACKET:
                    printf(" LBRACKET");
                    break;
                case RBRACKET:
                    printf(" RBRACKET");
                    break;
                case LACC:
                    printf(" LACC");
                    break;
                case RACC:
                    printf(" RACC");
                    break;
                    case ADD:
                    printf(" ADD");
                    break;
                case SUB:
                    printf(" SUB");
                    break;
                case MUL:
                    printf(" MUL");
                    break;
                case DIV:
                    printf(" DIV");
                    break;
                case DOT:
                    printf(" DOT");
                    break;
                case AND:
                    printf(" AND");
                    break;
                case OR:
                    printf(" OR");
                    break;
                case NOT:
                    printf(" NOT");
                    break;
                case ASSIGN:
                    printf(" ASSIGN");
                    break;
                case EQUAL:
                    printf(" EQUAL");
                    break;
                case NOTEQ:
                    printf(" NOTEQ");
                    break;
                case LESS:
                    printf(" LESS");
                    break;
                case LESSEQ:
                    printf(" LESSEQ");
                    break;
                case GREATER:
                    printf(" GREATER");
                    break;
                case GREATEREQ:
                    printf(" GREATEREQ");
                    break;
                case CT_CHAR:
                        printf(" CT_CHAR:%c",(char)p->i);
                        break;
                    case CT_STRING:
                            printf(" CT_STRING:%s",p->text);
                    break;
                    case CT_INT:
                    printf(" CT_INT:%li",p->i);
                    break;
                case CT_REAL:
                    printf(" CT_REAL:%lf",p->r);
                    break;
                
            }
        printf("\n");	      
        p=p->next;
        }
    
    }

    void terminare()  //eliberare memorie
    {   Token *p=tokens,*q;
           
        while(p!=NULL)
            {q=p;
            if(q->code==CT_STRING || q->code==ID)
            {free(q->text);
            }
            p=q->next;
            free(q);
            }
        tokens=NULL;
        lastToken=NULL;
    
    }



    void replace(char *c)
    {//inlocuieste secventele ESCAPE
    
        for(int i=0;i<strlen(c);i++)
            { if(c[i]=='\\')
            {strcpy(c+i,c+i+1);
            if(c[i]=='a')
                {c[i]='\a';}
            else if(c[i]=='b')
                {c[i]='\b';}
            else if(c[i]=='f')
                {c[i]='\f';}
            else if(c[i]=='n')
                {c[i]='\n';}
            else if(c[i]=='r')
                {c[i]='\r';}
            else if(c[i]=='t')
                {c[i]='\t';}
            else if(c[i]=='v')
                {c[i]='\v';}
            else if(c[i]=='\'')
                {c[i]='\'';}
            else if(c[i]=='\?')
                {c[i]='\?';}
            else if(c[i]=='\"')
                {c[i]='\"';}
            else if(c[i]=='\\')
                {c[i]='\\';}
            else if(c[i]=='\0')
                {c[i]='\0';}

            }
            }

    }
    
    char *createString(const char *pStartCh,char *pCrtCh)//extrage stringul ce incepe cu pStartCh si se termina pCrtCh
    {   char *c;
        int dim=pCrtCh-pStartCh+1;
        int i=0;
        c=(char*)malloc(dim+1);
        if(c==NULL)
            {err("Eroare la alocare");}
        if((*pStartCh)=='\"' || (*pStartCh)=='\'')
            { pStartCh++;dim=dim-2;}
        while(i<dim)
            {
            c[i]=(*pStartCh);
            i++;
            pStartCh++;
            }
            c[i-1]='\0';
        return c;
    
    }

    char *codeName(int code)//returneaza numele atomului
    {
        switch (code)
        {
        case 0:
            return "ID";
        case 1:
            return "END";
        case 2:
            return "BREAK";
        case 3:
            return "CHAR";
        case 4:
            return "DOUBLE";
        case 5:
            return "ELSE";
        case 6:
            return "FOR";
        case 7:
            return "IF";
        case 8:
            return "INT";
        case 9:
            return "RETURN";
        case 10:
            return "STRUCT";
        case 11:
            return "VOID";
        case 12:
            return "WHILE";
        case 13:
            return "COMMA";
        case 14:
            return "SEMICOLON";
        case 15:
            return "LPAR";
        case 16:
            return "RPAR";
        case 17:
            return "LBRACKET";
        case 18:
            return "RBRACKET";
        case 19:
            return "LACC";
        case 20:
            return "RACC";
        case 21:
            return "+";
        case 22:
            return "-";
        case 23:
            return "*";
        case 24:
            return "/";
        case 25:
            return "DOT";
        case 26:
            return "&&";
        case 27:
            return "||";
        case 28:
            return "!";
        case 29:
            return "=";
        case 30:
            return "==";
        case 31:
            return "!=";
        case 32:
            return "<";
        case 33:
            return "<=";
        case 34:
            return ">";
        case 35:
                return ">=";
        case 36:
                return "CT_CHAR";		
        case 37:
                return "CT_STRING";
        case 38:
                return "CT_INT";	
        case 39:
                return "CT_REAL";
            
            
        }
        return NULL;
    }

    int getNextToken()//adauga atomul lexical in lista si returneaza codul lui
    { int state=0,nCh;//starea, dimensiunea sirului(pCrtCh-pStartCh)
    char ch;//caracterul curent
    char *t;//pentru formarea nr in hexa, octa
    const char *pStartCh;
    Token *tk;//atom lexical

    while(1){
    ch=*pCrtCh;
    switch(state){
    
    case 0:
        if(isalpha(ch)||ch=='_'){
            pStartCh=pCrtCh; // memoreaza inceputul ID-ului
            pCrtCh++; // consuma caracterul
            state=1; // trece la noua stare
            }
        else if(ch==' '||ch=='\r'||ch=='\t'){
            pCrtCh++; // consuma caracterul si ramane in starea 0
            }
        else if(ch=='\n'){ // tratat separat pentru a actualiza linia curenta
            line++;
            pCrtCh++;
            }
        else if(ch=='/')
            {pCrtCh++;
            state=3;}
        else if(ch==',')
            {pCrtCh++;
            state=7;}
        else if(ch==';')
            {pCrtCh++;
            state=8;}
        else if(ch=='(')
            {pCrtCh++;
            state=9;}
        else if(ch==')')
            {pCrtCh++;
            state=10;}
        else if(ch=='[')
            {pCrtCh++;
            state=11;}
        else if(ch==']')
            {pCrtCh++;
            state=12;}
        else if(ch=='{')
            {pCrtCh++;
            state=13;}
        else if(ch=='}')
            {pCrtCh++;
            state=14;}
        else if(ch=='+')
            {pCrtCh++;
            state=15;}
        else if(ch=='-')
            {pCrtCh++;
            state=16;}
        else if(ch=='*')
            {pCrtCh++;
            state=17;}
        else if(ch=='.')
            {pCrtCh++;
            state=19;}
        else if(ch=='&')
            {pCrtCh++;
            state=20;}
        else if(ch=='|')
            {pCrtCh++;
            state=22;}
        else if(ch=='!')
            {pCrtCh++;
            state=24;}
        else if(ch=='=')
            {pCrtCh++;
            state=27;}
        else if(ch=='<')
            {pCrtCh++;
            state=30;}
        else if(ch=='>')
            {pCrtCh++;
            state=33;}
        else if(ch=='\'')
            {pStartCh=pCrtCh;
            pCrtCh++;
            state=36;}
        else if(ch=='\"')
            {pStartCh=pCrtCh;
            pCrtCh++;
            state=40;}
        else if(ch>='1'&&ch<='9')
                {pStartCh=pCrtCh;
            pCrtCh++;
            state=43;}
        else if(ch=='0')
                {pStartCh=pCrtCh;
            pCrtCh++;
            state=45;}
        else if(ch==0){ // sfarsit de sir
            addTk(END);
            return END;
            }
        else tkerr(addTk(END),"caracter invalid");
            break;


    case 1:
        if(isalnum(ch)||ch=='_')
            pCrtCh++;
        else  {state=2;}
            break;

    case 2:
        nCh=pCrtCh-pStartCh;//dim sirului
        if(nCh==5&&!memcmp(pStartCh,"break",5))tk=addTk(BREAK);
        else if(nCh==4&&!memcmp(pStartCh,"char",4))tk=addTk(CHAR);
        else if(nCh==6&&!memcmp(pStartCh,"double",6))tk=addTk(DOUBLE);
        else if(nCh==4&&!memcmp(pStartCh,"else",4))tk=addTk(ELSE);
        else if(nCh==3&&!memcmp(pStartCh,"for",3))tk=addTk(FOR);
        else if(nCh==2&&!memcmp(pStartCh,"if",2))tk=addTk(IF);
        else if(nCh==3&&!memcmp(pStartCh,"int",3))tk=addTk(INT);
        else if(nCh==6&&!memcmp(pStartCh,"return",6))tk=addTk(RETURN);
        else if(nCh==6&&!memcmp(pStartCh,"struct",6))tk=addTk(STRUCT);
        else if(nCh==4&&!memcmp(pStartCh,"void",4))tk=addTk(VOID);
        else if(nCh==5&&!memcmp(pStartCh,"while",5))tk=addTk(WHILE);
        else{ tk=addTk(ID);
            tk->text=createString(pStartCh,pCrtCh);}
        return tk->code;

    case 3:
        if(ch=='*')
            {pCrtCh++;
            state=4;}
        else if(ch=='/')
            {pCrtCh++; 
                state=6;}
        else { 
            state=18;}
        break;
        
    case 4:
        if(ch!='*')
        {if(ch=='\n') line++;
        pCrtCh++;}
        else if(ch=='*')
            {pCrtCh++;
            state=5;}
        break;
        
    case 5:
        if(ch=='*')
                pCrtCh++;
        else if(ch=='/')
            {pCrtCh++;
            state=0;}
        else if(ch!='*' && ch!='/')
            {pCrtCh++;
            state=4;}
        else tkerr(addTk(END),"caracter invalid");
        break;
    case 6:  
        if(ch!='\n' && ch!='\r' && ch!='\0')
                pCrtCh++;
        else   state=0;
        break;
    case 7:
        addTk(COMMA);
        return COMMA;

    case 8:
        addTk(SEMICOLON);
        return SEMICOLON;

    case 9:
        addTk(LPAR);
        return LPAR;

    case 10:
        addTk(RPAR);
        return RPAR;

    case 11:
        addTk(LBRACKET);
        return LBRACKET;

    case 12:
        addTk(RBRACKET);
        return RBRACKET;

    case 13:
        addTk(LACC);
        return LACC;

    case 14:
        addTk(RACC);
        return RACC;

    case 15:
        addTk(ADD);
        return ADD;

    case 16:
        addTk(SUB);
        return SUB;

    case 17:
        addTk(MUL);
        return MUL;

    case 18:
        addTk(DIV);
        return DIV;

    case 19:
        addTk(DOT);
        return DOT;

    case 20:
        if(ch=='&')
        {pCrtCh++;
            state=21;}
        else tkerr(addTk(END),"caracter invalid");
        break;

    case 21:
        addTk(AND);
        return AND;
        
    case 22:
        if(ch=='|')
        {pCrtCh++;
            state=23;}
        else tkerr(addTk(END),"caracter invalid");
        break;
        
    case 23:
        addTk(OR);
        return OR;
        
    case 24:
        if(ch=='=')
        {pCrtCh++;
            state=26;}
        else
        {
        state=25;}
        break;

    case 25:
        addTk(NOT);
        return NOT;

    case 26:
        addTk(NOTEQ);
        return NOTEQ;

    case 27:
        if(ch=='=')
        {pCrtCh++;
            state=29;}
        else
        {
        state=28;}
        break;

    case 28:
        addTk(ASSIGN);
        return ASSIGN;

    case 29:
        addTk(EQUAL);
        return EQUAL;

    case 30:  
        if(ch=='=')
        {pCrtCh++;
            state=32;}
        else
        {
        state=31;}
        break;

    case 31:
        addTk(LESS);
        return LESS;

    case 32:
        addTk(LESSEQ);
        return LESSEQ;

    case 33:  
        if(ch=='=')
        {pCrtCh++;
            state=34;}
        else
        {
        state=35;}
        break;


    case 34:
        addTk(GREATEREQ);
        return GREATEREQ;

    case 35:
        addTk(GREATER);
        return GREATER;

    case 36:
        if(ch=='\\')
        {pCrtCh++;
        state=37;}
        else if(ch!='\'' && ch!='\\')
        {pCrtCh++;
        state=38;}
        else tkerr(addTk(END),"caracter invalid");
            break;

    case 37:
        if(ch=='a' || ch=='b' || ch=='f' || ch=='n' || ch=='r' || ch=='t'
        || ch=='v' || ch=='\'' || ch=='?' || ch=='\"' || ch=='0' || ch=='\\')
        {pCrtCh++;
        state=38;}
        else tkerr(addTk(END),"caracter invalid");
        break;

    case 38:
        if(ch=='\'')
            {pCrtCh++;
        state=39;}
        else tkerr(addTk(END),"caracter invalid");
        break;

    
    case 39:
        tk=addTk(CT_CHAR);
        if (pStartCh[1] == '\\')
        {
                switch (pStartCh[2])
                    {
                    case 'a':
                        tk->i = '\a';
                        break;
                    case 'b':
                        tk->i = '\b';
                        break;
                    case 'f':
                        tk->i = '\f';
                        break;
                    case 'n':
                        tk->i = '\n';
                        break;
                    case 'r':
                        tk->i = '\r';
                        break;
                    case 't':
                        tk->i = '\t';
                        break;
                    case 'v':
                        tk->i = '\v';
                        break;
                    case '\'':
                        tk->i = '\'';
                        break;
                    case '?':
                        tk->i = '\?';
                        break;
                    case '\"':
                        tk->i = '\"';
                        break;
                    case '\\':
                        tk->i = '\\';
                        break;
                    case '\0':
                        tk->i = '\0';
                        break;
                    }
        }
        else
        {
            tk->i = pStartCh[1];
            }

                
        return CT_CHAR;
        
    case 40:
        if(ch=='\\')
        {pCrtCh++;
        state=41;}
        else if(ch!='\"' && ch!='\\')
        {pCrtCh++;}
        else if(ch=='\"')
        {pCrtCh++;
        state=42;}
        else tkerr(addTk(END),"caracter invalid");
        break;

    case 41:
        if(ch=='a' || ch=='b' || ch=='f' || ch=='n' || ch=='r' || ch=='t'
        || ch=='v' || ch=='\'' || ch=='?' || ch=='\"' || ch=='0' || ch=='\\')
        {pCrtCh++;
        state=40;}
        else tkerr(addTk(END),"caracter invalid");
        break;

    case 42:
        nCh=pCrtCh-pStartCh;//dim sir
        tk=addTk(CT_STRING);
        tk->text=createString(pStartCh,pCrtCh);
        replace(tk->text);//inlocuieste secventele ESCAPE
        return CT_STRING;

    case 43:
        if(ch>='0' &&ch<='9')
        pCrtCh++;
        else if(ch=='.')
        {pCrtCh++;
        state=50;}
        else if(ch=='e' || ch=='E')
        {pCrtCh++;
        state=52;}
        else state=44;
        break;

    case 44:
        nCh=pCrtCh-pStartCh;
        tk=addTk(CT_INT);
        t=createString(pStartCh,pCrtCh);
        if(t[0]=='0')
        {if(t[1]=='x')
        tk->i=strtol(t,&t,16);
        else
            tk->i=strtol(t,&t,8);
        }
        else
        tk->i=strtol(t,&t,10);
        return CT_INT;

    case 45:
        if(ch=='x')
        {pCrtCh++;
        state=47;}
        else if(ch=='.')
        {pCrtCh++;
        state=50;}
        else 
        state=46;
        break;

    case 46:
        if(ch>='0' && ch<='7')
        pCrtCh++;
        else if(ch=='e' || ch=='E')
        {pCrtCh++;
        state=52;}
        else if(ch>='8' && ch<='9')
        {pCrtCh++;
        state=49;}
        else if(ch=='.')
        {pCrtCh++;
        state=50;}
        else state=44;
        break;

    case 47:
        if(isdigit(ch) || (ch>='a' && ch<='f') || (ch>='A' && ch<='F'))
        {pCrtCh++;
        state=48;}
        else tkerr(addTk(END),"caracter invalid");
        break;

    case 48:
        if(isdigit(ch) || (ch>='a' && ch<='f') || (ch>='A' && ch<='F'))
        pCrtCh++;
        else state=44;
        break;

    case 49:
        if(isdigit(ch))
        pCrtCh++;
        else if(ch=='.')
        {pCrtCh++;
        state=50;}
        else if(ch=='e' || ch=='E')
        {pCrtCh++;
            state=52;}
        else tkerr(addTk(END),"caracter invalid");
        break;

    case 50:
        if(isdigit(ch))
        {pCrtCh++;
        state=51;}
        else tkerr(addTk(END),"lipsa parte zecimala");
        break;

    case 51:
        if(isdigit(ch))
        pCrtCh++;
        if(ch=='e' || ch=='E')
        {pCrtCh++;
        state=52;}
        else state=55;
        break;


    case 52:
        if(ch=='+')
        {pCrtCh++;
        state=53;}
        else  if(ch=='-')
        {pCrtCh++;
            state=53;}
        else state=53;
        break;

    case 53:
        if(isdigit(ch))
        {pCrtCh++;
        state=54;}
        else tkerr(addTk(END),"caracter invalid");
        break;

    case 54:
        if(isdigit(ch))
            pCrtCh++;
        else state=55; 
        break;

    case 55:
        nCh=pCrtCh-pStartCh;
        tk=addTk(CT_REAL);
        t=createString(pStartCh,pCrtCh);
        tk->r=atof(t);
        return tk->code;   
    }
    }
    }

    Token *getTokens()
    {//returneaza lista de atomi lexicali
        return tokens;
    }

    void read(char *s)
    {
        FILE *f;
        f=fopen(s, "r");
        if(f == NULL)
        {perror("the file cannot be opened for reading");
            exit(-1);}
        int n=fread(buf, 1, 30000, f);
        buf[n] = '\0';
            if(fclose(f)!=0)
        {perror("error closing file");
        exit(-1);
        }
        pCrtCh = buf;
        while (getNextToken() != END);
        showAtoms();
    }
