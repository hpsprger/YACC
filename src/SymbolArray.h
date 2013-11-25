#ifndef SYMBOL_ARRAY_H
#define SYMBOL_ARRAY_H


int typeIsInValidArgs(Symbol *s, type_t type);


/*
 * Given a linked list of ProxySymbols, returns the type which results
 * from using the linked list of ProxySymbols to access the array given
 * by var.
 * TODO: if index is const not part of scalar, see if its value falls in the
 * allowable range.
 *
 */
Symbol *
isValidArrayAccess(ProxySymbol *var, ProxySymbol *indices);


/*
 * Returns the dimension of the given array (assumes that the given
 * Symbol describing the array is the "first dimensional array")
 */
int
getArrayDim(Symbol *s);


/*
 * Returns the symbol which indexes the array.
 */
Symbol *
getArrayIndexSym(Symbol *s);


Symbol *
getArrayTerminalTypeSym(Symbol *s);


Symbol *
getArrayBaseSym(Symbol *s);



#endif