#ifndef _AQUMA_GRAPH_

#define _AQUMA_GRAPH_


#include <stdio.h>
#include <stdlib.h>

#include "options.hh"
#include "qubit.hh"
#include "circuit.hh"
#include "ilp.hh"
#include "statement.hh"

#include <algorithm>
#include <assert.h>
#include <string>
#include <cstring>

#include "isl/ctx.h"
#include "isl/polynomial.h"

using namespace std;

typedef struct {
  int q_source;
  int q_target;
  int freq;
} edge_freq_s;

inline bool swap_edge_freq (edge_freq_s & left, edge_freq_s & right)
{
  if (right.freq < left.freq)
    return true;
  return false;
}

typedef vector<vector<edge_freq_s > > edge_freq_matrix_t;

inline void sort_qubits_edge_by_frequency (vector<edge_freq_s> & qf)
{
  std::sort (qf.begin (), qf.end (), swap_edge_freq);
}

inline void print_qubit_edge_frequency_debug (vector<edge_freq_s> & qf)
{
  vector<edge_freq_s>::iterator qq;
  cout << "Showing qubit/edge frequencies: " << endl;
  for (qq = qf.begin (); qq != qf.end (); qq++)
  {
    cout << "<" << (*qq).q_source << " | ";
    cout << (*qq).q_target << " | ";
    cout << (*qq).freq << " > " << endl;
  }
}

inline void print_vector_int_debug (vector<int> & vec)
{
  vector<int>::iterator vv;
  for (vv = vec.begin (); vv != vec.end (); vv++)
  {
    cout << *vv << " ";
  }
  cout << endl;
}

class aquma_graph {
  public: 

  int m_size;       // number of qubits. Always representative even in sub-graphs.
  unsigned m_edges; // Number of edges in (sub-)graph; not the same in sub-graphs.
  int * m_coupling;
  //vector<aquma_qubit*> m_list;
  aquma_qubit_vector m_list;
  vector<unsigned> m_vcount;  // aggregated count of incidence degree.
  bool m_is_partition;    // mark the current graph as a partition.
  set<int> m_xset; // aggregated exclusion set.
  vector<int> m_layout; // new layout Q0->4; Q1->7, etc.
  unsigned m_allocated_q; // number of globally allocated qubits (an offset) 
  aquma_options * m_options;
  bool m_debug;
  edge_map m_gate2err;
  edge_map m_gate2time;
  vector<int> m_qsubstr; 
  vector<int> m_check;


  /*
   * This constructor is mostly used during the creation of sub-graphs.
   */
  aquma_graph (int nq, aquma_options * aopts) {
    allocate (nq);
    m_debug = aopts->debug == 1;
    m_options = aopts;
    m_edges = 0;
    m_vcount.resize (nq, 0);
    m_is_partition = false;
    this->reset_layout ();
  }

  ~ aquma_graph () {
    if (m_coupling)
      delete m_coupling;
    m_size = 0;
    m_edges = 0;
    if (m_list.size ()) {
      vector<aquma_qubit*>::iterator qq;
      for (qq = m_list.begin (); qq != m_list.end (); qq++)
        delete *qq;
      m_list.clear ();
    }
  }


  /*
   * This constructor is mostly used in the main driver.
   */
  aquma_graph (aquma_options * aopts = NULL) {
    m_options = aopts;
    if (m_options)
    {
      m_debug = m_options->debug == 1;
    }
    m_size = 0;
    m_edges = 0;
    m_coupling = NULL;
  }

  aquma_graph (aquma_graph & src ) 
  {
    m_size = src.get_size ();
    m_edges = src.get_num_edges ();
    m_coupling = new int [m_size * m_size]; 
    assert (src.coupling () != NULL);
    std::memcpy (m_coupling, src.coupling (), sizeof(int) * m_size * m_size);
    aquma_qubit_vector_iterator qq;
    for (qq = src.first_qubit (); qq != src.last_qubit (); qq++)
    {
      aquma_qubit * qbt = (*qq)->deep_copy (true);
      m_list.push_back (qbt);
    }
    m_debug = false;
    m_check.resize (m_size, 0);
  }

  int get_device_width ()
  {
    return m_size;
  }

  unsigned edge_to_source_qubit (unsigned edge_id)
  {
    return edge_id / m_size;
  }

  unsigned edge_to_target_qubit (unsigned edge_id)
  {
    return edge_id % m_size;
  }

  void mark_as_partition ()
  {
    m_is_partition = true;
  }

  void set_debug_mode (int debmode)
  {
    m_debug = debmode == 1;
  }

  vector<int> & get_substr_ref ()
  {
    return m_qsubstr;
  }

  void open_file (const char * filename, FILE * & ff)
  {
    ff = fopen (filename, "r");
    if (!ff)
    {
      fprintf (stderr, "Error opening file %s\n", filename);
      exit (42);
      return;
    }
  }

  bool use_isl_solver ()
  {
    return m_options->solver == SOLVER_ISL;
  }

  bool use_cplex_solver ()
  {
    return m_options->solver == SOLVER_CPLEX;
  }


  /*
   * Clear the qubit exclusion set
   */
  void initialize_layout ()
  {
    m_xset.clear ();
    this->reset_layout ();
    m_layout.resize (m_size, -1);
    m_allocated_q = 0;
  }

  /*
  * Among other things, assign any location to unmapped qubits.
  */
  void finalize_layout ()
  {
    unsigned qq;
    int next_q = m_allocated_q;
    printf ("Number of allocated qubits prior to finalization: %d\n", next_q);
    for (qq = 0; qq < m_layout.size (); qq++)
    {
      if (qq > 0)
      {
        printf (", ");
      }
      printf ("q[%u]: %d", qq, m_layout[qq]);
    }
    printf ("\n"); 
    for (qq = 0; qq < m_layout.size (); qq++)
      if (m_layout[qq] < 0)
    {
      m_layout[qq] = next_q;
      next_q++;
    }
    m_allocated_q = (unsigned)next_q;
    printf ("#Allocated: %d - Layout size: %lu\n", m_allocated_q, m_layout.size ());
    fflush (stdout);
    assert (m_allocated_q == m_layout.size ());
  }

  void print_layout ()
  {
    assert (m_options);
    string layout_file (m_options->basename);
    layout_file += ".layout";
    FILE * ff;
    ff = fopen (layout_file.c_str (), "w");
    printf ("\nComputed qubit layout: { ");
    fprintf (ff, "{ ");
    unsigned qq;
    for (qq = 0; qq < m_layout.size (); qq++)
    {
      if (qq > 0)
      {
        printf (", ");
        fprintf (ff, ", ");
      }
      //printf ("(QuantumRegister(%d, 'qr') : %u): %u", m_size, qq, m_layout[qq]);
      printf ("q[%u]: %d", qq, m_layout[qq]);
      fprintf (ff,"q[%u]: %d", qq, m_layout[qq]);
    }
    printf ("}\n\n");
    fprintf (ff, " }");
    fclose (ff);
  }

  void print_check_array ()
  {
    unsigned qq;
    printf ("Showing physical qubit allocation count : ");
    for (qq = 0; qq < m_layout.size (); qq++)
    {
      if (qq > 0)
        printf (", ");
      printf ("q%d (%d)", qq, m_check[qq]);
    }
    printf ("\n\n");
  }

  void reset_layout ()
  {
    m_layout.resize (m_size, -1);
  }

  unsigned get_allocated ()
  {
    return m_allocated_q;
  }

  void update_allocated (unsigned delta)
  {
    //m_allocated_q += delta;
    m_allocated_q = m_xset.size ();
  }

  void update_xset (set<int> & selected)
  {
    set<int>::iterator ss;
    for (ss = selected.begin (); ss != selected.end (); ss++)
      m_xset.insert (*ss);
  }

  set<int> & get_xset ()
  {
    return m_xset;
  }

  vector<int> & get_layout ()
  {
    return m_layout;
  }


  /*
   * Return the device size (number of qubits)
   */
  int get_size () const
  {
    return m_size;
  }

  unsigned get_num_edges () const
  {
    return m_edges;
  }

  int * coupling () const
  {
    return m_coupling;
  }

  const aquma_qubit_vector & qubits () const
  {
    return m_list;
  }

  const aquma_qubit_vector_iterator first_qubit () 
  {
    return m_list.begin ();
  }

  const aquma_qubit_vector_iterator last_qubit ()
  {
    return m_list.end ();
  }

