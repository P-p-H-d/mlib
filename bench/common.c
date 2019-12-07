/*
 * M*LIB - common functions for bench module
 *
 * Copyright (c) 2017-2019, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string.h>
#include "common.h"

void compiler_barrier(void *p)
{
  // Do nothing, but let the compiler thinks that we did.
  (void) p;
  // Can be used for FLTO building
  //  asm volatile("": "+r" (p) : :"memory");
}

struct parse_opt_s {
  int test_function;
  double from, to, step, grow;
  bool graph;
};

static void
parse_config(struct parse_opt_s *opt, int argc, const char *argv[])
{
  char *end;
  
  opt->test_function = 0;
  opt->from = 0.0;
  opt->to   = 0.0;
  opt->step = 0.0;
  opt->grow = 1.1;
  opt->graph = false;
  
  for(int i = 1; i < argc ; i++)
    {
      if (argv[i][0] == '-') {
	if (strcmp(argv[i], "--from") == 0){
	  i++;
	  opt->from = strtod(argv[i], &end);
	  opt->to   = opt->from;
	} else if (strcmp(argv[i], "--to") == 0) {
	  i++;
	  opt->to = strtod(argv[i], &end);
	  opt->grow = 1.1;
	} else if (strcmp(argv[i], "--step") == 0) {
	  i++;
	  opt->step = strtod(argv[i], &end);
	  opt->grow = 0;
	} else if (strcmp(argv[i], "--grow") == 0) {
	  i++;
	  opt->grow = strtod(argv[i], &end);
	  opt->step = 0;
	} else if (strcmp(argv[i], "--graph") == 0) {
	  opt->graph = true;
	} else {
	  fprintf(stderr, "ERROR: Option unkown: %s.\n",
		  argv[i]);
	  exit(-1);
	}
	if (*end != 0) {
	  fprintf(stderr, "ERROR: Cannot parse %s. Expected float\n",
		  argv[i]);
	  exit(-1);
	}

      } else {
	opt->test_function = strtol (argv[i], &end, 10);
	if (*end != 0) {
	  fprintf(stderr, "ERROR: Cannot parse %s. Expected integer\n",
		  argv[i]);
	  exit(-1);
	}
      }
    }
}

static int
select_config(int func, size_t n, const config_func_t functions[])
{
  for(size_t i = 0; i < n;i++) {
    if (functions[i].num == func)
      return i;
  }
  fprintf(stderr, "ERROR: Function number %d not found\n", func);
  exit(-1);
}

void
test(size_t n, const config_func_t functions[], int argc, const char *argv[])
{
  // options
  // NUMBER
  // --from NUMBER --to NUMBER --step NUMBER --grow NUMBER
  // --graph
  struct parse_opt_s arg;
  parse_config(&arg, argc, argv);
  int i = select_config(arg.test_function, n, functions);
  double from = arg.from == 0 ? functions[i].default_n : arg.from;
  double to   = arg.from == 0 ? functions[i].default_n : arg.to;
  // Do the bench
  for(double n = from; n <= to ; n = (arg.grow == 0 ? n + arg.step : n * arg.grow))
    {
      if (functions[i].init != NULL)
	functions[i].init(n);
      test_function(functions[i].funcname, (size_t) n, functions[i].func);
      if (functions[i].clear != NULL)
	functions[i].clear();
    }
  return;
}
