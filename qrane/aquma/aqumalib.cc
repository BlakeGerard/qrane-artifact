#include "aquma.hh"


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


int aquma_driver (aquma_options * aopts)
{
  aquma_graph * ag;
  ag = new aquma_graph (aopts);
  ag->read_graph (aopts->device_file);
  ag->show_graph ();

  aquma_qparser * parser;
  parser = new aquma_qparser (aopts->scop_file, ag->get_size ());
  assert (!parser->parse ());
  parser->build_qubit_string (ag->get_substr_ref ());

  aquma_scop * scop;
  scop = new aquma_scop (ag->get_size ());
  scop->set_debug_mode (aopts->debug);
  scop->read_from_file (aopts);
  scop->show ();
  ag->show_list ();

  high_resolution_clock::time_point t1 = high_resolution_clock::now();  

  // New version: doesn't use poly-stuff
  scop->compute_qubit_placement (ag, aopts);

  high_resolution_clock::time_point t2 = high_resolution_clock::now();  

  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

  std::cout << "AQUMA TIME: " << time_span.count() << " seconds." << std::endl;

  char tempname[100];
  sprintf (tempname,"%s.time", aopts->basename);
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
  aquma_options_free (aopts);
  return 0;
}
