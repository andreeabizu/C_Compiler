#ifndef GC
#define GC


Instr *createCondJmp(RetVal *rv);

int typeBaseSize(Type *type);

int typeFullSize(Type *type);

int typeArgSize(Type *type);

Instr *getRVal(RetVal *rv);

void addCastInstr(Instr *after, Type *actualType, Type *neededType);

#endif
