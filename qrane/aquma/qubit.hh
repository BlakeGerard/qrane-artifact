#ifndef _AQUMA_QUBIT_

#define _AQUMA_QUBIT_

#include <stdio.h>
#include <stdlib.h>

#include <set>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <iostream>

#include "utils.hh"

using namespace std;

class aquma_qubit;

typedef vector<aquma_qubit*> aquma_qubit_vector;
typedef vector<aquma_qubit*>::iterator aquma_qubit_vector_iterator;
typedef const vector<aquma_qubit*>::iterator aquma_qubit_vector_const_iterator;

typedef list<aquma_qubit*> aquma_qubit_list;
typedef list<aquma_qubit*>::iterator aquma_qubit_list_iterator;
typedef const list<aquma_qubit*>::iterator aquma_qubit_list_const_iterator;

typedef map<int,float> edge_map; // two gate error

#define PP_T1 0
#define PP_T2 1
#define PP_READ_ERROR 4
#define PP_READ_TIME 7
#define PP_PAULIX_ERROR 10

#define GATE2_ERROR 1
#define GATE2_TIME  2

/*
 * Pyquil: g1-time: 50ns, g2-time: 150ns, read-fidel: 
 *
 */

#define NUM_IBMQ_PP 11

#define RSCORE_INVAL (200)

class aquma_qubit {
  int m_id; // qubit id;
  unsigned m_size; // graph size
  set<int> m_set; // adjacency list
  float m_pp[NUM_IBMQ_PP]; // physical parameters
  edge_map m_err2gate; // two-qubit-gate-error hashed by qubit id
  edge_map m_time2gate; // two-qubit-gate-time  hashed by qubit id
  bool m_is_2gate; // is qubit usable in 2-qubit gates?
  float m_avg2g_err; // Average 2Q-gate error associated to Qubit (if avail)
  float m_avg2g_time; // Average 2Q-gate time associated to Qubit (if avail)
  bool m_mapped; // True if the qubit has already been mapped; False otherwise.
  float m_rscore; // \sum  (1/e_k) * (1/e_t)
  float m_g1_score;


  public:

  aquma_qubit () {
    m_id = -1;
    m_size = 0;
    m_is_2gate = false;
    m_is_2gate = 100000000;
    m_set.clear ();
    m_mapped = false;
    m_rscore = 0.0f;
    m_g1_score = 0.0f;
  }

  aquma_qubit (int pid, unsigned psize) {
    m_id = pid;
    m_size = psize;
    m_is_2gate = false;
    m_mapped = false;
    m_rscore = 0.0f;
    m_g1_score = 0.0f;
  }

  /* 
   * Constructor used when number of qubits is known.
   */
  aquma_qubit (unsigned psize) {
    m_id = -1;
    m_size = psize;
    m_is_2gate = false;
    m_mapped = false;
    m_rscore = 0.0f;
    m_g1_score = 0.0f;
  }

  ~ aquma_qubit () {

  }

  bool is_2qubit_gate ()
  {
    return m_is_2gate;
  }

  void add (int ww) {
    m_set.insert (ww);
  }

  void remove (int ww) {
    m_set.erase (ww);
  }

  void clear () {
    m_set.clear ();
  }

  bool is_mapped ()
  {
    return m_mapped;
  }

  void set_mapped ()
  {
    m_mapped = true;
  }

  void reset_mapped ()
  {
    m_mapped = false;
  }

  void load_adjacent_qubits (vector<int> & adj)
  {
    adj.clear ();
    set<int>::iterator aa;
    for (aa = m_set.begin (); aa != m_set.end (); aa++)
    {
      adj.push_back (*aa);
    }
    std::sort (adj.begin (), adj.end ());
    cout << "Qubit " << this->my_id () << " Preferences : ";
    for (aa = m_set.begin (); aa != m_set.end (); aa++)
    {
      if (aa != m_set.begin ())
        cout << ", ";
      cout << *aa;
    }
    cout << endl;
  }

  bool is_iteration_candidate (unsigned td)
  {
    if (td > 0 and m_set.size () < td)
      return false;
    if (m_mapped)
      return false;
    return true;
  }

