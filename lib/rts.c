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

#include <string.h>
#include <getopt.h>
#include <inttypes.h>
#include <sys/types.h>

#include "sail.h"
#include "rts.h"
#include "elf.h"

#ifdef __cplusplus
extern "C" {
#endif

static uint64_t g_elf_entry;
uint64_t g_cycle_count = 0;
static uint64_t g_cycle_limit;

extern void model_pre_exit();

unit sail_exit(unit u)
{
  model_pre_exit();
  exit(EXIT_SUCCESS);
  return UNIT;
}

static uint64_t g_verbosity = 0;

fbits sail_get_verbosity(const unit u)
{
  return g_verbosity;
}

bool g_sleeping = false;

unit sleep_request(const unit u)
{
  g_sleeping = true;
  return UNIT;
}

unit wakeup_request(const unit u)
{
  g_sleeping = false;
  return UNIT;
}

bool sleeping(const unit u)
{
    return g_sleeping;
}

/* ***** Sail memory builtins ***** */

struct memory_buffer sail_memory;
struct memory_buffer sail_tags;

void write_mem(uint64_t address, uint64_t byte)
{
  sail_memory.buffer[address] = byte;
  sail_memory.mask[address] = true;
}

uint64_t read_mem(uint64_t address)
{
  return sail_memory.buffer[address];
}

unit write_tag_bool(const uint64_t address, const bool tag)
{
  sail_tags.buffer[address] = tag;
  sail_tags.mask[address] = true;
  return UNIT;
}

unit emulator_write_tag(const uint64_t addr_size, const sbits addr, const bool tag)
{
  write_tag_bool(addr.bits, tag);
  return UNIT;
}

bool read_tag_bool(const uint64_t address)
{
  return sail_tags.buffer[address];
}

bool emulator_read_tag(const uint64_t addr_size, const sbits addr)
{
  return read_tag_bool(addr.bits);
}

void kill_mem()
{

}

// ***** Memory builtins *****

bool write_ram(const sail_int addr_size,     // Either 32 or 64
	       const sail_int data_size_mpz, // Number of bytes
	       const lbits hex_ram,       // Currently unused
	       const lbits addr_bv,
	       const lbits data)
{
  uint64_t addr = addr_bv.bits;
  uint64_t data_size = data_size_mpz;

  sail_int buf = data.bits;

  uint64_t byte;
  for(uint64_t i = 0; i < data_size; ++i) {
    // Take the 8 low bits of buf and write to addr.
    byte = buf & 0xFF;
    write_mem(addr + i, byte);

    // Then shift buf 8 bits right.
    buf = buf >> 8;
  }

  return true;
}

sbits fast_read_ram(const int64_t data_size,
		    const uint64_t addr)
{
  uint64_t r = 0;

  uint64_t byte;
  for(uint64_t i = (uint64_t) data_size; i > 0; --i) {
    byte = read_mem(addr + (i - 1));
    r = r << 8;
    r = r + byte;
  }
  sbits res = {.len = (uint64_t)data_size * 8, .bits = r };
  return res;
}

void read_ram(lbits *data,
	      const sail_int addr_size,
	      const sail_int data_size_mpz,
	      const lbits hex_ram,
	      const lbits addr_bv)
{
  uint64_t addr = addr_bv.bits;
  uint64_t data_size = data_size_mpz;

  data->bits = 0;
  data->len = data_size * 8;

  sail_int byte = 0;
  for(uint64_t i = data_size; i > 0; --i) {
    byte = read_mem(addr + (i - 1));
    data->bits = data->bits << 8;
    data->bits += byte;
  }
}

lbits platform_read_mem(const int read_kind, 
                        const int64_t addr_size,
                        const sbits addr, 
                        const sail_int n) 
{
  if (n <= 8) {
    return fast_read_ram(n, addr.bits);
  } else {
    lbits data;
    read_ram(&data, addr_size, n, (lbits){.len=0, .bits=0}, addr);
    return data;
  }
}

unit platform_write_mem_ea(const int write_kind,
                           const int64_t addr_size,
                           const sbits addr,
                           const sail_int n)
{
    return UNIT;
}

bool platform_write_mem(const int write_kind,
                        const int64_t addr_size,
                        const sbits addr,
                        const sail_int n,
                        const lbits data)
{
  return write_ram(addr_size, n, (lbits){.len=0, .bits=0}, addr, data);
}

bool platform_excl_res(const unit unit)
{
    return true;
}

unit platform_barrier()
{
    return UNIT;
}

lbits emulator_read_mem(const int64_t addr_size, 
                        const sbits addr,
                        const sail_int n) 
{
  return platform_read_mem(0, addr_size, addr, n);
}

lbits emulator_read_mem_ifetch(const int64_t addr_size, 
                               const sbits addr,
                               const sail_int n) 
{
  return platform_read_mem(0, addr_size, addr, n);
}

lbits emulator_read_mem_exclusive(const int64_t addr_size, 
                                  const sbits addr,
                                  const sail_int n) 
{
  return platform_read_mem(0, addr_size, addr, n);
}

bool emulator_write_mem(const int64_t addr_size,
                        const sbits addr,
                        const sail_int n,
                        const lbits data)
{
  return platform_write_mem(0, addr_size, addr, n, data);
}

bool emulator_write_mem_exclusive(const int64_t addr_size,
                                  const sbits addr,
                                  const sail_int n,
                                  const lbits data)
{
  return platform_write_mem(0, addr_size, addr, n, data);
}

unit load_raw(fbits addr, const_sail_string file)
{
  FILE *fp = fopen(file, "r");

  if (!fp) {
    fprintf(stderr, "[Sail] Raw file %s could not be loaded\n", file);
    exit(EXIT_FAILURE);
  }

  uint64_t byte;
  while ((byte = (uint64_t)fgetc(fp)) != EOF) {
    write_mem(addr, byte);
    addr++;
  }

  return UNIT;
}

void load_image(char *file)
{
  FILE *fp = fopen(file, "r");

  if (!fp) {
    fprintf(stderr, "[Sail] Image file %s could not be loaded\n", file);
    exit(EXIT_FAILURE);
  }

  char *addr = NULL;
  char *data = NULL;
  size_t len = 0;

  while (true) {
    ssize_t addr_len = getline(&addr, &len, fp);
    if (addr_len == -1) break;
    ssize_t data_len = getline(&data, &len, fp);
    if (data_len == -1) break;

    if (!strcmp(addr, "elf_entry\n")) {
      if (sscanf(data, "%" PRIu64 "\n", &g_elf_entry) != 1) {
	fprintf(stderr, "[Sail] Failed to parse elf_entry\n");
        exit(EXIT_FAILURE);
      };
      fprintf(stderr, "[Sail] Elf entry point: %" PRIx64 "\n", g_elf_entry);
    } else {
      write_mem((uint64_t) atoll(addr), (uint64_t) atoll(data));
    }
  }

  free(addr);
  free(data);
  fclose(fp);
}

// ***** Tracing support *****

static int64_t g_trace_depth;
//static int64_t g_trace_max_depth;
static bool g_trace_enabled;

unit enable_tracing(const unit u)
{
  g_trace_depth = 0;
  g_trace_enabled = true;
  return UNIT;
}

unit disable_tracing(const unit u)
{
  g_trace_depth = 0;
  g_trace_enabled = false;
  return UNIT;
}

bool is_tracing(const unit u)
{
  return g_trace_enabled;
}

void trace_fbits(const fbits x) {
  if (g_trace_enabled) fprintf(stderr, "0x%" PRIx64, x);
}

void trace_unit(const unit u) {
  if (g_trace_enabled) fputs("()", stderr);
}

void trace_sail_string(const_sail_string str) {
  if (g_trace_enabled) fputs(str, stderr);
}

void trace_sail_int(const sail_int op) {
  // if (g_trace_enabled) mpz_out_str(stderr, 10, op);
}

void trace_lbits(const lbits op) {
  // if (g_trace_enabled) fprint_bits("", op, "", stderr);
}

void trace_bool(const bool b) {
  if (g_trace_enabled) {
    if (b) {
      fprintf(stderr, "true");
    } else {
      fprintf(stderr, "false");
    }
  }
}

void trace_unknown(void) {
  if (g_trace_enabled) fputs("?", stderr);
}

void trace_argsep(void) {
  if (g_trace_enabled) fputs(", ", stderr);
}

void trace_argend(void) {
  if (g_trace_enabled) fputs(")\n", stderr);
}

void trace_retend(void) {
  if (g_trace_enabled) fputs("\n", stderr);
}

void trace_start(char *name)
{
  if (g_trace_enabled) {
    fprintf(stderr, "[TRACE] ");
    for (int64_t i = 0; i < g_trace_depth; ++i) {
      fprintf(stderr, "%s", "|   ");
    }
    fprintf(stderr, "%s(", name);
    g_trace_depth++;
  }
}

void trace_end(void)
{
  if (g_trace_enabled) {
    fprintf(stderr, "[TRACE] ");
    for (int64_t i = 0; i < g_trace_depth; ++i) {
      fprintf(stderr, "%s", "|   ");
    }
    g_trace_depth--;
  }
}

/* ***** ELF functions ***** */

sail_int elf_entry(const unit u)
{
  return g_elf_entry;
}

sail_int elf_tohost(const unit u)
{
  return 0x0ul;
}

/* ***** Cycle limit ***** */

/* NB Also increments cycle_count */
bool cycle_limit_reached(const unit u)
{
  return ++g_cycle_count >= g_cycle_limit && g_cycle_limit != 0;
}

unit cycle_count(const unit u)
{
  if (cycle_limit_reached(UNIT)) {
    printf("\n[Sail] TIMEOUT: exceeded %" PRId64 " cycles\n", g_cycle_limit);
    exit(EXIT_SUCCESS);
  }
  return UNIT;
}

void get_cycle_count(sail_int *rop, const unit u)
{
    *rop = g_cycle_count;
}

/* ***** Argument Parsing ***** */

static struct option options[] = {
  {"binary",     required_argument, 0, 'b'},
  {"cyclelimit", required_argument, 0, 'l'},
  {"config",     required_argument, 0, 'C'},
  {"elf",        required_argument, 0, 'e'},
  {"entry",      required_argument, 0, 'n'},
  {"image",      required_argument, 0, 'i'},
  {"verbosity",  required_argument, 0, 'v'},
  {"help",       no_argument,       0, 'h'},
  {0, 0, 0, 0}
};

static void print_usage()
{
  struct option *opt = options;
  while (opt->name) {
    printf("\t -%c\t %s\n", (char)opt->val, opt->name);
    opt++;
  }
  exit(EXIT_SUCCESS);
}

int process_arguments(int argc, char *argv[])
{
  int c;
  bool     elf_entry_set = false;
  uint64_t elf_entry;

  while (true) {
    int option_index = 0;
    c = getopt_long(argc, argv, "e:n:i:b:l:C:v:h", options, &option_index);

    if (c == -1) break;

    switch (c) {
    case 'C': {
        char arg[100];
        uint64_t value;
        if (sscanf(optarg, "%99[a-zA-Z0-9_-.]=0x%" PRIx64, arg, &value) == 2) {
            // do nothing
        } else if (sscanf(optarg, "%99[a-zA-Z0-9_-.]=%" PRId64, arg, &value) == 2) {
            // do nothing
        } else {
          fprintf(stderr, "Could not parse argument %s\n", optarg);
#ifdef HAVE_SETCONFIG
          z__ListConfig(UNIT);
#endif
          return -1;
        };
#ifdef HAVE_SETCONFIG
        mpz_t s_value;
        mpz_init_set_ui(s_value, value);
        z__SetConfig(arg, s_value);
        mpz_clear(s_value);
#else
        fprintf(stderr, "Ignoring flag -C %s", optarg);
#endif
      }
      break;

    case 'b': ;
      uint64_t addr;
      char *cp, *file;

      addr = strtoull(optarg, &cp, 0);
      if (cp == optarg || cp[0] != ',' || cp[1] == '\0') {
	fprintf(stderr, "Could not parse argument %s\n", optarg);
	return -1;
      };
      file = cp + 1;

      load_raw(addr, file);
      break;

    case 'i':
      load_image(optarg);
      break;

    case 'e':
      load_elf(optarg, NULL, &g_elf_entry);
      break;

    case 'n':
      if (!sscanf(optarg, "0x%" PRIx64, &elf_entry)) {
	fprintf(stderr, "Could not parse address %s\n", optarg);
	return -1;
      }
      elf_entry_set = true;
      break;

    case 'l':
      if (!sscanf(optarg, "%" PRId64, &g_cycle_limit)) {
	fprintf(stderr, "Could not parse cycle limit %s\n", optarg);
	return -1;
      }
      break;

    case 'v':
      if (!sscanf(optarg, "0x%" PRIx64, &g_verbosity)) {
       fprintf(stderr, "Could not parse verbosity flags %s\n", optarg);
       return -1;
      }
      break;

    case 'h':
      print_usage();
      break;

    default:
      fprintf(stderr, "Unrecognized option %s\n", optarg);
      print_usage();
      return -1;
    }
  }

  // assignment to g_elf_entry is deferred until the end of file so that an
  // explicit command line flag will override the address read from the ELF
  // file.
  if (elf_entry_set) {
      g_elf_entry = elf_entry;
  }

  return 0;
}

/* ***** Setup and cleanup functions for RTS ***** */

void setup_rts(void)
{
  srand(0x0);
  disable_tracing(UNIT);
}

void cleanup_rts(void)
{
  kill_mem();
}

#ifdef __cplusplus
}
#endif
