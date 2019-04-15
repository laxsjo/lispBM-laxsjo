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
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "heap.h"
#include "symrepr.h"
#ifdef VISUALIZE_HEAP
#include "heap_vis.h"
#endif

static cons_t*      heap = NULL;
static UINT         heap_base;
static heap_state_t heap_state;

static VALUE        NIL;

// ref_cell: returns a reference to the cell addressed by bits 3 - 26
//           Assumes user has checked that is_ptr was set
cons_t* ref_cell(VALUE addr) {
  return &heap[dec_ptr(addr)];
  //  return (cons_t*)(heap_base + (addr & PTR_VAL_MASK));
}

static VALUE read_car(cons_t *cell) {
  return cell->car;
}

static VALUE read_cdr(cons_t *cell) {
  return cell->cdr;
}

static void set_car_(cons_t *cell, VALUE v) {
  cell->car = v;
}

static void set_cdr_(cons_t *cell, VALUE v) {
  cell->cdr = v;
}

static void set_gc_mark(cons_t *cell) {
  VALUE cdr = read_cdr(cell);
  set_cdr_(cell, val_set_gc_mark(cdr));
}

static void clr_gc_mark(cons_t *cell) {
  VALUE cdr = read_cdr(cell);
  set_cdr_(cell, val_clr_gc_mark(cdr));
}

static bool get_gc_mark(cons_t* cell) {
  VALUE cdr = read_cdr(cell);
  return val_get_gc_mark(cdr);
}

int generate_freelist(size_t num_cells) {
  size_t i = 0;

  if (!heap) return 0;

  heap_state.freelist = enc_cons_ptr(0);

  cons_t *t;

  // Add all cells to free list
  for (i = 1; i < num_cells; i ++) {
    t = ref_cell(enc_cons_ptr(i-1));
    set_car_(t, NIL);    // all cars in free list are nil
    set_cdr_(t, enc_cons_ptr(i));
  }

  // Replace the incorrect pointer at the last cell.
  t = ref_cell(enc_cons_ptr(num_cells-1));
  set_cdr_(t, NIL);

  return 1;
}

int heap_init(unsigned int num_cells) {

  // retrieve nil symbol value f
  NIL = enc_sym(symrepr_nil());

  // Allocate heap
  heap = (cons_t *)malloc(num_cells * sizeof(cons_t));

  if (!heap) return 0;

  heap_base = (UINT)heap;

  // Initialize heap statistics
  heap_state.heap_base    = heap_base;
  heap_state.heap_bytes   = (unsigned int)(num_cells * sizeof(cons_t));
  heap_state.heap_size    = num_cells;

  heap_state.num_alloc           = 0;
  heap_state.num_alloc_arrays    = 0;
  heap_state.gc_num              = 0;
  heap_state.gc_marked           = 0;
  heap_state.gc_recovered        = 0;
  heap_state.gc_recovered_arrays = 0;

  return generate_freelist(num_cells);
}

void heap_del(void) {
  if (heap)
    free(heap);
}

unsigned int heap_num_free(void) {

  unsigned int count = 0;
  VALUE curr = heap_state.freelist;

  while (type_of(curr) == PTR_TYPE_CONS) {
    curr = read_cdr(ref_cell(curr));
    count++;
  }
  // Prudence.
  if (!(type_of(curr) == VAL_TYPE_SYMBOL) &&
      curr == NIL){
    return 0;
  }
  return count;
}