  /*
  * Deep copy. Create fresh new qubit with the same contents.
  * Optional boolean parameter determines whether adjacency list is
  * copied or not.
  */
  aquma_qubit * deep_copy (bool copy_adj = true) {
    aquma_qubit * ret = NULL;
    ret = new aquma_qubit (m_id, m_size);
    if (copy_adj) 
    {
      set<int>::iterator ww;
      for (ww = m_set.begin (); ww != m_set.end (); ww++)
        ret->add (*ww);
    }
    return ret;
  }

  unsigned size () const {
    return this->m_set.size ();
  }

  set<int> & neighbors () 
  {
    return m_set;
  }

  int my_id () const {
    return m_id;
  }

  float get_g2_average_error ()
  {
    return m_avg2g_err;
  }

  float get_g2_average_time ()
  {
    return m_avg2g_time;
  }

  bool is_adjacent (int ww) {
    set<int>::iterator ret;
    ret = m_set.find (ww);
    return ret != m_set.end ();
  }

  void print_qubit () {
    printf ("q%d [%lu] (R=%f):", m_id, m_set.size (), m_rscore);
    set<int>::iterator ww;   
    for (ww = m_set.begin (); ww != m_set.end (); ww++) {
      if (ww != m_set.begin ())
        printf (" --> ");
      printf (" %d", *ww);
    }
    printf ("\n");
  }

  bool operator == (const aquma_qubit & other_q) const {
    return m_id == other_q.my_id ();
  }

  bool select_neighbors_v1 (int deg_k, aquma_qubit_list_iterator & original,
    list<aquma_qubit*> & pending, 
    set<int> & done)
  {
    // Go through the pending list and check and grab the lowest deg_k 
    // qubits that are in the adjacency list of the current qubit (this)
    // Note: this must have been removed.
    done.insert (this->my_id());
    //vector<aquma_qubit*>::iterator ww;
    aquma_qubit_list_iterator ww;
    aquma_qubit_list_iterator * trashbin = new aquma_qubit_list_iterator[deg_k + 1];

    int count = 0;
    for (ww = pending.begin (); count < deg_k and ww != pending.end (); ww++)
      if (ww != original)
    {
      int neighbor = (*ww)->my_id ();
      bool not_done = done.find (neighbor) == done.end ();
      if ((*original)->is_adjacent (neighbor) and not_done)
      {
        //printf ("~~> Neighbor : %d\n", neighbor);
        trashbin[count] = ww;
        count++;
        this->add (neighbor);
        //pending.erase (ww);
      }
    }
    bool ret;
    if (count == deg_k)
    {
      // Update the pending and done structures if we found enough neighbors.
      int uu;
      int lead_id = (*original)->my_id();
      done.insert (lead_id);
      for (uu = 0; uu < count; uu++)
      {
        int neighbor = (*trashbin[uu])->my_id ();
        for (ww = pending.begin (); ww != pending.end (); ww++)
          if ((*ww)->is_adjacent (neighbor) or (*ww)->is_adjacent (lead_id))
        {
          (*ww)->remove (neighbor);
          (*ww)->remove (lead_id);
        }
        //printf ("\t==>Removing neighbor %d from pending \n",neighbor);
        done.insert (neighbor);
        pending.erase (trashbin[uu]);
      }
      ret = true;
    }
    else //if (count < deg_k)
    {
      ret = false;
    }
    free (trashbin);
    return ret;
  }

