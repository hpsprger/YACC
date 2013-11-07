#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "Definitions.h"

#include "ElementArray.h"
#include "Error.h"
#include "Hash.h"
#include "Type.h"
#include "Symbol.h"
#include "Utils.h"
#ifndef TESTBUILD
#include "parser.tab.h"	/* token definitions used in operator compat checks */
#endif
#ifdef TESTBUILD
#include "tokens.h"
#endif
extern struct hashElement *symbolTable[TABLE_SIZE];
extern int yylineno;
extern int colno;
/*
 * Utility functions. Can maybe be refactored into own module.
 */

/*
 * Check that the given types are compatible when using the given
 * operator.
 *
 * Return a pointer to a Symbol struct 
 * that is populated with the resulting type when performing the operation, and
 * the value of the expression if it can be evaluated (like when dealing with
 * constants).
 *
 * If the types are not combatible for this operator, set error and return NULL.
 * TODO: right now success returns non-NULL for the benefit of unit tests.
 */
Symbol *assertOpCompat(
    Symbol *type1, int opToken, Symbol *type2) {
	
	type_t s1_t, s2_t;
	char *errMsg;	
	s1_t = getType(type1);
	s2_t = getType(type2);

	/* if type1 pointer is null but the operator is PLUS or MINUS (i.e.,
	 * it is a unary operator) then we assume the best */ 
	if ((!type1) && (isUnaryOperator(opToken))) {
		if ((s2_t == REAL_T) || (s2_t == INTEGER_T)) return type2;
		return NULL; /* else error */
	} else if (!type1) return NULL; /* else it was an error */


	/* Only simple and string types are compatible with operators */
	if (!(isSimpleType(s1_t) && isSimpleType(s2_t)) &&
	    (s1_t != STRING_T)) {
		/* TODO: record error */
		return NULL;
	}

	/* If operator is a logical operator, we only accept boolean types */
	if (isLogicalOperator(opToken)) {
		if ((s1_t != BOOLEAN_T) || (s2_t != BOOLEAN_T)) return NULL;
		/* Else return pointer to pre-defined boolean type */
		return type1; 
	}

	/* If the operator is relational, we just need op compatible types */
	if ((isRelationalOperator(opToken)) && areOpCompatible(type1, type2)) {
		return type1;
	}

	if (areArithmeticCompatible(type1, type2)) {
		switch (opToken) {
			case PLUS:
			case MINUS:
			case MULTIPLY:
				if (areBothInts(type1, type2)) {
					/* Return pointer to int type */
					return type1;
				}
				else return type1; /* ret ptr to real type */
				break;
			case DIVIDE:
				/* return pointer to real type */
				return type1;
			case DIV:
			case MOD:
				if (areBothInts(type1, type2)) {
					/* return ptr to int type */
					return type1;
				}
				break;
			default:
				/* NOT REACHED */
				break;	
		}
	}

	/* Fell through to here, must have been an error */
	
	return NULL;
}

/*
 * Return 0 if the given types are assignment compatible, otherwise return
 * non-zero
 */
int isAssignmentCompat(Symbol type1, Symbol type2) {
	// TODO implement
	return -1;
}

/*
 * Parser action functions, in the order they appear in parser.y
 */

/*
 * Capture program header definitions of input and output arguments.
 *
 * Arguments may be null if program contains errors.
 */
void doProgramDecl(char *prog_name, char *in_name, char *out_name) {
	// TODO push lexical level, figure this out
}

/*
 * Perform actions necessary when exiting const decl part.
 */
void exitConstDeclPart(void) {
	// function does nothing.
}

/*
 * Create a new constant in the symbol table.
 * Extract the value and type information from the proxy.
 */
void doConstDecl(char *id, ProxySymbol *proxy) {
	Symbol *s = NULL;
	int lvl = 0;
	s = newConstSymFromProxy(lvl, id, proxy);		
	if (s) {
		/* Add s to symbol table */
	}

}

/*
 * Perform actions necessary when exiting type decl part.
 */
void exitTypeDeclPart(void) {
	// probably nothing to do here
}

/*
 * Create a new type identifier symbol in the symbol table.
 */
void doTypeDecl(char *id, Symbol *type) {
	Symbol * newTypeSym = NULL;
	int lvl = 0;	/* TODO: make this get actual lex lvl */
	
	newTypeSym = newTypeSymFromSym(lvl, id, type);
	if (newTypeSym) {
		/* add to symbol table */
	}

	/* Else, error.  newTypeSymFromSym performs error checking */
}

/*
 * Do a lookup in the symbol table for the type with the given id.
 *
 * Raise an error if the type is not available.
 *
 * Return a pointer to the type.
 */
Symbol *simpleTypeLookup(char *id) {
	return NULL;
}

/*
 * Add new_id as a mamber to scalar_list.
 *
 * Return scalar_list
 */
Symbol *appendToScalarListType(Symbol *scalar_list, char *new_id) {
	return scalar_list;
}

/*
 * Create a new scalar list type with id as the only member.
 *
 * Return a pointer to the new scalar list
 */
