#ifndef _AQUMA_H_

#define _AQUMA_H_

#include <stdio.h>
#include <string.h>
#include <ctime>
#include <ratio>
#include <chrono>

#include "graph.hh"
#include "scop.hh"
#include "options.hh"
#include "qparser.hh"
//#include "statement.hh"

using namespace std::chrono;

void aquma_options_init (aquma_options * aopts);

int aquma_options_check (aquma_options * aopts);

void aquma_options_free (aquma_options * aopts);

int aquma_driver (aquma_options * aopts);

void print_aquma_options (aquma_options * aopts);

#endif
