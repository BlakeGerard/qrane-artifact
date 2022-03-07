#ifndef _AQUMA_SCOP_HH_

#define _AQUMA_SCOP_HH_

#include <stdio.h>
#include "graph.hh"
#include "circuit.hh"
#include "statement.hh"
#include "utils.hh"
#include "options.hh"


using namespace std;

struct mini_scop_t {
  isl_union_set * doms; 
  isl_union_map * reads;
  isl_union_map * writes;
  isl_union_map * sched;
  vector<isl_set*> v_doms;
  vector<isl_map*> v_reads;
  vector<isl_map*> v_writes;
  vector<isl_map*> v_sched;
};
typedef struct mini_scop_t mini_scop;

inline
__isl_give isl_map * 
extract_map_from_union_map (__isl_keep isl_union_map * umap, __isl_keep isl_set * set)
{
  isl_union_set * uset;
  uset = isl_union_set_from_set (isl_set_copy (set));
  isl_map * ret;
  ret = isl_map_from_union_map (
    isl_union_map_intersect_domain (
      isl_union_map_copy (umap),
      uset));

  int nparam = isl_map_dim (ret, isl_dim_param);
  ret = isl_map_remove_dims (ret, isl_dim_param, 0, nparam);
  ret = isl_map_remove_divs (ret);
  ret = isl_map_intersect_domain (ret,
      isl_set_remove_dims (isl_set_copy (set), isl_dim_param, 0, nparam));
  return ret;
}

inline isl_stat split_statement_abstractions (__isl_take isl_set * set, void * usr)
{
  mini_scop * data = (mini_scop*)(usr);
  /*
  if (data->v_doms.size () >= 10)
  {
    isl_set_free (set);
    return isl_stat_ok;
  }
  */

  int nparam = isl_set_n_param (set);
  int ndiv = isl_set_dim (set, isl_dim_div);

  if (ndiv > 0)
  {
    set = isl_set_remove_divs (set);
  }
  data->v_reads.push_back (extract_map_from_union_map (data->reads, set));
  data->v_writes.push_back (extract_map_from_union_map (data->writes, set));
  data->v_sched.push_back (extract_map_from_union_map (data->sched, set));

  // Remove the param dimensions after extracting the corresponding maps.
  set = isl_set_remove_dims (set, isl_dim_param, 0, nparam);
  data->v_doms.push_back (isl_set_copy (set));
  isl_set_free (set);
  return isl_stat_ok;
}

/*
 * graph_properties structure defined in statement.hh
 */
inline graph_properties * alloc_graph_properties (int n_qubit) 
{
  graph_properties * ret;
  ret = new graph_properties;
  ret->n_nodes = 0;
  ret->n_edges = 0;
  // the vid vector has n_qubit entries, one counter for each qubit.
  ret->vid.resize (n_qubit, 0);
  return ret;
}

inline void show_qubit_counts (vector<unsigned> & qcount)
{
  unsigned qq;
  printf ("Showing qubit counts: \n");
  for (qq = 0; qq < qcount.size (); qq++)
  {
    if (qq > 0)
      printf (", ");
    printf ("q[%d]=%u ", qq, qcount[qq]);
  }
  printf ("\n");
}

inline void show_pair_counts (qmap_count & qmc)
{
  qmap_count_iterator mm;
  printf ("Count of vertices with fixed cardinality: ");
  for (mm = qmc.begin (); mm != qmc.end (); mm++)
  {
    if (mm != qmc.begin ())
      printf (", ");
    printf ("#%u (%u)", mm->first, mm->second);
  }
  printf ("\n");
}

void inline show_graph_qubit_properties (graph_properties * gprop)
{
  printf ("================================================\n");
  printf ("No. qubits: %d\n", gprop->n_nodes);
  printf ("No. links : %d\n", gprop->n_edges);
  printf ("Vertex counts : ");
  show_qubit_counts (gprop->vid);
  show_pair_counts (gprop->qmc);
  printf ("------------------------------------------------\n");
}

class aquma_scop {
  aquma_statement_vector m_statement;
  int m_nstmt;
  isl_ctx * m_ctx;
  unsigned m_nqubit;
  bool m_debug;
  aquma_circuit * m_circ; 

  public:

  aquma_scop (unsigned n_qubits) {
    m_nstmt = 0;
    m_ctx = isl_ctx_alloc ();
    m_nqubit = n_qubits;
    m_debug = false;
  }