Symbol *createScalarListType(char *id) {
	return NULL;
}

/*
 * Create a new array type given the index type and base type.
 */
Symbol *createArrayType(Symbol *index, Symbol *base) {
	Symbol * newArraySym = NULL;
	int lvl = 0;	/* TODO: get actual lexical level */

	newArraySym = newAnonArraySym(lvl, base, index);
	if (newArraySym) {
		/* TODO: Add to symbol table */
		return newArraySym;
	}

	/* Else, error.  Error reporting done in newAnonArraySym() */	
	return NULL;
}

/*
 * Check that the given type is a valid array index type.
 *
 * Raise an error if the type is not valid as an index type.
 *
 * Return the given type.
 */
Symbol *assertArrIndexType(Symbol *index_type) {
	type_t sym_t;

	sym_t = getType(index_type);

	if ( (sym_t != SUBRANGE_T) && (sym_t != SCALAR_T) ) {
		/* Set error */
		return NULL;
	}
	return index_type;
}

/*
 * Create a new subrange type. The arguments are proxy symbols that should
 * constain constant values for the range bounds.
 *
 * Return a pointer to the new subrange type.
 */
Symbol *createRangeType(ProxySymbol *lower, ProxySymbol *upper) {
	Symbol *s = NULL;
	int lvl = 0;
	s = newSubrangeSym(lvl, (Symbol *) lower, (Symbol *) upper);
	return s;
}

/*
 * Create a new record type from the given proxy. the proxy contains the name
 * of a field and the type of that field.
 *
 * Return a pointer to the new record type symbol.
 */
Symbol *createRecordType(ProxySymbol *first_field) {
	return NULL;
}

/*
 * Add a field to the given record type. The proxy new_field contains
 * the name and type of the new record field.
 *
 * Return a pointer to the record type.
 */
Symbol *appendFieldToRecordType(Symbol *record_type, ProxySymbol *new_field) {
	return record_type;
}

/*
 * Create a new ProxySymbol with the given id and type.
 *
 * Return a pointer to the new proxy.
 */
ProxySymbol *newRecordFieldProxy(char *id, Symbol *type) {
	return NULL;
}

/*
 * Perform actions necessary when exiting variable dec section.
 */
void exitVarDeclPart(void) {
	// nothing to do here
}

/*
 * Declare a new variable with the given type.
 *
 * Return a pointer to type.
 */
Symbol *doVarDecl(char *id, Symbol *type) {
	Symbol *s = NULL;
	int lvl = 0;
	/* TODO: do local look up for symbol */

	if ((!id) || !(type)) return NULL;

	s = newVariableSym(lvl, id, type);

	if (s) {
		/* TODO: add s to the symbol table */
	}

	return type;
}

/*
 * Exit a procedure or function definition.
 *
 * This is a good time to pop lexical level.
 */
void exitProcOrFuncDecl(void) {
	// TODO
}

/*
 * Declare a new procedure with given arguments and perform anything
 * necessary when entering the procedure body definition (like increasing
 * the lexical level).
 *
 * Both arguments can be NULL if the definition contains errors.
 *
 * Return a pointer to the procedure.
 */
Symbol *enterProcDecl(char *id, struct ElementArray *ea) {

	Symbol *procSym = NULL;
	int lvl = 0;
	/* TODO: local lookup of id in symbol table */
	
	/* if the above lookup returned nothing... */

	procSym = newProcSym(lvl, id, ea);
	return procSym;
}

/*
 * Declare a new function with the given arguments and perform anything
 * necessary when entering the function body definition.
 *
 * Both arguments can be NULL if the definition contains errors.
 * 
 * Return a pointer to the procedure.
 */
Symbol *enterFuncDecl(char *id, struct ElementArray *ea, Symbol *typeSym) {
	Symbol *funcSym = NULL;
	int lvl = 0;

	/* TODO: local lookup of id in symbol table */

	funcSym = newFuncSym(lvl, id, typeSym, ea);
	return funcSym;
}

/*
 * Create a new parameter list from the given parameter.
 *
 * Return a pointer to the parameter list.
 */
struct ElementArray *createParmList(Symbol *parm) {
	struct ElementArray *ea = NULL;

	if (!parm) return NULL;

	ea = newElementArray();
	growElementArray(ea);
	appendElement(ea, parm);
	return ea;	
}

/*
 * Append the given parameter to the parameter list.
 *
 * Return a poinnter to the parameter list.
 */
struct ElementArray *appendParmToParmList(
    struct ElementArray *ea, Symbol *parm) {

	if ( !(ea) || !(parm) ) return NULL;
	appendElement(ea, parm);	
	return ea;
}

/*
 * Create a new parameter with the given name and type.
 *
 * Return a pointer to the new parameter.
 */
Symbol *createNewParm(char *id, Symbol *type) {
	
	/* TODO: NO LOOKUP IN SYMBOL TABLE NECESSARY */

	int lvl = 0;

	if ((!id) || (!type)) return NULL;
	return newParamSym(lvl, id, type);
}

