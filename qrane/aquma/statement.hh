#ifndef _AQUMA_STATEMENT_HH_

#define _AQUMA_STATEMENT_HH_

#include <map>
#include <vector>
#include <algorithm>

#include "utils.hh"
#include "isl/set.h"
#include "isl/map.h"
#include "isl/ctx.h"
#include "isl/polynomial.h"
#include "barvinok/isl.h"
//#include "barvinok/evalue.h"

using namespace std;

class aquma_statement;

enum access_type_e { 
  acc_other=1, 
  acc_parallel,
  acc_uniform,
  acc_pipe_1,
  acc_seq_no_comm,
  acc_pipe_k,
  acc_partial_share
};

static const char * access_type_str[] = {"E", "O", "P", "U", "PIPE1", "SwoC", "PIPEK", "P-Share"};

struct s_qubit_properties {
  unsigned mask;
};
typedef struct s_qubit_properties qubit_properties;

#define set_qubit_property(prop,attr) prop = ((prop) | (1 << (attr));
#define get_qubit_property(prop,attr) ((prop) & (1 << (attr)))
#define has_qubit_property(prop,attr) (get_qubit_property(prop,attr) > 0)
#define pset_qubit_property(cond,prop,attr) prop = ((cond) > 0 ? ((prop) | (1 << (attr))) : (prop));


typedef map<unsigned,unsigned> qmap_count;
typedef map<unsigned,unsigned>::iterator qmap_count_iterator;

struct s_graph_properties {
  unsigned n_nodes;
  unsigned n_edges;
  vector<unsigned> vid; // vertex incidence degree
  qmap_count qmc; // aggregated count: no. of vertices with certain cardinalities.
};
typedef struct s_graph_properties graph_properties;

struct s_qubit_info {
  isl_set * domain; // initial statement domain 
  isl_set * set; // the qubits
  isl_set * sid; // set of unique identifiers assigned to statements accessing the slice
  unsigned set_size;
  unsigned dom_size;
  unsigned num_dom;
};
typedef struct s_qubit_info qubit_info;

inline void qmap_update (qmap_count & qmc, unsigned val)
{
  if (qmc.find (val) == qmc.end ())
    qmc.insert (pair<unsigned,unsigned>(val,0));
  qmc[val]++;
}

inline bool qmap_geq (qmap_count & req, qmap_count & cand)
{
  qmap_count_iterator aa;
  for (aa = req.begin (); aa != req.end (); aa++)
  {
    if (cand.find (aa->first) == cand.end ())
      return false;
    if (cand[aa->first] < aa->second)
      return false;
  }
  return true;
}

inline bool qmap_geq (vector<unsigned> & goals, vector<unsigned> & state)
{
  vector<unsigned> sum_g;
  vector<unsigned> sum_s;
  assert (goals.size () == state.size ());
  printf ("testing ...\n");
  int nn = goals.size ();
  sum_g.resize (nn, 0);
  sum_s.resize (nn, 0);
  int ii;
  printf ("Current state: ");
  for (ii = 0; ii < nn; ii++)
  {
    printf ("%d ", state[ii]);
  }
  printf ("\n");
  
  for (ii = nn-1; ii >= 0; ii--)
  {
    if (ii == nn-1)
    {
      sum_g[ii] = goals[ii];
      sum_s[ii] = state[ii];
    }
    else
    {
      sum_g[ii] = sum_g[ii+1] + goals[ii];
      sum_s[ii] = sum_s[ii+1] + state[ii];
    }
  }
  bool ret = true;
  for (ii = 0; ii < nn; ii++)
  {
    if (sum_s[ii] < sum_g[ii])
      ret = false;
    printf ("%d / %d, ", sum_s[ii], sum_g[ii]);
  }
  printf ("\n");
  return ret;
}


inline bool set_less_than (qubit_info * s1, qubit_info * s2)
{
  return (s1->set_size < s2->set_size);
}

inline bool set_greater_than (qubit_info * s1, qubit_info * s2)
{
  return (s1->set_size > s2->set_size);
}