  void add_qubit (aquma_qubit * qubit)
  {
    assert (qubit && "Attempting to pass NULL qubit");
    if (m_size <= 0)
    {
      assert (0 && "Graph must be initialized with number of available qubits.");
    }
    if (!m_coupling)
    {
      m_coupling = new int [m_size * m_size];
    }
    aquma_qubit_vector_iterator qq;
    int row = qubit->my_id ();
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      int qq_id = (*qq)->my_id ();
      if (qubit->is_adjacent (qq_id))
      {
        // Add the two directed edges.
        (*this)(row,qq_id) = m_edges;
        (*this)(qq_id,row) = m_edges;
        m_edges++;
        // Update incidence count per qubit.
        m_vcount[row] ++;
        m_vcount[qq_id] ++;
      }
    }
    m_list.push_back (qubit->deep_copy (true));
    printf ("Added qubit %d\n", qubit->my_id());
  }

  #define NO_EDGE (-1)
  /*
   * Allocate memory for the m_coupling adjacency matrix.
   * Initialize entries too.
   */
  void allocate (int nq) {
    m_size = nq;
    m_coupling = new int[nq * nq];
    int ii; 
    int jj;
    for (ii = 0; ii < m_size; ii++) {
      for (jj = 0; jj < m_size; jj++) {
        m_coupling[ii * m_size + jj] = NO_EDGE;
      }
    }
  }

  int & operator()(int rr, int cc) {
    return m_coupling[rr * m_size + cc];
  }

  /*
   * Read quantum device topology and physical parameters.
   */
  void read_graph (const char * filename) 
  {
    if (is_ibmq_device (filename))
      read_ibmq_graph (filename);
    FILE * ff;
    int ret;
    this->open_file (filename, ff);
    ret = fscanf (ff, "%d %u", &m_size, &m_edges);
    if (ret < 2) {
      fclose (ff);
      return;
    }
    this->allocate (m_size);
    unsigned ee;
    int edge_id = 0;
    for (ee = 0; ee < m_edges; ee++) {
      int uu, ww;
      ret = fscanf (ff, "%d %d", &uu, &ww);
      if (ret < 2)
        break;
      m_coupling[uu * m_size + ww] = edge_id;
      m_coupling[ww * m_size + uu] = edge_id;
      edge_id ++;
    }
    fclose (ff);
    m_is_partition = false;
    this->to_list ();
    this->convert_to_json (filename);
    this->write_topo_as_python_edge_list (filename);
  }



  bool is_ibmq_device (const char * filename)
  {
    char * loc;
    loc = strstr ((char*)(filename), "ibmq"); 
    return loc != NULL;
  }

  int count_lines_in_file (const char * filename)
  {
    int ret = 0;
    int cc;
    FILE * ff;
    this->open_file (filename, ff);
    while ((cc = fgetc (ff)) != EOF) 
    {
      if (cc == '\n')
        ret++;
    }
    fclose (ff);
    return ret;
  }

  void skip_to_next_line (FILE * ff)
  {
    int cc;
    while ((cc = fgetc(ff)))
    {
      if (cc == '\n')
        break;
      if (cc == EOF)
        break;
    }
  }
    
  /*
   * Read IBM quantum device topology and physical parameters.
   */
  void read_ibmq_graph (const char * filename)
  {
    printf ("Reading ibm file %s\n", filename);
    this->m_size = this->count_lines_in_file (filename) - 1;
    FILE * ff;
    this->open_file (filename, ff);
    this->allocate (m_size);
    int qq;
    int edge_id = 0;
    this->skip_to_next_line (ff);
    for (qq = 0; qq < this->m_size; qq++)
    {
      printf ("Reading qubit %d / %d\n", qq, this->m_size);
      aquma_qubit * qu = new aquma_qubit (this->m_size);
      qu->read_qubit_physical_parameters (ff, this->m_coupling, edge_id);
      qu->print_qubit_parameters ();
      this->m_list.push_back (qu);
    }
    printf ("Done reading physical parameters\n");
    fclose (ff);
    m_is_partition = false;
    this->convert_to_json (filename);
    this->write_topo_as_python_edge_list (filename);

    m_check.resize (m_size, 0);
  }

  /*
   * Build an ISL topology map from the list of vectors.
   */
  string build_isl_topo_map_str_internal ()
  {
    if (m_list.size () <= 0)
      return "ERROR";
    string ret = "";
    int uu;
    set<int> links;
    // Initialize the set of all links in grid form.
    for (uu = 0; uu < m_size; uu++)
    {
      int vv;
      for (vv = 0; vv < m_size; vv++)
      {
        int eid = uu * m_size + vv;
        links.insert (eid);
      }
    }

    vector<aquma_qubit*>::iterator qq;
    // Query the qubits, remove them from the links set,
    // and append the new string.
    char buf[21];
    set<int>::iterator vv;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      aquma_qubit * qu = (aquma_qubit*)(*qq);
      set<int> & adj = qu->neighbors ();
      int src = qu->my_id ();
      string src_str = std::to_string (src);
      assert (src <= 9999 && "Can only handle up to 9999 qubits.");
      for (vv = adj.begin (); vv != adj.end (); vv++)
      {
        int tgt = *vv;
        assert (tgt <= 9999 && "Can only handle up to 9999 qubits.");
        int eid = src * m_size + tgt;
        string tgt_str = std::to_string (tgt);
        links.erase (eid);
        // Format: "[q1,q2] -> (1); " -> Each q1: 4 bytes -> 4*2+12 +1 = 21
        sprintf (buf,"[%d,%d] -> (1); ", src, tgt);
        ret += string(buf);
      }
    }
    
    for (vv = links.begin (); vv != links.end (); vv++)
    {
      int src = *vv / m_size;
      int tgt = *vv % m_size;
      sprintf (buf,"[%d,%d] -> (0); ", src, tgt);
      ret += string(buf);
    }
    ret = "{" + ret + "}";
    return ret;
  }

  string build_isl_topo_map_str ()
  {
    string ret = "";
    isl_ctx * ctx;
    ctx = isl_ctx_alloc ();
    string qpol_str = this->build_isl_topo_map_str_internal ();
    isl_pw_qpolynomial * pwq;
    pwq = isl_pw_qpolynomial_read_from_str (ctx, qpol_str.c_str ());
    pwq = isl_pw_qpolynomial_coalesce (pwq);
    char * nicestr;
    nicestr = isl_pw_qpolynomial_to_str (pwq);
    isl_pw_qpolynomial_free (pwq);
    isl_ctx_free (ctx);
    ret = string (nicestr);
    free (nicestr);
    return ret;
  }

  /*
   * Dump the device topology to a .json file, as those used
   * by Enfield.
   */
  void convert_to_json (const char * filename)
  {
    FILE * ff;
    char * jname = (char*)malloc(strlen(filename) + 10);
    sprintf (jname,"%s.json", (char*)filename);
    ff = fopen (jname, "w");
    fprintf (ff,"{\n");
    fprintf (ff,"\t\"qubits\": %d,\n",m_size);
    fprintf (ff,"\t\"registers\": [ {\"name\": \"q\", \"qubits\":%d} ],\n", m_size);
    fprintf (ff,"\t\"adj\": [\n");

    int ii;
    for (ii = 0; ii < m_size; ii++) 
    {
      fprintf (ff,"\t  [ ");
      int edge_count = 0;
      set<int> & adj = m_list[ii]->neighbors ();
      set<int>::iterator ss;
      for (ss = adj.begin (); ss != adj.end (); ss++)
      {
        int val = *ss;
        {
          if (edge_count > 0)
            fprintf (ff,", ");
          fprintf (ff," { \"v\": \"q[%d]\" }", val);
          edge_count++;
        }
      }
      fprintf (ff," ] ");
      if (ii != m_size - 1)
        fprintf (ff, ",");
      fprintf (ff,"\n");
    }
    fprintf (ff,"\t]\n");
    fprintf (ff,"}");
    fclose (ff);
    free (jname);
  }

  /*
   * Print the topology and gate physical properties in the format
   * friendly to OLSQ.
   */
  void write_topo_as_python_edge_list (const char * filename)
  {
    int ii;
    vector<float> g2f; // 2-qubit gate fidelity
    vector<float> g1f; // 1-qubit gate fidelity
    vector<float> g1r; // 1-qubit gate reading
    vector<float>::iterator ee;
    vector<std::pair<int,int>> elist;
    vector<std::pair<int,int>>::iterator ll;

    for (ii = 0; ii < m_size; ii++) 
    {
      aquma_qubit * qq = m_list[ii];
      qq->collect_edges (elist);
      qq->collect_g2_error (g2f);
      qq->collect_g1_error (g1f);
      qq->collect_g1_read (g1r);
    }

    FILE * ff;
    char * jname = (char*)malloc(strlen(filename) + 10);
    sprintf (jname,"%s.py", (char*)filename);
    ff = fopen (jname, "w");
    
    int cc;
    fprintf (ff,"self.list_qubit_edge = [");
    for (ll = elist.begin (), cc = 0; ll != elist.end (); ll++, cc++)
    {
      if (cc > 0)
        fprintf (ff, ", ");
      fprintf (ff, "(%d, %d)", ll->first, ll->second);
    }
    fprintf (ff," ]");
    fprintf (ff,"\n\n");

    fprintf (ff,"self.list_fidelity_two = [");
    for (ee = g2f.begin (), cc = 0; ee != g2f.end (); ee++, cc++)
    {
      if (cc > 0)
        fprintf (ff, ", ");
      fprintf (ff, "%f", *ee);
    }
    fprintf (ff," ]");
    fprintf (ff,"\n\n");

    fprintf (ff,"self.list_fidelity_single = [");
    for (ee = g1f.begin (), cc = 0; ee != g1f.end (); ee++, cc++)
    {
      if (cc > 0)
        fprintf (ff, ", ");
      fprintf (ff, "%f", *ee);
    }
    fprintf (ff," ]\n\n");

    fprintf (ff,"self.list_fidelity_measure = [");
    for (ee = g1r.begin (), cc = 0; ee != g1r.end (); ee++, cc++)
    {
      if (cc > 0)
        fprintf (ff, ", ");
      fprintf (ff, "%f", *ee);
    }
    fprintf (ff," ]\n\n");

    fclose (ff);
    free (jname);
  }


  void show_graph () {
    int ii; 
    int jj;
    printf ("Connectivity Matrix:\n");
    for (ii = 0; ii < m_size; ii++) {
      printf ("%d) ", ii);
      for (jj = 0; jj < m_size; jj++) 
      {
        int val = m_coupling[ii * m_size + jj];
        if (val >= 0)
          printf (" e%d", m_coupling[ii * m_size + jj]); 
        else
          printf (" / ");
      }
      printf ("\n");
    }
  }

  void print_dot_file ()
  {
    assert (m_options);
    string device_file (m_options->device_file);
    device_file += ".dot";
    FILE * ff;
    ff = fopen (device_file.c_str (), "w");
    int ii; 
    int jj;
    fprintf (ff,"graph {\n");
    for (ii = 0; ii < m_size - 1; ii++) {
      for (jj = ii+1; jj < m_size; jj++) 
      {
        int val = m_coupling[ii * m_size + jj];
        if (val >= 0)
        {
          fprintf (ff,"q%d -- q%d;\n", ii, jj);
        }
      }
    }
    fprintf (ff,"}\n");
    fclose (ff);
  }

  void show_list () {
    int ii; 
    printf ("Adjacency Lists:\n");
    for (ii = 0; ii < m_size; ii++) {
      m_list[ii]->print_qubit ();
    }
    printf ("\n");
  }

  void show_list (list<aquma_qubit*> & ext_list) {
    aquma_qubit_list_iterator uu;
    printf ("Adjacency Lists:\n");
    for (uu = ext_list.begin (); uu != ext_list.end (); uu++)
    {
      (*uu)->print_qubit ();
    }
    printf ("\n");
  }

  /*
   * Populate the m_list attribute from the coupling graph (matrix).
   * Assume that an edge exists if the value in the coupling matrix is >= 0.
   */
  void to_list () {
    if (m_list.size () > 0)
      return;
    int uu;
    for (uu = 0; uu < m_size; uu++) {
      int ww;
      aquma_qubit * qq;
      qq = new aquma_qubit (uu, m_size);
      for (ww = 0; ww < m_size; ww++) 
        if (m_coupling[uu * m_size + ww] >= 0)
          qq->add (ww);
      m_list.push_back (qq);
    }
  }

  /*
   * Sort the m_list vector of qubits in non-descending order of the
   * nodes incidence order.
   */
  void sort () {
    if (m_list.empty ())
      this->to_list ();      
    std::sort (m_list.begin (), m_list.end (), qubit_less_than );
  }

  __isl_give isl_set * isl_qubit_set (isl_ctx * ctx)
  {
    isl_set * ret;
    string cst = "";
    cst += "{ q[i] : 0 <= i < ";
    cst += std::to_string (m_size);
    cst += "};";
    ret = isl_set_read_from_str (ctx, cst.c_str ());
    return ret;
  }

  void show_partition (set<aquma_qubit*> & part)
  {
    printf ("Graph Partition ==> %lu parts\n", part.size ());
    set<aquma_qubit*>::iterator uu;
    for (uu = part.begin (); uu != part.end (); uu++)
    {
      (*uu)->print_qubit ();
    }
    printf ("\n");
  }

  void show_path (aquma_qubit_list & part)
  {
    printf ("Longest Path (%lu)\n", part.size ());
    aquma_qubit_list_iterator uu;
    for (uu = part.begin (); uu != part.end (); uu++)
    {
      (*uu)->print_qubit ();
    }
    printf ("\n");
  }

  void show_int_set (const set<int> & intset)
  {
    set<int>::iterator ii;
    for (ii = intset.begin (); ii != intset.end (); ii++)
    {
      printf ("[%d] ", *ii); 
    }
    printf ("\n");
  }

  void do_partition_v1 (unsigned deg_k) {
    list<aquma_qubit*> pending;
    set<aquma_qubit*> leads;
    set<int> done;
    aquma_qubit_vector_iterator vv;
    printf ("Deep copying ...\n");
    for (vv = m_list.begin (); vv != m_list.end (); vv++)
      pending.push_back ((*vv)->deep_copy ());

    pending.sort (qubit_less_than);
    printf ("Starting partitioning |%lu|\n", pending.size ());
    while (1) {
      if (pending.size () < deg_k + 1)
        break;

      printf ("Attempting to find partition number %lu (left %lu)\n", leads.size (), pending.size ());
      bool found = false;

      aquma_qubit_list_iterator uu;
      for (uu = pending.begin (); not found and uu != pending.end (); uu++)
        if ((*uu)->size () >= deg_k) 
      {
        // Remove lead and neighbors from pending.
        // Let the function to also update the done set.
        aquma_qubit * newlead = (*uu)->deep_copy (false);
        assert (newlead);
        printf ("Selected qubit %d as lead\n", newlead->my_id ());    
        this->show_list (pending);
      
        // Select neighbors of the lead
        //printf ("Selecting neighbors... done set:");
        //show_int_set (done); 

        found = newlead->select_neighbors_v1 (deg_k, uu, pending, done);

        if (found)
        {
          //printf ("Removing lead (%d) from pending...\n", newlead->my_id ());
          pending.erase (uu);

          // Adding lead to leads
          leads.insert (newlead);

        }
        else
        {
          // Current candidate wasn't viable, likely because its neighbors
          // were already in the done list.
          delete newlead;
        }

      }

      // Didn't find any viable candidate, so break
      if (not found)
        break;
    
      pending.sort (qubit_less_than);
    }
    this->show_partition (leads);
  }

  void do_partition_v2 (unsigned deg_k) {
    list<aquma_qubit*> pending;
    set<aquma_qubit*> leads;
    set<int> done;
    aquma_qubit_vector_iterator vv;
    printf ("Deep copying ...\n");
    for (vv = m_list.begin (); vv != m_list.end (); vv++)
      pending.push_back ((*vv)->deep_copy ());

    pending.sort (qubit_less_than);
    printf ("Starting partitioning |%lu|\n", pending.size ());
    while (1) {
      if (pending.size () < deg_k + 1)
        break;

      printf ("Attempting to find partition number %lu (left %lu)\n", leads.size (), pending.size ());

      aquma_qubit_list_iterator uu;

      aquma_qubit * newlead;
      assert (newlead);
      for (uu = pending.begin (); uu != pending.end (); uu++)
        if ((*uu)->size () >= deg_k) 
      {
        newlead = (*uu)->deep_copy ();
        pending.erase (uu);
        break;
      }
      // Remove the lead from all possible qubits left in pending.
      for (uu = pending.begin (); uu != pending.end (); uu++)
        if (newlead->is_adjacent ((*uu)->my_id ()))
      {
        (*uu)->remove (newlead->my_id ());
      }
      pending.sort (qubit_less_than);
      bool found = false;

      // Remove lead and neighbors from pending.
      // Let the function to also update the done set.
      printf ("Selected qubit %d as lead\n", newlead->my_id ());    
      this->show_list (pending);
      
      // Select neighbors of the lead
      //printf ("Selecting neighbors... done set:");
      //show_int_set (done); 

      found = newlead->select_neighbors (deg_k, pending, done);

      if (found)
      {
        //printf ("Removing lead (%d) from pending...\n", newlead->my_id ());
        // Adding lead to leads
        leads.insert (newlead);
      }
      else
      {
        // Current candidate wasn't viable, likely because its neighbors
        // were already in the done list.
        delete newlead;
      }

      // Didn't find any viable candidate, so break
      if (not found)
        break;
    
      pending.sort (qubit_less_than);
    }
    this->show_partition (leads);
  }

  void find_longest_path () 
  {
    printf ("Searching for longest path...\n");
    aquma_qubit_list pending;
    aquma_qubit_list lpath;
    set<int> done;
    aquma_qubit_vector_iterator vv;
    printf ("Deep copying ...\n");
    for (vv = m_list.begin (); vv != m_list.end (); vv++)
      pending.push_back ((*vv)->deep_copy ());

    printf ("Finding Longest Path |%lu|\n", pending.size ());

    aquma_qubit * front, * back;
    pending.sort (qubit_less_than);

    aquma_qubit_list_iterator ww;   
    // Remove vertices with degree 1
    while (1) 
    {
      front = pending.front ();
      if (front->size () == 1)
        pending.pop_front ();
      else
        break;
    }
    // Get the first vertex with degree 2.
    front = pending.front ();
    printf ("Starting with %d\n", front->my_id ());
    done.insert (front->my_id());
    lpath.push_back (front->deep_copy ());
    pending.pop_front ();
    while (pending.size () > 0)
    {
      int next;
      back = lpath.back ();
      assert (back);
      back->update_connectivity (pending);
      next = back->get_next_fringe (pending, done);
      printf ("Next pick is %d (left = %lu)\n", next, pending.size ());

      if (next < 0)
        break;

      done.insert (next);

      for (ww = pending.begin (); ww != pending.end (); ww++)
        if ((*ww)->my_id () == next)
      {
        //printf ("Deep copying ...\n");
        lpath.push_back ((*ww)->deep_copy ());
        //printf ("Removing...\n");
        pending.remove (*ww);
        break;
      }
      //pending.sort (qubit_less_than);
    }
    // Get a neighbor of the last vertex added that is not in the 
    // @done list.
    back = lpath.back ();
    for (ww = pending.begin (); ww != pending.end (); ww++)
      if (back->is_adjacent ((*ww)->my_id ()))
    {
      lpath.push_back ((*ww)->deep_copy ());
      pending.remove (*ww);
      break;
    }
    this->show_path (lpath);
    show_int_set  (done);
  }
  

  void do_partition (int deg_k) {
    do_partition_v1 (deg_k);
    //do_partition_v2 (deg_k);
  }

  //ilp_system * build_ilp_string (int p_max_nodes, int p_max_edges,
  //  int p_ngoal, int p_igoal, int p_egoal,
  //  vector<int> & vtd)
  ilp_system * build_ilp (int p_ngoal, /*int p_igoal, */ int p_egoal, vector<int> & vtd)
  {
    // vtd : Vector of Target Degrees
    int max_nodes = m_size; //p_max_nodes;
    unsigned max_edges = m_edges; //p_max_edges;
    int nodes_goal = p_ngoal;
    int edges_goal = p_egoal;
    int incidence_goal = 2 * p_egoal; //p_igoal; // April 5th, just duplicate p_egoal
    ilp_system * ilp;
    ilp = new ilp_system (m_options);
    ilp->create_vars (vertex_target_degree_e, vtd.size (), -nodes_goal, 0, true);
    //ilp->create_vars_boolean (vertex_bool_e, max_nodes);
    //ilp->create_vars (vertex_count_e, max_nodes, 0, max_nodes); // check this
    ilp->create_qubit_variables (m_list);
    // Pending to set the true upper bound of the v variables.
    ilp->create_vars_boolean (edge_bool_e, max_edges);
    ilp->create_vars_boolean (vertex_edge_bool_e, max_nodes, max_edges);

    ilp_var * obj_ngoal;
    ilp_var * obj_igoal;
    ilp_var * obj_egoal;
    obj_ngoal = ilp->add_var_front ("Ngoal", objective_min_e, 0, nodes_goal);
    obj_igoal = ilp->add_var_front ("Igoal", objective_min_e, 0, incidence_goal);
    obj_egoal = ilp->add_var_front ("Egoal", objective_min_e, 0, edges_goal);

    //ilp->print_system_variable_list_string ();
    //ilp->print_constraint_string ();

    ilp->clear_ilp ();
    ilp->add_objective (obj_ngoal, vertex_bool_e, nodes_goal);
    ilp->add_objective (obj_egoal, edge_bool_e, edges_goal);
    ilp->add_objective (obj_igoal, vertex_edge_bool_e, incidence_goal);

    vector<int>::iterator xx;
    int kk = 0;
    printf ("vtd:\n");
    for (xx = vtd.begin (); xx != vtd.end (); xx++, kk++)
    {
      cout << "vtd[" << kk << "] : " << *xx << endl;
    }
    printf ("\n");

    ilp->set_lower_bounds (vertex_target_degree_e, vtd, true);
    ilp->add_boolean_to_count_vertex_constraints (max_nodes);
    ilp->add_vertex_degree_count_constraint (m_list, vtd);

    int q1, q2;
    for (q1 = 0; q1 < max_nodes; q1++)
      for (q2 = q1 + 1; q2 < max_nodes; q2++)
    {
      int edge_id = m_coupling[q1 * m_size + q2];
      if (edge_id >= 0)
        ilp->add_connectivity_constraints (q1, q2, edge_id);
    }

    aquma_qubit_vector_iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      ilp->add_incidence_constraint (*qq, m_coupling);
    }

    // Call this last since we update some bounds up to the last minute.
    ilp->add_basic_constraints ();

    ilp->print_ilp ();

    //ilp->solve_ilp ();

    return ilp;
  }

  void compute_layout (graph_properties * gprop, aquma_graph * device)
  {
    //vector<int> & global_layout, unsigned pre_assigned_q)
    ilp_system * ilp;
    vector<int> target_degrees;
    qmap_count_iterator qq;
    target_degrees.resize (m_size, 0);
    for (qq = gprop->qmc.begin (); qq != gprop->qmc.end (); qq++)
    {
      target_degrees[qq->first] = qq->second;
      printf ("Setting V[%d] to %d\n", qq->first, qq->second);
    }
    fflush (stdout);

    ilp = this->build_ilp (gprop->n_nodes, gprop->n_edges, target_degrees);
    ilp->print_ilp_to_cplex_script ();

    ilp->print_ilp ();

    //ilp->solve_ilp (); // solve with ISL
    ilp->solve_with_cplex ();

    int delta_q;
    delta_q = ilp->update_global_layout (device->get_layout (), 
      device->get_allocated (), device->get_xset ());
    device->update_allocated (delta_q);

    delete ilp;
  }





  aquma_qubit * find_landmark ()
  {
    aquma_qubit_vector_iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
      if ((*qq)->size () == 1)
        return (*qq);
    return NULL;
  }

  bool meets_partition_requirements (graph_properties * gprop, 
    bool ignore_edges, bool dump_status = false)
  {
    qmap_count current;
    unsigned uu;
    for (uu = 0; uu < m_vcount.size (); uu++)
    {
      qmap_update (current, m_vcount[uu]); 
    }
    if (not dump_status)
    {
      if (not m_is_partition)
        return false;
      if (m_list.size () < gprop->n_nodes)
        return false;
      if (m_edges < gprop->n_edges and not ignore_edges)
        return false;
      //return qmap_geq (gprop->qmc, current);
      return qmap_geq (gprop->vid, m_vcount);
    }
    fflush (stderr);
    printf ("Partition status: is_par=%d, q_size=%lu/%u, q_edges=%u/%u\n", m_is_partition, m_list.size (), gprop->n_nodes, m_edges, gprop->n_edges);

    for (uu = 0; uu < m_vcount.size (); uu++)
    {
      if (uu > 0)
        printf (", ");
      printf ("#%u=%u/%u ", uu, m_vcount[uu], gprop->vid[uu]);
    }
    printf ("\n");
    fflush (stdout);
    return false;
  }

  bool qubit_used (aquma_qubit * qubit)
  {
    if (m_xset.find (qubit->my_id ()) != m_xset.end ())
      return true;
    return false;
  }

  aquma_graph * gen_partition (qubit_info * qinfo, graph_properties * gprop)
  {
    if (!qinfo)
    {
      printf ("Warning: Attempting to generate partition with NULL qubit_info pointer\n");
      return NULL;
    }
    if (!gprop)
    {
      printf ("Warning: Attempting to generate partition with NULL graph_property pointer\n");
      return NULL;
    }
    aquma_graph * ret = new aquma_graph (m_size, m_options);
    ret->mark_as_partition ();
    if (m_debug)
    {
      ret->show_graph ();
    }
    //int added_qubits = 0;
    int added_edges = 0;

    set<int> done;
    aquma_qubit_list pending;
    aquma_qubit_list selected;
    aquma_qubit_vector_iterator qvi;
    for (qvi = m_list.begin (); qvi != m_list.end (); qvi++)
      if (not this->qubit_used (*qvi))
    {
      pending.push_back ((*qvi)->deep_copy ());
    }

    printf ("Extracting sub-graph partition...\n");

    pending.sort (qubit_greater_than);
    selected.push_back (pending.front ()->deep_copy (true)); // check if original adjacency list should be copied. (Thinking yes)
    ret->add_qubit (pending.front ());
    done.insert (pending.front ()->my_id ());
    pending.pop_front ();
    //selected.front ()->update_connectivity (pending);
    int n_rounds = 0;
    bool result = false;
    while (1)
    {
      unsigned delta_edges;
      if (pending.size () == 0)
        break;
      if (n_rounds >= m_size)
        break;
      aquma_qubit * next = pending.front()->next_maximal_vertex (pending, done, delta_edges);
      int next_qid = next->my_id ();
      added_edges += delta_edges;
      ret->add_qubit (next);
      done.insert (next_qid);
      selected.front ()->remove_qubit (pending, next_qid);
      // pick next qubit to add to the partition, remove it from list
       
      selected.push_back (next->deep_copy (true));
      // Update counts

      // Check if we have met objectives. Start checking once we have enough qubits.
      if (selected.size () >= gprop->n_nodes)
      {
        if (ret->meets_partition_requirements (gprop, false))
        {
          result = true;
          break;
        }
      }
      n_rounds ++;
    }
    if (not result)
    {
      printf ("[INFO] Result of graph partitioning: [FAIL]\n");
      if (ret->meets_partition_requirements (gprop, true, true))
      {
        result = true;
      }
      else
      {
        if (m_debug)
        {
          ret->show_graph ();
        }
        delete ret;
        return NULL;
      }
    }
    printf ("Result of graph partitioning: [SUCCESS]\n");
    // Do not update this set here. Do it from the main loop.
    //this->update_xset (done);
    if (m_debug)
    {
      ret->show_graph ();
    }
    return ret;
  }

  /*
  aquma_graph * partition_driver (qubit_info * qinfo, graph_properties * gprop)
  {
    aquma_graph * ret = new aquma_graph (*this);
    ret->show_graph ();
    ret->gen_partition (qinfo, gprop);
    return ret;
  }
  */

  /*
   * Collect the physical parameters from each 2Q-gate qubit and store them
   * in the corresponding edge map. If the target degree (@td) argument is
   * positive, restrict the selection to only qubits with that degree.
   */
  void collect_edge_properties (int mode, unsigned td = 0) 
  {
    //gatevals.clear ();
    if (mode == GATE2_ERROR)
      this->m_gate2err.clear ();
    if (mode == GATE2_TIME)
      this->m_gate2time.clear ();
    vector<aquma_qubit*>::iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      aquma_qubit * qubit = (*qq);
      if (not qubit->is_2qubit_gate ())
        continue;
      if (qubit->is_mapped ())
        continue;
      if (td > 0 and qubit->size () < td)
        continue;
      assert (qubit);
      if (mode == GATE2_ERROR)
        qubit->append_values_to_map (& m_gate2err, mode);
      if (mode == GATE2_TIME)
        qubit->append_values_to_map (& m_gate2time, mode);
    }
  }

  void average_qubit_parameters ()
  {
    vector<aquma_qubit*>::iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      (*qq)->compute_qubit_parameter_averages_from_edges  ();
    }
  }

  float compute_sum_g2_penalty_error (unsigned td = 0)
  {
    float ret = 0.0f;
    vector<aquma_qubit*>::iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      if ((*qq)->is_mapped ())
        continue;
      if (td > 0 and (*qq)->size () < td)
        continue;
      ret += (*qq)->size () * (*qq)->get_g2_average_error ();
    }
    return ret;
  }

  float compute_sum_g2_penalty_time (unsigned td = 0)
  {
    float ret = 0.0f;
    vector<aquma_qubit*>::iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      if ((*qq)->is_mapped ())
        continue;
      if (td > 0 and (*qq)->size () < td)
        continue;
      ret += (*qq)->size () * (*qq)->get_g2_average_time ();
    }
    return ret;
  }

  //  vector<int> & vtd)
  ilp_system * build_fidelity_ilp (
    int p_ngoal, int p_egoal, vector<int> & vtd)
  {
    // vtd : Vector of Target Degrees
    int nodes_goal = p_ngoal;
    int edges_goal = p_egoal;

    vector<float> g2error;
    vector<float> g2time;

    cout << "Collecting G2-error properties ..." << endl;
    this->collect_edge_properties (GATE2_ERROR);
    cout << "Collecting G2-time properties ..." << endl;
    this->collect_edge_properties (GATE2_TIME);
    cout << "Averaging 2-gate qubit physical parameters ..." << endl;
    this->average_qubit_parameters ();

    cout << "Proceeding to create ILP ..." << endl;

    fflush (stdout);
    ilp_system * ilp;
    ilp = new ilp_system (m_options, m_size);
    int G2_ERROR_UB = round_g2_error (ilp->sum_from_edge_map (m_gate2err));
    int G2_TIME_UB = round_g2_time (ilp->sum_from_edge_map (m_gate2time));
    int G2_ERR_PNTY_UB = round_g2_error (this->compute_sum_g2_penalty_error ());
    int G2_TIME_PNTY_UB = round_g2_time (this->compute_sum_g2_penalty_time ());

    cout << "G2 ERROR UB : " << G2_ERROR_UB << endl;
    cout << "G2 TIME UB : " << G2_TIME_UB << endl;

    ilp->create_vars_boolean_from_edge_map (edge_bool_e, m_gate2err);
    /*
     * Create 'vertex-target-degree' variables. Each td_{k} represents
     * the number of desired nodes with incidence 'k'. 
     */
    ilp->create_vars (vertex_target_degree_e, vtd.size (), -nodes_goal, 0, true);
    ilp->create_qubit_variables (m_list);
    //ilp->create_link_variables (m_gate2err, m_size);
    

    // Pending to set the true upper bound of the v variables.
    //ilp->create_vars_boolean (edge_bool_e, max_edges);
    //ilp->create_vars_boolean (vertex_edge_bool_e, max_nodes, max_edges);

    ilp_var * obj_ngoal;
    //ilp_var * obj_igoal;
    ilp_var * obj_egoal;
    ilp_var * obj_g2error;
    ilp_var * obj_g2time;
    ilp_var * obj_penalty_g2err;
    ilp_var * obj_penalty_g2time;

    obj_ngoal = ilp->add_var_front ("Ngoal", objective_min_e, -nodes_goal, 0);
    //obj_igoal = ilp->add_var_front ("Igoal", objective_min_e, 0, incidence_goal);
    obj_g2time = ilp->add_var_front ("G2time", objective_min_eq_e, 0, G2_TIME_UB);
    obj_penalty_g2time = ilp->add_var_front ("PeG2time", objective_min_eq_e, 0, G2_TIME_PNTY_UB);
    obj_penalty_g2err = ilp->add_var_front ("PeG2err", objective_min_eq_e, 0, G2_ERR_PNTY_UB);
    obj_g2error = ilp->add_var_front ("G2err", objective_min_eq_e, 0, G2_ERROR_UB);
    obj_egoal = ilp->add_var_front ("Egoal", objective_min_e, -edges_goal, 0);

    //ilp->print_system_variable_list_string ();
    ilp->print_constraint_string ();


    ilp->clear_ilp ();
    ilp->add_objective (obj_ngoal, vertex_bool_e, 0);
    ilp->add_objective (obj_egoal, edge_bool_e, 0);
    //ilp->add_objective (obj_igoal, edge_bool_e /*vertex_edge_bool_e*/, incidence_goal);

    ilp->add_weighted_objective (obj_g2error, edge_bool_e, m_gate2err, m_list, GATE2_ERROR, 0);
    ilp->add_weighted_objective (obj_g2time, edge_bool_e, m_gate2time, m_list, GATE2_TIME, 0);
    ilp->add_g2_penalty_objective (obj_penalty_g2err, m_list, GATE2_ERROR, 0);
    ilp->add_g2_penalty_objective (obj_penalty_g2time, m_list, GATE2_TIME, 0);

    vector<int>::iterator xx;
    int kk = 0;
    printf ("vtd:\n");
    for (xx = vtd.begin (); xx != vtd.end (); xx++, kk++)
    {
      cout << "vtd[" << kk << "] : " << *xx << endl;
    }
    printf ("\n");

    ilp->set_lower_bounds (vertex_target_degree_e, vtd, true);
    ilp->add_vertex_degree_count_constraint (m_list, vtd);

    // Below we link boolean edge variables to boolean node variables.
    // If an edge is used, it will bump up the corresponding node variable.
    edge_map::iterator ee;
    for (ee = m_gate2err.begin (); ee != m_gate2err.end (); ee++)
    {
      int q1, q2;
      q1 = ilp->get_source_qubit_id_from_edge (ee->first);
      q2 = ilp->get_target_qubit_id_from_edge (ee->first);
      int edge_id = ee->first;
      ilp->add_connectivity_constraints (q1, q2, edge_id);
    }

    aquma_qubit_vector_iterator qq;
    // Reminder: are used entries in m_coupling >=0 or > 0?
    // Below we link v_k (counting node variables) to the sum of the boolean
    // node variables.
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      ilp->add_incidence_constraint (*qq);
      ilp->add_boolean_to_count_vertex_constraints (*qq);
    }

    // Call this last since we update some bounds up to the last minute.
    // Adds typed and semantic fixed bounds to variables.
    ilp->add_basic_constraints ();


    ilp->print_variables ();
    ilp->print_constraints_debug ();

    ilp->print_ilp ();

    //ilp->solve_ilp ();

    return ilp;
  }

  void collect_all_qubit_incidence_degree (aquma_circuit * circuit, vector<int> & degrees)
  {
    unsigned qq;
    // Compute the maximum incidence among all gates.
    int max_degree = 0;
    for (qq = 0; qq < (unsigned)(m_size); qq++)
    {
      int count = circuit->get_ideal_out_degree (qq);
      max_degree = (count > max_degree ? count : max_degree);
    }
    max_degree += 2;
    degrees.resize (max_degree, 0);
    for (qq = 0; qq < (unsigned)(m_size); qq++)
    {
      int count = circuit->get_ideal_out_degree (qq);
      assert (count < max_degree);
      printf ("Q[%d] = %d : TD = %d\n", qq, count, degrees[count]);
      degrees[count] ++;
    }
    for (qq = 0; qq < (unsigned)(max_degree); qq++)
    {
      printf ("Setting target_degree[%d] = %d\n", qq, degrees[qq]);
    }
    fflush (stdout);
  }

  void compute_non_poly_layout (aquma_graph * device, aquma_circuit * circuit)
  {
    ilp_system * ilp;
    vector<int> target_degrees;
    this->collect_all_qubit_incidence_degree (circuit, target_degrees);

    // Need to set this here!
    m_edges = circuit->get_total_distinct_dir_edge_count ();
    unsigned node_target = circuit->get_number_distinct_qubits_in_2Q_gates ();
    unsigned edge_target = m_edges; //circuit->get_total_dir_edge_count ();

    cout << "G2 count : " << node_target << endl;
    cout << "Edge target : " << edge_target << endl;
    
    ilp = this->build_fidelity_ilp (node_target, edge_target, target_degrees);

    if (this->use_isl_solver ())
    {
      ilp->solve_ilp (); // solve with ISL
    }

    if (this->use_cplex_solver ())
    {
      ilp->print_ilp_to_cplex_script ();
      ilp->solve_with_cplex ();
    }

    int delta_q;
    delta_q = ilp->update_global_layout (device->get_layout (), 
      device->get_allocated (), device->get_xset ());
    device->update_allocated (delta_q);

    delete ilp;
  }

  /*
   * Build an ILP to minimize error and time, but restricted to a subset
   * of qubits with some required target degree (incidence).
   */
  ilp_system *
  build_iterative_ilp (unsigned node_goal, unsigned edge_target, int kk, int td)
  {
    cout << "Collecting G2-error properties (RESTRICTED) ..." << endl;
    this->collect_edge_properties (GATE2_ERROR, kk);
    if (m_gate2err.size () == 0)
      this->collect_edge_properties (GATE2_ERROR, 0);
    cout << "Collecting G2-time properties (RESTRICTED) ..." << endl;
    this->collect_edge_properties (GATE2_TIME, kk);
    if (m_gate2time.size () == 0)
      this->collect_edge_properties (GATE2_TIME, 0);
    cout << "Averaging 2-gate qubit physical parameters ..." << endl;
    this->average_qubit_parameters ();

    cout << "Proceeding to create ILP ..." << endl;

    fflush (stdout);
    ilp_system * ilp;
    ilp = new ilp_system (m_options, m_size);
    int G2_ERROR_UB = round_g2_error (ilp->sum_from_edge_map (m_gate2err));
    int G2_TIME_UB = round_g2_time (ilp->sum_from_edge_map (m_gate2time));

    cout << "G2 ERROR UB : " << G2_ERROR_UB << endl;
    cout << "G2 TIME UB : " << G2_TIME_UB << endl;

    ilp->create_vars_boolean_from_edge_map (edge_bool_e, m_gate2err);
    /*
     * Create 'vertex-target-degree' variables. Each td_{k} represents
     * the number of desired nodes with incidence 'k'. 
     */
    ilp->create_one_var (vertex_target_degree_e, kk, -node_goal, 0);

    // The below call needs the original (full) list of qubits in m_list.
    // The method will filter non-candidate qubits internally.
    ilp->create_qubit_variables (m_list, kk);
    //ilp->create_link_variables (m_gate2err, m_size);
    
    ilp_var * obj_ngoal;
    obj_ngoal = ilp->add_var_front ("Ngoal", objective_min_e, -node_goal, 0);

    //ilp_var * obj_penalty_g2time;
    //int G2_TIME_PNTY_UB = round_g2_time (this->compute_sum_g2_penalty_time (kk));
    //obj_penalty_g2time = ilp->add_var_front ("PeG2time", objective_min_eq_e, 0, G2_TIME_PNTY_UB);

    ilp_var * obj_g2time;
    obj_g2time = ilp->add_var_front ("G2time", objective_min_eq_e, 0, G2_TIME_UB);

    //ilp_var * obj_penalty_g2err;
    //int G2_ERR_PNTY_UB = round_g2_error (this->compute_sum_g2_penalty_error (kk));
    //obj_penalty_g2err = ilp->add_var_front ("PeG2err", objective_min_eq_e, 0, G2_ERR_PNTY_UB);

    ilp_var * obj_g2error;
    obj_g2error = ilp->add_var_front ("G2err", objective_min_eq_e, 0, G2_ERROR_UB);

    ilp_var * obj_egoal;
    obj_egoal = ilp->add_var_front ("Egoal", objective_min_e, -edge_target, 0);

    ilp->clear_ilp ();

    ilp->add_objective (obj_ngoal, vertex_bool_e, 0);
    ilp->add_objective (obj_egoal, edge_bool_e, 0);
    //ilp->add_objective (obj_igoal, edge_bool_e /*vertex_edge_bool_e*/, incidence_goal);

    aquma_qubit_vector q_restricted;
    aquma_qubit_vector_iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
      if ((*qq)->is_iteration_candidate (kk))
    {
      q_restricted.push_back ((*qq)); 
    }
    // NOTE: The next two objectives will use all edge variables previously created.
    // Hence the importance of previously restricting their creation to the
    // required target degree.
    ilp->add_weighted_objective (obj_g2error, edge_bool_e, m_gate2err, q_restricted, GATE2_ERROR, 0);
    ilp->add_weighted_objective (obj_g2time, edge_bool_e, m_gate2time, q_restricted, GATE2_TIME, 0);
    //ilp->add_g2_penalty_objective (obj_penalty_g2err, q_restricted, GATE2_ERROR, 0);
    //ilp->add_g2_penalty_objective (obj_penalty_g2time, q_restricted, GATE2_TIME, 0);

    //ilp->set_lower_bounds (vertex_target_degree_e, vtd, true);
    ilp->add_one_vertex_degree_count_constraint (q_restricted, kk, td);

    // Below we link boolean edge variables to boolean node variables.
    // If an edge is used, it will bump up the corresponding node variable.
    edge_map::iterator ee;
    for (ee = m_gate2err.begin (); ee != m_gate2err.end (); ee++)
    {
      int q1, q2;
      q1 = ilp->get_source_qubit_id_from_edge (ee->first);
      q2 = ilp->get_target_qubit_id_from_edge (ee->first);
      int edge_id = ee->first;
      ilp->add_connectivity_constraints (q1, q2, edge_id);
    }

    // Reminder: are used entries in m_coupling >=0 or > 0?
    // Below we link v_k (counting node variables) to the sum of the boolean
    // node variables.
    for (qq = q_restricted.begin (); qq != q_restricted.end (); qq++)
    {
      ilp->add_incidence_constraint (*qq);
      ilp->add_boolean_to_count_vertex_constraints (*qq);
    }

    // Call this last since we update some bounds up to the last minute.
    // Adds typed and semantic fixed bounds to variables.
    ilp->add_basic_constraints ();

    ilp->print_variables ();
    ilp->print_constraints_debug ();

    ilp->print_ilp ();

    return ilp;
  }

  void update_mapped_qubits (set<int> & mapped)
  {
    aquma_qubit_vector_iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      aquma_qubit * qu = (*qq);
      if (mapped.find (qu->my_id ()) != mapped.end ())
        qu->set_mapped ();
    }
  }


  int count_2Q_with_incidence_K (unsigned incidence)
  {
    aquma_qubit_vector_iterator qq;
    int ret = 0;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
      if ((*qq)->size () >= incidence)
    {
      ret ++;
    }
    return ret;
  }


  void compute_iterative_layout (aquma_graph * device, aquma_circuit * circuit)
  {
    vector<int> target_degrees;
    this->collect_all_qubit_incidence_degree (circuit, target_degrees);

    m_edges = circuit->get_total_distinct_dir_edge_count ();

    vector<int>::iterator td;
    unsigned kk;
    int ii;
    for (kk = target_degrees.size () - 1, ii = 1; kk > 1; kk--)
    {
      // kk is the incidence degree required for each potential qubit.
      int td = target_degrees[kk];
      if (td <= 0)
        continue;

      cout << endl << "[Iteration # " << ii << " ]: Degree=" << kk << ", Card=" << td << endl;

      ilp_system * ilp;
      // The below is only needed for the upper bound of selected qubits and edges.
      // So it's ok to use the full list of qubits.
      //unsigned node_target = this->count_2Q_with_incidence_K (kk);
      //unsigned edge_target = kk * node_target;
      unsigned node_target = circuit->get_number_distinct_qubits_in_2Q_gates ();
      unsigned edge_target = m_edges; //circuit->get_total_dir_edge_count ();
      ilp = this->build_iterative_ilp (node_target, edge_target, kk, td);

      if (this->use_isl_solver ())
      {
        ilp->solve_ilp (); // solve with ISL
      }

      if (this->use_cplex_solver ())
      {
        ilp->print_ilp_to_cplex_script ();
        ilp->solve_with_cplex ();
      }
      int delta_q;
      delta_q = ilp->update_global_layout (device->get_layout (), 
        device->get_allocated (), device->get_xset ());
      device->update_allocated (delta_q);

      this->update_mapped_qubits (device->get_xset ());

      delete ilp;

      if (m_xset.size () == m_list.size ())
        break;

      ii++;
    }
  }

  ilp_system * 
  build_fast_ilp (aquma_qubit * qubit, set<int> & frontier)
  {
    ilp_system * ilp;
    ilp = new ilp_system (m_options, m_size);

    #if 0
    cout << "Collecting G2-error properties (RESTRICTED) ..." << endl;
    this->collect_edge_properties (GATE2_ERROR, kk);
    if (m_gate2err.size () == 0)
      this->collect_edge_properties (GATE2_ERROR, 0);
    cout << "Collecting G2-time properties (RESTRICTED) ..." << endl;
    this->collect_edge_properties (GATE2_TIME, kk);
    if (m_gate2time.size () == 0)
      this->collect_edge_properties (GATE2_TIME, 0);
    cout << "Averaging 2-gate qubit physical parameters ..." << endl;
    this->average_qubit_parameters ();

    cout << "Proceeding to create ILP ..." << endl;

    fflush (stdout);
    int G2_ERROR_UB = round_g2_error (ilp->sum_from_edge_map (m_gate2err));
    int G2_TIME_UB = round_g2_time (ilp->sum_from_edge_map (m_gate2time));
    int G2_ERR_PNTY_UB = round_g2_error (this->compute_sum_g2_penalty_error (kk));
    int G2_TIME_PNTY_UB = round_g2_time (this->compute_sum_g2_penalty_time (kk));

    cout << "G2 ERROR UB : " << G2_ERROR_UB << endl;
    cout << "G2 TIME UB : " << G2_TIME_UB << endl;
    #endif

    //ilp->create_vars_boolean_from_edge_map (edge_bool_e, m_gate2err);

    /*
     * Create 'vertex-target-degree' variables. Each td_{k} represents
     * the number of desired nodes with incidence 'k'. 
     */

    cout << "Creating vertex (qubit) vars ..." << endl;

    // Will instantiate boolean and R-score variables for each qubit not
    // yet mapped.
    ilp->create_boolean_vars (m_list);
    int rscore_ubound = ilp->create_rscore_vars (m_list);
    
    ilp_var * obj_ngoal;
    ilp_var * obj_rscore;
    //ilp_var * obj_egoal;
    //ilp_var * obj_g2error;
    //ilp_var * obj_g2time;
    //ilp_var * obj_penalty_g2err;
    //ilp_var * obj_penalty_g2time;

    int node_goal = 1;

    obj_rscore = ilp->add_var_front ("Rscore", objective_min_e, -rscore_ubound, 0);
    obj_ngoal = ilp->add_var_front ("Ngoal", objective_min_eq_e, -node_goal, 0);
    //obj_penalty_g2time = ilp->add_var_front ("PeG2time", objective_min_eq_e, 0, G2_TIME_PNTY_UB);
    //obj_g2time = ilp->add_var_front ("G2time", objective_min_eq_e, 0, G2_TIME_UB);
    //obj_penalty_g2err = ilp->add_var_front ("PeG2err", objective_min_eq_e, 0, G2_ERR_PNTY_UB);
    //obj_g2error = ilp->add_var_front ("G2err", objective_min_eq_e, 0, G2_ERROR_UB);
    //obj_egoal = ilp->add_var_front ("Egoal", objective_min_e, -edge_target, 0);

    ilp->clear_ilp ();

    cout << "Adding objectives ..." << endl;

    ilp->add_objective (obj_ngoal, vertex_bool_e, 0);
    //ilp->add_objective (obj_egoal, edge_bool_e, 0);

    cout << "Adding R-Score objectives ..." << endl;
    aquma_qubit_vector_iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      aquma_qubit * qu = (*qq);
      if (qu->is_mapped ())
        continue;
      ilp->add_rscore_objective (qu);
      ilp->add_rscore_qubit_bound (obj_rscore, qu);
    }
    #if 0
    // NOTE: The next two objectives will use all edge variables previously created.
    // Hence the importance of previously restricting their creation to the
    // required target degree.
    ilp->add_weighted_objective (obj_g2error, edge_bool_e, m_gate2err, q_restricted, GATE2_ERROR, 0);
    ilp->add_weighted_objective (obj_g2time, edge_bool_e, m_gate2time, q_restricted, GATE2_TIME, 0);
    //ilp->add_g2_penalty_objective (obj_penalty_g2err, q_restricted, GATE2_ERROR, 0);
    //ilp->add_g2_penalty_objective (obj_penalty_g2time, q_restricted, GATE2_TIME, 0);

    //ilp->set_lower_bounds (vertex_target_degree_e, vtd, true);
    ilp->add_one_vertex_degree_count_constraint (q_restricted, kk, td);

    // Below we link boolean edge variables to boolean node variables.
    // If an edge is used, it will bump up the corresponding node variable.
    edge_map::iterator ee;
    for (ee = m_gate2err.begin (); ee != m_gate2err.end (); ee++)
    {
      int q1, q2;
      q1 = ilp->get_source_qubit_id_from_edge (ee->first);
      q2 = ilp->get_target_qubit_id_from_edge (ee->first);
      int edge_id = ee->first;
      ilp->add_connectivity_constraints (q1, q2, edge_id);
    }

    // Reminder: are used entries in m_coupling >=0 or > 0?
    // Below we link v_k (counting node variables) to the sum of the boolean
    // node variables.
    for (qq = q_restricted.begin (); qq != q_restricted.end (); qq++)
    {
      ilp->add_incidence_constraint (*qq);
      ilp->add_boolean_to_count_vertex_constraints (*qq);
    }
    #endif

    // Call this last since we update some bounds up to the last minute.
    // Adds typed and semantic fixed bounds to variables.
    ilp->add_basic_constraints ();

    ilp->print_variables ();
    ilp->print_constraints_debug ();

    ilp->print_ilp ();

    return ilp;
  }

  void print_triplet_debug (vector<edge_freq_s> & trip)
  {
    vector<edge_freq_s>::iterator tt;
    for (tt = trip.begin (); tt != trip.end (); tt++)
    {
      //printf ("\t<q%d,q%d> = %d\n", (*tt).q_source, (*tt).q_target, (*tt).freq);
      cout << "\t<q" << (*tt).q_source;
      cout << ",q" << (*tt).q_target;
      cout << "> = " << (*tt).freq;
      cout << endl;
    }
    cout << endl;
    //printf ("\n");
  }

  /*
   * Compute a frequency map for each logical qubit given the full list of qubit
   * pairs accessed in a program, @opfreq. The map will store for each 
   * *PENDING LOGICAL* "source" qubit the "target" qubits used with together with 
   * the usage frequency of each pair.
   * The result is returned in @numap, 
   * @pending is used only for querying the list of pending qubits.
   * Function returns TRUE if @numap contains at least one qubit.
   */
  bool build_qubit_edge_frequency_tree (
    de_counter_t & opfreq, edge_freq_matrix_t & numap, set<int> & pending)
  {
    de_counter_iter_t ee; 
    numap.clear ();
    numap.resize (m_size);
    bool found = false;
    // Convert edge map frequency to triplet format to sort and select.
    for (ee = opfreq.begin (); ee != opfreq.end (); ee++)
    {
      int q_source = (int)(this->edge_to_source_qubit (ee->first));
      int q_target = (int)(this->edge_to_target_qubit (ee->first));

      if (pending.find (q_source) == pending.end ())
        continue;
      if (pending.find (q_target) == pending.end ())
        continue;

      cout << "Considering pair <" << q_source << "," <<  q_target << ">" << endl;

      edge_freq_s triplet;
      triplet.q_source = q_source;
      triplet.q_target = q_target;
      triplet.freq = (int)(ee->second);

      numap[q_source].push_back (triplet); 
      found = true;
    }

    if (not found)
      return false;

    unsigned ii;

    #if 1
    cout << "Triplets prior to sorting: " << endl;
    for (ii = 0; ii < numap.size (); ii++)
      if (numap[ii].size () > 0)
    {
      cout << " Qubit entry " << ii << " : " << endl;
      print_triplet_debug (numap[ii]);
    }
    cout << endl;
    #endif

    for (ii = 0; ii < numap.size (); ii++)
      if (numap[ii].size () > 0)
    {
      sort_qubits_edge_by_frequency (numap[ii]);
    }

    #if 1
    cout << "Triplets sorted to: " << endl;
    for (ii = 0; ii < numap.size (); ii++)
      if (numap[ii].size () > 0)
    {
      print_triplet_debug (numap[ii]);
    }
    cout << endl;
    #endif

    cout << "printing ended " << endl;

    return found;
  }


  /*
   * Compute statistics aggregated per (source) qubit: mean and stdv.
   * Then store the entries in @qws (qubit weight ...)
   *
   */
  bool compute_qubit_edge_statistics (
    de_counter_t & opfreq, edge_freq_matrix_t & numap, 
    float & mean, float & stdv, vector<edge_freq_s> & qws, set<int> & hiq)
  {
    unsigned maxconn = 0;
    aquma_qubit_vector_iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      if ((*qq)->is_mapped ())
        continue;
      unsigned conn = (*qq)->size ();
      maxconn = (conn > maxconn ? conn : maxconn);
    }
    unsigned ii;
    vector<unsigned> bestk;
    bestk.resize (m_list.size (), 0);
    mean = 0.0f;
    unsigned nused = 0;
    for (ii = 0; ii < m_list.size (); ii++)
    {
      unsigned nn = numap[ii].size ();
      if (!nn)
        continue;
      nused ++;
      unsigned jj;
      for (jj = 0; jj < nn && jj < maxconn; jj++)
      {
        bestk[ii] += numap[ii][jj].freq;
        mean += numap[ii][jj].freq;
      }
    }
    if (!nused)
      return false;
    mean = mean / nused;
    stdv = 0.0f;
    for (ii = 0; ii < m_list.size (); ii++)
    {
      float diff = bestk[ii] - mean; 
      stdv += diff * diff;
    }
    stdv = stdv / nused;
    stdv = sqrt (stdv);
    float threshold = mean;  //+ 2 * stdv;
    qws.clear ();
    //qws.resize (m_list.size ());
    cout << "Threshold for logical qubits : " << threshold << endl;
    for (ii = 0; ii < m_list.size (); ii++)
    {
      edge_freq_s qq;
      qq.q_source = -1;
      qq.q_target = -1;
      qq.freq = 0;
      cout << ii << "] : " << bestk[ii] << endl;
      if (bestk[ii] > threshold)
      {
        cout << "Adding qubit " << ii << " with " << bestk[ii] << endl;
        qq.q_source = ii;
        qq.freq = bestk[ii];
        hiq.insert (ii);
      }
      qws.push_back (qq);
    }
    sort_qubits_edge_by_frequency (qws);
    print_qubit_edge_frequency_debug (qws);
    return true;
  }

  /*
   * Select logical qubits to map. Constructs the frequency list of pairs of qubits,
   * sorts them, and selects the one most used.
   * Updated attributes: m_layout, m_xset. 
   * Out arguments: @pending (pending qubits), @recent (physical qubits selected 
   * in current round) and @frontier (logical qubits selected in current round).
   */
  bool map_ops_to_edges (aquma_qubit * qubit, de_counter_t & opfreq,
    vector<edge_freq_s> * qws, // qubit weights
    set<int> * hiq, // high impact (logical) qubits
    set<int> & frontier, // logical qubits mapped from adjacency lists.
    aquma_circuit * circuit, 
    set<int> & pending, // logical set of pending (to allocate) qubits
    set<int> & recent)  // set of physical qubits mapped in current step
  {
    if (!qubit)
      return false;

    if (qubit->is_mapped ())
      return false;

    if (m_check[qubit->my_id ()] > 0)
      return false;

    // Fetch the map storing pairs of <edge_id,freq>
    de_counter_iter_t ee; 
    cout << "Showing contents of opfreq: " << endl;
    for (ee = opfreq.begin (); ee != opfreq.end (); ee++)
    {
      cout << "Opfreq edge : " << ee->first << " repeated " << ee->second << endl;
    }
    cout << endl;

    vector<int> adj;
    // Load qubit id adjacent to selected qubit.
    qubit->load_adjacent_qubits (adj);
    edge_freq_matrix_t numap;
    
    bool res_ft = this->build_qubit_edge_frequency_tree (opfreq, numap, pending);

    if (not res_ft)
    {
      cout << "Failed to construct edge frequency tree..." << endl;
      return false;
    }

    // Find the "best candidate".
    int candidate = -1;
    int highest = 0;
    unsigned ii;

    if (!qws || !hiq)
    {
      // This logical selection is done for "fast (iterative) layout".
      for (ii = 0; ii < numap.size (); ii++)
        if (numap[ii].size () > 0)
      {
        unsigned jj;
        int cost = 0;
        for (jj = 0; jj < qubit->size () && jj < numap[ii].size (); jj++)
        {
          cost += numap[ii][jj].freq; 
        }
        if (cost > highest)
        {
          highest = cost;
          candidate = ii;
        }
      }
    }
    else
    {
      // If we use high-impact qubits, the @qws list is already 
      // sorted. So we just pick the first one that is still pending.
      for (ii = 0; ii < qws->size (); ii++)
      {
        int temp_cand = (*qws)[ii].q_source;
        if (pending.find (temp_cand) == pending.end ())
          continue;
        candidate = temp_cand;
        break;
      }
    }

    if (candidate < 0)
    {
      cout << "Failed to find viable logical candidate..." << endl;
      return false;
    }

    cout << "Chosen lead logical qubit to remap : " << candidate << endl;
   
    // Select as many ops as possible and map the old qubits used
    // to the qubits adjacent to @selected.
    bool added = false;
    for (ii = 0; ii < adj.size () && ii < numap[candidate].size (); ii++)
    {
      int q_source = numap[candidate][ii].q_source;
      assert (q_source == candidate);
      int old_qubit = numap[candidate][ii].q_target;
      int new_qubit = adj[ii];

      if (m_layout[old_qubit] != -1)
        continue;
      if (m_check[new_qubit] != 0)
        continue;
      //if (old_qubit == 64)
      //  assert (0);
      if (m_check[new_qubit] != 0)
      {
        cout << "Attempting to map physical edge (" << qubit->my_id () << "," << new_qubit << ")" << endl;
        assert (m_check[new_qubit] == 0 && "Attempting to choose already allocated qubit.");
      }
      if (m_layout[old_qubit] >= 0)
        continue;
      m_layout[old_qubit] = new_qubit;
      cout << "Remapping qubit " << old_qubit << " to " << new_qubit << endl;

      pending.erase (old_qubit);
      frontier.insert (old_qubit);
      recent.insert (new_qubit);
      m_xset.insert (new_qubit);
      m_check[new_qubit]++;

      added = true;
      //aquma_qubit_vector_iterator qq;
    }

    if (added)
    {
      assert (m_check[qubit->my_id ()] == 0 && "Attempting to choose already allocated qubit.");
      cout << "Mapping logical qubit " << candidate << " to physical qubit " << qubit->my_id () << endl;
      m_layout[candidate] = qubit->my_id ();
      m_check[qubit->my_id ()]++;
      pending.erase (candidate);
      recent.insert (qubit->my_id ());
    }

    return added;
  }

  void initialize_pending_qubits (set<int> & pending, aquma_circuit * circuit)
  {
    de_counter_t & opfreq = circuit->get_edge_counter_map ();
    de_counter_iter_t ee; 
    for (ee = opfreq.begin (); ee != opfreq.end (); ee++)
    {
      unsigned q_source = this->edge_to_source_qubit (ee->first);
      unsigned q_target = this->edge_to_target_qubit (ee->first);
      if (m_layout[q_source] < 0)
        pending.insert (q_source);
      if (m_layout[q_target] < 0)
        pending.insert (q_target);
    }
  }

  /*
   * Compute reciprocals, reset qubit mapped attributes and sort by Q2
   * physical properties.
   */
  void init_qubit_reciprocals ()
  {
    aquma_qubit_vector_iterator qq;
    // Sort qubits by R-Score.
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      (*qq)->compute_reciprocal ();
      (*qq)->reset_mapped ();
    }
    sort_qubits_edge_error_time (m_list);
    this->show_list ();
  }

  /*
   * Search and return the qubit (in attribute m_list) that matches the 
   * given identifier.
   * Also update the found qubit to "mapped".
   */
  aquma_qubit * find_qubit_with_id (int selected)
  {
    aquma_qubit * ret = NULL;
    aquma_qubit_vector_iterator qq;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
      if ((*qq)->my_id () == selected)
    {
      (*qq)->set_mapped ();
      ret = (*qq);
      break;
    }
    return ret;
  }


  /*
   * Update the list of qubits from the set of recently (@recent) assigned qubits.
   * @recent is obtained from map_ops_to_edges ().
   */
  void update_qubit_status_from_recent (set<int> & recent, bool added)
  {
    aquma_qubit_vector_iterator qq;
    for (qq = m_list.begin (); added and qq != m_list.end (); qq++)
    {
      aquma_qubit * qu = (*qq);
      if (recent.find (qu->my_id ()) != recent.end ())
      {
        cout << "Marking qubit " << qu->my_id () << " as used. " << endl;
        qu->set_mapped ();
      }
      // Below: recompute reciprocal and remove allocated qubits from adjacency lists.
      qu->update_properties (recent);
    }
  }

  /*
   * Assign available physical qubits to pending logical qubits after sorting
   * the list of available qubits by their Q1 physical properties.
   */
  void allocate_qubits_by_q1_reciprocal (set<int> * pending)
  {
    if (pending->size () <= 0)
    {
      cout << "[INFO] No logical qubits pending for allocation found." << endl;
      cout << "[INFO] Will complete allocation by Q1-properties. " << endl;
    }
    cout << "Pending to allocate " << pending->size () << " qubits: " << endl;
    set<int>::iterator kk;
    for (kk = pending->begin (); kk != pending->end (); kk++)
    {
      cout << *kk << " ";
    }
    cout << endl << endl;

    aquma_qubit_vector_iterator qq;
    sort_qubits_g1_error_time (m_list);
    cout << "Sorting qubits by G1-properties ... " << endl;
    for (qq = m_list.begin (); qq != m_list.end (); qq++)
    {
      cout << (*qq)->my_id () << "(" << (*qq)->get_g1_score () << ") " << " - mapped = " << (*qq)->is_mapped () << endl;
    }
    cout << " /* end */ " << endl;

    int ll, next = 0;
    #if 0
    for (ll = 0; ll < m_size; ll++)
    {
      printf ("Qubit %d left unmapped (%d)\n", ll, m_layout[ll]);
    }
    #endif

    int lunmapped = 0;
    int pmapped = 0;
    int lcheck = 0;
    for (ll = 0; ll < m_size; ll++)
    {
      if (m_layout[ll] < 0)
        lunmapped++;
      if (m_list[ll]->is_mapped ())
        pmapped++;
      if (m_check[ll] == 1)
        lcheck++;
    }
    cout << "#Logical unmapped qubits : " << lunmapped << endl;
    cout << "#Physical mapped qubits  : " << pmapped << endl;
    cout << "#Used check entries      : " << lcheck << endl;

    cout << "Dev size: " << m_size << endl;
    for (ll = 0; ll < m_size; ll++)
      if (m_layout[ll] < 0)
    {
      aquma_qubit * phq = NULL;
      cout << "Attempting to map logical qubit " << ll << " ...";
      for (next = 0; next < m_size; next++)
      {
        aquma_qubit * tempq = m_list[next];
        if (not tempq->is_mapped () and m_check[tempq->my_id ()] == 0)
        {
          phq = tempq;
          break;
        }
      }
      if (!phq)
        continue;
      //assert (phq && "Did not find an available physical qubit.");
      cout << " to physical qubit " << phq->my_id () << endl;
      m_layout[ll] = phq->my_id ();
      m_check[phq->my_id ()] = 1;
      phq->set_mapped ();
    }
  }

  void compute_fast_iterative_layout (aquma_graph * device, aquma_circuit * circuit)
  {
    aquma_qubit_vector_iterator qq;

    set<int> frontier; 
    int ii;
    vector<int> ordered_sol;
    set<int> pending;
    set<int>::iterator xx;
    this->initialize_pending_qubits (pending, circuit);

    for (ii = 0; pending.size () > 1 && m_xset.size () < (unsigned)(m_size); ii++)
    {

      cout << "Fast Iterative Selection - Round " << ii;
      cout << " [Pending=" << pending.size () << "] - ";
      cout << " Unmapped qubits: " << m_size - m_xset.size () << endl;

      cout << "Pending qubits: ";
      for (xx = pending.begin (); xx != pending.end (); xx++)
      {
        cout << " ";
        cout << *xx;
      }
      cout << endl << endl;

      ilp_system * ilp;
      // The below is only needed for the upper bound of selected qubits and edges.
      // So it's ok to use the full list of qubits.
      //unsigned node_target = this->count_2Q_with_incidence_K (kk);
      //unsigned edge_target = kk * node_target;

      //unsigned node_target = circuit->get_number_distinct_qubits_in_2Q_gates ();
      //unsigned edge_target = m_edges; //circuit->get_total_dir_edge_count ();
      //assert (edge_target > 0 && node_target > 0);

      ilp = this->build_fast_ilp (NULL, frontier);

      if (this->use_isl_solver ())
      {
        ilp->solve_ilp (); // solve with ISL
      }

      if (this->use_cplex_solver ())
      {
        ilp->print_ilp_to_cplex_script ();
        ilp->solve_with_cplex ();
      }

      //delta_q = ilp->update_global_layout (device->get_layout (), 
      int selected = ilp->find_selected_qubit ();

      cout << "Qubit selected in round " << ii << " : " << selected << endl;      

      m_xset.insert (selected);
      ordered_sol.push_back (selected);

      aquma_qubit * qu = this->find_qubit_with_id (selected);
      #if 0
      for (qq = m_list.begin (); qq != m_list.end (); qq++)
        if ((*qq)->my_id () == selected)
      {
        (*qq)->set_mapped ();
        qu = (*qq);
        break;
      }
      #endif
      assert (qu && "Qubit object not found.");

      set<int> recent;
      // Fetch the list of edge frequencies
      de_counter_t & opfreq = circuit->get_edge_counter_map ();
      bool added = this->map_ops_to_edges (qu, opfreq, NULL, NULL, 
          frontier, circuit, pending, recent);

      // Set map and remove used adjacent qubits (connections) to avoid
      // double-using any qubit.
      #if 0
      for (qq = m_list.begin (); added and qq != m_list.end (); qq++)
      {
        qu = (*qq);
        if (recent.find (qu->my_id ()) != recent.end ())
        {
          qu->set_mapped ();
        }
        qu->update_properties (recent);
      }
      #endif
      if (added)
      {
        recent.insert (qu->my_id ());
        this->update_qubit_status_from_recent (recent, added);
        sort_qubits_edge_error_time (m_list);

        cout << "Recapping current qubit status : " << endl;
        for (qq = m_list.begin (); qq != m_list.end (); qq++)
        {
          cout << "Qubit status " << (*qq)->my_id () << " : " << (*qq)->is_mapped () << endl;
        }
      }

      delete ilp;

      if (not added)
        break;
    }

    // Finish allocating any pending qubits by their q1 reciprocal
    this->allocate_qubits_by_q1_reciprocal (&pending);

    #if 0
    for (ll = 0; ll < m_size; ll++)
    {
      printf ("==>Qubit %d left unmapped (%d)\n", ll, m_layout[ll]);
    }

    for (ll = 0; ll < m_size; ll++)
    {
      printf ("==>Is Qubit %d unmapped ? (%d)\n", ll, m_list[ll]->is_mapped ());
    }
    #endif


    m_allocated_q = m_xset.size ();
    vector<int>::iterator ss;
    cout << "Main Qubits selected in the following order: " << endl;
    for (ss = ordered_sol.begin (); ss != ordered_sol.end (); ss++)
    {
      if (ss != ordered_sol.begin ())
        cout << ", ";
      cout << *ss;
    }
    cout << endl;
    this->show_list ();
  }

  void print_pending_status (set<int> & pending, unsigned ii, string call_site = "")
  {
    set<int>::iterator xx;
    cout << "SubStr-based Selection " << call_site << " - Round " << ii;
    cout << " [Pending=" << pending.size () << "] - ";
    cout << " Unmapped qubits: " << m_size - m_xset.size () << endl;

    cout << "Pending qubits: ";
    for (xx = pending.begin (); xx != pending.end (); xx++)
    {
      cout << " ";
      cout << *xx;
    }
    cout << endl << endl;
  }

  /*
   * Return the first available (unmapped) qubit in the m_list.
   */
  aquma_qubit * get_next_physical_qubit (set<int> * frontier = NULL)
  {
    unsigned ii;
    for (ii = 0; ii < m_list.size (); ii++)
    {
      if (m_list[ii]->is_mapped ())
        continue;

      // If @frontier is NULL, simply return the first available 
      // physical qubit.
      bool unmapped = not m_list[ii]->is_mapped ();

      //if (!frontier && unmapped)
        return m_list[ii];

      #if 0
      // If @frontier is not NULL but its size is zero, return the 1st 
      // available physical qubit.
      bool empty_front = (frontier != NULL && frontier->size () == 0);
      if (empty_front && unmapped)
        return m_list[ii];

      // If @frontier is not NULL and its size > zero, return the 1st 
      // available physical qubit that is also in @frontier.
      int qubit_id = m_list[ii]->my_id ();
      bool frontcond = (frontier != NULL and unmapped and
          frontier->find (qubit_id) == frontier->end ());
      if (frontcond)
        return m_list[ii];
      #endif
    }
    return NULL;
  }
  
  void show_high_impact_qubits (set<int> & hiq)
  {
    set<int>::iterator qq;
    cout << "Current HIQ list : ";
    for (qq = hiq.begin (); qq != hiq.end (); qq++)
    {
      cout << *qq << " ";
    }
    cout << endl;
  }

  #define AXS(rr,cc) ((rr) * (nn) + (cc))
  #define AXS_MOD2(rr,cc) (((rr) % 2) * (nn) + (cc))
  void compute_substr_layout (aquma_graph * device, aquma_circuit * circuit)
  {
    set<int> frontier; 
    unsigned ii;
    vector<int> ordered_sol;
    set<int> pending;
    set<int> hiq; // high impact qubits
    this->initialize_pending_qubits (pending, circuit);

    sort_qubits_edge_error_time (m_list);

    de_counter_t & globfreq = circuit->get_edge_counter_map ();
    edge_freq_matrix_t numap;
    float emean, estdv;
    vector<edge_freq_s> qws;

    
    bool any = true;
    for (ii = 0; 
          any && pending.size () > 1 && m_xset.size () < (unsigned)(m_size) 
          && m_qsubstr.size () > 0; ii++)
    {
      any = false;

      this->print_pending_status (pending, ii, "compute_substr_layout");

      cout << "Computing initial statistics : " << endl;

      // Compute the initial "numap" containing logical pairs of qubits from the global "globfreq".
      bool ok;
      ok = this->build_qubit_edge_frequency_tree (globfreq, numap, pending);

      cout << "returned from freqs" << endl;

      if (not ok)
      {
        cout << "Breaking: Could not construct frequency tree (1)" << endl;
        break;
      }

      cout << "Moving to statistics ..." << endl;

      ok = this->compute_qubit_edge_statistics (globfreq, numap, emean, estdv, qws, hiq);

      if (not ok)
        break;

      this->show_high_impact_qubits (hiq);

      cout << "[CSSLT] Edge Mean : " << emean << endl;
      cout << "[CSSLT] Edge Stdv : " << estdv << endl;
      cout << "[CSSLT] High Impact Threshold : " << emean + 2 * estdv << endl;  

      cout << "Proceeding to extract longest repeated substr " << endl;

      vector<int> rstr;
      this->extract_longest_repeated_non_overlapping_substr (rstr);
      //int nn = m_qsubstr.size ();

      if (rstr.size () <= 1)
        break;

      cout << "Counting repetitions " << endl;

      // TODO: somewhere here, use the threshold to select the logical qubits.
      vector<unsigned> locs;
      unsigned reps = this->count_substr_repetitions (m_qsubstr, rstr, locs);
      
      cout << "Building histogram for substring (" << reps << "x)" << endl;

      de_counter_t ehist;
      set<int> recent;
      this->substr_histogram (rstr, ehist);

      cout << "Selecting next physical qubit " << endl;

      // Select the next physical qubit. Internally will select among 3 choices,
      // depending on whether @recent is NULL, non-NULL but empty or non-NULL
      // and non-empty.
      aquma_qubit * qu;
      bool added = false;
      int l2r;

      for (qu = this->get_next_physical_qubit (), l2r = 0;
            qu;
          qu = this->get_next_physical_qubit (), l2r++)
      {
        if (!qu)
          continue;

        assert (not qu->is_mapped ());

        cout << "Level 1 [round=" << ii << "] - ";
        cout << "Level 2 [round=" << l2r << "] Mapping operations to edges ... " << endl;
        this->print_pending_status (pending, ii, "substr-layout-level2");

        added = this->map_ops_to_edges (qu, ehist, 
            &qws, &hiq, frontier, circuit, pending, recent);

        if (added)
        {
          cout << "Updating status ..." << endl;
          //qu->set_mapped ();
          recent.insert (qu->my_id ()); // force updating of selected qubit
          this->update_qubit_status_from_recent (recent, added);

          if (recent.find (4) != recent.end ())
            cout << "Recent set contains 4" << endl;
          //recent.erase (qu->my_id ()); // do not consider this qubit again

          // Question: is re-sorting needed: YES
          sort_qubits_edge_error_time (m_list);
        }

        any = any or added;

        if (not added)
          break;

      }

      this->print_layout ();
      this->print_check_array ();

      if (any)
      {
        // Will splice the sub-string in attribute m_qsubstr, causing it to shrink.
        this->splice_substr (locs, rstr.size ());
        sort_qubits_edge_error_time (m_list);
        this->update_edge_frequency_with_pending (globfreq, pending);

        hiq.clear ();
        qws.clear ();
        // TODO: Check how to use global frequencies.
      }
    }
  }

  void compute_global_frequency_layout (aquma_graph * device, aquma_circuit * circuit)
  {
    set<int> frontier; 
    unsigned ii;
    vector<int> ordered_sol;
    set<int> pending;

    this->compute_pending_qubits_from_layout (pending);

    sort_qubits_edge_error_time (m_list);

    de_counter_t & globfreq = circuit->get_edge_counter_map ();
    edge_freq_matrix_t numap;

    for (ii = 0; 
          pending.size () > 1 && m_xset.size () < (unsigned)(m_size) 
          && m_qsubstr.size () > 0; ii++)
    {
      bool status = false;
      set<int> recent;

      this->print_pending_status (pending, ii, "global_freq_layout");

      cout << "Computing initial statistics : " << endl;

      // Compute the initial "numap" containing logical pairs of qubits from the global "globfreq".
      status = this->build_qubit_edge_frequency_tree (globfreq, numap, pending);

      cout << "returned from freqs" << endl;

      if (not status)
      {
        cout << "Breaking: Could not construct frequency tree (2)" << endl;
        break;
      }

      aquma_qubit * qu = this->get_next_physical_qubit (NULL);

      status = this->map_ops_to_edges (qu, globfreq, 
          NULL, NULL, frontier, circuit, pending, recent);

      if (status)
      {
        cout << "Updating status ..." << endl;
        //qu->set_mapped ();
        recent.insert (qu->my_id ()); // force updating of selected qubit
        this->update_qubit_status_from_recent (recent, status);

        // Re-sort after using qubits
        sort_qubits_edge_error_time (m_list);
      }

      if (not status)
        break;
    }
  }

  void compute_pending_qubits_from_layout (set<int> & pending)
  {
    unsigned ii;
    for (ii = 0; ii < m_layout.size (); ii++)
    {
      if (m_layout[ii] < 0)
        pending.insert (ii);
    }
  }

  void update_edge_frequency_with_pending (de_counter_t & freqlist, set<int> & pending)
  {
    bool change;
    while (1)
    {
      change = false;
      map<unsigned,unsigned>::iterator ee;
      for (ee = freqlist.begin (); ee != freqlist.end (); ee++)
      {
        // Check this?
        unsigned q_source = this->edge_to_source_qubit (ee->first);
        if (pending.find (q_source) == pending.end ())
        {
          freqlist.erase (ee);
          change = true;
          break;
        }
      }
      if (not change)
        break;
    }
  }

  void naive_lrnoss (vector<int> & LCSRe, int & index, int & res_length)
  {
    res_length  = 0; // To store length of result
    // building table in bottom-up manner
    index = 0;

    int ii;
    int nn = m_qsubstr.size ();
    unsigned long long total = (nn+1) * (nn+1);
    LCSRe.resize (total, 0);

    for (ii=1; ii<=nn; ii++)
    {
      int jj;
      for (jj=ii+1; jj<=nn; jj++)
      {
        // (j-i) > LCSRe[i-1][j-1] to remove overlapping
        if (m_qsubstr[ii-1] == m_qsubstr[jj-1] &&
            LCSRe[AXS(ii-1,jj-1)] < (jj - ii))
        {
          LCSRe[AXS(ii,jj)] = LCSRe[AXS(ii-1,jj-1)] + 1;

          // Update maximum length of the substring and update
          // the finishing/last index of the suffix
          if (LCSRe[AXS(ii,jj)] > res_length)
          {
            res_length = LCSRe[AXS(ii,jj)];
            index = max(ii, index);
          }
        }
        else
        {
          LCSRe[AXS(ii,jj)] = 0;
        }
      }
    }
  }

  void compressed_lrnoss (vector<int> & LCSRe, int & index, int & res_length)
  {
    res_length  = 0; // To store length of result
    // building table in bottom-up manner
    index = 0;

    int ii;
    int nn = m_qsubstr.size ();
    unsigned long long total = (nn+1) * 2;
    LCSRe.resize (total, 0);

    for (ii=1; ii<=nn; ii++)
    {
      int jj;
      for (jj=ii+1; jj<=nn; jj++)
      {
        // (j-i) > LCSRe[i-1][j-1] to remove overlapping
        if (m_qsubstr[ii-1] == m_qsubstr[jj-1] &&
            LCSRe[AXS_MOD2(ii-1,jj-1)] < (jj - ii))
        {
          LCSRe[AXS_MOD2(ii,jj)] = LCSRe[AXS_MOD2(ii-1,jj-1)] + 1;

          // Update maximum length of the substring and update
          // the finishing/last index of the suffix
          if (LCSRe[AXS_MOD2(ii,jj)] > res_length)
          {
            res_length = LCSRe[AXS_MOD2(ii,jj)];
            index = max(ii, index);
          }
        }
        else
        {
          LCSRe[AXS_MOD2(ii,jj)] = 0;
        }
      }
    }
  }

  void extract_longest_repeated_non_overlapping_substr (vector<int> & rstr)
  {
    int nn = m_qsubstr.size ();
    unsigned long long total = (nn+1) * (nn+1);
    cout << "Haystack size : " << nn << endl;
    cout << "Allocating matrix of size : " << total << endl;

    int ii;

    this->print_haystack_string ();
  
    vector<int> LCSRe;
    int index = 0;
    int res_length  = 0; // To store length of result

    //this->naive_lrnoss (LCSRe, index, res_length);
    this->compressed_lrnoss (LCSRe, index, res_length);

    #if 0 
    int res_length  = 0; // To store length of result

    LCSRe.resize (total, 0);

    // building table in bottom-up manner
    int index = 0;
    for (ii=1; ii<=nn; ii++)
    {
      int jj;
      for (jj=ii+1; jj<=nn; jj++)
      {
          // (j-i) > LCSRe[i-1][j-1] to remove overlapping
          if (m_qsubstr[ii-1] == m_qsubstr[jj-1] &&
              LCSRe[AXS(ii-1,jj-1)] < (jj - ii))
          {
              LCSRe[AXS(ii,jj)] = LCSRe[AXS(ii-1,jj-1)] + 1;

              // updating maximum length of the
              // substring and updating the finishing
              // index of the suffix
              if (LCSRe[AXS(ii,jj)] > res_length)
              {
                  res_length = LCSRe[AXS(ii,jj)];
                  index = max(ii, index);
              }
          }
          else
              LCSRe[AXS(ii,jj)] = 0;
      }
    }
    #endif

    vector<int> res; // To store result
    // If we have a non-empty result, insert all the characters from first 
    // to the last.
    if (res_length > 0)
        for (ii = index - res_length + 1; ii <= index; ii++)
            res.push_back(m_qsubstr[ii-1]);
    rstr = res;

    // Some debug printing.
    this->print_debug_string_patterns (res, res_length);
  }

  void print_debug_string_patterns (vector<int> & res, int res_length)
  {
    int nn = m_qsubstr.size ();
    int ii;
    cout << "Haystack: ";
    for (ii = 0; ii < nn; ii++)
      cout << m_qsubstr[ii] << " ";
    cout << endl;
    cout << endl; 
    cout << "Solution : ";
    for (ii = 0; ii < (int)(res.size ()); ii++)
      cout << res[ii] << " ";
    cout << endl;
    cout << "Sol length : " << res_length << " - haystack length : " << nn << endl;
  }

  void print_haystack_string ()
  {
    int nn = m_qsubstr.size ();
    int ii;
    cout << "Initial Haystack: ";
    for (ii = 0; ii < nn; ii++)
      cout << m_qsubstr[ii] << " ";
    cout << endl << endl;
  }

  unsigned 
  count_substr_repetitions (vector<int> & sh, vector<int> & sn, vector<unsigned> & locs)
  {
    int ii;
    int lh = sh.size ();
    int ln = sn.size ();
    unsigned count = 0;
    for (ii = 0; ii < lh - ln; )
    {
      int jj;
      for (jj = 0; sh[ii+jj] == sn[jj] && jj < ln; jj++);
      if (jj == ln && sh[ii+jj-1] == sn[jj-1])
      {
        locs.push_back (ii);
        count ++;
        ii += ln;
        continue;
      }
      ii++;
    }
    vector<unsigned>::iterator ss;
    cout << "String repeated " << count << " times " << endl;
    cout << "Starts at : ";
    for (ss = locs.begin (); ss != locs.end (); ss++)
    {
      cout << *ss << " " ; 
    }
    cout << endl << endl;
    return count;
  }

  void splice_substr (vector<unsigned> & pos, unsigned len)
  {
    unsigned ss, tt;
    vector<int> temp; 
    temp.resize (m_qsubstr.size () - len * pos.size ());
    unsigned segment;
    for (tt = 0, ss = 0, segment = 0; segment < pos.size () and ss < m_qsubstr.size (); )
    {
      if (ss < pos[segment])
        temp[tt++] = m_qsubstr[ss++];
      else
      {
        cout << "Reached position " << pos[segment] << ", advancing ..." << endl;
        segment++;
        ss += len;
      }
    }
    for ( ; ss < m_qsubstr.size (); ss++, tt++)
    {
      temp[tt] = m_qsubstr[ss];
    }
    cout << "Splice length : " << len << ", reps " << pos.size () << endl;
    cout << "Haystack size before splicing : " << m_qsubstr.size () << endl;
    cout << "Haystack before splicing : ";
    print_vector_int_debug (m_qsubstr);
    m_qsubstr.clear ();
    m_qsubstr.resize (temp.size ());
    m_qsubstr = temp;
    cout << "Haystack after splicing : ";
    print_vector_int_debug (m_qsubstr);
    cout << "Haystack size after  splicing : " << m_qsubstr.size () << endl;
  }

  void substr_histogram (vector<int> & qstr, de_counter_t & emap)
  {
    int ii, nn = qstr.size ();
    for (ii = 0; ii < nn; ii++)
    {
      unsigned idx = unsigned (qstr[ii]);
      if (emap.find (idx) == emap.end ())
        emap[idx] = 1;
      else
        emap[idx] += 1;
    }
    de_counter_iter_t pp;
    cout << "Showing substr edge histogram : " << endl;
    for (pp = emap.begin (), ii = 0; pp != emap.end (); pp++, ii++)
    {
      unsigned qsrc = this->edge_to_source_qubit (pp->first);
      unsigned qdst = this->edge_to_target_qubit (pp->first);

      cout << "[" << ii << "] Edge " << pp->first << "(" << qsrc << " -> " << qdst << ") : " << pp->second << endl;
    }
    cout << endl;
  }
  #undef AXS

  void compute_kmp_prefix_function (vector<int> & seq, vector<int> & prefix)
  {
    unsigned mm = seq.size ();
    prefix.clear ();
    prefix.resize (seq.size ()+1);
    prefix[1] = 0;
    unsigned kk;
    unsigned qq;
    for (kk = 0, qq = 2; qq < mm; qq++)
    {
      for ( ; kk > 0 && prefix[kk+1] != prefix[qq]; )
      {
        kk = prefix[kk];
      }
      kk += (prefix[kk+1] == prefix[qq] ? 1 : 0);
      prefix[qq] = kk;
    }
  }


  bool is_mapping_unique ()
  {
    vector<int> count;
    count.resize (m_layout.size (), 0);
    vector<int>::iterator qq;
    for (qq = this->m_layout.begin (); qq != this->m_layout.end (); qq++)
    {
      if (*qq >= 0)
        count[*qq]++;
    }
    bool ret = true;
    unsigned ii;
    for (qq = count.begin (), ii = 0; qq != count.end (); qq++, ii++)
    {
      if (*qq != 1)
      {
        cout << "Physical qubit " << ii << " mapped " << *qq << " times." << endl;
        ret = false;
      }
    }
    return ret;
  }
};


#endif