  ~ aquma_scop () {
    if (m_ctx)
      isl_ctx_free (m_ctx);
  }

  isl_ctx * ctx()
  {
    return m_ctx;
  }

  void set_debug_mode (int debug_mode)
  {
    m_debug = debug_mode == 1;
  }

  FILE * open_file (const char * filename)
  {
    FILE * ff;
    ff = fopen (filename, "r");
    if (!ff)
    {
      fprintf (stderr, "Scop file %s not found. Aborting...\n", filename);
      return NULL;
    }
    return ff;
  }

  void read_from_file (aquma_options * aopts)
  {
    if (aopts->input_format == FORMAT_QASM)
      this->read_from_qasm (aopts->scop_file);
    if (aopts->input_format == FORMAT_SINGLE)
      this->read_from_file_single (aopts->scop_file);
    if (aopts->input_format == FORMAT_MULTI)
      this->read_from_file_multi (aopts->scop_file);
  }

  void read_from_file_multi (const char * filename) {
    FILE * ff = this->open_file (filename);
    assert (ff && "Error opening circuit file. Aborting ...\n");
    int discard;
    discard = fscanf (ff, "%d", &m_nstmt);
    assert (discard > 0);
    int ii;
    char buf[1000];
    char * ptr;
    ptr = fgets (buf, 1000, ff);
    assert (ptr);
    unsigned next_stmt_id = 0;
    for (ii = 0; ii < m_nstmt; ii++)
    {
      aquma_statement * stmt;
      stmt = new aquma_statement (m_ctx, ++next_stmt_id);
      stmt->read_statement (ff);
      m_statement.push_back (stmt);
    }
    if (ff)
      fclose (ff);
  }

  void read_from_qasm (const char * filename) 
  {
    int retval;
    char cmdstr[1000];
    const char * rawcirc = "raw-circuit.txt";
    sprintf (cmdstr, "python ${AQUMA_HOME}/extract-circuit-properties.py %s > %s", 
      (char*)filename, (char*)rawcirc); 
    retval = system (cmdstr);
    assert (! retval && "Circuit property extraction failed.");
    int nopts;
    FILE * ff = this->open_file (rawcirc);
    assert (ff && "Error opening raw-circuit file. Aborting ...\n");
    retval = fscanf (ff, "%d", &nopts);
    printf ("Will read %d q-opt\n", nopts);
    this->m_circ = new aquma_circuit (m_nqubit, nopts);
    this->m_circ->read_qasm (ff);
    this->m_circ->compute_qubit_source_incidence ();
    this->m_circ->show_qubit_histograms ();
    if (ff)
      fclose (ff);
  }

  #define MAX_ISL_STR_SIZE 50000
  void read_from_file_single (const char * filename) 
  {
    FILE * ff = this->open_file (filename);
    assert (ff && "Error opening circuit file. Aborting ...\n");
    char buf[MAX_ISL_STR_SIZE];
    char * ptr;
    mini_scop ms;

    ptr = fgets (buf, MAX_ISL_STR_SIZE , ff);
    assert (ptr);
    ms.doms = isl_union_set_coalesce (isl_union_set_read_from_str (m_ctx, buf));

    ptr = fgets (buf, MAX_ISL_STR_SIZE , ff);
    assert (ptr);
    ms.reads = isl_union_map_coalesce (isl_union_map_read_from_str (m_ctx, buf));

    ptr = fgets (buf, MAX_ISL_STR_SIZE , ff);
    assert (ptr);
    ms.writes = isl_union_map_coalesce (isl_union_map_read_from_str (m_ctx, buf));

    ptr = fgets (buf, MAX_ISL_STR_SIZE , ff);
    assert (ptr);
    ms.sched = isl_union_map_coalesce (isl_union_map_read_from_str (m_ctx, buf));

    isl_stat stat;
    stat = isl_union_set_foreach_set (ms.doms, &split_statement_abstractions, (void*)(&ms));
    assert (stat == isl_stat_ok);

    unsigned ss;
    for (ss = 0; ss < ms.v_doms.size (); ss++)
    {
      aquma_statement * stmt;
      stmt = new aquma_statement (m_ctx, ms.v_doms[ss], ms.v_reads[ss], 
        ms.v_writes[ss], ms.v_sched[ss], ss+1);
      m_statement.push_back (stmt);
    }
    sort_statements_by_domain_size (m_statement);
    #if 0
    int first_under_5 = -1;
    for (ss = 0; ss < ms.v_doms.size (); ss++)
    {
      // Keep the position of the first domain with less that 5 ops.
      if (m_statement[ss]->get_domain_size () < 5)
      {
        first_under_5 = ss;
      }
      //printf ("Statement %u : %ld\n", m_statement[ss]->get_plain_id (), m_statement[ss]->get_domain_size ());
    }
    //printf ("\n"); fflush (stdout);
    #endif

    // Only consider statements that have a sufficient number of points.
    unsigned magic_cut = 6;
    if (m_statement.size () > magic_cut)
    {
      m_statement.erase (m_statement.begin () + magic_cut, m_statement.end ());
    }
    if (ff)
      fclose (ff);
  }