inline void print_qubit_properties (unsigned prop)
{
  int pp;
  for (pp = acc_other; pp <= acc_partial_share; pp++)
  {
    if (pp > acc_other)
      printf (", ");
    printf ("[%s=%d]", access_type_str[pp], has_qubit_property(prop,pp));
  }
  printf ("\n");
}

inline void print_qubit_info (qubit_info* qinfo)
{
  char * str = isl_set_to_str ((qinfo)->set);
  char * ids = isl_set_to_str ((qinfo)->sid);
  const char * domname = isl_set_get_tuple_name ((qinfo)->domain);
  printf ("[%s] %s : %u, %u, %d [%s]\n", (char*)domname, str, (qinfo)->set_size, (qinfo)->dom_size, (qinfo)->num_dom, ids);
  free (str);
  free (ids);
}

inline void print_list_qubit_info (list<qubit_info*> & mylist)
{
  list<qubit_info*>::iterator ss;
  int ii;
  for (ss = mylist.begin (), ii = 0; ss != mylist.end (); ss++, ii++)
  {
    print_qubit_info (*ss);
  }
}

inline qubit_info * copy_qubit_info (qubit_info * sinfo)
{
  qubit_info * ret;
  ret = new qubit_info;
  *ret = *sinfo;
  ret->sid = isl_set_copy (sinfo->sid);
  ret->set = isl_set_copy (sinfo->set);
  return ret;
}



class aquma_statement {

  isl_set * m_id;
  unsigned m_plain_id;
  bool m_is_2qbit; // is it a two-qubit gate ?
  isl_set * m_domain;
  isl_map * m_read;
  isl_map * m_write;
  isl_map * m_schedule;
  isl_map * m_affinity;
  isl_ctx * m_ctx;
  long m_size;

  public:

  aquma_statement (isl_ctx * pctx, unsigned statement_id = 0) 
  {
    m_ctx = pctx;
    m_plain_id = statement_id;
    m_id = NULL;
    m_is_2qbit = true;
    m_domain = NULL;
    m_read = NULL;
    m_write = NULL;
    m_schedule = NULL;
    m_affinity = NULL;
    if (statement_id > 0)
      m_id = this->build_anonymous_singleton_(statement_id);
    m_size = -1;
  }

  aquma_statement (isl_ctx * pctx, isl_set * a_dom, isl_map * a_read, 
    isl_map * a_write, isl_map * a_sched, unsigned statement_id = 0) 
  {
    assert (a_dom);
    m_ctx = pctx;
    m_id = NULL;
    m_is_2qbit = true;
    m_domain = isl_set_copy (a_dom);
    m_read = isl_map_copy (a_read);
    m_write = isl_map_copy (a_write);
    m_schedule = isl_map_copy (a_sched);
    m_affinity = NULL;
    if (statement_id > 0)
    {
      m_id = this->build_anonymous_singleton_(statement_id);
      m_plain_id = statement_id;
    }
    m_size = aquma_utils::get_set_size_nd (m_domain);
  }


  __isl_give isl_set * build_set_from_qubit (unsigned qubit)
  {
    string cst = "";
    cst += " { q[";
    cst += std::to_string(qubit);
    cst += " ] };";
    isl_set * ret;
    ret = isl_set_read_from_str (m_ctx, cst.c_str ());
    return ret;
  }

  ~ aquma_statement () {
    if (m_domain)
      isl_set_free (m_domain);
    if (m_read)
      isl_map_free (m_read);
    if (m_write)
      isl_map_free (m_write);
    if (m_schedule)
      isl_map_free (m_schedule);
    if (m_affinity)
      isl_map_free (m_affinity);
  }

  __isl_give isl_set * my_id ()
  {
    return isl_set_copy (m_id);
  }

  unsigned get_plain_id ()
  {
    return m_plain_id;
  }

  long get_domain_size ()
  {
    if (m_size < 0 && m_domain)
    {
      m_size = aquma_utils::get_set_size_nd (m_domain);
    }
    return m_size;
  }

  __isl_give isl_set * build_anonymous_singleton_(unsigned val)
  {
    string cst = "";
    cst += " { [";
    cst += std::to_string(val);
    cst += " ] };";
    isl_set * ret;
    ret = isl_set_read_from_str (m_ctx, cst.c_str ());
    return ret;
  }

