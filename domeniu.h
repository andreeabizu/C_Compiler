	#ifndef DOMENIU
	#define DOMENIU

	enum{TB_INT,
	     TB_DOUBLE,
	     TB_CHAR,
	     TB_STRUCT,
	     TB_VOID};

	struct _Symbol;
	typedef struct _Symbol Symbol;

	typedef struct{
	     int typeBase; // TB_*
	     Symbol *s; // struct definition for TB_STRUCT
	     int nElements; // >0 array of given size, 0=array without size, <0 non array
	}Type;


	typedef struct{
	Symbol **begin; // the beginning of the symbols, or NULL
	Symbol **end; // the position after the last symbol
	Symbol **after; // the position after the allocated space
	}Symbols;


	enum{CLS_VAR,
	     CLS_FUNC,
	     CLS_EXTFUNC,
	     CLS_STRUCT};


	enum{MEM_GLOBAL,
	     MEM_ARG,
	     MEM_LOCAL};

	typedef struct _Symbol{
	  const char *name; // a reference to the name stored in a token
	  int cls; // CLS_*
	  int mem; // MEM_*
	  Type type;
	  int depth; // 0-global, 1-in function, 2... - nested blocks in function
	  union{
	    Symbols args; // used only of functions
	    Symbols members; // used only for structs
	  };
	  union{
	    void *addr; // vm: the memory address for global symbols
	    int offset; // vm: the stack offset for local symbols
	  };
	}Symbol;


	typedef union{
	int i; // int, char
	double d; // double
	const char *str; // char[]
	}CtVal;

	typedef struct{
	Type type; // type of the result
	int isLVal; // if it is a LVal
	int isCtVal; // if it is a constant value (int, real, char, char[])
	CtVal ctVal; // the constat value
	}RetVal;


	Symbols symbols;
	int crtDepth;
	extern Symbol *crtFunc, *crtStruct;
	extern Token *consumedTk, *crtTk;
	extern int offset;

	void initSymbols(Symbols *symbols);

	Symbol *addSymbol(Symbols *symbols, const char *name, int cls);

	Symbol *findSymbol(Symbols *symbols, const char *name);

	void deleteSymbolsAfter(Symbols *symbols, Symbol *start);

	Type createType(int typeBase, int nElements);

	void addExtFuncs();

	void addVar(Token *tkName, Type *t);

	void cast(Type *dst, Type *src);

	Type getArithType(Type *s1, Type *s2);

	void printSymbols(Symbols *symbols);

	Symbol *requireSymbol(Symbols *symbols, const char *name);
	#endif
