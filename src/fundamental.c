/*
    Copyright 2019 Joel Svensson	svenssonjoel@yahoo.se

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "symrepr.h"
#include "stack.h"
#include "heap.h"

#include <stdio.h>
/*
  Fundamental operations expect number of arguments and arguments pushed
  onto the stack.
  | n-args |
  | arg1   |
  | ...    |
  | argn   |
  After execution of a fundamental operation the result will be located
  at the top of the K-stack.
 */
  
static UINT as_i(UINT a) {
  
  UINT tmp;
  FLOAT f_tmp;
  
  switch (type_of(a)) {
  case VAL_TYPE_I: 
    return dec_i(a);
  case VAL_TYPE_U:
    return (INT) dec_u(a);
  case PTR_TYPE_BOXED_I:
  case PTR_TYPE_BOXED_U:
    return (INT)car(a);
  case PTR_TYPE_BOXED_F:
    tmp = car(a);
    memcpy(&f_tmp, &tmp, sizeof(FLOAT));
    return (INT)f_tmp;
  }
  return 0;
}

static UINT as_u(UINT a) {
  
  UINT tmp;
  FLOAT f_tmp;
  
  switch (type_of(a)) {
  case VAL_TYPE_I: 
    return (UINT) dec_i(a);
  case VAL_TYPE_U:
    return dec_u(a);
  case PTR_TYPE_BOXED_I:
  case PTR_TYPE_BOXED_U:
    return (UINT)car(a);
  case PTR_TYPE_BOXED_F:
    tmp = car(a);
    memcpy(&f_tmp, &tmp, sizeof(FLOAT));
    return (UINT)f_tmp;
  }
  return 0;
}

static UINT as_f(UINT a) {
  
  UINT tmp;
  FLOAT f_tmp;
  
  switch (type_of(a)) {
  case VAL_TYPE_I: 
    return (FLOAT) dec_i(a);
  case VAL_TYPE_U:
    return (FLOAT)dec_u(a);
  case PTR_TYPE_BOXED_I:
  case PTR_TYPE_BOXED_U:
    return (FLOAT)car(a);
  case PTR_TYPE_BOXED_F:
    tmp = car(a);
    memcpy(&f_tmp, &tmp, sizeof(FLOAT));
    return f_tmp;
  }
  return 0;
}

/*
static  UINT as_t(UINT a, UINT t) {
  UINT tmp;
  FLOAT f_tmp;
  
  switch (type_of(t)) {
  case VAL_TYPE_I: 
    return enc_i(as_i(a));
  case VAL_TYPE_U:
    return enc_u(as_u(a));
  case PTR_TYPE_BOXED_I:
    return enc_I(as_i(a));
  case PTR_TYPE_BOXED_U:
    return enc_U(as_u(a));;
  case PTR_TYPE_BOXED_F:
    tmp = car(a);
    memcpy(&f_tmp, &tmp, sizeof(FLOAT));
    return f_tmp;
  }
  return 0;
}
*/

static UINT add2(UINT a, UINT b) {

  UINT retval = enc_sym(symrepr_terror());
  UINT t_min;
  UINT t_max;
  INT i0;
  INT i1;
  UINT u0;
  UINT u1;
  FLOAT f0;
  FLOAT f1;
      
  
  if (type_of(a) < type_of(b)) {
    t_min = a;
    t_max = b;
  } else {
    t_min = b;
    t_max = a;
  }

  if (type_of(t_min) > VAL_TYPE_CHAR) { 
    switch (type_of(t_max)) {
    case VAL_TYPE_I:
      i0 = dec_i(t_max);
      i1 = as_i(t_min);
      retval = enc_i(i0 + i1);
      break;
    case VAL_TYPE_U: 
      u0 = dec_u(t_max);
      u1 = as_u(t_min);
      retval = enc_u(u0 + u1);
      break;
    case PTR_TYPE_BOXED_U:
      u0 = dec_U(t_max);
      u1 = as_u(t_min);
      retval = enc_U(u0+u1); //cons(u0+u1, enc_sym(DEF_REPR_BOXED_U_TYPE));
      break;
    case PTR_TYPE_BOXED_I:
      i0 = dec_I(t_max);
      i1 = as_u(t_min);
      retval = enc_I(i0 + i1); //cons(i0+i1, enc_sym(DEF_REPR_BOXED_I_TYPE));
      break;
    case PTR_TYPE_BOXED_F:
      f0 = dec_f(t_max);
      f1 = as_f(t_min);
      f0 = f0 + f1;
      //memcpy(&retval, &f0, sizeof(FLOAT));
      retval = enc_F(f0); //cons(retval, enc_sym(DEF_REPR_BOXED_F_TYPE));
      break;
    }
  } 
  return retval;
}