  isl_set * copy_domain () {
    if (m_domain)
      return isl_set_copy (m_domain);
    return NULL;
  }

  isl_map * copy_read () {
    if (m_read)
      return isl_map_copy (m_read);
    return NULL;
  }

  isl_map * copy_read_with_domain () {
    if (m_read && m_domain)
      return isl_map_intersect_domain (
        isl_map_copy (m_read), isl_set_copy (m_domain));
    return NULL;
  }

  isl_map * copy_write () {
    if (m_write)
      return isl_map_copy (m_write);
    return NULL;
  }

  isl_map * copy_write_with_domain () {
    if (m_write && m_domain)
      return isl_map_intersect_domain (
        isl_map_copy (m_write), isl_set_copy (m_domain));
    return NULL;
  }

  isl_map * copy_schedule () {
    if (m_schedule)
      return isl_map_copy (m_schedule);
    return NULL;
  }

  isl_map * copy_affinity () {
    if (m_affinity)
      return isl_map_copy (m_affinity);
    return NULL;
  }

  __isl_give isl_set * complete_qubit_pairs (__isl_take isl_set * qbset)
  {
    if (not m_is_2qbit)
    {
      isl_set * ret = isl_set_copy (qbset);
      isl_set_free (qbset);
      return ret;
    }

    isl_map * pairs = this->compute_distance_map ();
    isl_set * acc1;
    isl_set * acc2;
    //fflush (stdout); printf ("Intersecting qbtset: "); isl_set_dump (qbset); fflush (stderr);
    acc1 =  isl_map_range (
      isl_map_intersect_domain (isl_map_copy (pairs), isl_set_copy (qbset)));
    acc2 =  isl_map_domain (
      isl_map_intersect_range (isl_map_copy (pairs), isl_set_copy (qbset)));
    //isl_set_free (qbset);
    isl_map_free (pairs);
    //fprintf (stderr, "l-");
    //isl_set_dump (acc1); 
    //fprintf (stderr, "r-");
    //isl_set_dump (acc2); 
    qbset = isl_set_union (qbset, acc1);
    qbset = isl_set_union (qbset, acc2);
    //fprintf (stderr, "u-");
    //isl_set_dump (ret); fflush (stderr);
    return qbset;
  }

  #define MAXBUF 5000
  void read_statement (FILE * ff)
  {
    if (!ff)
      return;
    char buf[MAXBUF];
    char * ptr;
    // Assume a 1-qubit gate doesn't have a read relation.
    // TODO: update m_is_2qbit depending on the contents of the read relation.
    ptr = fgets (buf, MAXBUF, ff);
    assert (ptr);
    printf ("Line read 1: %s\n", ptr);
    m_domain = isl_set_read_from_str (m_ctx, buf);
    ptr = fgets (buf, MAXBUF, ff);
    assert (ptr);
    printf ("Line read 2: %s\n", ptr);
    m_read = isl_map_read_from_str (m_ctx, buf);
    ptr = fgets (buf, MAXBUF, ff);
    assert (ptr);
    printf ("Line read 3: %s\n", ptr);
    m_write = isl_map_read_from_str (m_ctx, buf);
    ptr = fgets (buf, MAXBUF, ff);
    assert (ptr);
    printf ("Line read 4: %s\n", ptr);
    m_schedule = isl_map_read_from_str (m_ctx, buf);
    ptr = fgets (buf, MAXBUF, ff);
    assert (ptr);
    printf ("Line read 5: %s\n", ptr);
    m_affinity = isl_map_read_from_str (m_ctx, buf);
  }

  bool is_included_in (qubit_info * qinfo)
  {
    bool ret = (isl_set_is_subset (m_id, qinfo->sid) == isl_bool_true);  
    /*
    fprintf (stderr, "Checking for statement inclusion in slice (%d):", ret);
    isl_set_dump (m_id);
    isl_set_dump (qinfo->sid);
    fprintf (stderr,"\n"); fflush (stderr);
    */
    return ret;
  }

