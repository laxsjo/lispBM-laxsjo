/*
    Copyright 2018 Joel Svensson	svenssonjoel@yahoo.se

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

#include <stdio.h>

#include "print.h"
#include "heap.h"
#include "symrepr.h"


int is_closure(uint32_t t) {

  uint32_t head = car(t);
  
  if (!IS_PTR(head)  && VAL_TYPE(head) == VAL_TYPE_SYMBOL &&
      DEC_SYM(car) == symrepr_closure()) {
    return 1;
  }
  return 0; 
}

int simple_print_env(uint32_t env) {
  uint32_t curr = env;

  uint32_t a;
  uint32_t b;
  printf("(");
  while (IS_PTR(curr) && PTR_TYPE(curr) == PTR_TYPE_CONS) {
    uint32_t head = car(curr);
    if (IS_PTR(head)) {
      a = car(car(head));
      b = cdr(car(head));
      printf("(");
      simple_print(a); printf(" . ");
      if (is_closure(b)) 
	printf("CLOSURE");
      else simple_print(b);
      printf(") ");
    }
    curr = cdr(curr);
  }
  printf(")");
}


int simple_print_lambda(uint32_t t) {

  uint32_t lam = car(t);
  uint32_t vars = car(cdr(t));
  uint32_t exp = car(cdr(cdr(t)));
  uint32_t env = car(cdr(cdr(cdr(t))));
  
  printf("(");
  simple_print(lam); printf(" ");
  simple_print(vars); printf(" ");
  simple_print(exp); printf(" ");
  simple_print_env(env); printf(" ");
}


int simple_print(uint32_t t){

  char *str_ptr;
  
  if (IS_PTR(t) & PTR_TYPE(t) == PTR_TYPE_CONS) {
    // TODO: Switch on the type of object pointed to.

    uint32_t car_val = car(t);

    if (DEC_SYM(car_val) == symrepr_lambda()) {
      simple_print_lambda(t);
    } else if ((t & PTR_TYPE_MASK) == PTR_TYPE_CONS) {
      printf("(");
      simple_print(car(t));
      printf(" "); 
      simple_print(cdr(t));
      printf(")");			
    }
    return 1; 
  }

  if (IS_PTR(t) && PTR_TYPE(t) == PTR_TYPE_F32) {
    uint32_t uv = car(t);
    float v = *(float*)(&uv);
    printf("{%f}", v); 
    return 1; 
  }

  if (IS_PTR(t) && PTR_TYPE(t) == PTR_TYPE_U32) {
    uint32_t v = car(t);
    printf("{%u}", v); 
    return 1; 
  }

  if (IS_PTR(t) && PTR_TYPE(t) == PTR_TYPE_I32) {
    int32_t v = (int32_t)car(t);
    printf("{%d}", v); 
    return 1; 
  }

  if (IS_PTR(t) && PTR_TYPE(t) == PTR_TYPE_ARRAY) {
    array_t *array = (array_t *)car(t);
    switch (array->elt_type){
    case VAL_TYPE_CHAR:
      printf("\"%s\"", array->data.c);
      break;
    default:
      printf("Array type not supported\n"); 
      break; 
    }
  }

  if (!IS_PTR(t)) { // Value, symbol 
    switch (t & VAL_TYPE_MASK) {
    case VAL_TYPE_SYMBOL:
      str_ptr = symrepr_lookup_name(DEC_SYM(t));
      if (str_ptr == NULL) {
	printf("Error: Symbol not in table %u\n", DEC_SYM(t));
      } else {  
	printf("%s", str_ptr);
      }
      break;
    case VAL_TYPE_I28:
      printf("%d", DEC_I28(t)); 
      break;
    case VAL_TYPE_U28:
      printf("%u", DEC_U28(t));
      break;
    case VAL_TYPE_CHAR: // dont yet have syntax for Char and String
      printf("#\\%c", DEC_CHAR(t));
      break;
    default:
      printf("simple_print: Error\n");
      return 0; 
    }
  }
  return 1;
}


/*
int simple_print(cons_t *t) {

  cons_t *curr = t; 

  printf("("); 
  
  switch (GET_CAR_TYPE(curr->type)) {
  case NIL:
    printf (" () ");
    break;
  case INTEGER:
    printf(" %d ", curr->car.i); 
    break;
  case FLOAT:
    printf(" %f ", curr->car.f); 
    break;
    
  case SYMBOL:
    printf(" %d ", curr->car.s); 
      break;
      
  case POINTER:
    simple_print(curr->car.cons);
      break;
  }

  switch (GET_CDR_TYPE(curr->type)) {
  case NIL:
    printf("()");
    break;
  case INTEGER:
    printf(" %d ", curr->cdr.i); 
    break;
  case FLOAT:
    printf(" %f ", curr->cdr.f); 
    break;
    
  case SYMBOL:
    printf(" %d ", curr->cdr.s); 
      break;
      
  case POINTER:
    simple_print(curr->cdr.cons);
      break;
  }

  printf(")"); 

  return 1; // TODO: ERROR CHECKING 
}
*/