  bool select_neighbors (int deg_k, 
    list<aquma_qubit*> & pending, 
    set<int> & done)
  {
    // Go through the pending list and check and grab the lowest deg_k 
    // qubits that are in the adjacency list of the current qubit (this)
    // Note: this must have been removed.
    done.insert (this->my_id());
    //vector<aquma_qubit*>::iterator ww;
    aquma_qubit_list_iterator ww;
    aquma_qubit_list_iterator * trashbin = new aquma_qubit_list_iterator[deg_k + 1];

    int count = 0;
    for (ww = pending.begin (); count < deg_k and ww != pending.end (); ww++)
    {
      int neighbor = (*ww)->my_id ();
      bool not_done = done.find (neighbor) == done.end ();
      if (this->is_adjacent (neighbor) and not_done)
      {
        //printf ("~~> Neighbor : %d\n", neighbor);
        trashbin[count] = ww;
        count++;
        this->add (neighbor);
        //pending.erase (ww);
      }
    }
    bool ret;
    this->clear ();
    if (count == deg_k)
    {
      // Update the pending and done structures if we found enough neighbors.
      int uu;
      int lead_id = (this)->my_id();
      done.insert (lead_id);
      for (uu = 0; uu < count; uu++)
      {
        int neighbor = (*trashbin[uu])->my_id ();
        for (ww = pending.begin (); ww != pending.end (); ww++)
          if ((*ww)->is_adjacent (neighbor))
        {
          (*ww)->remove (neighbor);
        }
        this->add (neighbor);
        //printf ("\t==>Removing neighbor %d from pending \n",neighbor);
        done.insert (neighbor);
        pending.erase (trashbin[uu]);
      }
      ret = true;
    }
    else //if (count < deg_k)
    {
      printf ("Did not find sufficient neighbors\n");
      ret = false;
    }
    free (trashbin);
    return ret;
  }

  /*
    Given the argument @alist, update the adjacency list of all elements,
    that are adjacent to @this.
  */
  void update_connectivity (aquma_qubit_list & alist)
  {
    aquma_qubit_list_iterator ww;
    int out_id = this->my_id ();
    for (ww = alist.begin (); ww != alist.end (); ww++)
      if ((*ww)->my_id () != this->my_id ())
    {
      if (this->is_adjacent ((*ww)->my_id ()))
      {
        (*ww)->remove (out_id);
      }
    }
  }

  int get_next_fringe (aquma_qubit_list & alist,
    set<int> & done) 
  {
    aquma_qubit_list_iterator fringe = alist.end ();
    unsigned fringe_card = m_size + 1; // graph size + 1
    aquma_qubit_list_iterator ww;   
    for (ww = alist.begin (); ww != alist.end (); ww++)
      if (done.find ((*ww)->my_id ()) == done.end () and
        this->is_adjacent ((*ww)->my_id ()))
    {
      if ((*ww)->size () < fringe_card and (*ww)->size () >= 1)
      {
        printf ("Found candidate\n");
        fringe_card = (*ww)->size ();
        fringe = ww;
      }
    }
    if (fringe != alist.end ())
      return (*fringe)->my_id ();
    return -1;
  }

  /*
  * Select the next qubit from @qlist that meets the conditions:
  * i) has the maximal incidence degree.
  * ii) has the highest connectivity to vertices in @done.
  */
  aquma_qubit * next_maximal_vertex (aquma_qubit_list & qlist, set<int> & done, unsigned & added)
  {
    aquma_qubit_list_iterator qq;
    aquma_qubit * ret = NULL;
    unsigned edge_count = 0;
    unsigned common_count = 0;
    for (qq = qlist.begin (); qq != qlist.end (); qq++)
    {
      unsigned qq_common = aquma_utils::count_common_items ((*qq)->neighbors (), done);
      unsigned qq_size = (*qq)->size ();
      if (qq_size > edge_count)
      {
        ret = *qq;
        edge_count = qq_size;
        common_count = qq_common;
      }
      else if (qq_size == edge_count && qq_common > common_count)
      {
        ret = *qq;
        edge_count = qq_size;
        common_count = qq_common;
      }
    }
    // Return the number of edges effectively added to the partition/sub-graph. 
    // We could just recount, but this is simpler.
    added = common_count;
    return ret;
  }

  void remove_qubit (aquma_qubit_list & qlist, int tgt_id)
  {
    aquma_qubit_list_iterator qq;
    for (qq = qlist.begin (); qq != qlist.end (); qq++)
      if ((*qq)->my_id () == tgt_id)
    {
      qlist.remove (*qq);
      break;
    }
  }