  /*
    Compute the set of qubits that are common to both 
    the read and write relations.
  */
  isl_set * compute_shared_access ()
  {
    isl_map * q1;
    isl_map * q2;
    q1 = this->copy_read_with_domain ();
    q2 = this->copy_write_with_domain ();
    return isl_set_intersect (
      isl_map_range (q1), isl_map_range (q2));
  }

  /*
   * q1 : D -> q
   * q2 : D -> q
   * Result = inverse(q1) . q2
   */
  __isl_give isl_map * compute_distance_map ()
  {
    isl_map * q1;
    isl_map * q2;
    q1 = this->copy_read_with_domain ();
    q1 = isl_map_reverse (q1);
    q2 = this->copy_write_with_domain ();
    q1 = isl_map_apply_range (q1, q2);
    return q1;
  }

  unsigned compute_read_size ()
  {
    isl_set * qspace;
    qspace = isl_map_range (this->copy_read_with_domain ());
    int ret = this->compute_set_size (qspace);
    if (ret >= 0)
      return (unsigned)(ret);
    return 0;
  }

  unsigned compute_write_size ()
  {
    isl_set * qspace;
    qspace = isl_map_range (this->copy_write_with_domain ());
    int ret = this->compute_set_size (qspace);
    if (ret >= 0)
      return (unsigned)(ret);
    return 0;
  }

  /*
   * q1 : D -> q
   * q2 : D -> q
   * dir1 = inverse(q1) . q2
   * dir2 = inverse(dir2)
   * Result = dir1 U dir2
   */
  isl_map * compute_directed_link_map (__isl_take isl_set * qpart = NULL)
  {
    isl_map * dir1 = this->compute_distance_map ();
    if (qpart)
    {
      dir1 = isl_map_intersect_domain (dir1, isl_set_copy (qpart));
      dir1 = isl_map_intersect_range (dir1, isl_set_copy (qpart));
    }
    isl_map * dir2 = isl_map_copy (dir1);
    dir2 = isl_map_reverse (dir2);
    dir1 = isl_map_union (dir1, dir2);
    return dir1;
  }

  /*
   * Count the number of links used by the current statement.
   * Example: CX i, i+1 with i in 1:4 would result in 4 (<1,2>, <2,1> ... <4,5>).
   */
  //unsigned count_used_links (__isl_take isl_map * dlm = NULL)
  unsigned count_used_links (__isl_take isl_set * qpart = NULL)
  {
    isl_map * dlm;
    dlm = this->compute_directed_link_map ();
    if (qpart)
    {
      dlm = isl_map_intersect_domain (dlm, isl_set_copy (qpart));
      dlm = isl_map_intersect_range (dlm, isl_set_copy (qpart));
      isl_set_free (qpart);
    }
    unsigned ret;
    ret = this->compute_set_size (isl_map_wrap (dlm)) / 2;
    return ret;
  }

  unsigned count_qubit_incidence (unsigned qubit, __isl_keep isl_set * qpart = NULL)
  {
    isl_set * qset = this->build_set_from_qubit (qubit);
    isl_map * qmap = this->compute_distance_map ();
    if (qpart) 
    {
      qmap = isl_map_intersect_domain (qmap, isl_set_copy (qpart));
      qmap = isl_map_intersect_range (qmap, isl_set_copy (qpart));
    }
    isl_map * rqmap = isl_map_reverse (isl_map_copy (qmap));
    qmap = isl_map_intersect_domain (qmap, isl_set_copy (qset));
    rqmap = isl_map_intersect_domain (rqmap, isl_set_copy (qset));
    isl_set * wqmap = isl_map_wrap (qmap);
    isl_set * wrqmap = isl_map_wrap (rqmap);
    wqmap = isl_set_union (wqmap, wrqmap);
    unsigned count;
    count = this->compute_set_size (wqmap);
    isl_set_free (qset);
    return count;
  }

