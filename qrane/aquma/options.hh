#ifndef _AQUMA_OPTIONS_

#define _AQUMA_OPTIONS_

struct aquma_options_s {
  const char * scop_file;
  const char * device_file;
  char * outfile;
  char * basename;
  int slice_mode;
  int input_format;
  int solver;
  int algorithm;
  int debug;
  int g2time;
  int g2error;
  int substrqd; // sub-string qubit detection
};
typedef struct aquma_options_s aquma_options;

#define SLICE_NORMAL 1
#define SLICE_INTRA  2
#define SLICE_HYBRID 3

#define FORMAT_SINGLE 1
#define FORMAT_MULTI 2
#define FORMAT_QASM 3

#define SOLVER_ISL 1
#define SOLVER_CPLEX 2

#define ALGO_ILP_SINGLE 1
#define ALGO_ILP_ITER   2
#define ALGO_ILP_FAST   3
#define ALGO_SUBSTR_QD   4
#define ALGO_GLOBAL_FREQ 5
#define ALGO_GLOBAL_SS   6


#define AQUMA_OPT_INPUT_FORMAT 0
#define AQUMA_OPT_DEVICE_FILE 1
#define AQUMA_OPT_SLICE_MODE 2
#define AQUMA_OPT_SOLVER 3
#define AQUMA_OPT_ALGO 4
#define AQUMA_OPT_DEBUG 5
#define AQUMA_OPT_HELP 6

#endif