  void show () {
    aquma_statement_vector_iterator ss;
    printf ("SCOP:\n");
    for (ss = m_statement.begin (); ss != m_statement.end (); ss++)
    {
      printf ("-------------------------------------\n"); fflush (stdout);
      (*ss)->show ();
    }
    printf ("-------------------------------------\n"); fflush (stdout);
  }



  void partition_qubit_index_space (aquma_graph * graph, aquma_options * options)
  {
    int n_qubits = graph->get_size (); // get number of qubits in graph (Read m_size attribute, not m_list).
    printf ("Qubit Index Partitioning\n");
    isl_set * indices = this->initialize_qubit_set (n_qubits);
    isl_set_dump (indices);
    //int n_stmt = m_statement.size ();
    //isl_set * ssa[n_stmt]; // statement shared access
    //isl_set * qspace[n_stmt]; // statement qubit data space (includes intra-sharing)
    aquma_statement_vector_iterator ss;
    int sid;
    // Compute intra-statement conflicts

    // Initialize the qinfo struct
    // 
    list<qubit_info*> sinfo;
    for (ss = m_statement.begin (), sid = 0; ss != m_statement.end (); ss++, sid++)
    {
      qubit_info * info = new qubit_info;
      fprintf (stderr, "Computing info of statement %d\n", sid);
      //ssa[sid] = (*ss)->compute_shared_access ();
      //isl_set_dump (ssa[sid]); fflush (stderr);
      //qspace[sid] = (*ss)->compute_qubit_space ();
      //temp = (*ss)->compute_domain_conflicts ();
      isl_set * temp;
      if (options->slice_mode == SLICE_NORMAL)
      {
        temp = (*ss)->compute_qubit_space ();
      }
      else if (options->slice_mode == SLICE_INTRA)
        temp = (*ss)->compute_shared_access ();
      else if (options->slice_mode == SLICE_HYBRID)
      {
        temp = (*ss)->compute_shared_access ();     
        if (isl_set_is_empty (temp) == isl_bool_true)
        {
          isl_set_free (temp);
          temp = (*ss)->compute_qubit_space ();
        }
      }
      else
        assert (0 && "Unknown qubit slicing mode selected.");
      //isl_set_free (temp);
      //isl_pw_qpolynomial * pwq = (*ss)->get_expression_from_set (temp);
      int card = (*ss)->compute_set_size (isl_set_copy (temp));
      //isl_pw_qpolynomial_free (pwq);
      //fprintf (stderr, "Set: (%d)\n", card); fflush (stderr);
      info->set = temp; //ssa[sid];
      info->sid = (*ss)->my_id ();
      info->domain = (*ss)->copy_domain ();
      info->set_size = aquma_utils::get_set_size_nd (info->set);
      info->dom_size = card;
      info->num_dom = 1;
      sinfo.push_back (info);
    }
    
    // Sort the elements of sinfo by their qubit size.
    sinfo.sort (set_greater_than);
    printf ("Initial sinfo structure...\n");
    print_list_qubit_info (sinfo);

    printf ("Starting ...\n");
    int n_iter = 0;
    list<qubit_info*> parts;
    // Do fixed point algorithm.    
    while (isl_set_is_empty (indices) == isl_bool_false && n_iter < n_qubits)
    {
      printf ("Round %d\n", n_iter);
      list<qubit_info*>::iterator pp;
      qubit_info * front = sinfo.front ();
      qubit_info * next = copy_qubit_info (front);
      printf ("\t==>Starting selecting sets ...\n");
      for (pp = sinfo.begin (); pp != sinfo.end (); pp++)
        if (front != *pp)
      {
        qubit_info temp;
        temp.set = aquma_utils::intersect_sets (next->set, (*pp)->set);
        temp.set_size = aquma_utils::get_set_size_nd (temp.set);
        if (temp.set_size * (next->num_dom + 1) > next->set_size)
        {
          isl_set_free (next->set);
          next->set = temp.set;
          next->sid = aquma_utils::add_to_set (next->sid, (*pp)->sid); // Do not destroy the set in pp
          next->set_size = temp.set_size * (next->num_dom + 1);
          next->num_dom ++;
        }
      }
      // Add the 'adjacent' qubits that may have been left out. 
      // Final set of qubits must represent pairs of accessed qubits
      // in a 2-qubit gate.
      int xyz;
      for (ss = m_statement.begin (), xyz=0; ss != m_statement.end (); ss++, xyz++)
      {
        //printf ("s%d ...\n", xyz);
        next->set = (*ss)->complete_qubit_pairs (next->set);
      }
      //printf ("paired qubit set: "); isl_set_dump (next->set);
      if (isl_set_is_empty (next->set) == isl_bool_true)
        break;
      printf ("\t==>Updating statements...\n");
      for (pp = sinfo.begin (); pp != sinfo.end (); pp++)
      {
        qubit_info * ptr = (qubit_info*)(*pp);
        ptr->set = isl_set_subtract (ptr->set, isl_set_copy (next->set));
        ptr->set_size = aquma_utils::get_set_size_nd (ptr->set);
      }
      printf ("\t==>Updating qubit set...\n");
      indices = isl_set_subtract (indices, isl_set_copy (next->set));
      parts.push_back (next);
      printf ("\t==>Resorting list...\n");
      sinfo.sort (set_greater_than);
      n_iter ++;
    }
    // Extract some properties

    printf ("\n\nShowing computed qubit slicing information\n");
    print_list_qubit_info (parts);
    list<qubit_info*>::iterator pp;

    printf ("\n\nExtracting qubit properties ...\n");
    graph->initialize_layout ();
    int slice_id = 0;
    for (pp = parts.begin (); pp != parts.end (); pp++, slice_id++)
    {
      print_qubit_info (*pp);
      graph_properties * gprop = this->extract_partition_requirements (*pp);
      show_graph_qubit_properties (gprop);
      aquma_graph * subg = graph->gen_partition (*pp, gprop);
      printf ("Extract sub-graph for slice %d\n", slice_id);
      if (subg)
      {
        printf ("==> Building ILP for sub-graph %d\n", slice_id); fflush (stdout);
        subg->compute_layout (gprop, graph);
        // TODO: each call to subg has its own qubit set, which must be
        // added to the main graph set. 
        graph->update_xset (subg->get_xset ());
      }
    }
    graph->finalize_layout ();
    graph->print_layout ();
    graph->print_dot_file ();
  }


