#ifndef _AQUMA_CIRCUIT_HH_

#define _AQUMA_CIRCUIT_HH_

using namespace std;
#include <list>
#include <vector>
#include <iostream>
#include <map>

#include "utils.hh"
#include "options.hh"
#include <math.h>

#define G2_ERROR_ROUND_FACTOR (10000)
#define G2_TIME_ROUND_FACTOR (0.1f)
#define G2_RSCORE_ROUND_FACTOR (100)

inline
int 
round_g2_error (float val)
{
  return (int)(floorf (val * G2_ERROR_ROUND_FACTOR));
}

inline
int 
round_g2_time (float val)
{
  return (int)(floorf (roundf (val * G2_TIME_ROUND_FACTOR)));
}

inline
int 
round_rscore (float val)
{
  return (int)(floorf (roundf (val * G2_RSCORE_ROUND_FACTOR)));
}

typedef map<unsigned,unsigned> de_counter_t;
typedef map<unsigned,unsigned>::iterator de_counter_iter_t;

class aquma_circuit {
  int m_nops;
  unsigned m_dev_width; // number of qubits
  vector<unsigned> m_gates1;
  vector<unsigned> m_quasa; // No. times qubit i is used as source of a 2Q-gate.
  vector<unsigned> m_outdeg; // Ideal Out degree of qubit at position i (doesn't consider topo)
  de_counter_t m_d_emap; // directed edge map counter
  int m_num_quasa; // number of gates using 2-qubits

  public:

  aquma_circuit (unsigned p_dev_width, int n_opts)
  {
    m_nops = n_opts;
    m_dev_width = p_dev_width;
    cout << "No. QUBITS : " << m_dev_width << endl;
    m_gates1.resize (m_dev_width);
    m_quasa.resize (m_dev_width);
    m_outdeg.resize (m_dev_width);
    m_num_quasa = 0;
  }

  ~ aquma_circuit ()
  {

  }

  de_counter_t & get_edge_counter_map ()
  {
    return m_d_emap;
  }

  int get_ideal_out_degree (unsigned qidx)
  {
    if (qidx >= m_dev_width)
      return -1;
    return m_outdeg[qidx];
  }

  /*
   * Return TOTAL number of qubits used 2Q-gates.
   */
  unsigned get_total_g2_count ()
  {
    unsigned ret = 0;
    vector<unsigned>::iterator nn;
    for (nn = m_quasa.begin (); nn != m_quasa.end (); nn++)
      ret += *nn;
    return ret;
  }

  /*
   * Return number of distinct qubits used 2Q-gates.
   */
  unsigned get_number_distinct_qubits_in_2Q_gates ()
  {
    unsigned ret = 0;
    vector<unsigned>::iterator nn;
    for (nn = m_quasa.begin (); nn != m_quasa.end (); nn++)
      if (*nn > 0)
        ret ++;
    return ret;
  }

  unsigned get_total_weighted_dir_edge_count ()
  {
    unsigned ret = 0;
    de_counter_t::iterator ee;
    for (ee = m_d_emap.begin (); ee != m_d_emap.end (); ee++)
      ret += ee->second;
    return ret;
  }

  unsigned get_total_distinct_dir_edge_count ()
  {
    unsigned ret = 0;
    de_counter_t::iterator ee;
    for (ee = m_d_emap.begin (); ee != m_d_emap.end (); ee++)
      if (ee->second > 0)
        ret ++;
    return ret;
  }


  float get_edge_count_mean ()
  {
    float ret = 0.0f;
    de_counter_t::iterator ee;
    for (ee = m_d_emap.begin (); ee != m_d_emap.end (); ee++)
    {
      ret += ee->second;
    }
    if (ret > 0)
      ret = ret / m_d_emap.size ();
    return ret;
  }

  float get_edge_count_stdv (float emean)
  {
    float ret = 0.0f;
    de_counter_t::iterator ee;
    for (ee = m_d_emap.begin (); ee != m_d_emap.end (); ee++)
    {
      float diff = ee->second - emean;
      ret += diff * diff;
    }
    if (ret > 0)
      ret = ret / m_d_emap.size ();
    ret = sqrt (ret);
    return ret;
  }

