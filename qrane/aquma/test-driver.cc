#include "aquma.hh"


const char * aquma_options_list[] = {
  "--input-format=",
  "--device-file=",
  "--slicing=",
  "--solver=",
  "--algo=",
  "--debug",
  "--help",
  NULL
};

const char * aquma_options_example [] = {
  "--input-format=<single|multi|qasm>",
  "--device-file=<filename>",
  "--slicing=<normal|hybrid>",
  "--solver=<isl|cplex>",
  "--algo=<ilp-iter|ilp-single|ilp-fast|substr-qd|gfreq|glb-fss>",
  //"--g2objective=<E:Error|T:Time|ET:Error+Time|TE:Time+Error>",
  "--debug",
  "--help",
  NULL
};

void print_help ()
{
  const char * opt;
  int ii;
  printf ("Aquma options: \n");
  for (ii = 0; aquma_options_example[ii] && ii < 10; ii++);
  int nopts = ii;
  for (ii = 0; ii < nopts; ii++)
  {
    opt = aquma_options_example[ii];
    printf ("%s\n", opt);
  }
  printf ("\n");
}


const char * get_option_value (const char * flag)
{
  char * pp;
  for (pp = (char*)flag; *pp != '\0' && *pp != '='; pp++);
  if (*pp == '=')
  {
    pp++;
    return pp;
  }
  return NULL;
}

char * build_out_filename (const char * filename)
{
  char * pp;
  char buf[100];
  strcpy (buf, filename);
  for (pp = buf; *pp != '\0'; pp++);
  for (  ; *pp != '.'; pp--);
  sprintf (pp,".layout");
  return strdup (buf);
}

char * build_basename (const char * filename)
{
  char * pp;
  char buf[100];
  strcpy (buf, filename);
  for (pp = buf; *pp != '\0'; pp++);
  for (  ; *pp != '.' && pp != buf; pp--);
  *pp = '\0';
  for (  ; *pp != '/' && pp != buf; pp--);
  if (*pp == '/')
    pp++;
  return strdup (pp);
}


int autodetect_input_format (const char * filename)
{
  char * pp;
  char buf[100];
  for (pp = (char*)filename; *pp != '\0'; pp++);
  for (  ; *pp != '.' && pp != filename; pp--);
  pp++;
  strcpy (buf, pp);
  if (strcmp (buf, "aquma") == 0)
    return FORMAT_SINGLE;
  if (strcmp (buf, "aqs") == 0)
    return FORMAT_MULTI;
  if (strcmp (buf, "qasm") == 0)
    return FORMAT_QASM;
  return 0;
}

void aquma_options_init (aquma_options * aopts)
{
  aopts->outfile = NULL;
  aopts->basename = NULL;
  aopts->slice_mode = SLICE_NORMAL;
  aopts->input_format = FORMAT_QASM;
  aopts->solver = SOLVER_ISL;
  aopts->algorithm = ALGO_ILP_FAST;
  aopts->substrqd = 0;
}


int aquma_options_check (aquma_options * aopts)
{
  if (!aopts->outfile)
    return 0;
  if (!aopts->slice_mode)
    return 0;
  if (!aopts->input_format)
    return 0;
  FILE * dgf = fopen (aopts->device_file, "r");
  if (!dgf)
  {
    printf ("Device graph file %s not found. Aborting ...\n", aopts->device_file);
    return 0;
  }
  if (dgf)
    fclose (dgf);

  FILE * sf = fopen (aopts->scop_file, "r");
  if (!sf)
  {
    printf ("Scop file %s not found. Aborting ...\n", aopts->scop_file);
    return 0;
  }
  if (sf)
    fclose (sf);
  return 1;
}

