/****************************************************************************/
/*     Sail                                                                 */
/*                                                                          */
/*  Sail and the Sail architecture models here, comprising all files and    */
/*  directories except the ASL-derived Sail code in the aarch64 directory,  */
/*  are subject to the BSD two-clause licence below.                        */
/*                                                                          */
/*  The ASL derived parts of the ARMv8.3 specification in                   */
/*  aarch64/no_vector and aarch64/full are copyright ARM Ltd.               */
/*                                                                          */
/*  Copyright (c) 2013-2021                                                 */
/*    Kathyrn Gray                                                          */
/*    Shaked Flur                                                           */
/*    Stephen Kell                                                          */
/*    Gabriel Kerneis                                                       */
/*    Robert Norton-Wright                                                  */
/*    Christopher Pulte                                                     */
/*    Peter Sewell                                                          */
/*    Alasdair Armstrong                                                    */
/*    Brian Campbell                                                        */
/*    Thomas Bauereiss                                                      */
/*    Anthony Fox                                                           */
/*    Jon French                                                            */
/*    Dominic Mulligan                                                      */
/*    Stephen Kell                                                          */
/*    Mark Wassell                                                          */
/*    Alastair Reid (Arm Ltd)                                               */
/*                                                                          */
/*  All rights reserved.                                                    */
/*                                                                          */
/*  This work was partially supported by EPSRC grant EP/K008528/1 <a        */
/*  href="http://www.cl.cam.ac.uk/users/pes20/rems">REMS: Rigorous          */
/*  Engineering for Mainstream Systems</a>, an ARM iCASE award, EPSRC IAA   */
/*  KTF funding, and donations from Arm.  This project has received         */
/*  funding from the European Research Council (ERC) under the European     */
/*  Union’s Horizon 2020 research and innovation programme (grant           */
/*  agreement No 789108, ELVER).                                            */
/*                                                                          */
/*  This software was developed by SRI International and the University of  */
/*  Cambridge Computer Laboratory (Department of Computer Science and       */
/*  Technology) under DARPA/AFRL contracts FA8650-18-C-7809 ("CIFV")        */
/*  and FA8750-10-C-0237 ("CTSRD").                                         */
/*                                                                          */
/*  Redistribution and use in source and binary forms, with or without      */
/*  modification, are permitted provided that the following conditions      */
/*  are met:                                                                */
/*  1. Redistributions of source code must retain the above copyright       */
/*     notice, this list of conditions and the following disclaimer.        */
/*  2. Redistributions in binary form must reproduce the above copyright    */
/*     notice, this list of conditions and the following disclaimer in      */
/*     the documentation and/or other materials provided with the           */
/*     distribution.                                                        */
/*                                                                          */
/*  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''      */
/*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED       */
/*  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A         */
/*  PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR     */
/*  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,            */
/*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF        */
/*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND     */
/*  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,      */
/*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT      */
/*  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF      */
/*  SUCH DAMAGE.                                                            */
/****************************************************************************/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include<assert.h>
#include<inttypes.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include"sail.h"