  void print_qubit_parameters ()
  {
    cout << "Qubit ID: " << this->m_id << endl;
    int ii;
    for (ii = 0; ii < NUM_IBMQ_PP; ii++)
    {
      cout << " " << this->m_pp[ii];
    }
    cout << endl << "  ";
    edge_map::iterator pp;
    cout << "2-gate error: ";
    for (pp = this->m_err2gate.begin (); pp != this->m_err2gate.end (); pp++)
    {
      cout << "(" << pp->first << "," << pp->second << ")" << " ";
    }
    cout << std::endl;
    cout << "  2-gate time: ";
    for (pp = this->m_time2gate.begin (); pp != this->m_time2gate.end (); pp++)
    {
      cout << "(" << pp->first << "," << pp->second << ")" << " ";
    }
    cout << std::endl;
    cout << std::endl << std::endl;
  }

  /*
   * Read physical parameters for adjacent qubits.
   * Return number of adjacent qubits.
   */
  int read_twogate_parameters (FILE * & ff, edge_map & emap, int * cg, int ec)
  {
    int cc;
    int retval;
    int edges = 0;
    while (1) 
    {
      // Read info and store it in the error and time emaps.
      // Add adjcent qubit to the m_set.
      int q1, q2;
      retval = fscanf (ff, "%d_%d", &q1, &q2);
      assert (retval == 2 && "Expected two qubit indices.");
      cc = fgetc (ff);
      float physparam;
      retval = fscanf (ff, "%f", &physparam);
      assert (retval == 1 && "Expected one physical parameter.");
      cg[this->m_id * m_size + q2] = ec + edges;
      edges++;
      emap.insert (std::pair<int,float>(q2,physparam));
      this->m_set.insert (q2);

      m_is_2gate = true;

      // Now we prepare for parsing the next entry.
      cc = fgetc (ff);
      if (cc == ',')
      {
        ungetc (cc, ff);
        break;
      }
      if (cc == ';')
      {
        cc = fgetc (ff);
      }
      if (cc == '\n')
      {
        break;
      }
      if (cc == EOF)
      {
        ungetc (cc, ff);
        break;
      }
    }
    return edges;
  }

  void read_qubit_physical_parameters (FILE * ff, int * cg, int & ec) 
  {
  // cg: coupling graph in linearized array.
  // ec: edge count, must increment after each edge read.
    char cc;
    int retval;
    int temp_id;
    retval = fscanf (ff,"Q%d",&temp_id);
    assert (retval == 1 && "Expected qubit index.");
    this->m_id = temp_id;
    int ii;
    for (ii = 0; ii < NUM_IBMQ_PP; ii++)
    {
      float pp;
      while (1)
      {
        cc = fgetc (ff);
        if (isdigit (cc))
          break;
      }
      ungetc (cc, ff);
      retval = fscanf (ff,"%f", &pp);
      printf ("Read PP_%d : %f\n", ii, pp);
      this->m_pp[ii] = pp;
    }
    int edges_read;
    printf ("Reading two-qubit-gate properties of qubit %d\n", this->m_id);
    cc = fgetc (ff);
    edges_read = this->read_twogate_parameters (ff, this->m_err2gate, cg, ec);
    printf ("Read two-qubit-gate-error of %d edges \n", edges_read);
    cc = fgetc (ff);
    edges_read = this->read_twogate_parameters (ff, this->m_time2gate, cg, ec);
    printf ("Read two-qubit-gate-time of %d edges \n", edges_read);
    ec += edges_read;
  }

  void append_values_to_map (edge_map * gatemap, int mode)
  {
    edge_map::iterator ee;
    edge_map * emap = NULL;
    if (mode == GATE2_ERROR)
      emap = & this->m_err2gate;
    if (mode == GATE2_TIME)
      emap = & this->m_time2gate;
    assert (emap && "Invalid mode given for edge map.");
    int ii = 0;
    for (ee = emap->begin (); ee != emap->end (); ee++, ii++)
    {
      int idx = m_id * m_size + ee->first;
      cout << "Adding pair <" << m_id << "," << ee->first << "> =" << ee->second << endl;
      gatemap->insert (std::pair<int,float>(idx,ee->second));
    }
  }