static UINT negate(UINT a) {

  UINT retval = enc_sym(symrepr_terror());
  INT i0;
  UINT u0;
  FLOAT f0;

  if (type_of(a) > VAL_TYPE_CHAR) { 
    switch (type_of(a)) {
    case VAL_TYPE_I:
      i0 = dec_i(a);
      retval = enc_i(-i0);
      break;
    case VAL_TYPE_U: 
      u0 = dec_u(a);
      retval = enc_u(-u0);
      break;
    case PTR_TYPE_BOXED_U:
      u0 = dec_U(a);
      retval = enc_U(-u0); //cons(-u0, enc_sym(DEF_REPR_BOXED_U_TYPE));
      break;
    case PTR_TYPE_BOXED_I:
      i0 = dec_I(a);
      retval = enc_I(-i0); //cons(-i0, enc_sym(DEF_REPR_BOXED_I_TYPE));
      break;
    case PTR_TYPE_BOXED_F:
      f0 = dec_f(a);
      f0 = -f0;
      //memcpy(&retval, &f0, sizeof(FLOAT));
      retval = enc_F(f0); //cons(retval, enc_sym(DEF_REPR_BOXED_F_TYPE));
      break;
    }
  } 
  return retval;
}

static UINT sub2(UINT a, UINT b) {

  UINT retval = enc_sym(symrepr_terror());
  UINT t_min;
  UINT t_max;
  INT i0;
  INT i1;
  UINT u0;
  UINT u1;
  FLOAT f0;
  FLOAT f1;
      
  
  if (type_of(a) < type_of(b)) {
    t_min = a;
    t_max = b;
  } else {
    t_min = b;
    t_max = a;
  }

  if (type_of(t_min) > VAL_TYPE_CHAR) { 
    switch (type_of(t_max)) {
    case VAL_TYPE_I:
      i0 = dec_i(t_max);
      i1 = as_i(t_min);
      retval = enc_i(i0 - i1);
      break;
    case VAL_TYPE_U: 
      u0 = dec_u(t_max);
      u1 = as_u(t_min);
      retval = enc_u(u0 - u1);
      break;
    case PTR_TYPE_BOXED_U:
      u0 = dec_U(t_max);
      u1 = as_u(t_min);
      retval = enc_U(u0 - u1);
      break;
    case PTR_TYPE_BOXED_I:
      i0 = dec_I(t_max);
      i1 = as_u(t_min);
      retval = enc_I(i0 - i1);
      break;
    case PTR_TYPE_BOXED_F:
      f0 = dec_f(t_max);
      f1 = as_f(t_min);
      f0 = f0 - f1;
      retval = enc_F(f0);
      break;
    }
  } 
  return retval;
}


static bool struct_eq(VALUE a, VALUE b) {

  if (!is_ptr(a) && !is_ptr(b)) {
    if (val_type(a) == val_type(b)){
      switch (val_type(a)) {
      case VAL_TYPE_SYMBOL:
	return (dec_sym(a) == dec_sym(b));
      case VAL_TYPE_I:
	return (dec_i(a) == dec_i(b));
      case VAL_TYPE_U:
	return (dec_u(a) == dec_u(b));
      case VAL_TYPE_CHAR:
	return (dec_char(a) == dec_char(b));
      default:
	return false;
	break;
      }
    } else {
      return false;
    }
  }

  if (is_ptr(a) && is_ptr(b)) {
    if (ptr_type(a) == ptr_type(b)) {
      switch (ptr_type(a)) {
      case PTR_TYPE_CONS:
	return ( struct_eq(car(a),car(b)) &&
	  	 struct_eq(cdr(a),cdr(b)) );
      case PTR_TYPE_BOXED_I:
	return ((INT)car(a) == (INT)car(b));
      case PTR_TYPE_BOXED_U:
	return (car(a) == car(b));
      case PTR_TYPE_BOXED_F:
	return ((FLOAT)car(a) == (FLOAT)car(b));
      case PTR_TYPE_ARRAY:
	return false; //array_equality(a, b); 
      default:
	printf("TODO: Structural equality for this ptr type not implemented\n");
	return false;
      }
    }
  }
  return false;
}

static int cmpi(INT a, INT b, bool swapped) {
  int res = (a > b) - (a < b);
  return swapped ? -res : res;
}

static int cmpu(UINT a, UINT b, bool swapped) {
  int res = (a > b) - (a < b);
  return swapped ? -res : res;
}

static int cmpf(FLOAT a, FLOAT b, bool swapped) {
  int res = (a > b) - (a < b);
  return swapped ? -res : res;
}