#ifdef __cplusplus
extern "C" {
#endif

// zero bits from high index, same semantics as bzhi intrinsic
uint64_t bzhi_u64(uint64_t bits, uint64_t len)
{
  return bits & (UINT64_MAX >> (64 - len));
}

void setup_library(void)
{
  srand(0x0);
}

void cleanup_library(void) {}

bool EQUAL(unit)(const unit a, const unit b)
{
  return true;
}

unit UNDEFINED(unit)(const unit u)
{
  return UNIT;
}

unit skip(const unit u)
{
  return UNIT;
}

/* ***** Sail bit type ***** */

bool eq_bit(const fbits a, const fbits b)
{
  return a == b;
}

/* ***** Sail booleans ***** */

bool EQUAL(bool)(const bool a, const bool b) {
  return a == b;
}

bool UNDEFINED(bool)(const unit u) {
  return false;
}

/* ***** Sail strings ***** */

void CREATE(sail_string)(sail_string *str)
{
  char *istr = (char *) sail_malloc(1 * sizeof(char));
  istr[0] = '\0';
  *str = istr;
}

void RECREATE(sail_string)(sail_string *str)
{
  sail_free(*str);
  char *istr = (char *) sail_malloc(1 * sizeof(char));
  istr[0] = '\0';
  *str = istr;
}

void COPY(sail_string)(sail_string *str1, const_sail_string str2)
{
  size_t len = strlen(str2);
  *str1 = (sail_string)realloc(*str1, len + 1);
  *str1 = strcpy(*str1, str2);
}

void KILL(sail_string)(sail_string *str)
{
  sail_free(*str);
}

void dec_str(sail_string *str, const sail_int n)
{
  sail_free(*str);
  asprintf(str, "%" PRId64, n);
}

void hex_str(sail_string *str, const sail_int n)
{
  sail_free(*str);
  asprintf(str, "0x%" PRIx64, n);
}

void hex_str_upper(sail_string *str, const sail_int n)
{
  sail_free(*str);
  asprintf(str, "0x%" PRIX64, n);
}

bool eq_string(const_sail_string str1, const_sail_string str2)
{
  return strcmp(str1, str2) == 0;
}

bool EQUAL(sail_string)(const_sail_string str1, const_sail_string str2)
{
  return strcmp(str1, str2) == 0;
}

void undefined_string(sail_string *str, const unit u) {}

void concat_str(sail_string *stro, const_sail_string str1, const_sail_string str2)
{
  *stro = (sail_string)realloc(*stro, strlen(str1) + strlen(str2) + 1);
  (*stro)[0] = '\0';
  strcat(*stro, str1);
  strcat(*stro, str2);
}

bool string_startswith(const_sail_string s, const_sail_string prefix)
{
  return strstr(s, prefix) == s;
}

sail_int string_length(const_sail_string s)
{
  return strlen(s);
}

void string_drop(sail_string *dst, const_sail_string s, sail_int ns)
{
  size_t len = strlen(s);
  mach_int n = CREATE_OF(mach_int, sail_int)(ns);
  if (len >= n) {
    *dst = (sail_string)realloc(*dst, (len - n) + 1);
    memcpy(*dst, s + n, len - n);
    (*dst)[len - n] = '\0';
  } else {
    *dst = (sail_string)realloc(*dst, 1);
    **dst = '\0';
  }
}

void string_take(sail_string *dst, const_sail_string s, sail_int ns)
{
  size_t len = strlen(s);
  mach_int n = CREATE_OF(mach_int, sail_int)(ns);
  mach_int to_copy;
  if (len <= n) {
    to_copy = len;
  } else {
    to_copy = n;
  }
  *dst = (sail_string)realloc(*dst, to_copy + 1);
  memcpy(*dst, s, to_copy);
  (*dst)[to_copy] = '\0';
}

/* ***** Sail integers ***** */

uint64_t sail_int_get_ui(const sail_int op)
{
  return (uint64_t)op;
}

bool EQUAL(mach_int)(const mach_int op1, const mach_int op2)
{
  return op1 == op2;
}

/* void COPY(sail_int)(sail_int *rop, const sail_int op) */
/* { */
/*   mpz_set(*rop, op); */
/* } */

/* void CREATE(sail_int)(sail_int *rop) */
/* { */
/*   mpz_init(*rop); */
/* } */

/* void RECREATE(sail_int)(sail_int *rop) */
/* { */
/*   mpz_set_ui(*rop, 0); */
/* } */

/* void KILL(sail_int)(sail_int *rop) */
/* { */
/*   mpz_clear(*rop); */
/* } */

sail_int CREATE_OF(sail_int, mach_int)(const mach_int op)
{
     return (sail_int) op;
}

mach_int CREATE_OF(mach_int, sail_int)(const sail_int op)
{
     if (MACH_INT_MIN <= op && op <= MACH_INT_MAX) {
          return (mach_int) op;
     } else {
          sail_failure("Lost precision when converting from sail integer to machine integer");
          return -1;
     }
}

/* void RECREATE_OF(sail_int, mach_int)(sail_int *rop, mach_int op) */
/* { */
/*   mpz_set_si(*rop, op); */
/* } */

/* void CREATE_OF(sail_int, sail_string)(sail_int *rop, const_sail_string str) */
/* { */
/*   mpz_init_set_str(*rop, str, 10); */
/* } */

/* void CONVERT_OF(sail_int, sail_string)(sail_int *rop, const_sail_string str) */
/* { */
/*   mpz_set_str(*rop, str, 10); */
/* } */

/* void RECREATE_OF(sail_int, sail_string)(mpz_t *rop, const_sail_string str) */
/* { */
/*   mpz_set_str(*rop, str, 10); */
/* } */

mach_int CONVERT_OF(mach_int, sail_int)(const sail_int op)
{
     if (MACH_INT_MIN <= op && op <= MACH_INT_MAX) {
          return (mach_int) op;
     } else {
          sail_failure("Lost precision when converting from sail integer to machine integer");
          return -1;
     }
}

sail_int CONVERT_OF(sail_int, mach_int)(const mach_int op)
{
     return (sail_int) op;
}

bool eq_int(const sail_int op1, const sail_int op2)
{
     return op1 == op2;
}

bool EQUAL(sail_int)(const sail_int op1, const sail_int op2)
{
     return op1 == op2;
}

bool lt(const sail_int op1, const sail_int op2)
{
     return op1 < op2;
}

bool gt(const sail_int op1, const sail_int op2)
{
     return op1 > op2;
}

bool lteq(const sail_int op1, const sail_int op2)
{
     return op1 <= op2;
}

bool gteq(const sail_int op1, const sail_int op2)
{
     return op1 >= op2;
}

sail_int shl_int(const sail_int op1, const sail_int op2)
{
     return op1 << op2;
}

mach_int shl_mach_int(const mach_int op1, const mach_int op2)
{
  return op1 << op2;
}

sail_int shr_int(const sail_int op1, const sail_int op2)
{
     return op1 >> op2;
}

mach_int shr_mach_int(const mach_int op1, const mach_int op2)
{
  return op1 >> op2;
}

sail_int undefined_int(const int n)
{
     return (sail_int) n;
}

sail_int undefined_nat(const unit u)
{
  return (sail_int) 0;
}

sail_int undefined_range(const sail_int lower, const sail_int upper)
{
     return lower;
}

sail_int add_int(const sail_int op1, const sail_int op2)
{
     if ((op2 > 0) && (op1 > SAIL_INT_MAX - op2)) {
          sail_failure("Sail integer addition would overflow");
          return -1;
     } else if ((op2 < 0) && (op1 < SAIL_INT_MIN - op2)) {
          sail_failure("Sail integer addition would underflow");
          return -1;
     } else {
          return op1 + op2;
     }
}

sail_int sub_int(const sail_int op1, const sail_int op2)
{
     if ((op2 < 0) && (op1 > SAIL_INT_MAX + op2)) {
          sail_failure("Sail integer subtraction would overflow");
          return -1;
     } else if ((op2 > 0) && (op1 < SAIL_INT_MIN + op2)) {
          sail_failure("Sail integer subtraction would underflow");
          return -1;
     } else {
          return op1 - op2;
     }
}

sail_int sub_nat(const sail_int op1, const sail_int op2)
{
     sail_int rop = sub_int(op1, op2);
     if (rop < 0) {
          return (sail_int) 0;
     } else {
          return rop;
     }
}

sail_int mult_int(const sail_int op1, const sail_int op2)
{
  // TODO check overflow
  return op1 * op2;

  /* if (op1 > SAIL_INT_MAX / op2) { */
  /*      sail_failure("Sail integer multiplication would overflow"); */
  /*      return -1; */
  /* } else if (op1 < SAIL_INT_MIN / op2) { */
  /*      sail_failure("Sail integer multiplication would underflow"); */
  /*      return -1; */
  /* } else { */
  /*      return op1 * op2; */
  /* } */
}

sail_int ediv_int(const sail_int op1, const sail_int op2)
{
     return op1 / op2;
}

sail_int emod_int(const sail_int op1, const sail_int op2)
{
     return op1 % op2;
}

sail_int tdiv_int(const sail_int op1, const sail_int op2)
{
     return op1 / op2;
}

sail_int tmod_int(const sail_int op1, const sail_int op2)
{
     return op1 % op2;
}

sail_int fdiv_int(const sail_int op1, const sail_int op2)
{
     return op1 / op2;
}

sail_int fmod_int(const sail_int op1, const sail_int op2)
{
     return op1 % op2;
}

sail_int max_int(const sail_int op1, const sail_int op2)
{
     if (op1 < op2) {
          return op2;
     } else {
          return op1;
     }
}

sail_int min_int(const sail_int op1, const sail_int op2)
{
     if (op1 > op2) {
          return op2;
     } else {
          return op1;
     }
}

sail_int neg_int(const sail_int op)
{
     if (op == SAIL_INT_MIN) {
          sail_failure("Sail integer negation would overflow");
          return -1;
     }
     return -op;
}

sail_int abs_int(const sail_int op)
{
     if (op < 0) {
          return neg_int(op);
     } else {
          return op;
     }
}

sail_int pow_int(sail_int base, sail_int exp)
{
     sail_int result = 1;
     while (true)
     {
          if (exp & 1) {
               result *= base;
          }
          exp >>= 1;
          if (!exp) {
               break;
          }
          base *= base;
     }
     return result;
}

sail_int pow2(const sail_int exp)
{
     return pow_int(2, exp);
}

/* ***** Sail bitvectors ***** */

bool EQUAL(fbits)(const fbits op1, const fbits op2)
{
  return op1 == op2;
}

bool EQUAL(ref_fbits)(const fbits *op1, const fbits *op2)
{
  return *op1 == *op2;
}

/* void CREATE(lbits)(lbits *rop) */
/* { */
/*   rop->bits = (mpz_t *)sail_malloc(sizeof(mpz_t)); */
/*   rop->len = 0; */
/*   mpz_init(*rop->bits); */
/* } */

/* void RECREATE(lbits)(lbits *rop) */
/* { */
/*   rop->len = 0; */
/*   mpz_set_ui(*rop->bits, 0); */
/* } */

/* void COPY(lbits)(lbits *rop, const lbits op) */
/* { */
/*   rop->len = op.len; */
/*   mpz_set(*rop->bits, *op.bits); */
/* } */

/* void KILL(lbits)(lbits *rop) */
/* { */
/*   mpz_clear(*rop->bits); */
/*   sail_free(rop->bits); */
/* } */

/* void CREATE_OF(lbits, fbits)(lbits *rop, const uint64_t op, const uint64_t len, const bool direction) */
/* { */
/*   rop->bits = (mpz_t *)sail_malloc(sizeof(mpz_t)); */
/*   rop->len = len; */
/*   mpz_init_set_ui(*rop->bits, op); */
/* } */

/* fbits CREATE_OF(fbits, lbits)(const lbits op, const bool direction) */
/* { */
/*   return mpz_get_ui(*op.bits); */
/* } */

/* sbits CREATE_OF(sbits, lbits)(const lbits op, const bool direction) */
/* { */
/*   sbits rop; */
/*   rop.bits = mpz_get_ui(*op.bits); */
/*   rop.len = op.len; */
/*   return rop; */
/* } */

/* sbits CREATE_OF(sbits, fbits)(const fbits op, const uint64_t len, const bool direction) */
/* { */
/*   sbits rop; */
/*   rop.bits = op; */
/*   rop.len = len; */
/*   return rop; */
/* } */

/* void RECREATE_OF(lbits, fbits)(lbits *rop, const uint64_t op, const uint64_t len, const bool direction) */
/* { */
/*   rop->len = len; */
/*   mpz_set_ui(*rop->bits, op); */
/* } */

/* void CREATE_OF(lbits, sbits)(lbits *rop, const sbits op, const bool direction) */
/* { */
/*   rop->bits = (mpz_t *)sail_malloc(sizeof(mpz_t)); */
/*   rop->len = op.len; */
/*   mpz_init_set_ui(*rop->bits, op.bits); */
/* } */

/* void RECREATE_OF(lbits, sbits)(lbits *rop, const sbits op, const bool direction) */
/* { */
/*   rop->len = op.len; */
/*   mpz_set_ui(*rop->bits, op.bits); */
/* } */

// Bitvector conversions

fbits CONVERT_OF(fbits, lbits)(const lbits op, const bool direction)
{
     return op.bits;
}

fbits CONVERT_OF(fbits, sbits)(const sbits op, const bool direction)
{
  return op.bits;
}

lbits CONVERT_OF(lbits, fbits)(const fbits op, const uint64_t len, const bool order)
{
     lbits rop;
     rop.len = len;
     rop.bits = op;
     return rop;
}

lbits CONVERT_OF(lbits, sbits)(const sbits op, const bool order)
{
     return op;
}

/* sbits CONVERT_OF(sbits, fbits)(const fbits op, const uint64_t len, const bool direction) */
/* { */
/*   sbits rop; */
/*   rop.len = len; */
/*   rop.bits = op; */
/*   return rop; */
/* } */

sbits CONVERT_OF(sbits, lbits)(const lbits op, const bool order)
{
     return op;
}

lbits UNDEFINED(lbits)(const sail_int len)
{
     lbits rop;
     rop.bits = 0;
     rop.len = (uint64_t) len;
     return rop;
}

fbits UNDEFINED(fbits)(const unit u) { return 0; }

sbits undefined_sbits(void)
{
  sbits rop;
  rop.bits = UINT64_C(0);
  rop.len = UINT64_C(0);
  return rop;
}

fbits safe_rshift(const fbits x, const fbits n)
{
  if (n >= 64) {
    return 0ul;
  } else {
    return x >> n;
  }
}

/* void normalize_lbits(lbits *rop) { */
/*   /\* TODO optimisation: keep a set of masks of various sizes handy *\/ */
/*   mpz_set_ui(sail_lib_tmp1, 1); */
/*   mpz_mul_2exp(sail_lib_tmp1, sail_lib_tmp1, rop->len); */
/*   mpz_sub_ui(sail_lib_tmp1, sail_lib_tmp1, 1); */
/*   mpz_and(*rop->bits, *rop->bits, sail_lib_tmp1); */
/* } */

/* void append_64(lbits *rop, const lbits op, const fbits chunk) */
/* { */
/*   rop->len = rop->len + 64ul; */
/*   mpz_mul_2exp(*rop->bits, *op.bits, 64ul); */
/*   mpz_add_ui(*rop->bits, *rop->bits, chunk); */
/* } */

lbits add_bits(const lbits op1, const lbits op2)
{
     lbits rop;
     rop.bits = (op1.bits + op2.bits) & sail_bzhi_u64(UINT64_MAX, op1.len);
     rop.len = op1.len;
     return rop;
}

lbits sub_bits(const lbits op1, const lbits op2)
{
     lbits rop;
     rop.bits = (op1.bits - op2.bits) & sail_bzhi_u64(UINT64_MAX, op1.len);
     rop.len = op1.len;
     return rop;
}

lbits add_bits_int(const lbits op1, const sail_int op2)
{
     lbits rop;
     rop.bits = (op1.bits + ((uint64_t) op2)) & sail_bzhi_u64(UINT64_MAX, op1.len);
     rop.len = op1.len;
     return rop;
}

lbits sub_bits_int(const lbits op1, const sail_int op2)
{
     lbits rop;
     rop.bits = (op1.bits - ((uint64_t) op2)) & sail_bzhi_u64(UINT64_MAX, op1.len);
     rop.len = op1.len;
     return rop;
}

lbits and_bits(const lbits op1, const lbits op2)
{
     lbits rop;
     rop.bits = op1.bits & op2.bits;
     rop.len = op1.len;
     return rop;
}

lbits or_bits(const lbits op1, const lbits op2)
{
     lbits rop;
     rop.bits = op1.bits | op2.bits;
     rop.len = op1.len;
     return rop;
}

lbits xor_bits(const lbits op1, const lbits op2)
{
     lbits rop;
     rop.bits = op1.bits ^ op2.bits;
     rop.len = op1.len;
     return rop;
}

lbits not_bits(const lbits op)
{
     lbits rop;
     rop.bits = (~op.bits) & sail_bzhi_u64(UINT64_MAX, op.len);
     rop.len = op.len;
     return rop;
}

/* void mults_vec(lbits *rop, const lbits op1, const lbits op2) */
/* { */
/*   mpz_t op1_int, op2_int; */
/*   mpz_init(op1_int); */
/*   mpz_init(op2_int); */
/*   sail_signed(&op1_int, op1); */
/*   sail_signed(&op2_int, op2); */
/*   rop->len = op1.len * 2; */
/*   mpz_mul(*rop->bits, op1_int, op2_int); */
/*   normalize_lbits(rop); */
/*   mpz_clear(op1_int); */
/*   mpz_clear(op2_int); */
/* } */

/* void mult_vec(lbits *rop, const lbits op1, const lbits op2) */
/* { */
/*   rop->len = op1.len * 2; */
/*   mpz_mul(*rop->bits, *op1.bits, *op2.bits); */
/*   normalize_lbits(rop); /\* necessary? *\/ */
/* } */


lbits zeros(const sail_int len)
{
     lbits rop;
     rop.bits = 0;
     rop.len = (uint64_t) len;
     return rop;
}

lbits zero_extend(const lbits op, const sail_int len)
{
  lbits rop;
  assert(op.len <= len);
  rop.len = len;
  rop.bits = op.bits;
  return rop;
}

fbits fast_zero_extend(const sbits op, const uint64_t n)
{
  return op.bits;
}

lbits sign_extend(const lbits op, const sail_int len)
{
  lbits rop;
  assert(op.len <= len);
  rop.len = len;
  rop.bits = op.bits;
  if (op.bits & (UINT64_C(1) << (op.len - 1))) {
    for (uint64_t i = len - 1; i >= op.len; i--) {
      rop.bits = rop.bits | (UINT64_C(1) << i);
    }
    return rop;
  } else {
    return rop;
  }
}

fbits fast_sign_extend(const fbits op, const uint64_t n, const uint64_t m)
{
  uint64_t rop = op;
  if (op & (UINT64_C(1) << (n - 1))) {
    for (uint64_t i = m - 1; i >= n; i--) {
      rop = rop | (UINT64_C(1) << i);
    }
    return rop;
  } else {
    return rop;
  }
}

fbits fast_sign_extend2(const sbits op, const uint64_t m)
{
  uint64_t rop = op.bits;
  if (op.bits & (UINT64_C(1) << (op.len - 1))) {
    for (uint64_t i = m - 1; i >= op.len; i--) {
      rop = rop | (UINT64_C(1) << i);
    }
    return rop;
  } else {
    return rop;
  }
}

sail_int length_lbits(const lbits op)
{
  return op.len;
}

/* sail_int count_leading_zeros(const lbits op) */
/* { */
/*   if (mpz_cmp_ui(*op.bits, 0) == 0) { */
/*     mpz_set_ui(*rop, op.len); */
/*   } else { */
/*     size_t bits = mpz_sizeinbase(*op.bits, 2); */
/*     mpz_set_ui(*rop, op.len - bits); */
/*   } */
/* } */

bool eq_bits(const lbits op1, const lbits op2)
{
     return op1.bits == op2.bits;
}

bool EQUAL(lbits)(const lbits op1, const lbits op2)
{
  return eq_bits(op1, op2);
}

bool EQUAL(ref_lbits)(const lbits *op1, const lbits *op2)
{
  return eq_bits(*op1, *op2);
}

bool neq_bits(const lbits op1, const lbits op2)
{
     return op1.bits != op2.bits;
}

lbits vector_subrange_lbits(const lbits op, const sail_int n, const sail_int m)
{
  lbits rop;
  rop.len = n - m + 1;
  rop.bits = op.bits >> m;
  uint64_t mask = 0;
  for (size_t i = 0; i < rop.len; ++i) {
    mask = (mask << 1) + 1;
  }
  rop.bits = rop.bits & mask;
  return rop;
}

/* void vector_subrange_inc_lbits(lbits *rop, */
/* 			       const lbits op, */
/* 			       const sail_int n_mpz, */
/* 			       const sail_int m_mpz) */
/* { */
/*   uint64_t n = mpz_get_ui(n_mpz); */
/*   uint64_t m = mpz_get_ui(m_mpz); */

/*   rop->len = m - (n - 1ul); */
/*   mpz_fdiv_q_2exp(*rop->bits, *op.bits, (op.len - 1) - m); */
/*   normalize_lbits(rop); */
/* } */

lbits sail_truncate(const lbits op, const sail_int len)
{
  lbits rop;
  assert(op.len >= len);
  rop.len = len;
  uint64_t mask = 0;
  for (size_t i = 0; i < len; ++i) {
    mask = mask << 1;
    mask += 1;
  }
  rop.bits = op.bits & mask;
  return rop;
}

/* void sail_truncateLSB(lbits *rop, const lbits op, const sail_int len) */
/* { */
/*   uint64_t rlen = mpz_get_ui(len); */
/*   assert(op.len >= rlen); */
/*   rop->len = rlen; */
/*   // similar to vector_subrange_lbits above -- right shift LSBs away */
/*   mpz_fdiv_q_2exp(*rop->bits, *op.bits, op.len - rlen); */
/*   normalize_lbits(rop); */
/* } */

fbits bitvector_access(const lbits bv, const sail_int n)
{
     return 1 & (bv.bits >> ((uint64_t) n));
}

/* fbits bitvector_access_inc(const lbits op, const sail_int n_mpz) */
/* { */
/*   uint64_t n = mpz_get_ui(n_mpz); */
/*   return (fbits) mpz_tstbit(*op.bits, (op.len - 1) - n); */
/* } */

fbits update_fbits(const fbits op, const uint64_t n, const fbits bit)
{
     if ((bit & 1) == 1) {
          return op | (bit << n);
     } else {
          return op & ~(bit << n);
     }
}

sail_int sail_unsigned(const lbits op)
{
     return (sail_int) op.bits;
}

sail_int sail_signed(const lbits op)
{
  if (op.len == 0) {
    return 0;
  } else {
    uint64_t mask = ((uint64_t) 1) << (op.len - 1);
    if (op.bits & mask) {
      uint64_t mask = 1;
      for (size_t i = 0; i < op.len - 1; ++i) {
        mask = (mask << 1) + 1;
      }
      sail_int rop = (~op.bits & mask) + 1;
      return -rop;
    } else {
      return (sail_int) op.bits;
    }
  }
}

mach_int fast_unsigned(const fbits op)
{
  return (mach_int) op;
}

mach_int fast_signed(const fbits op, const uint64_t n)
{
  if (op & (UINT64_C(1) << (n - 1))) {
    uint64_t rop = op & ~(UINT64_C(1) << (n - 1));
    return (mach_int) (rop - (UINT64_C(1) << (n - 1)));
  } else {
    return (mach_int) op;
  }
}

lbits append(const lbits op1, const lbits op2)
{
     lbits rop;
     rop.bits = (op1.bits << op2.len) | op2.bits;
     rop.len = op1.len + op2.len;
     return rop;
}

sbits append_sf(const sbits op1, const fbits op2, const uint64_t len)
{
  sbits rop;
  rop.bits = (op1.bits << len) | op2;
  rop.len = op1.len + len;
  return rop;
}

sbits append_fs(const fbits op1, const uint64_t len, const sbits op2)
{
  sbits rop;
  rop.bits = (op1 << op2.len) | op2.bits;
  rop.len = len + op2.len;
  return rop;
}

sbits append_ss(const sbits op1, const sbits op2)
{
  sbits rop;
  rop.bits = (op1.bits << op2.len) | op2.bits;
  rop.len = op1.len + op2.len;
  return rop;
}

lbits replicate_bits(const lbits op, const sail_int n)
{
     return op;
}

uint64_t fast_replicate_bits(const uint64_t shift, const uint64_t v, const int64_t times)
{
  uint64_t r = v;
  for (int i = 1; i < times; ++i) {
    r |= (r << shift);
  }
  return r;
}

// Takes a slice of the (two's complement) binary representation of
// integer n, starting at bit start, and of length len. With the
// argument in the following order:
//
// get_slice_int(len, n, start)
//
// For example:
//
// get_slice_int(8, 1680, 4) =
//
//                    11           0
//                    V            V
// get_slice_int(8, 0b0110_1001_0000, 4) = 0b0110_1001
//                    <-------^
//                    (8 bit) 4
//
lbits get_slice_int(const sail_int len, const sail_int n, const sail_int start)
{
  lbits rop;
  rop.len = len;
  rop.bits = 0;

  for (sail_int i =  start; i < start + len; ++i) {
    sail_int mask = ((sail_int) 1) << i;
    rop.bits += ((n & mask) ? (sail_int) 1 : (sail_int) 0) << (i - start);
  }

  return rop;
}

// Set slice uses the same indexing scheme as get_slice_int, but it
// puts a bitvector slice into an integer rather than returning it.
sail_int set_slice_int(const sail_int len, const sail_int n, const sail_int start, const lbits slice)
{
  if (len != slice.len) {
    sail_failure("len != slice.len in set_slice_int");
  }

  if (start + len > sizeof(sail_int)) {
    sail_failure("set_slice_bit: start + len > sizeof(sail_int)");
  }

  sail_int rop = n;
  sail_int mask1 = slice.bits;
  sail_int mask2 = slice.bits;

  mask1 = mask1 << start;
  for (size_t i = 0; i < start; ++i) {
    mask2 = (mask2 << 1) + 1;
  }

  rop = rop | mask1;
  rop = rop & mask2;

  return rop;
}

lbits update_lbits(const lbits op, const sail_int n, const uint64_t bit)
{
  lbits op_copy = op;
  lbits rop;
  rop.bits = op.bits;
  rop.len = op.len;

  uint64_t mask = 1;

  for (size_t i = 0; i < n; ++i) {
    mask = mask << 1;
  }

  if (bit == UINT64_C(0)) {
    rop.bits = (rop.bits & (~mask));
  } else {
    rop.bits = rop.bits | mask;
  }

  return rop;
}

/* void update_lbits_inc(lbits *rop, const lbits op, const sail_int n_mpz, const uint64_t bit) */
/* { */
/*   uint64_t n = mpz_get_ui(n_mpz); */

/*   mpz_set(*rop->bits, *op.bits); */
/*   rop->len = op.len; */

/*   if (bit == UINT64_C(0)) { */
/*     mpz_clrbit(*rop->bits, (op.len - 1) - n); */
/*   } else { */
/*     mpz_setbit(*rop->bits, (op.len - 1) - n); */
/*   } */
/* } */

lbits vector_update_subrange_lbits(const lbits op, const sail_int n, const sail_int m, const lbits slice)
{
  lbits rop;
  rop.bits = op.bits;
  rop.len = op.len;

  for (uint64_t i = 0; i < n - (m - 1ul); i++) {
    uint64_t mask = 1;
    mask = mask << i;
    uint64_t bit = (slice.bits & mask) ? 1 : 0;
    rop = update_lbits(rop, i + m, bit);
  }

  return rop;
}

/* void vector_update_subrange_inc_lbits(lbits *rop, */
/*                                       const lbits op, */
/*                                       const sail_int n_mpz, */
/*                                       const sail_int m_mpz, */
/*                                       const lbits slice) */
/* { */
/*   uint64_t n = mpz_get_ui(n_mpz); */
/*   uint64_t m = mpz_get_ui(m_mpz); */

/*   mpz_set(*rop->bits, *op.bits); */
/*   rop->len = op.len; */

/*   for (uint64_t i = 0; i < m - (n - 1ul); i++) { */
/*     uint64_t out_bit = ((op.len - 1) - m) + i; */
/*     if (mpz_tstbit(*slice.bits, (slice.len - 1) - i)) { */
/*       mpz_setbit(*rop->bits, out_bit); */
/*     } else { */
/*       mpz_clrbit(*rop->bits, out_bit); */
/*     } */
/*   } */
/* } */

fbits fast_update_subrange(const fbits op,
			   const mach_int n,
			   const mach_int m,
			   const fbits slice)
{
  fbits rop = op;
  for (mach_int i = 0; i < n - (m - UINT64_C(1)); i++) {
    uint64_t bit = UINT64_C(1) << ((uint64_t) i);
    if (slice & bit) {
      rop |= (bit << m);
    } else {
      rop &= ~(bit << m);
    }
  }
  return rop;
}

lbits slice(const lbits op, const sail_int start, const sail_int len)
{
     uint64_t l = len;
     lbits rop;
     rop.bits = sail_bzhi_u64(op.bits >> ((uint64_t) start), l);
     rop.len = l;
     return rop;
}

/* void slice_inc(lbits *rop, const lbits op, const sail_int start_mpz, const sail_int len_mpz) */
/* { */
/*   assert(mpz_get_ui(start_mpz) + mpz_get_ui(len_mpz) <= op.len); */
/*   uint64_t start = mpz_get_ui(start_mpz); */
/*   uint64_t len = mpz_get_ui(len_mpz); */

/*   mpz_set_ui(*rop->bits, 0); */
/*   rop->len = len; */

/*   for (uint64_t i = 0; i < len; i++) { */
/*     if (mpz_tstbit(*op.bits, ((op.len - 1) - start) - i)) mpz_setbit(*rop->bits, (rop->len - 1) - i); */
/*   } */
/* } */

sbits sslice(const fbits op, const mach_int start, const mach_int len)
{
  sbits rop;
  rop.bits = bzhi_u64(op >> start, len);
  rop.len = len;
  return rop;
}

lbits set_slice(const sail_int len,
                const sail_int slen,
                const lbits op,
                const sail_int start,
                const lbits slice)
{
     uint64_t s = start;
     uint64_t mask = (1 << (uint64_t) slen) - 1;
     lbits rop;
     rop.len = op.len;
     rop.bits = op.bits;
     rop.bits &= ~(mask << s);
     rop.bits |= slice.bits << s;
     return rop;
}

lbits shift_bits_left(const lbits op1, const lbits op2)
{
  lbits rop;
  rop.len = op1.len;
  rop.bits = op1.bits << op2.bits;
  uint64_t mask = 0;
  for (size_t i = 0; i < op1.len; ++i) {
    mask = (mask << 1) + 1;
  }
  rop.bits = rop.bits & mask;
  return rop;
}

lbits shift_bits_right(const lbits op1, const lbits op2)
{
  lbits rop;
  rop.len = op1.len;
  rop.bits = op1.bits >> op2.bits;
  return rop;
}

/* FIXME */
/* void shift_bits_right_arith(lbits *rop, const lbits op1, const lbits op2) */
/* { */
/*   rop->len = op1.len; */
/*   mp_bitcnt_t shift_amt = mpz_get_ui(*op2.bits); */
/*   mp_bitcnt_t sign_bit = op1.len - 1; */
/*   mpz_fdiv_q_2exp(*rop->bits, *op1.bits, shift_amt); */
/*   if(mpz_tstbit(*op1.bits, sign_bit) != 0) { */
/*     /\* *\/ */
/*     for(; shift_amt > 0; shift_amt--) { */
/*       mpz_setbit(*rop->bits, sign_bit - shift_amt + 1); */
/*     } */
/*   } */
/* } */

/* void arith_shiftr(lbits *rop, const lbits op1, const sail_int op2) */
/* { */
/*   rop->len = op1.len; */
/*   mp_bitcnt_t shift_amt = mpz_get_ui(op2); */
/*   mp_bitcnt_t sign_bit = op1.len - 1; */
/*   mpz_fdiv_q_2exp(*rop->bits, *op1.bits, shift_amt); */
/*   if(mpz_tstbit(*op1.bits, sign_bit) != 0) { */
/*     /\* *\/ */
/*     for(; shift_amt > 0; shift_amt--) { */
/*       mpz_setbit(*rop->bits, sign_bit - shift_amt + 1); */
/*     } */
/*   } */
/* } */

lbits shiftl(const lbits op1, const sail_int op2)
{
  lbits rop;
  rop.bits = op1.bits << op2;
  rop.len = op1.len + op2;
  return rop;
}

lbits shiftr(const lbits op1, const sail_int op2)
{
  lbits rop;
  rop.bits = op1.bits >> op2;
  rop.len = op1.len - op2;
  return rop;
}

lbits reverse_endianness(const lbits op)
{
  assert(0);
}

bool eq_sbits(const sbits op1, const sbits op2)
{
  return op1.bits == op2.bits;
}

bool neq_sbits(const sbits op1, const sbits op2)
{
  return op1.bits != op2.bits;
}

sbits not_sbits(const sbits op)
{
  sbits rop;
  rop.bits = (~op.bits) & bzhi_u64(UINT64_MAX, op.len);
  rop.len = op.len;
  return rop;
}

sbits xor_sbits(const sbits op1, const sbits op2)
{
  sbits rop;
  rop.bits = op1.bits ^ op2.bits;
  rop.len = op1.len;
  return rop;
}

sbits or_sbits(const sbits op1, const sbits op2)
{
  sbits rop;
  rop.bits = op1.bits | op2.bits;
  rop.len = op1.len;
  return rop;
}

sbits and_sbits(const sbits op1, const sbits op2)
{
  sbits rop;
  rop.bits = op1.bits & op2.bits;
  rop.len = op1.len;
  return rop;
}

sbits add_sbits(const sbits op1, const sbits op2)
{
  sbits rop;
  rop.bits = (op1.bits + op2.bits) & bzhi_u64(UINT64_MAX, op1.len);
  rop.len = op1.len;
  return rop;
}

sbits sub_sbits(const sbits op1, const sbits op2)
{
  sbits rop;
  rop.bits = (op1.bits - op2.bits) & bzhi_u64(UINT64_MAX, op1.len);
  rop.len = op1.len;
  return rop;
}

/* ***** Sail Reals ***** */

void CREATE(real)(real *rop) {}

void RECREATE(real)(real *rop) {}

void KILL(real)(real *rop) {}

void COPY(real)(real *rop, const real op) {
  *rop = op;
}

void UNDEFINED(real)(real *rop, unit u)
{
  *rop = 0;
}

void neg_real(real *rop, const real op)
{
  *rop = -op;
}

void mult_real(real *rop, const real op1, const real op2) {
  *rop = op1 * op2;
}

void sub_real(real *rop, const real op1, const real op2)
{
  *rop = op1 - op2;
}

void add_real(real *rop, const real op1, const real op2)
{
  *rop = op1 + op2;
}

void div_real(real *rop, const real op1, const real op2)
{
  *rop = op1 / op2;
}

/* #define SQRT_PRECISION 30 */

/*
 * sqrt_real first checks whether the numerator and denominator are both
 * perfect squares (i.e. their square roots are integers), then it
 * will return the exact square root. If that's not the case we use the
 * Babylonian method to calculate the square root to SQRT_PRECISION decimal
 * places.
 */
/* void sqrt_real(mpq_t *rop, const mpq_t op) */
/* { */
/*   mpq_t tmp; */
/*   mpz_t tmp_z; */
/*   mpq_t p; /\* previous estimate, p *\/ */
/*   mpq_t n; /\* next estimate, n *\/ */

/*   mpq_init(tmp); */
/*   mpz_init(tmp_z); */
/*   mpq_init(p); */
/*   mpq_init(n); */

/*   /\* calculate an initial guess using mpz_sqrt *\/ */
/*   mpz_sqrt(tmp_z, mpq_numref(op)); */
/*   mpq_set_num(p, tmp_z); */
/*   mpz_sqrt(tmp_z, mpq_denref(op)); */
/*   mpq_set_den(p, tmp_z); */

/*   /\* Check if op is a square *\/ */
/*   mpq_mul(tmp, p, p); */
/*   if (mpq_cmp(tmp, op) == 0) { */
/*     mpq_set(*rop, p); */

/*     mpq_clear(tmp); */
/*     mpz_clear(tmp_z); */
/*     mpq_clear(p); */
/*     mpq_clear(n); */
/*     return; */
/*   } */


/*   /\* initialise convergence based on SQRT_PRECISION *\/ */
/*   /\* convergence is the precision (in decimal places) we want to reach as a fraction 1/(10^precision) *\/ */
/*   mpq_t convergence; */
/*   mpq_init(convergence); */
/*   mpz_set_ui(tmp_z, 10); */
/*   mpz_pow_ui(tmp_z, tmp_z, SQRT_PRECISION); */
/*   mpz_set_ui(mpq_numref(convergence), 1); */
/*   mpq_set_den(convergence, tmp_z); */
/*   /\* if op < 1 then we switch to checking relative precision for convergence *\/ */
/*   if (mpq_cmp_ui(op, 1, 1) < 0) { */
/*       mpq_mul(convergence, op, convergence); */
/*   } */

/*   while (true) { */
/*     // n = (p + op / p) / 2 */
/*     mpq_div(tmp, op, p); */
/*     mpq_add(tmp, tmp, p); */
/*     mpq_div_2exp(n, tmp, 1); */

/*     /\* calculate the difference between n and p *\/ */
/*     mpq_sub(tmp, p, n); */
/*     mpq_abs(tmp, tmp); */

/*     /\* if the difference is small enough, return *\/ */
/*     if (mpq_cmp(tmp, convergence) < 0) { */
/*       mpq_set(*rop, n); */
/*       break; */
/*     } */

/*     mpq_swap(n, p); */
/*   } */

/*   mpq_clear(tmp); */
/*   mpz_clear(tmp_z); */
/*   mpq_clear(p); */
/*   mpq_clear(n); */
/*   mpq_clear(convergence); */
/* } */

void abs_real(real *rop, const real op)
{
  if (op < 0) {
    *rop = -op;
  } else {
    *rop = op;
  }
}

sail_int round_up(const real op)
{
  return (sail_int) op;
}

sail_int round_down(const real op)
{
  return (sail_int) op;
}

void to_real(real *rop, const sail_int op)
{
     *rop = (double) op;
}

bool EQUAL(real)(const real op1, const real op2)
{
  return (op1 - op2) < 0.00000001;
}

bool lt_real(const real op1, const real op2)
{
  return op1 < op2;;
}

bool gt_real(const real op1, const real op2)
{
  return op1 > op2;;
}

bool lteq_real(const real op1, const real op2)
{
  return op1 <= op2;
}

bool gteq_real(const real op1, const real op2)
{
  return op1 >= op2;
}

/* void real_power(real *rop, const real base, const sail_int exp) */
/* { */
/*   int64_t exp_si = mpz_get_si(exp); */

/*   mpz_set_ui(mpq_numref(*rop), 1); */
/*   mpz_set_ui(mpq_denref(*rop), 1); */

/*   real b; */
/*   mpq_init(b); */
/*   mpq_set(b, base); */
/*   int64_t pexp = llabs(exp_si); */
/*   while (pexp != 0) { */
/*     // invariant: rop * b^pexp == base^abs(exp) */
/*     if (pexp & 1) { // b^(e+1) = b * b^e */
/*       mpq_mul(*rop, *rop, b); */
/*       pexp -= 1; */
/*     } else { // b^(2e) = (b*b)^e */
/*       mpq_mul(b, b, b); */
/*       pexp >>= 1; */
/*     } */
/*   } */
/*   if (exp_si < 0) { */
/*     mpq_inv(*rop, *rop); */
/*   } */
/*   mpq_clear(b); */
/* } */

/* void CREATE_OF(real, sail_string)(real *rop, const_sail_string op) */
/* { */
/*   int decimal; */
/*   int total; */

/*   mpq_init(*rop); */
/*   gmp_sscanf(op, "%Zd.%n%Zd%n", sail_lib_tmp1, &decimal, sail_lib_tmp2, &total); */

/*   int len = total - decimal; */
/*   mpz_ui_pow_ui(sail_lib_tmp3, 10, len); */
/*   mpz_set(mpq_numref(*rop), sail_lib_tmp2); */
/*   mpz_set(mpq_denref(*rop), sail_lib_tmp3); */
/*   mpq_canonicalize(*rop); */
/*   mpz_set(mpq_numref(sail_lib_tmp_real), sail_lib_tmp1); */
/*   mpz_set_ui(mpq_denref(sail_lib_tmp_real), 1); */
/*   mpq_add(*rop, *rop, sail_lib_tmp_real); */
/* } */

/* void CONVERT_OF(real, sail_string)(real *rop, const_sail_string op) */
/* { */
/*   int decimal; */
/*   int total; */

/*   gmp_sscanf(op, "%Zd.%n%Zd%n", sail_lib_tmp1, &decimal, sail_lib_tmp2, &total); */

/*   int len = total - decimal; */
/*   mpz_ui_pow_ui(sail_lib_tmp3, 10, len); */
/*   mpz_set(mpq_numref(*rop), sail_lib_tmp2); */
/*   mpz_set(mpq_denref(*rop), sail_lib_tmp3); */
/*   mpq_canonicalize(*rop); */
/*   mpz_set(mpq_numref(sail_lib_tmp_real), sail_lib_tmp1); */
/*   mpz_set_ui(mpq_denref(sail_lib_tmp_real), 1); */
/*   mpq_add(*rop, *rop, sail_lib_tmp_real); */
/* } */

/* unit print_real(const_sail_string str, const real op) */
/* { */
/*   gmp_printf("%s%Qd\n", str, op); */
/*   return UNIT; */
/* } */

/* unit prerr_real(const_sail_string str, const real op) */
/* { */
/*   gmp_fprintf(stderr, "%s%Qd\n", str, op); */
/*   return UNIT; */
/* } */

/* void random_real(real *rop, const unit u) */
/* { */
/*   if (rand() & 1) { */
/*     mpz_set_si(mpq_numref(*rop), rand()); */
/*   } else { */
/*     mpz_set_si(mpq_numref(*rop), -rand()); */
/*   } */
/*   mpz_set_si(mpq_denref(*rop), rand()); */
/*   mpq_canonicalize(*rop); */
/* } */

/* ***** Printing functions ***** */

void string_of_int(sail_string *str, const sail_int i)
{
  sail_free(*str);
  asprintf(str, "%li", i);
}

/* asprintf is a GNU extension, but it should exist on BSD */
void string_of_fbits(sail_string *str, const fbits op)
{
  sail_free(*str);
  int bytes = asprintf(str, "0x%" PRIx64, op);
  if (bytes == -1) {
    fprintf(stderr, "Could not print bits 0x%" PRIx64 "\n", op);
  }
}

void string_of_lbits(sail_string *str, const lbits op)
{
  sail_free(*str);
  if ((op.len % 4) == 0) {
    asprintf(str, "0x%0*lX", (int)(op.len / 4), op.bits);
  } else {
    *str = (char *) sail_malloc((op.len + 3) * sizeof(char));
    (*str)[0] = '0';
    (*str)[1] = 'b';
    for (int i = 1; i <= op.len; ++i) {
      (*str)[i + 1] = (op.bits >> (op.len - i) & 0x1) + 0x30;
      /* (*str)[i + 1] = mpz_tstbit(*op.bits, op.len - i) + 0x30; */
    }
    (*str)[op.len + 2] = '\0';
  }
  /* sail_free(*str); */
  /* if ((op.len % 4) == 0) { */
  /*   gmp_asprintf(str, "0x%*0ZX", op.len / 4, *op.bits); */
  /* } else { */
  /*   *str = (char *) sail_malloc((op.len + 3) * sizeof(char)); */
  /*   (*str)[0] = '0'; */
  /*   (*str)[1] = 'b'; */
  /*   for (int i = 1; i <= op.len; ++i) { */
  /*     (*str)[i + 1] = mpz_tstbit(*op.bits, op.len - i) + 0x30; */
  /*   } */
  /*   (*str)[op.len + 2] = '\0'; */
  /* } */
}

void decimal_string_of_fbits(sail_string *str, const fbits op)
{
  sail_free(*str);
  int bytes = asprintf(str, "%" PRId64, op);
  if (bytes == -1) {
    fprintf(stderr, "Could not print bits %" PRId64 "\n", op);
  }
}

void decimal_string_of_lbits(sail_string *str, const lbits op)
{
  sail_free(*str);
  asprintf(str, "%li", op.bits);
}

/* void parse_hex_bits(lbits *res, const mpz_t n, const_sail_string hex) */
/* { */
/*   if (!valid_hex_bits(n, hex)) { */
/*     goto failure; */
/*   } */

/*   mpz_t value; */
/*   mpz_init(value); */
/*   if (mpz_set_str(value, hex + 2, 16) == 0) { */
/*     res->len = mpz_get_ui(n); */
/*     mpz_set(*res->bits, value); */
/*     mpz_clear(value); */
/*     return; */
/*   } */
/*   mpz_clear(value); */

/*   // On failure, we return a zero bitvector of the correct width */
/* failure: */
/*   res->len = mpz_get_ui(n); */
/*   mpz_set_ui(*res->bits, 0); */
/* } */

/* bool valid_hex_bits(const mpz_t n, const_sail_string hex) { */
/*   // The string must be prefixed by '0x' */
/*   if (strncmp(hex, "0x", 2) != 0) { */
/*     return false; */
/*   } */

/*   size_t len = strlen(hex); */

/*   // There must be at least one character after the '0x' */
/*   if (len < 3) { */
/*     return false; */
/*   } */

/*   // Ignore any leading zeros */
/*   int non_zero = 2; */
/*   while (hex[non_zero] == '0' && non_zero < len - 1) { */
/*     non_zero++; */
/*   } */

/*   // Check how many bits we need for the first-non-zero (fnz) character. */
/*   int fnz_width; */
/*   char fnz = hex[non_zero]; */
/*   if (fnz == '0') { */
/*     fnz_width = 0; */
/*   } else if (fnz == '1') { */
/*     fnz_width = 1; */
/*   } else if (fnz >= '2' && fnz <= '3') { */
/*     fnz_width = 2; */
/*   } else if (fnz >= '4' && fnz <= '7') { */
/*     fnz_width = 3; */
/*   } else { */
/*     fnz_width = 4; */
/*   } */

/*   // The width of the hex string is the width of the first non zero, */
/*   // plus 4 times the remaining hex digits */
/*   int hex_width = fnz_width + ((len - (non_zero + 1)) * 4); */
/*   if (mpz_cmp_si(n, hex_width) < 0) { */
/*     return false; */
/*   } */

/*   // All the non-zero characters must be valid hex digits */
/*   for (int i = non_zero; i < len; i++) { */
/*     char c = hex[i]; */
/*     if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) { */
/*       return false; */
/*     } */
/*   } */

/*   return true; */
/* } */

void fprint_bits(const_sail_string pre,
		 const lbits op,
		 const_sail_string post,
		 FILE *stream)
{
  fputs(pre, stream);

  if (op.len % 4 == 0) {
    fputs("0x", stream);
    uint64_t buf = op.bits;

    char *hex = (char *)sail_malloc((op.len / 4) * sizeof(char));

    for (int i = 0; i < op.len / 4; ++i) {
      char c = (char) ((0xF & buf) + 0x30);
      hex[i] = (c < 0x3A) ? c : c + 0x7;
      buf = buf >> 4;
    }

    for (int i = op.len / 4; i > 0; --i) {
      fputc(hex[i - 1], stream);
    }

    sail_free(hex);
  } else {
    fputs("0b", stream);
    for (int i = op.len; i > 0; --i) {
      uint64_t mask = 1;
      mask = mask << (i - 1);
      char set = (mask & op.bits) ? 1 : 0;
      fputc(set + 0x30, stream);
    }
  }

  fputs(post, stream);
}

unit print_bits(const_sail_string str, const lbits op)
{
  fprint_bits(str, op, "\n", stdout);
  return UNIT;
}

unit prerr_bits(const_sail_string str, const lbits op)
{
  fprint_bits(str, op, "\n", stderr);
  return UNIT;
}

unit print(const_sail_string str)
{
  printf("%s", str);
  return UNIT;
}

unit print_endline(const_sail_string str)
{
  printf("%s\n", str);
  return UNIT;
}

unit prerr(const_sail_string str)
{
  fprintf(stderr, "%s", str);
  return UNIT;
}

unit prerr_endline(const_sail_string str)
{
  fprintf(stderr, "%s\n", str);
  return UNIT;
}

unit print_int(const_sail_string str, const sail_int op)
{
  fputs(str, stdout);
  fprintf(stdout, "%li", op);
  putchar('\n');
  return UNIT;
}

unit prerr_int(const_sail_string str, const sail_int op)
{
  fputs(str, stderr);
  fprintf(stderr, "%li", op);
  fputs("\n", stderr);
  return UNIT;
}

unit sail_putchar(const sail_int op)
{
  char c = (char) op;
  putchar(c);
  fflush(stdout);
  return UNIT;
}

sail_int get_time_ns(const unit u)
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec * 1000000000 + t.tv_nsec;
}

// ARM specific optimisations

/* void arm_align(lbits *rop, const lbits x_bv, const sail_int y_mpz) */
/* { */
/*   uint64_t x = mpz_get_ui(*x_bv.bits); */
/*   uint64_t y = mpz_get_ui(y_mpz); */
/*   uint64_t z = y * (x / y); */
/*   mp_bitcnt_t n = x_bv.len; */
/*   mpz_set_ui(*rop->bits, safe_rshift(UINT64_MAX, 64l - (n - 1)) & z); */
/*   rop->len = n; */
/* } */

/* // Monomorphisation */
/* void make_the_value(sail_int *rop, const sail_int op) */
/* { */
/*   mpz_set(*rop, op); */
/* } */

/* void size_itself_int(sail_int *rop, const sail_int op) */
/* { */
/*   mpz_set(*rop, op); */
/* } */

#ifdef __cplusplus
}
#endif