void aquma_options_extract (int narg, char * args[], aquma_options * aopts)
{
  int ii;
  aopts->debug = 0;
  for (ii = 1; ii < narg; ii++)
  {
    int kk;
    int valid = 0;
    for (kk = 0; aquma_options_list[kk]; kk++)
      if (strstr (args[ii], aquma_options_list[kk]))
    {
      const char * sval = get_option_value (args[ii]);
      printf ("Extract option value: %s\n", (char*)(sval));
      switch (kk) {
        case AQUMA_OPT_INPUT_FORMAT: // 0
          if (strcmp (sval,"single") == 0)
            aopts->input_format = FORMAT_SINGLE;
          if (strcmp (sval,"multi") == 0)
            aopts->input_format = FORMAT_MULTI;
          if (strcmp (sval,"qasm") == 0)
            aopts->input_format = FORMAT_QASM;
          valid=1;
          break;
        case AQUMA_OPT_DEVICE_FILE: // 1
          aopts->device_file = sval;
          valid=1;
          break;
        case AQUMA_OPT_SLICE_MODE: // 2
          if (strcmp (sval,"normal") == 0)
            aopts->slice_mode = SLICE_NORMAL;
          if (strcmp (sval,"intra") == 0)
            aopts->slice_mode = SLICE_INTRA;
          if (strcmp (sval,"hybrid") == 0)
            aopts->slice_mode = SLICE_HYBRID;
          valid=1;
          break;
        case AQUMA_OPT_SOLVER: // 3
          if (strcmp (sval,"isl") == 0)
            aopts->solver = SOLVER_ISL;
          if (strcmp (sval,"cplex") == 0)
            aopts->solver = SOLVER_CPLEX;
          valid=1;
          break;
        case AQUMA_OPT_ALGO: // 4
          if (strcmp (sval,(char*)(ALGO_ILP_SINGLE_STR)) == 0)
            aopts->algorithm = ALGO_ILP_SINGLE;
          if (strcmp (sval,(char*)(ALGO_ILP_ITER_STR)) == 0)
            aopts->algorithm = ALGO_ILP_ITER;
          if (strcmp (sval,(char*)(ALGO_ILP_FAST_STR)) == 0)
            aopts->algorithm = ALGO_ILP_FAST;
          if (strcmp (sval,(char*)(ALGO_SUBSTR_QD_STR)) == 0)
            aopts->algorithm = ALGO_SUBSTR_QD;
          if (strcmp (sval,(char*)(ALGO_GLOBAL_FREQ_STR)) == 0)
            aopts->algorithm = ALGO_GLOBAL_FREQ;
          if (strcmp (sval,(char*)(ALGO_GLOBAL_SS_STR)) == 0)
            aopts->algorithm = ALGO_GLOBAL_SS;
          valid=1;
          break;
        case AQUMA_OPT_DEBUG: // 5
          aopts->debug = 1;
          valid = 1;
          break;
        case AQUMA_OPT_HELP:
          print_help ();
          exit (0);
      }
    }
    if (!valid)
    {
      aopts->scop_file = args[ii];
      aopts->outfile = build_out_filename (args[ii]);
      aopts->basename = build_basename (args[ii]);
      aopts->input_format = autodetect_input_format (args[ii]);
      assert (aopts->input_format && "Unexpected file input format");
    }
  }
}

void aquma_options_free (aquma_options * aopts)
{
  free (aopts->outfile);
  free (aopts->basename);
}

void print_aquma_options (aquma_options * aopts)
{
  printf ("Collected Aquma options: \n");
  printf ("\tInput scop   : %s\n", aopts->scop_file);
  printf ("\tInput device : %s\n", aopts->device_file);
  printf ("\tSlice mode   : %d\n", aopts->slice_mode);
  printf ("\tInput format : %d\n", aopts->input_format);
  printf ("\tOutput file  : %s\n", aopts->outfile);
  printf ("\tBasename     : %s\n", aopts->basename);
  printf ("\n\n");
}


int main (int narg, char * args[]) 
{
  if (narg < 4)
  {
    print_help ();
    exit (0);
  }

  aquma_options aopts;
  aquma_options_init (&aopts);
  aquma_options_extract (narg, args, &aopts);
  print_aquma_options (&aopts);

  if (!aquma_options_check (&aopts))
  {
    printf ("One or more errors found in Aquma flags and inputs. Aborting ...\n");
    return 1;
  }

  int res = aquma_driver (&aopts);
  assert (!res && "Unknown Aquma error.");

  return 0;
}
