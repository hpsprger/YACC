#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Kind.h"
#include "Type.h"

/*
 * Allocates memory for the kindPtr of the symbol based on the kind of the
 * symbol passed.
 * WARNING: make sure you set s->kind BEFORE calling this function.
 * 
 * Parameter: pointer to symbol
 *
 * Return: void
 */
void
allocateKindPtr(Symbol *s)
{
	s->typeOriginator = 0;
	if (!s) return;
	s->typeOriginator = 0;	
	switch (s->kind) {
	case CONST_KIND: 
		s->kindPtr.ConstKind = allocateConstantKind();
		break;
	case FUNC_KIND:
		s->kindPtr.FuncKind = allocateFunctionKind();
		break;
	case PARAM_KIND:
		s->kindPtr.ParamKind = allocateParamKind();
		break;
	case PROC_KIND:
		s->kindPtr.ProcKind = allocateProcedureKind();
		break;
	case TYPE_KIND:
		s->kindPtr.TypeKind = allocateTypeKind();
		s->typeOriginator = 1;
		break;
	case VAR_KIND:
		s->kindPtr.VarKind = allocateVariableKind();
		break;	
	default:
		/* NOT REACHED */
		break;
	}	
}

/*
 * Allocate space for ConstantKind
 * 
 * Parameter: none
 * 
 * Return: pointer to struct ConstantKind
 */
struct ConstantKind *
allocateConstantKind()
{
	struct ConstantKind *ck = calloc(1, sizeof(struct ConstantKind));
	allocationErrorCheck(ck);
	return ck;
}

/*
 * Allocate space for FunctionKind
 * 
 * Parameter: none
 * 
 * Return: pointer to struct FunctionKind
 */
struct FunctionKind *
allocateFunctionKind()
{
	struct FunctionKind *fk = calloc(1, sizeof(struct FunctionKind));
	allocationErrorCheck(fk);
	return fk;
}

/*
 * Allocate space for ParamKind
 * 
 * Parameter: none
 * 
 * Return: pointer to struct ParamKind
 */
struct ParamKind *
allocateParamKind()
{
	struct ParamKind *pk = calloc(1, sizeof(struct ParamKind));
	allocationErrorCheck(pk);
	return pk;
}

/*
 * Allocate space for ProcedureKind
 * 
 * Parameter: none
 * 
 * Return: pointer to struct ProcedureKind
 */
struct ProcedureKind *
allocateProcedureKind()
{
	struct ProcedureKind *pk = calloc(1, sizeof(struct ProcedureKind));
	allocationErrorCheck(pk);
	return pk;
}

/*
 * Allocate space for TypeKind
 * 
 * Parameter: none
 * 
 * Return: pointer to struct TypeKind
 */
struct TypeKind *
allocateTypeKind()
{
	struct TypeKind *tk = calloc(1, sizeof(struct TypeKind));
	allocationErrorCheck(tk);
	return tk;
}

/*
 * Allocate space for VariableKind
 * 
 * Parameter: none
 * 
 * Return: pointer to struct VariableKind
 */
struct VariableKind *
allocateVariableKind() 
{
	struct VariableKind *vk = calloc(1, sizeof(struct VariableKind));
	allocationErrorCheck(vk);
	return vk;
}

/*
 * Check if the memory allocation successful or not
 * 
 * Parameter: pointer to struct
 * 
 * Return: void
 */
void
allocationErrorCheck(void *p)
{
	if (!p) {
		err(1, "Failed to allocate memory for kind ptr!");
		exit(1);
	}
}

/*
 * Acquire value of an anonymous constant
 * 
 * Parameter: pointer to symbol
 * 
 * Return: pointer to AnonConstVal
 */
AnonConstVal *
getConstVal(Symbol *s)
{
	if (s == NULL) {
		err(EXIT_FAILURE, "NULL argument passed to getConstVal");
	}
	return &(s->kindPtr.ConstKind->value);
}

/*
 * Acquire the actual value of an anonymous constant symbol which is of a 
 * simple type.
 * WARNING: to be used only in calculating expressions resultant from
 * constant symbols.
 * Parameters
 *    s : the const kind symbol whose value is to be returned
 */