  /*
   * Count the number of edges incident on the qubits of the current slice.
   *
   */
  void count_all_qubit_incidence (vector<unsigned> & qcount, __isl_keep isl_set * qpart)
  {
    set<unsigned> * qubits;
    set<unsigned>::iterator qq;
    qubits = aquma_utils::isl_set_to_unsigned_set (qpart);
    //assert (qcount.size () == qubits->size ());
    for (qq = qubits->begin (); qq != qubits->end(); qq++)
    {
      assert (*qq < qcount.size ());
      unsigned qi = this->count_qubit_incidence (*qq, qpart);
      qcount[*qq] += qi;
    }
    delete qubits;
  }

  /*
    Compute the set of qubits accessed (read or write)
    by the statement.
  */
  isl_set * compute_qubit_space (__isl_keep isl_set * qpart = NULL)
  {
    isl_map * q1;
    isl_map * q2;
    q1 = this->copy_read_with_domain ();
    q2 = this->copy_write_with_domain ();
    if (qpart)
    {
      //q1 = isl_map_intersect_domain (q1, isl_set_copy (qpart));
      q1 = isl_map_intersect_range (q1, isl_set_copy (qpart));
      //q2 = isl_map_intersect_domain (q2, isl_set_copy (qpart));
      q2 = isl_map_intersect_range (q2, isl_set_copy (qpart));
    }
    return isl_set_union (
      isl_map_range (q1), isl_map_range (q2));
  }

  /*
    Compute points in iteration domain that have self-conflicts.
  */
  isl_set * compute_domain_conflicts ()
  {
    isl_map * cr; // conflicted reads
    isl_map * cw; // conflicted writes
    isl_set * conflicted;
    conflicted = this->compute_shared_access ();
    cr = isl_map_intersect_range (
      this->copy_read_with_domain (), isl_set_copy (conflicted));
    cw = isl_map_intersect_range (
      this->copy_write_with_domain (), isl_set_copy (conflicted));
    isl_set * ret;
    ret = isl_set_union (
      isl_map_domain (cr), isl_map_domain (cw));
    isl_set_free (conflicted);
    
    //fprintf (stderr," Conflicts: ");
    //isl_set_dump (ret); fflush (stderr);

    return ret;
  }

  isl_pw_qpolynomial * get_expression_from_set (isl_set * aset = NULL) 
  {
    if (!aset)
    {
      aset = compute_domain_conflicts ();
    }
    //isl_set * temp = isl_set_copy (aset);
    // NOTE: isl_set_card "takes" the set given as argument.
    isl_pw_qpolynomial * ret = isl_set_card (aset);
    //isl_pw_qpolynomial_dump (ret);

    //ret = isl_pw_qpolynomial_sum (ret);
    //ret = isl_pw_qpolynomial_sum_intersect_domain (ret, temp)
    return ret;
  }

  /*
    Destructive compute_set_size()
  */
  long compute_set_size (__isl_take isl_set * aset)
  {
    isl_pw_qpolynomial * pwqp = this->get_expression_from_set (aset);
    isl_qpolynomial * qp = NULL;
    long ret = -1;
    //evalue * val;
    //val = isl_qpolynomial_to_evalue (pwqp);
    if (isl_pw_qpolynomial_isa_qpolynomial (pwqp) == isl_bool_true)
    {
      qp = isl_pw_qpolynomial_as_qpolynomial (pwqp);
      isl_val * val = isl_qpolynomial_get_constant_val (qp);
      ret = isl_val_get_num_si (val);
      long denom = isl_val_get_den_si (val);
      if (denom != 1)
      {
        printf ("Val from set is: %ld/%ld\n", ret, denom);
        printf ("Offending val is: "); 
        isl_val_dump (val); fflush (stderr);
        fprintf (stderr, "\n");
        assert (denom == 1);
      }
      isl_qpolynomial_free (qp);
      isl_val_free (val);
    }
    //assert (val->d == 1);
    //isl_qpolynomial_free (pwqp);
    return ret;
  }

  /*
    Non-destructive compute_set_size()
  */
  long compute_set_size_nd (__isl_keep isl_set * aset)
  {
    isl_set * temp = isl_set_copy (aset);
    return this->compute_set_size (temp);
  }