/* returns -1 if a < b; 0 if a = b; 1 if a > b */
static int compare(UINT a, UINT b) {
  int retval = 1;
  UINT tmp;
  INT i0;
  INT i1;
  UINT u0;
  UINT u1;
  FLOAT f0;
  FLOAT f1;
  bool swapped = false;
  
  if (type_of(a) < type_of(b)) {
    tmp = a;
    a   = b;
    b   = tmp;
    swapped = true;
  } 

  if (type_of(b) > VAL_TYPE_CHAR) { 
    switch (type_of(a)) {
    case VAL_TYPE_I:
      i0 = dec_i(a);
      i1 = as_i(b);
      retval = cmpi(i0,i1,swapped);
      break;
    case VAL_TYPE_U: 
      u0 = dec_u(a);
      u1 = as_u(b);
      retval = cmpu(u0,u1,swapped);
      break;
    case PTR_TYPE_BOXED_U:
      u0 = dec_U(a);
      u1 = as_u(b);
      retval = cmpu(u0,u1,swapped);
      break;
    case PTR_TYPE_BOXED_I:
      i0 = dec_I(a);
      i1 = as_u(b);
      retval = cmpi(i0,i1,swapped);
      break;
    case PTR_TYPE_BOXED_F:
      f0 = dec_f(a);
      f1 = as_f(b);
      retval = cmpf(f0,f1,swapped);
      break;
    }
  } 
  return retval;
}

bool fundamental_exec(stack *K, VALUE op) {
  bool ret = true;
  UINT nargs;
  UINT tmp;
  UINT result = enc_sym(symrepr_eerror());
  int cmp_res = -1;
  pop_u32(K, &nargs);
  /* for now assume that all of these will take at least one argument */
  nargs = dec_u(nargs);
  if (nargs < 1) {
    push_u32(K, symrepr_nil());
  }
  
  switch (dec_sym(op)) {
  case SYM_CONS: {
    UINT a;
    UINT b;
    pop_u32(K, &a); nargs--;
    pop_u32(K, &b); nargs--;
    result = cons(a,b);
    break;
  }
  case SYM_CAR: {
    pop_u32(K, &tmp); nargs--;
    result = car(tmp);
    break;
  }
  case SYM_CDR: {
    pop_u32(K, &tmp); nargs--;
    result = cdr(tmp);
    break;
  }
  case SYM_LIST: {
    result = enc_sym(symrepr_nil());
    for (UINT i = 1; i <= nargs; i ++) {
      UINT a = enc_sym(symrepr_eerror());
      stack_arg_ix(K, (nargs - i), &a);
      result = cons(a, result);
      if (type_of(result) == VAL_TYPE_SYMBOL)
	break;
    }
    break; 
  }
  case SYM_ADD: {
    UINT sum;
    UINT value;
    pop_u32(K, &sum); nargs--;
    while (nargs > 0) {
      pop_u32(K, &value); nargs--;
      sum = add2(sum, value);
    }
    result = sum;
    break;
  }
  case SYM_SUB: {
    UINT res;
    UINT value;
    pop_u32(K,&res); nargs--;
    if (nargs == 0) {
      res = negate(res);
    } else {
      while (nargs > 0) {
	pop_u32(K, &value); nargs--;
	res = sub2(res, value);
      }
    }
    result = res;
    break;
  }
  case SYM_MUL:
    break;
  case SYM_DIV:
    break;
  case SYM_MOD:
    break;
  case SYM_EQ: {
    UINT a;
    UINT b;
    bool r = true;
    pop_u32(K, &a); nargs--;
    
    while (nargs > 0) {
      pop_u32(K, &b); nargs--;
      r = r && struct_eq(a, b);
    }
    if (r) {
      result = enc_sym(symrepr_true());
    } else {
      result = enc_sym(symrepr_nil());
    }
    break;
  }
  case SYM_NUMEQ:
    cmp_res = 0;
    /* fall through */
  case SYM_GT:
    if (dec_sym(op) == SYM_GT) cmp_res = 1;
    /* fall through */
  case SYM_LT: {
    UINT a;
    UINT b;
    bool r = true;
    bool ok = true;
    pop_u32(K, &a); nargs--;
    if (!is_number(a)) {
      result = enc_sym(symrepr_terror());
      break;
    }
    while (nargs > 0) {
      pop_u32(K, &b); nargs--;
      if (!is_number(b)) {
	ok = false;
	break;
      }	
      r = r && (compare(a, b) == cmp_res);
    }
    if (ok) {
      if (r) {
	result = enc_sym(symrepr_true());
      } else {
	result = enc_sym(symrepr_nil());
      }
    } else {
      result = enc_sym(symrepr_terror());
    }
    break;
  }
  default:
    ret = false;
    break;
  }
  
  /* clean up the stack */
  while (nargs > 0) {
    pop_u32(K, &tmp); nargs--;
  }

  if (ret) push_u32(K,result);
  return ret;
}

