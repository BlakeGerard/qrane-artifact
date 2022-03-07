#include "aquma.hh"


const char * aquma_options_list[] = {
  "<empty>",
  "--topo-file=",
  "<empty>",
  "--solver=",
  "--algo=",
  "--debug",
  "--help",
  NULL
};

const char * aquma_options_example [] = {
  "<empty>",
  "--topo-file=<filename>",
  "<empty>",
  "--solver=<isl|cplex>",
  "--algo=<ilp-iter|ilp-single|ilp-fast|substr-qd|gfreq|glb-fss>",
  //"--g2objective=<E:Error|T:Time|ET:Error+Time|TE:Time+Error>",
  "--debug",
  "--help",
  NULL
};

const char * ALGO_ILP_SINGLE_STR="ilp-single";
const char * ALGO_ILP_ITER_STR="ilp-iter";
const char * ALGO_ILP_FAST_STR="ilp-fast";
const char * ALGO_SUBSTR_QD_STR="substr-qd";
const char * ALGO_GLOBAL_FREQ_STR="gfreq";
const char * ALGO_GLOBAL_SS_STR="glb-fss";

void print_help ()
{
  const char * opt;
  int ii;
  printf ("IterQM options: \n");
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

void aquma_options_extract (int narg, char * args[], aquma_options * aopts)
{
  int ii;
  aopts->debug = 0;
  aopts->solver = SOLVER_ISL;
  aopts->input_format = FORMAT_QASM;
  for (ii = 1; ii < narg; ii++)
  {
    int kk;
    int is_filename = 1;
    for (kk = 0; aquma_options_list[kk]; kk++)
      if (strstr (args[ii], aquma_options_list[kk]))
    {
      const char * sval = get_option_value (args[ii]);
      printf ("Extracted option value: %s\n", (char*)(sval));
      switch (kk) {
        case AQUMA_OPT_DEVICE_FILE: // 1
          aopts->device_file = sval;
          is_filename = 0;
          break;
        case AQUMA_OPT_SOLVER: // 3
          if (strcmp (sval,"isl") == 0)
            aopts->solver = SOLVER_ISL;
          if (strcmp (sval,"cplex") == 0)
            aopts->solver = SOLVER_CPLEX;
          is_filename = 0;
          break;
        case AQUMA_OPT_ALGO: // 4
          if (strcmp (sval,(char*)(ALGO_ILP_SINGLE_STR)) == 0)
            aopts->algorithm = ALGO_ILP_SINGLE;
          is_filename = 0;
          break;
        case AQUMA_OPT_DEBUG: // 5
          aopts->debug = 1;
          is_filename = 0;
          break;
        case AQUMA_OPT_HELP:
          print_help ();
          exit (0);
      }
    }
    if (is_filename) // 
    {
      aopts->scop_file = args[ii];
      aopts->outfile = build_out_filename (args[ii]);
      aopts->basename = build_basename (args[ii]);
      //aopts->input_format = autodetect_input_format (args[ii]);
      assert (aopts->input_format && "Unexpected file input format");
    }
  }
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

  aquma_graph * ag;
  ag = new aquma_graph (&aopts);
  ag->read_graph (aopts.device_file);
  ag->show_graph ();

  cout << "About to call isl_topo_map_str " << endl;
  string topo_str = ag->build_isl_topo_map_str ();
  cout << "Topo map ISL string: " << topo_str << endl;

  aquma_qparser * parser;
  parser = new aquma_qparser (aopts.scop_file, ag->get_size ());
  assert (!parser->parse ());
  parser->build_qubit_string (ag->get_substr_ref ());

  printf ("Testing (Affine Iterative Qubit Mapper) functionality...\n");

  aquma_scop * scop;
  scop = new aquma_scop (ag->get_size ());
  scop->set_debug_mode (aopts.debug);
  scop->read_from_file (&aopts);
  scop->show ();
  ag->show_list ();

  high_resolution_clock::time_point t1 = high_resolution_clock::now();  

  // New version: doesn't use poly-stuff
  scop->compute_qubit_placement (ag, &aopts);

  high_resolution_clock::time_point t2 = high_resolution_clock::now();  

  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

  std::cout << "ITQUMA TIME: " << time_span.count() << " seconds." << std::endl;

  char tempname[100];
  sprintf (tempname,"%s.time", aopts.basename);
  FILE * ff;
  ff = fopen (tempname, "w");
  fprintf (ff,"%lf",time_span.count ());
  fclose (ff);

  parser->update_layout (ag->get_layout (), ag->get_device_width ());
  parser->pretty_print ();


  assert (ag->is_mapping_unique () && "Mapping not unique.");

  delete scop;
  delete ag;
  delete parser;
  aquma_options_free (&aopts);
  return 0;
}