  void show () 
  {
    printf ("Domain: "); fflush (stdout);
    if (m_domain)
      isl_set_dump (m_domain);
    printf ("Read     : "); fflush (stdout);
    if (m_read)
      isl_map_dump (m_read);
    printf ("Write    : "); fflush (stdout);
    if (m_write)
      isl_map_dump (m_write);
    printf ("Schedule : "); fflush (stdout);
    if (m_schedule)
      isl_map_dump (m_schedule);
    printf ("Affinity : "); fflush (stdout);
    if (m_affinity)
      isl_map_dump (m_affinity);
    if (m_read && m_write)
    {
      /*
      bool is_uniform = this->is_access_uniform ();
      bool is_dist_1 = this->is_access_pipeline_dist (1);
      bool is_dist_2 = this->is_access_pipeline_dist (2);
      bool is_parallel = this->is_parallel ();
      bool is_seq_with_comm = this->is_sequential_with_comm ();
      printf ("Properties : (P=%d), (SeqComm=%d), (U=%d), (D1=%d), (D2=%d)\n", 
        is_parallel, is_seq_with_comm, is_uniform, is_dist_1, is_dist_2);
      */
      float shared_fraction = this->compute_qubit_shared_fraction ();
      unsigned properties = this->compute_qubit_properties ();
      unsigned num_links = this->count_used_links ();
      print_qubit_properties (properties);
      unsigned qspace_size  = this->compute_set_size (this->compute_qubit_space ());
      printf ("No. of Used Qubits: %d\n", qspace_size);
      printf ("No. of Comm. Links : %d\n", num_links);
      printf ("Shared Fraction : %.2f\n", 100 * shared_fraction);
      unsigned qcount = this->count_qubit_incidence (0);
      printf ("Incidence of q[0] : %d\n",qcount);
    }
  }

  /*
   * Determine if the given statement has a uniform (pipelined) access
   * with an specific distance K. For example
   * CX i, i+1 ==> K = 1
   * CX 2i, 2i + 2 ==> K = 2
   * CX 2i, 2i + 1 ==> Distance is always 1, but it's not pipelined, it's parallel.
   * point in the iteration domain must be the same.
   * Compute the deltas set, and this must be a singleton with a fixed value.
   */
  bool is_access_pipeline_dist (unsigned dist)
  {
    if (not m_is_2qbit)
      return false;
    isl_map * dm = this->compute_distance_map ();
    isl_set * deltas = isl_map_deltas (dm);
    bool ret = false;
    if (isl_set_is_singleton (deltas) == isl_bool_true)
    {
      isl_val * val = isl_set_plain_get_val_if_fixed (deltas, isl_dim_set, 0);
      unsigned distance = isl_val_get_num_si (val);
      assert (isl_val_get_den_si (val) == 1);
      ret = (distance == dist);
      isl_val_free (val);
    }
    isl_set_free (deltas);
    if (not ret)
      return ret;
    isl_set * qspace = this->compute_shared_access ();
    unsigned shared_size = this->compute_set_size (qspace);
    unsigned write_size = this->compute_read_size ();
    unsigned read_size = this->compute_write_size ();
    ret = ret and (shared_size >= 0.5 * read_size and read_size == write_size);
    return ret;
  }


  /*
  * Return the distance, if uniform. Returns -1 if not uniform.
  * 
  *
  */
  int get_qubit_distance_if_uniform ()
  {
    int ret = -1;
    if (not m_is_2qbit)
      return ret;
    isl_map * dm = this->compute_distance_map ();
    isl_set * deltas = isl_map_deltas (dm);
    if (isl_set_is_singleton (deltas) == isl_bool_true)
    {
      isl_val * val = isl_set_plain_get_val_if_fixed (deltas, isl_dim_set, 0);
      unsigned distance = isl_val_get_num_si (val);
      assert (isl_val_get_den_si (val) == 1);
      isl_val_free (val);
      ret = distance;
    }
    isl_set_free (deltas);
    return ret;
  }