/*
 * Create a new var parm with the given name and type.
 *
 * Return a pointer to the new parameter.
 */
Symbol *createNewVarParm(char *id, Symbol *type) {
	Symbol *s = createNewParm(id, type);
	if (s) s->kindPtr.ParamKind->byRef = 1;
	return s;
}

/*
 * Perform assignment of x to y.
 */
void assignOp(ProxySymbol *x, ProxySymbol *y) {
}

ProxySymbol *hashLookupToProxy(char *id) {
	return NULL;
}

ProxySymbol *recordAccessToProxy(char *id1, char *id3) {
	return NULL;
}

/*
 * Access an array given the list of indexes.
 *
 * Return a ProxySymbol of the expected type.
 */
ProxySymbol *arrayIndexAccess(ProxySymbol *var, ProxySymbol *indexes) {
	return NULL;
}

/*
 * Concatenate two arrays of array indexes, maintaining order.
 *
 * Return a pointer to a concatenated list.
 */
ProxySymbol *concatArrayIndexList(ProxySymbol *list1, ProxySymbol *list2) {
	return list1;
}

/*
 * Create a new array index list as a ProxySymbol.
 *
 * Return a pointer to the new list.
 */
ProxySymbol *createArrayIndexList(ProxySymbol *exp) {
	return NULL;
}

ProxySymbol *eqOp(ProxySymbol *x, ProxySymbol *y) {

	if ((!x) || (!y)) return NULL;
	
	/* 
	 * If x or y is not a constant, we have no responsibility
	 * with regard to insuring the propogation of a compile time
	 * known funciton.
	 */
	if ((x->kind != CONST_KIND) || (y->kind != CONST_KIND)) {
		return assertOpCompat(getTypeSym((Symbol *) x), EQUAL, 
		    getTypeSym((Symbol *)y));
	}	

	/* Else, we have two CONST_KIND symbols.  We must evaluate */

	return NULL;
}

ProxySymbol *notEqOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *lessOrEqOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *lessOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *gtOrEqOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *gtOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *unaryPlusOp(ProxySymbol *y) {
	return NULL;
}

ProxySymbol *unaryMinusOp(ProxySymbol *y) {
	return NULL;
}

ProxySymbol *plusOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *minusOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *orOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *multOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *divideOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *divOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *modOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *andOp(ProxySymbol *x, ProxySymbol *y) {
	return NULL;
}

ProxySymbol *unaryNotOp(ProxySymbol *x) {
	return NULL;
}

/*
 * Make a new ProxySymbol with type integer and given value.
 * Return pointer to the proxy
 */
ProxySymbol *proxyIntLiteral(int value) {
	Symbol *integerType = NULL;
	return newConstProxySym(&value, integerType); 
}
	
/*
 * Make a new anonymous symbol with type real and given value.
 * Return a pointer to the hash symbol.
 */
ProxySymbol *proxyRealLiteral(double value) {
	Symbol *realType = NULL;
	return newConstProxySym(&value, realType);
}

/*
 * Make a new anonymous symbol with the given string.
 * Return a pointer to the hash symbol.
 */
ProxySymbol *proxyStringLiteral(char *value) {
	// TODO: we require the length of the string in order
	// to avoid the cases where the string literal may have null
	// bytes.
	int lvl = 0, strlen = 0;
	return newStringProxySym(lvl, value, strlen);
}

/*
 * Invoke procedure with given name.
 *
 * The argument argv contains a list of arguments.
 */
void procInvok(char *id, ProxySymbol *argv) {
	// TODO
}

/*
 * Invoke function with the given name.
 *
 * The argument argv contains a list of arguments.
 *
 * Return a ProxySymbol containing the type returned.
 */
ProxySymbol *funcInvok(char *id, ProxySymbol *argv) {

	return NULL;
}

/*
 * Create an argument list for function and procedure invocation.
 *
 * Return a pointer to a ProxySymbol containing the list.
 */
struct ElementArray *createArgList(Symbol *arg) {
	struct ElementArray * ea = NULL;

	if (!arg) {
		/* ERROR */
		return NULL;
	}
	ea = newElementArray();
	growElementArray(ea);
	appendElement(ea, getTypeSym(arg));	
	return ea;
}

/*
 * Concatenate two lists of arguments, maintaining order.
 *
 * Return a pointer to a list containing both sets of arguments.
 * Whether to add the arguments to one of the lists or two make a third
 * list and add all arguments is up to implementation.
 */
struct ElementArray *concatArgLists(struct ElementArray *ea, Symbol *arg) {
	if ((!ea) || (!arg)) return NULL;
	appendElement(ea, arg);	
	return ea;
}

/*
 * Encountered 'continue' statement. Verify we're in a while loop and then
 * continue to next iteration.
 */
void continueLoop(void) {
	// TODO
}

/*
 * Encountered 'exit' statement. Verify we're in a while loop and then
 * exit innermost loop.
 */
void exitLoop(void) {
	// TODO
}

/*
 * End of while loop.
 */
void endWhileLoop(void) {
	// TODO
}