VALUE heap_allocate_cell(TYPE ptr_type) {

  VALUE res;

  if (!is_ptr(heap_state.freelist)) {
    // Free list not a ptr (should be Symbol NIL)
    if ((type_of(heap_state.freelist) == VAL_TYPE_SYMBOL) &&
	(heap_state.freelist == NIL)) {
      // all is as it should be (but no free cells)
      return enc_sym(symrepr_merror());
    } else {
      printf("BROKEN HEAP %"PRIx32"\n", type_of(heap_state.freelist));
      // something is most likely very wrong
      //printf("heap_allocate_cell Error\n");
      return enc_sym(symrepr_merror());
    }
  }

  // it is a ptr replace freelist with cdr of freelist;
  res = heap_state.freelist;

  if (type_of(res) != PTR_TYPE_CONS) {
    printf("ERROR: freelist is corrupt\n");
  }

  heap_state.freelist = cdr(heap_state.freelist);

  heap_state.num_alloc++;

  // set some ok initial values (nil . nil)
  set_car_(ref_cell(res), NIL);
  set_cdr_(ref_cell(res), NIL);

  // clear GC bit on allocated cell
  clr_gc_mark(ref_cell(res));

  res = res | ptr_type;
  return res;
}

unsigned int heap_num_allocated(void) {
  return heap_state.num_alloc;
}
unsigned int heap_size() {
  return heap_state.heap_size;
}

unsigned int heap_size_bytes(void) {
  return heap_state.heap_bytes;
}

void heap_get_state(heap_state_t *res) {
  res->heap_base           = heap_state.heap_base;
  res->freelist            = heap_state.freelist;
  res->heap_size           = heap_state.heap_size;
  res->heap_bytes          = heap_state.heap_bytes;
  res->num_alloc           = heap_state.num_alloc;
  res->num_alloc_arrays    = heap_state.num_alloc_arrays;
  res->gc_num              = heap_state.gc_num;
  res->gc_marked           = heap_state.gc_marked;
  res->gc_recovered        = heap_state.gc_recovered;
  res->gc_recovered_arrays = heap_state.gc_recovered_arrays;
}

// Recursive implementation can exhaust stack!
int gc_mark_phase(VALUE env) {

  if (!is_ptr(env)) {
      return 1; // Nothing to mark here
  }

  if (get_gc_mark(ref_cell(env))) {
    return 1; // Circular object on heap, or visited..
  }

  // There is at least a pointer to one cell here. Mark it and recurse over  car and cdr 
  heap_state.gc_marked ++;

  set_gc_mark(ref_cell(env));



  VALUE car_env = car(env);
  VALUE cdr_env = cdr(env);
  VALUE t_car   = type_of(car_env);
  VALUE t_cdr   = type_of(cdr_env);

  if (t_car == PTR_TYPE_I32 ||
      t_car == PTR_TYPE_U32 ||
      t_car == PTR_TYPE_F32 ||
      t_car == PTR_TYPE_ARRAY) {
    set_gc_mark(ref_cell(car_env));
    return 1;
  }

  if (t_cdr == PTR_TYPE_I32 ||
      t_cdr == PTR_TYPE_U32 ||
      t_cdr == PTR_TYPE_F32 ||
      t_cdr == PTR_TYPE_ARRAY) {
    set_gc_mark(ref_cell(cdr_env));
    return 1;
  }

  int res = 1;
  if (is_ptr(car(env)) && ptr_type(car(env)) == PTR_TYPE_CONS)
    res &= gc_mark_phase(car(env));
  if (is_ptr(cdr(env)) && ptr_type(cdr(env)) == PTR_TYPE_CONS)
    res &= gc_mark_phase(cdr(env));

  return res;
}

// The free list should be a "proper list"
// Using a while loop to traverse over the cdrs
int gc_mark_freelist() {

  VALUE curr;
  cons_t *t;
  VALUE fl = heap_state.freelist;

  if (!is_ptr(fl)) {
    if (val_type(fl) == VAL_TYPE_SYMBOL &&
	fl == NIL){
      return 1; // Nothing to mark here
    } else {
      printf(" ERROR CASE! %"PRIx32" \n", fl);
      return 0;
    }
  }

  curr = fl;
  while (is_ptr(curr)){
     t = ref_cell(curr);
     set_gc_mark(t);
     curr = read_cdr(t);

     heap_state.gc_marked ++;
  }

  return 1;
}