  void compute_qubit_placement (aquma_graph * graph, aquma_options * options)
  {
    //int n_qubits = graph->get_size (); 

    // Resize m_layout to the number of physical qubits and 
    // Set all the entries of m_layout to -1.
    graph->initialize_layout ();

    if (options->algorithm == ALGO_ILP_FAST)
    {
      // Sort qubits by R-Score.
      graph->init_qubit_reciprocals ();
      graph->compute_fast_iterative_layout (graph, m_circ);
    }
    else if (options->algorithm == ALGO_GLOBAL_SS)
    {
      cout << "Using Combined Global Frequency and Substring Qubit Detection Method" << endl;
      graph->init_qubit_reciprocals ();
      graph->compute_substr_layout (graph, m_circ);
      graph->compute_global_frequency_layout (graph, m_circ);
      cout << "moving to g1 allocation " << endl;

      // Finish allocating any pending qubits by their q1 reciprocal
      set<int> pending;
      graph->compute_pending_qubits_from_layout (pending);
      graph->allocate_qubits_by_q1_reciprocal (&pending);
    }
    else if (options->algorithm == ALGO_SUBSTR_QD)
    {
      cout << "Using Substring Qubit Detection Method" << endl;
      graph->init_qubit_reciprocals ();
      graph->compute_substr_layout (graph, m_circ);
      cout << "moving to g1 allocation " << endl;

      // Finish allocating any pending qubits by their q1 reciprocal
      set<int> pending;
      graph->compute_pending_qubits_from_layout (pending);
      graph->allocate_qubits_by_q1_reciprocal (&pending);
    }
    else if (options->algorithm == ALGO_GLOBAL_FREQ)
    {
      cout << "Using Global Frequency Detection Method" << endl;
      graph->init_qubit_reciprocals ();
      graph->compute_global_frequency_layout (graph, m_circ);
      cout << "moving to g1 allocation " << endl;

      // Finish allocating any pending qubits by their q1 reciprocal
      set<int> pending;
      graph->compute_pending_qubits_from_layout (pending);
      graph->allocate_qubits_by_q1_reciprocal (&pending);
    }
    else if (options->algorithm == ALGO_ILP_ITER)
    {
      graph->compute_iterative_layout (graph, m_circ);
      graph->finalize_layout ();
    }
    else if (options->algorithm == ALGO_ILP_SINGLE)
    {
      graph->compute_non_poly_layout (graph, m_circ);
      graph->finalize_layout ();
    }
    else 
      assert (0 && "Unknown layout requested.");

    graph->print_layout ();
    graph->print_check_array ();
    graph->print_dot_file ();
  }