double
getSimpleConstVal(Symbol *s)
{
	if (!s) return 0;
	
	switch (getType(s)) {
  
	case BOOLEAN_T:
		return (double)(getConstVal(s)->Boolean.value);
    
	case CHAR_T:
		return (double)(getConstVal(s)->Char.value);

	case INTEGER_T:
		return (double)(getConstVal(s)->Integer.value);

	case SCALARINT_T:
		return (double)(getConstVal(s)->Integer.value);

	case REAL_T:
		return (double)(getConstVal(s)->Real.value);

	default:
    	/* SHOULD NOT BE REACHED */
	return 0;
	}
}

/*
 * Assigning the actual value of an anonymous constant symbol which is of a 
 * simple type.
 * WARNING: to be used only in calculating expressions resultant from
 * constant symbols.
 * Parameters
 *		s : the const kind symbol whose value is to be returned
 *		val : the result of calculation.	
 */
void
setSimpleConstVal(Symbol *s, double val){
	if (!s) {
		err(1, "Can't set value to the symbol due to NULL pointer");
		exit(1);
	}
	
	switch (getType(s)) {
  
	case BOOLEAN_T:
		getConstVal(s)->Boolean.value = (int)val;
    	break;
    	
	case CHAR_T:
		getConstVal(s)->Char.value = (char)val;
		break;
		
	case INTEGER_T:
		getConstVal(s)->Integer.value = (int)val;
		break;
	case SCALARINT_T:
		getConstVal(s)->Integer.value = (int)val;
		break;
	case REAL_T:
		getConstVal(s)->Real.value = (double)val;
		break;
		
	default:
    		/* SHOULD NOT BE REACHED */
		err(EXIT_FAILURE,
		    "This type of value can't be set to the symbol %s",
		    typeToString(getType(s)));
	}
}

/*
 * Makes a copy of the given AnonConstVal based on type -- used when
 * creating a const symbol from a proxy symbol.
 * 
 * Parameter: pointer to new AnonConstVal, pointer to AnonConstVal, 
 * and type of AnonConstVal
 * 
 * Return: void
 */
void
copyConstVal(AnonConstVal *newacl, AnonConstVal * acl, type_t type)
{
	if (!acl) return;
	switch (type) {
	case BOOLEAN_T:
		newacl->Boolean.value = acl->Boolean.value;
		break;
	case CHAR_T:
		newacl->Char.value = acl->Char.value;
		break;
	case INTEGER_T:
		newacl->Integer.value = acl->Integer.value;
		break;
	case REAL_T:
		newacl->Real.value = acl->Real.value;
		break;
	case STRING_T:
		setStringStr(&newacl->String, acl->String.str, acl->String.strlen);

		/*
			newacl->String.str = calloc(1, sizeof(AnonConstVal));
		if (!newacl->String.str) {
			err(1, "failed to allocate memory for string literal!");
			exit(1);
		}
		newacl->String.strlen = acl->String.strlen;
		strncpy(
		    newacl->String.str, acl->String.str, newacl->String.strlen);
		*/
		break;
	default:
		break;
	}	

}

/*	Check whehter a function is invocked correctly by its name.
 *	
 *  Parameters: Symbol in function kind
 * 	Return: 1: invocation is correct
 *			0: invocation is wrong
 */
int isInvocInstance(Symbol *sym){
	return sym->kindPtr.FuncKind->invocationInstance;
}

/* Ensure that the kind of the given sybmols are actually compatible with each other
 * before assigning one to the other one.
 * This function checks the following:
 *		Assigning to a TYPE_KIND Symbol 
 * areKindAssignCompat also be able to validate the correctness of function 
 * inovaction by calling isInvocInstance(). (May need to refactor this later)
 *
 *	Parameters: Symbol x on LHS
 *				Symbol y on RHS
 *
 *	Returns: 1 if two sym are compatible
 *			 0 if not
 */
 
int areKindAssignCompat(Symbol *x, Symbol *y){

	if ((x->kind == VAR_KIND) && (y->kind == FUNC_KIND)) {
		if (!isInvocInstance(y)){
			return 0;
		}
	}
	
	if ((x->kind == TYPE_KIND) && (x->isAddress != 1)) {
		return 0;
	}
	
	return 1;
}