int gc_mark_aux(UINT *aux_data, unsigned int aux_size) {

  for (unsigned int i = 0; i < aux_size; i ++) {
    if (is_ptr(aux_data[i])) {

      TYPE pt_t = ptr_type(aux_data[i]);
      UINT pt_v = dec_ptr(aux_data[i]);

      if ( (pt_t == PTR_TYPE_CONS ||
	    pt_t == PTR_TYPE_I32 ||
	    pt_t == PTR_TYPE_U32 ||
	    pt_t == PTR_TYPE_F32 ||
	    pt_t == PTR_TYPE_ARRAY ||
	    pt_t == PTR_TYPE_REF ||
	    pt_t == PTR_TYPE_STREAM) &&
	   pt_v < heap_state.heap_size) {

	gc_mark_phase(aux_data[i]);
      }
    }
  }

  return 1;
}


// Sweep moves non-marked heap objects to the free list.
int gc_sweep_phase(void) {

  unsigned int i = 0;
  cons_t *heap = (cons_t *)heap_base;

  for (i = 0; i < heap_state.heap_size; i ++) {
    if ( !get_gc_mark(&heap[i])){

      // Check if this cell is a pointer to an array
      // and free it.

      // TODO: Maybe also has to check for boxed values
      //
      if (type_of(heap[i].cdr) == VAL_TYPE_SYMBOL &&
	  dec_sym(heap[i].cdr) == SPECIAL_SYM_ARRAY) {
	array_t *arr = (array_t*)heap[i].car;
	switch(arr->elt_type) {
	case VAL_TYPE_CHAR:
	  if (arr->data.c) free(arr->data.c);
	  break;
	case VAL_TYPE_I28:
	case PTR_TYPE_I32:
	  if (arr->data.i32) free(arr->data.i32);
	  break;
	case VAL_TYPE_U28:
	case PTR_TYPE_U32:
	case VAL_TYPE_SYMBOL:
	  if (arr->data.u32) free(arr->data.u32);
	  break;
	case PTR_TYPE_F32:
	  if (arr->data.f) free(arr->data.f);
	  break;
	default:
	  return 0; // Error case: unrecognized element type.
	}
	free(arr);
	heap_state.gc_recovered_arrays++;
      }

      // create pointer to use as new freelist
      UINT addr = enc_cons_ptr(i);

      // Clear the "freed" cell.
      heap[i].car = NIL;
      heap[i].cdr = heap_state.freelist;
      heap_state.freelist = addr;

      heap_state.num_alloc --;
      heap_state.gc_recovered ++;
    }
    clr_gc_mark(&heap[i]);
  }
  return 1;
}

int heap_perform_gc(VALUE env) {
  heap_state.gc_num ++;
  heap_state.gc_recovered = 0;
  heap_state.gc_marked = 0;

  gc_mark_freelist();
  gc_mark_phase(env);
  return gc_sweep_phase();
}

int heap_perform_gc_aux(VALUE env, VALUE env2, VALUE exp, VALUE exp2, UINT *aux_data, unsigned int aux_size) {
  heap_state.gc_num ++;
  heap_state.gc_recovered = 0;
  heap_state.gc_marked = 0;

  gc_mark_freelist();
  gc_mark_phase(exp);
  gc_mark_phase(exp2);
  gc_mark_phase(env);
  gc_mark_phase(env2);
  gc_mark_aux(aux_data, aux_size);

#ifdef VISUALIZE_HEAP
  heap_vis_gen_image();
#endif

  return gc_sweep_phase();
}


// construct, alter and break apart
VALUE cons(VALUE car, VALUE cdr) {
  VALUE addr = heap_allocate_cell(PTR_TYPE_CONS);
  if ( is_ptr(addr)) {
    set_car_(ref_cell(addr), car);
    set_cdr_(ref_cell(addr), cdr);
  }

  // heap_allocate_cell returns NIL if out of heap.
  return addr;
}