  isl_set * initialize_qubit_set (int n_qubits)
  {
    isl_set * ret;
    string str = "";
    str += " { q[i] : 0 <= i < ";
    str += std::to_string (n_qubits);
    str += " };";
    ret = isl_set_read_from_str (m_ctx, str.c_str ());
    return ret;
  }

  #if 0
  void compute_incidence_degrees (int n_qubits)
  {
    vector<int> qcount;
    qcount.resize (n_qubits, 0);
    aquma_statement_vector_iterator ss;
    for (ss = m_statement.begin (); ss != m_statement.end (); ss++)
    {
      (*ss)->count_all_qubit_incidence (qcount);
    }
    show_qubit_counts (qcount);
  }
  #endif


  void aggregate_incidence_counts (qmap_count & qmc, vector<unsigned> & vec)
  {
    vector<unsigned>::iterator qq;
    unsigned ii;
    for (qq = vec.begin (), ii=0; qq != vec.end (); qq++, ii++)
      if (*qq > 0) // if this qubit is actually used, its count will be > 0
    {
      if (qmc.find (*qq) == qmc.end())
      {
        // If the qubit is not in the qmc map, then add it with a count to 1.
        qmc.insert (pair<unsigned,unsigned>(*qq,1));
        continue;
      }
      // If we didn't have to create an entry in the map, just increment it.
      qmc[*qq] += 1;
    }
  }

  inline graph_properties * extract_partition_requirements (qubit_info * qinfo)
  {
    // TODO: Store in the qinfo structure the statement identifiers.
    unsigned n_qubits = m_nqubit; //qinfo->set_size;
    graph_properties * gprop = alloc_graph_properties (n_qubits);
    aquma_statement_vector_iterator ss;
    // We need the number of total qubits.
    unsigned total_qubits = 0; // must be the number of qubits x no. of statements
    isl_map * all_edges = NULL;
    for (ss = m_statement.begin (); ss != m_statement.end (); ss++)
      if ((*ss)->is_included_in (qinfo))
    {
      printf ("Counting incidence...\n");
      if (ss == m_statement.begin ())
      {
        // Do this only for the very first statement of the partition.
        (*ss)->count_all_qubit_incidence (gprop->vid, qinfo->set);
      }
      printf ("Counting quits...\n"); fflush (stdout);
      total_qubits += (*ss)->compute_set_size ((*ss)->compute_qubit_space (qinfo->set));
      printf ("Counting links ...\n");
      isl_map * curr_links = (*ss)->compute_directed_link_map (qinfo->set);
      printf ("Aggregating links ...\n");
      if (!all_edges)
      {
        all_edges = curr_links;
      }
      else
      {
        all_edges = isl_map_union (all_edges, curr_links);
      }
    }
    //assert (total_qubits == qinfo->set_size * qinfo->num_dom);
    //show_qubit_counts (gprop->vid);
    gprop->n_nodes = total_qubits / qinfo->num_dom;
    gprop->n_edges = aquma_utils::get_set_size (isl_map_wrap (all_edges));
    // Number of edges -> pairs of qubits, <i,j> = <j,i>
    this->aggregate_incidence_counts (gprop->qmc, gprop->vid);
    return gprop;
  }
};


#endif