  /*
   * Recognize cases such as:
   * CX i, i+1
   * CX 2i, 2i + 1
   * CX 3i, 3i + 2
   * The distance between every pair of qubits accessed for a given
   * point in the iteration domain must be the same.
   * We do not have the shared/disjoint space criterion. So statement
   * can have parallel and/or serial operations.
   * Compute the deltas set, and this must be a singleton with a fixed value.
   */
  bool is_access_uniform ()
  {
    isl_map * dm = this->compute_distance_map ();
    isl_set * deltas = isl_map_deltas (dm);
    bool ret = false;
    if (isl_set_is_singleton (deltas) == isl_bool_true)
    {
      ret = true;
    }
    isl_set_free (deltas);
    return ret;
  }

  /*
   * Recognize cases such as:
   * CX 2i, 2i+1
   * Conditions: 
   * i) access disjoint qubit-spaces.
   * ii) access the same number of qubits.
   * iii) Only use with 2-qubit gates.
   */
  bool is_parallel ()
  {
    if (not m_is_2qbit)
      return true;
    isl_set * qspace = this->compute_shared_access ();
    bool ret = false;
    if (isl_set_is_empty (qspace) == isl_bool_true)
      ret = true;
    isl_set_free (qspace);
    if (not ret)
      return ret;
    ret = ret and (this->compute_read_size () == this->compute_write_size ());    
    return ret;
  }

  /*
   * Recognize cases such as:
   * CX 0, i 
   * CX 2i+1 , N
   * The constant expression forces serialization.
   */
  bool is_sequential_with_comm ()
  {
    isl_set * qspace = this->compute_shared_access ();
    bool ret = true;
    if (isl_set_is_empty (qspace) == isl_bool_false)
      ret = false;
    isl_set_free (qspace);
    if (not ret)
      return ret;
    ret = ret and (this->compute_read_size () + this->compute_write_size () > 2);
    return ret;
  }

  float compute_qubit_shared_fraction ()
  {
    isl_set * sharedq = this->compute_shared_access ();
    isl_set * qspace = this->compute_qubit_space ();
    unsigned shared_size = this->compute_set_size (sharedq);
    unsigned full_size = this->compute_set_size (qspace);
    return (1.0 * shared_size) / (1.0 * full_size);
  }

  
  unsigned compute_qubit_properties ()
  {
    bool is_uniform = this->is_access_uniform ();
    bool is_dist_1 = this->is_access_pipeline_dist (1);
    //bool is_dist_2 = this->is_access_pipeline_dist (2);
    int distance = this->get_qubit_distance_if_uniform ();
    bool is_uniform_dist_k = (is_uniform and distance > 0);
    bool is_parallel = this->is_parallel ();
    bool is_seq_with_comm = this->is_sequential_with_comm ();
    float shared_fraction = this->compute_qubit_shared_fraction ();
    bool is_partial_share = (shared_fraction >= 0.1);
    unsigned ret = 0;
    //printf ("Properties : (P=%d), (SeqComm=%d), (U=%d), (D1=%d), (D2=%d)\n", 
    //  is_parallel, is_seq_with_comm, is_uniform, is_dist_1, is_dist_2);
    //printf ("Shared Fraction : %.2f\n", 100 * shared_fraction);
    pset_qubit_property (is_uniform,ret,acc_uniform);
    pset_qubit_property (is_parallel,ret,acc_parallel);
    pset_qubit_property (is_dist_1,ret,acc_pipe_1);
    pset_qubit_property (is_seq_with_comm,ret,acc_seq_no_comm);
    pset_qubit_property (is_uniform_dist_k,ret,acc_pipe_k);
    pset_qubit_property (is_partial_share,ret,acc_pipe_k);
    //acc_other=1, acc_parallel, acc_uniform, acc_pipe_1, acc_seq_no_comm, acc_pipe_k, acc_partial_share};
    return ret;
  }

};


typedef vector<aquma_statement*> aquma_statement_vector;
typedef vector<aquma_statement*>::iterator aquma_statement_vector_iterator;

inline bool set_gt (aquma_statement* left, aquma_statement* right)
{
  return (left->get_domain_size () > right->get_domain_size ());
}

inline void sort_statements_by_domain_size (aquma_statement_vector & vs)
{
  std::sort (vs.begin (), vs.end (), set_gt);
}


#endif