  unsigned get_edge_weight (unsigned q1, unsigned q2)
  {
    unsigned idx = q1 * m_dev_width + q2;
    de_counter_t::iterator ee = m_d_emap.find (idx);
    if (ee == m_d_emap.end ())
      return 0;
    return ee->second;
  }


  unsigned set_edge_weight (unsigned q1, unsigned q2)
  {
    unsigned idx = q1 * m_dev_width + q2;
    unsigned count = this->get_edge_weight (q1, q2);
    if (!count)
    {
      m_d_emap.insert (std::pair<unsigned,unsigned>(idx,1));
      return 1;
    }
    m_d_emap[idx] = count + 1;
    return count + 1;
  }

  void read_qasm (FILE * ff)
  {
    int discard;
    int narg;
    while ((discard = fscanf (ff, "%d", &narg)) > 0)
    {
      unsigned q1, q2;
      discard = fscanf (ff, "%u", &q1);
      assert (narg >= 1 && narg <= 2);
      if (narg == 2)
      {
        discard = fscanf (ff, "%u", &q2);
        this->set_edge_weight (q1, q2);
        //m_quasa[q1]++;
        //m_quasa[q2]++; // skip this, we are only interested in the 'Incidence' of q1.
        m_num_quasa++;
      }
      else
      {
        m_gates1[q1]++;
      }
    }
  }

  void compute_qubit_source_incidence ()
  {
    map<unsigned,unsigned>::iterator ee;
    for (ee = m_d_emap.begin (); ee != m_d_emap.end (); ee++)
    {
      unsigned q1 = ee->first / m_dev_width;
      //unsigned q2 = ee->first % m_dev_width;
      m_quasa[q1] += ee->second;
      m_outdeg[q1] ++;
    }
    
  }

  void show_qubit_histograms ()
  {
    vector<unsigned>::iterator qq;
    unsigned idx;
    cout << "Histogram 1: " << endl;
    for (qq = this->m_gates1.begin (), idx = 0; qq != this->m_gates1.end (); qq++, idx++)
    {
      cout << "q" << idx << "[" << *qq << "]" << endl;
    }
    cout << "No. times Q_{k} used a Source Argument: " << endl;
    for (qq = this->m_quasa.begin (), idx = 0; qq != this->m_quasa.end (); qq++, idx++)
    {
      cout << "q" << idx << "[" << *qq << "]" << endl;
    }
    cout << "Ideal Out-Degree for Q_{k} : " << endl;
    for (qq = this->m_outdeg.begin (), idx = 0; qq != this->m_outdeg.end (); qq++, idx++)
    {
      cout << "q" << idx << "[" << *qq << "]" << endl;
    }
    cout << "Directed Edge histogram: " << endl;
    map<unsigned,unsigned>::iterator ee;
    unsigned total_edge = 0;
    for (ee = m_d_emap.begin (); ee != m_d_emap.end (); ee++)
    {
      unsigned q1 = ee->first / m_dev_width;
      unsigned q2 = ee->first % m_dev_width;
      total_edge += ee->second;
      cout << "(" << q1 << "->" << q2 << ") : " << ee->second << endl;
    }
    cout << "Total edges : " << total_edge << endl;
    float emean = this->get_edge_count_mean ();
    float estdv = this->get_edge_count_stdv (emean);
    cout << "Edge mean          : " << emean << endl;
    cout << "Edge standard desv : " << estdv << endl;
    int ii;
    for (ii = 1; ii <= 4; ii++)
    {
      cout << "Mean + " << ii << " stdv : " << emean + (ii) * estdv << endl;
    }
    cout << endl;
    
  }

  #if 0
  void collect_high_impact_qubits (vector<int> & hiq)
  {
    hiq.clear ();
    float emean = this->get_edge_count_mean ();
    float estdv = this->get_edge_count_stdv (emean);
    map<unsigned,unsigned>::iterator ee;
    for (ee = m_d_emap.begin (); ee != m_d_emap.end (); ee++)
    {
      unsigned q1 = ee->first / m_dev_width;
      unsigned q2 = ee->first % m_dev_width;
      // Pending!!!
    }
  }
  #endif
};

#endif