  void compute_qubit_parameter_averages_from_edges ()
  {
    if (not this->is_2qubit_gate ())
      return;

    edge_map::iterator ee;

    m_avg2g_err = 0;
    for (ee = m_err2gate.begin (); ee != m_err2gate.end (); ee++)
      m_avg2g_err += ee->second;
    m_avg2g_err =  m_avg2g_err / m_err2gate.size ();

    m_avg2g_time = 0;
    for (ee = m_time2gate.begin (); ee != m_time2gate.end (); ee++)
      m_avg2g_time += ee->second;
    m_avg2g_time =  m_avg2g_time / m_time2gate.size ();
  }

  void mark_neighbors_as_pending (vector<bool> & pending, set<int> & skip)
  {
    set<int>::iterator nn;
    for (nn = m_set.begin (); nn != m_set.end (); nn++)
    {
      if (skip.find (*nn) != skip.end ())
        continue;
      pending[*nn] = true;
    }
  }

  void compute_reciprocal ()
  {
    edge_map::iterator ee, tt;     
    m_rscore = 0.0f;
    for (ee = m_err2gate.begin (), tt = m_time2gate.begin (); 
        ee != m_err2gate.end () and tt != m_time2gate.end (); ee++, tt++)
    {
      if (m_set.find (ee->first) != m_set.end ())
        m_rscore += 1.0f / (ee->second * tt->second);
    }
    m_g1_score = m_pp[PP_READ_TIME] * ( m_pp[PP_READ_ERROR] + m_pp[PP_PAULIX_ERROR] );
    m_g1_score = 1.0f / m_g1_score;
  }

  void update_properties (set<int> & recent)
  {
    set<int>::iterator rr;
    for (rr = recent.begin (); rr != recent.end (); rr++)
    {
      m_err2gate.erase (*rr);
      m_time2gate.erase (*rr);
      //unsigned size_before = m_set.size ();
      m_set.erase (*rr);
      //unsigned size_after = m_set.size ();
      //cout << "Updating physical qubit " << m_id << " [" << size_before << " -> " << size_after << endl;
      if (*rr == m_id)
        this->set_mapped ();
    }
    this->compute_reciprocal ();
  }

  void collect_edges (vector<std::pair<int,int>> & vec)
  {
    edge_map::iterator ee;
    for (ee = m_err2gate.begin (); ee != m_err2gate.end (); ee++)
    {
      vec.push_back (std::pair<int,int>(m_id,ee->first));
    }
  }

  void collect_g2_error (vector<float> & vec)
  {
    edge_map::iterator ee;
    for (ee = m_err2gate.begin (); ee != m_err2gate.end (); ee++)
    {
      vec.push_back (ee->second);
    }
  }

  void collect_g1_error (vector<float> & vec)
  {
    vec.push_back (m_pp[PP_PAULIX_ERROR]);
  }

  void collect_g1_read (vector<float> & vec)
  {
    vec.push_back (m_pp[PP_READ_ERROR]);
  }

  float get_rscore ()
  {
    return m_rscore;
  }

  float get_g1_score ()
  {
    return m_g1_score;
  }
};

inline bool qubit_less_than (const aquma_qubit * left, const aquma_qubit * right) {
  return left->size () < right->size ();
}

inline bool qubit_greater_than (const aquma_qubit * left, const aquma_qubit * right) {
  return left->size () > right->size ();
}

inline bool swap_qubit_g2 (aquma_qubit * left, aquma_qubit * right)
{
  if (right->get_rscore () < left->get_rscore ())
    return true;
  return false;
}

inline void sort_qubits_edge_error_time (aquma_qubit_vector & qv)
{
  std::sort (qv.begin (), qv.end (), swap_qubit_g2);
}

inline bool swap_qubit_g1 (aquma_qubit * left, aquma_qubit * right)
{
  if (right->get_g1_score () < left->get_g1_score ())
    return true;
  return false;
}

inline void sort_qubits_g1_error_time (aquma_qubit_vector & qv)
{
  std::sort (qv.begin (), qv.end (), swap_qubit_g1);
}


#endif
