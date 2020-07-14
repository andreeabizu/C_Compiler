#ifndef ALEX
#define ALEX

#define SAFEALLOC(var,Type)              \
        if((var=(Type*)malloc(sizeof(Type)))==NULL)      \
	  err("not enough memory");


enum{ID, END, BREAK,CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE, COMMA, SEMICOLON, LPAR, RPAR, LBRACKET, RBRACKET, LACC, RACC, ADD, SUB, MUL, DIV, DOT, AND, OR, NOT, ASSIGN, EQUAL, NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ, CT_CHAR, CT_STRING,CT_INT, CT_REAL};

typedef struct _Token{
int code; // codul (numele)
union{
char *text; // folosit pentru ID, CT_STRING (alocat dinamic)
long int i; // folosit pentru CT_INT, CT_CHAR
double r; // folosit pentru CT_REAL
};
int line; // linia din fisierul de intrare
struct _Token *next; // inlantuire la urmatorul AL
}Token;


void err(const char *fmt, ...);

void tkerr(const Token *tk, const char *fmt, ...);

void terminare();

char *codeName(int code);

void read(char *s);

Token *getTokens();


#endif