VALUE car(VALUE c){

  if (type_of(c) == VAL_TYPE_SYMBOL &&
      c == NIL) {
    return c; // if nil, return nil.
  }

  if (is_ptr(c) ){
    cons_t *cell = ref_cell(c);
    return read_car(cell);
  }
  return enc_sym(symrepr_terror());
}

VALUE cdr(VALUE c){

  if (type_of(c) == VAL_TYPE_SYMBOL &&
      c == NIL) {
    return c; // if nil, return nil.
  }

  if (type_of(c) == PTR_TYPE_CONS) {
    cons_t *cell = ref_cell(c);
    return read_cdr(cell);
  }
  return enc_sym(symrepr_terror());
}

void set_car(VALUE c, VALUE v) {
  if (is_ptr(c) && ptr_type(c) == PTR_TYPE_CONS) {
    cons_t *cell = ref_cell(c);
    set_car_(cell,v);
  }
}

void set_cdr(VALUE c, VALUE v) {
  if (type_of(c) == PTR_TYPE_CONS){
    cons_t *cell = ref_cell(c);
    set_cdr_(cell,v);
  }
}

/* calculate length of a proper list */
unsigned int length(VALUE c) {
  unsigned int len = 0;

  while (type_of(c) == PTR_TYPE_CONS){
    len ++;
    c = cdr(c);
  }
  return len;
}

/* reverse a proper list */
VALUE reverse(VALUE list) {
  if (type_of(list) == VAL_TYPE_SYMBOL &&
      list == NIL) {
    return list;
  }

  VALUE curr = list;

  VALUE new_list = NIL;
  while (type_of(curr) == PTR_TYPE_CONS) {

    new_list = cons(car(curr), new_list);
    if (type_of(new_list) == VAL_TYPE_SYMBOL) {
      return enc_sym(symrepr_merror());
    }
    curr = cdr(curr);
  }
  return new_list;
}




////////////////////////////////////////////////////////////
// ARRAY, REF and Stream functionality
////////////////////////////////////////////////////////////

// Arrays are part of the heap module because their lifespan is managed
// by the garbage collector. The data in the array is not stored
// in the "heap of cons cells".
int heap_allocate_array(VALUE *res, unsigned int size, TYPE type){

  array_t *array = malloc(sizeof(array_t));
  // allocating a cell that will, to start with, be a cons cell.
  VALUE cell  = heap_allocate_cell(PTR_TYPE_CONS);

  switch(type) {
  case PTR_TYPE_I32: // array of I32
    array->data.i32 = (INT*)malloc(size * sizeof(INT));
    if (array->data.i32 == NULL) return 0;
    break;
  case PTR_TYPE_U32: // array of U32
    array->data.u32 = (UINT*)malloc(size * sizeof(UINT));
    if (array->data.u32 == NULL) return 0;
    break;
  case PTR_TYPE_F32: // array of Float
    array->data.f = (float*)malloc(size * sizeof(float));
    if (array->data.f == NULL) return 0;
    break;
  case VAL_TYPE_CHAR: // Array of Char
    array->data.c = (char*)malloc(size * sizeof(char));
    if (array->data.c == NULL) return 0;
    break;
  case VAL_TYPE_I28:
    array->data.i32 = (INT*)malloc(size * sizeof(INT));
    break;
  case VAL_TYPE_U28:
    array->data.u32 = (UINT*)malloc(size * sizeof(UINT));
    break;
  case VAL_TYPE_SYMBOL:
    array->data.u32 = (UINT*)malloc(size * sizeof(UINT));
    break;
  default:
    *res = NIL;
    return 0;
  }

  array->elt_type = type;
  array->size = size;

  set_car(cell, (UINT)array);
  set_cdr(cell, enc_sym(SPECIAL_SYM_ARRAY));

  cell = cell | PTR_TYPE_ARRAY;

  *res = cell;

  heap_state.num_alloc_arrays ++;

  return 1;
}
