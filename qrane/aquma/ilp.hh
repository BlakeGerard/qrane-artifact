#ifndef _AQUMA_ILP_HH_


#define _AQUMA_ILP_HH_

#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <cmath>
#include <assert.h>

#include "isl/set.h"
#include "isl/ctx.h"

#include "options.hh"
#include "circuit.hh"

using namespace std;

typedef union {
  int as_int;
  float as_float;
} opt_var_t;

enum ilp_var_type {
  undefined_type_e,
  vertex_bool_e, vertex_count_e, vertex_target_degree_e, vertex_rscore_e,
  edge_bool_e, edge_count_e, edge_2gate_bool_e,
  vertex_edge_bool_e, vertex_edge_count_e,
  objective_min_e, objective_max_e, objective_min_eq_e, objective_max_eq_e
};
typedef enum ilp_var_type ilp_var_type_t;

#define ILP_VAR_INT 0
#define ILP_VAR_FLT 1

class ilp_var
{

  string m_name;
  opt_var_t m_lb;
  opt_var_t m_ub;
  opt_var_t m_opt;
  ilp_var_type_t m_type;
  int m_datatype;

  int m_qid; // qubit id; -1 if the variable is not associated to a qubit.
  int m_eid; // edge id, -1 if the variable is not associated to an edge.

  public:

  ilp_var ()
  {
    m_name = "";
    m_lb.as_int = 1000000;
    m_ub.as_int = -1;
    m_type = undefined_type_e;
    m_opt.as_int = 0;
    m_qid = -1;
    m_eid = -1;
    m_datatype = ILP_VAR_INT;
  }

  ilp_var (string pname, ilp_var_type_t p_vartype, 
      int plb, int pub, int qubit_id = -1, int edge_id = -1)
  {
    this->set_attributes (pname, p_vartype, plb, pub, qubit_id, edge_id);
  }

  ilp_var (string pname, ilp_var_type_t p_vartype, 
      float plb, float pub, int qubit_id = -1, int edge_id = -1)
  {
    this->set_attributes (pname, p_vartype, plb, pub, qubit_id, edge_id);
  }

  ~ ilp_var ()
  {

  }

  bool is_objective ()
  {
    if (m_type == objective_min_e)
      return true;
    if (m_type == objective_max_e)
       return true;     
    if (m_type == objective_min_eq_e)
      return true;
    if (m_type == objective_max_eq_e)
      return true;
    return false;
  }

  bool is_non_negative () // so >= 0
  {
    if (this->is_int ())
      return m_lb.as_int >= 0 and m_ub.as_int >= 0;
    if (this->is_float ())
      return m_lb.as_float >= 0 and m_ub.as_float >= 0;
    return false;
  }

  bool is_non_positive () // so <= 0
  {
    if (this->is_int ())
      return m_lb.as_int <= 0 and m_ub.as_int <= 0;
    if (this->is_float ())
      return m_lb.as_float <= 0 and m_ub.as_float <= 0;
    return false;
  }


  bool is_boolean ()
  {
    if (m_type == vertex_bool_e)
      return true;
    if (m_type == edge_bool_e)
      return true;
    if (m_type == vertex_edge_bool_e)
      return true;
    return false;
  }

  bool is_int ()
  {
    return m_datatype == ILP_VAR_INT;
  }

  bool is_float ()
  {
    return m_datatype == ILP_VAR_FLT;
  }

  string get_operator_goal ()
  {
    // NOTE: assume constraint on the left, and cst on the right.
    switch (m_type) 
    {
      case objective_min_e:
        return " >= ";
      case objective_max_e:
        return " <= ";
      case objective_min_eq_e:
        return " = ";
      case objective_max_eq_e:
        return " = ";
      default:
        cerr << "Variable " + m_name + " is not an objective variable." << endl;
        assert (0);
        return " ERROR ";
    }
    return " ERROR";
  }

  void set_attributes (string pname, ilp_var_type_t p_vartype, 
      int plb, int pub, int qubit_id, int edge_id)
  {
    m_name = pname;
    m_type = p_vartype;
    m_lb.as_int = plb;
    m_ub.as_int = pub;
    m_opt.as_int = 0;
    m_datatype = ILP_VAR_INT;
    m_qid = qubit_id;
    m_eid = edge_id;
  }

  void set_attributes (string pname, ilp_var_type_t p_vartype, 
      float plb, float pub, int qubit_id, int edge_id)
  {
    m_name = pname;
    m_type = p_vartype;
    m_lb.as_float = plb;
    m_ub.as_float = pub;
    m_opt.as_float = 0.000000;
    m_datatype = ILP_VAR_FLT;
    m_qid = qubit_id;
    m_eid = edge_id;
  }

  void set_lb (int p_lb)
  {
    m_lb.as_int = p_lb;
  }

  void set_ub (int p_ub)
  {
    m_ub.as_int = p_ub;
  }

  void set_lb (float p_lb)
  {
    m_lb.as_float = p_lb;
  }

  void set_ub (float p_ub)
  {
    m_ub.as_float = p_ub;
  }

  int get_lb ()
  {
    return m_lb.as_int;
  }

  int get_ub ()
  {
    return m_ub.as_int;
  }

  string get_name ()
  {
    return m_name;
  }

  int get_qubit_id ()
  {
    return m_qid;
  }


  void set_optimum (int opt_val)
  {
    m_opt.as_int = opt_val;
  }

  void set_optimum (float opt_val)
  {
    m_opt.as_float = opt_val;
  }

  int get_optimum_as_int ()
  {
    return m_opt.as_int;
  }

  float get_optimum_as_float ()
  {
    return m_opt.as_float;
  }

  int get_edge_id ()
  {
    return m_eid;
  }

  ilp_var_type_t get_type ()
  {
    return m_type;
  }

  string get_optimum_as_string ()
  {
    if (m_datatype == ILP_VAR_INT)
      return std::to_string (m_opt.as_int);
    if (m_datatype == ILP_VAR_FLT)
      return std::to_string (m_opt.as_float);
    return "ERROR";
  }

  string get_lb_as_string ()
  {
    if (m_datatype == ILP_VAR_INT)
      return std::to_string (m_lb.as_int);
    if (m_datatype == ILP_VAR_FLT)
      return std::to_string (m_lb.as_float);
    return "ERROR";
  }

  int get_lb_as_int ()
  {
    assert (m_datatype == ILP_VAR_INT);
    return m_lb.as_int;
  }

  float get_lb_as_float ()
  {
    assert (m_datatype == ILP_VAR_FLT);
    return m_lb.as_float;
  }

  int get_ub_as_int ()
  {
    assert (m_datatype == ILP_VAR_INT);
    return m_ub.as_int;
  }

  float get_ub_as_float ()
  {
    assert (m_datatype == ILP_VAR_FLT);
    return m_ub.as_float;
  }

  string get_ub_as_string ()
  {
    if (m_datatype == ILP_VAR_INT)
      return std::to_string (m_ub.as_int);
    if (m_datatype == ILP_VAR_FLT)
      return std::to_string (m_ub.as_float);
    return "ERROR";
  }


  string get_constraints_string (list<string> & m_cst)
  {
    string ret = "";
    ret += m_name + " >= " + this->get_lb_as_string ();
    ret += " and ";
    ret += m_name + " <= " + this->get_ub_as_string ();

    // Store constraint in m_cst For cplex  ILP.
    // Do not add basic contraints to the list of constraints.
    // These are printed to a different part of the ilp script.
    if (true)
    {
      m_cst.push_back (m_name + " >= " + this->get_lb_as_string ());
      m_cst.push_back (m_name + " <= " + this->get_ub_as_string ());
    }
    return ret;
  }

  void print_info ()
  {
    cout << "[" << m_name;
    cout << ", " << this->get_lb_as_string ();
    cout << ", " << this->get_ub_as_string ();
    cout << ", " << this->var_type_to_string ();
    cout << ", " << this->get_optimum_as_string ();
    cout << "]" << endl;
  }

  string var_type_to_string ()
  {
    switch (m_type)
    {
      case vertex_bool_e: return "V_B";
      case vertex_count_e: return "V_C"; 
      case vertex_rscore_e: return "R_S";
      case edge_bool_e : return "E_B";
      case edge_count_e : return "E_C";
      case vertex_target_degree_e: return "V_deg";
      case vertex_edge_bool_e : return "V_E_B";
      case vertex_edge_count_e: return "V_E_C";
      case objective_min_e : return "O_min";
      case objective_max_e: return "O_max";
      case objective_min_eq_e : return "O=min";
      case objective_max_eq_e: return "O=max";
      default: return "undefined";
    }
    return "not-supposed-to-be-here";
  }
};

typedef list<ilp_var*> ilp_var_list;
typedef list<ilp_var*>::iterator ilp_var_list_iterator;

class ilp_system 
{
  ilp_var_list m_vars;    
  string m_ilp;
  list<string> m_cst;
  aquma_options * m_options;
  int m_dev_width;

  public:

  ilp_system (aquma_options * aopts)
  {
    m_ilp = "";
    m_options = aopts;
  }

  ilp_system (aquma_options * aopts, unsigned dev_width)
  {
    m_ilp = "";
    m_options = aopts;
    m_dev_width = dev_width;
  }

  ~ ilp_system ()
  {
    ilp_var_list_iterator vv;
    for (vv = m_vars.begin (); vv != m_vars.end (); vv++)
      delete *vv;
  }

  void print_variables ()
  {
    ilp_var_list_iterator vv;
    cout << "!!!Showing variables in system: " << endl;
    for (vv = m_vars.end (); vv != m_vars.end (); vv++)
    {
      (*vv)->print_info ();
      cout << endl;
    }
  }

  int get_source_qubit_id_from_edge (int edge_id)
  {
    return edge_id / m_dev_width;
  }

  int get_target_qubit_id_from_edge (int edge_id)
  {
    return edge_id % m_dev_width;
  }

  string get_variable_prefix (ilp_var_type p_vartype)
  {
    string prefix;
    switch (p_vartype)
    {
      case undefined_type_e: 
        assert (0);
      case vertex_bool_e: 
        prefix = "n";
        break;
      case vertex_count_e:
        prefix = "v";
        break;
      case vertex_target_degree_e:
        prefix = "td";
        break;
      case vertex_rscore_e:
        prefix = "rq";
        break;
      case edge_bool_e:
        prefix = "ql";  // cplex doesn't like the 'e' prefix, gets confused with exponents
        break;
      case edge_count_e:
        prefix = "qlk"; // qubit link count
        break;
      case edge_2gate_bool_e:
        prefix = "L";
        break;
      case vertex_edge_bool_e:
        prefix = "ve";
        break;
      case vertex_edge_count_e:
        prefix = "f";
        break;
      case objective_min_e:
        prefix = ">=obj_min";
        break;
      case objective_max_e:
        prefix = "<=obj_max";
        break;
      case objective_min_eq_e:
        prefix = "=obj_min";
        break;
      case objective_max_eq_e:
        prefix = "=obj_max";
        break;
    }
    return prefix;
  }

  string variable_name (ilp_var_type p_vartype, int idx)
  {
    string ret = "";
    string prefix = this->get_variable_prefix (p_vartype);
    ret = prefix + std::to_string (idx);
    return ret;
  }

  string variable_name (ilp_var_type p_vartype, int qid, int eid)
  {
    string ret = "";
    if (p_vartype != vertex_edge_bool_e and p_vartype != edge_bool_e)
      return "FAIL";
    string prefix1 = this->get_variable_prefix (vertex_bool_e);
    string prefix2 = this->get_variable_prefix (vertex_bool_e);
    ret = prefix1 + std::to_string (qid) + prefix2 + std::to_string (eid);
    return ret;
  }

  int create_rscore_vars (aquma_qubit_vector & qubits)
  {
    aquma_qubit_vector_iterator qq;
    int rscore_ub = 0;
    for (qq = qubits.begin (); qq != qubits.end (); qq++)
    {
      if ((*qq)->is_mapped ())
        continue;
      string varname = this->variable_name (vertex_rscore_e, (*qq)->my_id ());
      int ub = round_rscore ((*qq)->get_rscore ()) + RSCORE_INVAL;
      ilp_var * var = new ilp_var (varname, vertex_rscore_e, -ub, 0);
      m_vars.push_back (var);
      rscore_ub = (ub > rscore_ub ? ub : rscore_ub);
    }
    return rscore_ub;
  }

  void create_boolean_vars (aquma_qubit_vector & qubits)
  {
    aquma_qubit_vector_iterator qq;
    for (qq = qubits.begin (); qq != qubits.end (); qq++)
    {
      if ((*qq)->is_mapped ())
        continue;
      this->create_one_var (vertex_bool_e, (*qq), 0, 1);
    }
  }


  void create_vars (ilp_var_type_t p_vartype, int p_num, int p_lb, int p_ub, bool reverse = false)
  {
    int ii;
    for (ii = (not reverse ? 0 : p_num - 1); 
        ; ii += (not reverse ? 1 : -1))
    {
      if (reverse and ii < 0)
        break;
      if (not reverse and ii >= p_num)
        break;
      //string varname = p_prefix + std::to_string (ii);
      string varname = this->variable_name (p_vartype, ii);
      ilp_var * var = new ilp_var (varname, p_vartype, p_lb, p_ub);
      m_vars.push_back (var);
    }
  }

  void create_vars (ilp_var_type_t p_vartype, 
    int p_num1, int p_num2, int p_lb, int p_ub)
  {
    int ii;
    int jj;
    for (ii = 0; ii < p_num1; ii++)
      for (jj = 0; jj < p_num2; jj++)
    {
      string varname = "";
      //varname += p_prefix1 + std::to_string (ii);
      //varname += p_prefix2 + std::to_string (jj);
      varname = this->variable_name (p_vartype, ii, jj);
      ilp_var * var = new ilp_var(varname, p_vartype, p_lb, p_ub);
      m_vars.push_back (var);
    }
  }

  void create_vars_boolean (ilp_var_type_t p_vartype, int p_num)
  {
    create_vars (p_vartype, p_num, 0, 1);
  }

  void create_vars_boolean (ilp_var_type_t p_vartype, 
    int p_num1, int p_num2)
  {
    create_vars (p_vartype, p_num1, p_num2, 0, 1);
  }

  float sum_from_edge_map (edge_map & emap)
  {
    edge_map::iterator ee;
    float ret = 0;
    for (ee = emap.begin (); ee != emap.end (); ee++)
      ret += ee->second;
    return ret;
  }

  /*
   * Create one ILP variable for each entry in the edge map given.
   * Use the first component of each pair in the map as the ID of the
   * variable.
   * Variable automatically generated with the variable type and ID.
   */
  void 
  create_vars_boolean_from_edge_map (ilp_var_type_t p_vartype, edge_map & emap)
  {
    edge_map::iterator ee;
    int ii = 0;
    cout << "Entries in edge-map: " << emap.size () << endl;
    for (ee = emap.begin (); ee != emap.end (); ee++, ii++)
    {
      string varname = "";
      unsigned q1 = this->get_source_qubit_id_from_edge (ee->first);
      unsigned q2 = this->get_target_qubit_id_from_edge (ee->first);
      varname = this->variable_name (p_vartype, q1, q2);
      cout << ii << ") Creating variable : " << varname << endl;
      ilp_var * var = new ilp_var (varname, p_vartype, 0, 1, -1, ee->first);
      m_vars.push_back (var);
    }
  }

  void create_one_var (ilp_var_type_t p_vartype, int identifier, int lb, int ub)
  {
    string varname = "";
    varname = this->variable_name (p_vartype, identifier);
    ilp_var * var = new ilp_var (varname, p_vartype, lb, ub, -1, -1);
    m_vars.push_back (var);
  }

  void create_one_var (ilp_var_type_t p_vartype, aquma_qubit * qu, int lb, int ub)
  {
    assert (qu);
    string varname = "";
    varname = this->variable_name (p_vartype, qu->my_id ());
    ilp_var * var = new ilp_var (varname, p_vartype, lb, ub, qu->my_id (), -1);
    m_vars.push_back (var);
  }


  

  string build_basic_constraint_string ()
  {
    string ret = "";
    ilp_var_list_iterator vv;
    for (vv = m_vars.begin (); vv != m_vars.end (); vv++)
    {
      if (ret != "")
        ret += " and ";
      ret += (*vv)->get_constraints_string (this->m_cst);
    }
    return ret;
  }

  string build_system_variable_list_string ()
  {
    string ret = "";
    ilp_var_list_iterator vv;
    for (vv = m_vars.begin (); vv != m_vars.end (); vv++)
    {
      if (ret != "")
        ret += ", ";
      ret += (*vv)->get_name ();
    }
    return ret;
  }

  void print_constraint_string ()
  {
    string ret = this->build_basic_constraint_string ();
    cout << "System Basic Constraints: " << ret << endl;
  }

  void print_constraints_debug ()
  {
    list<string>::iterator ss;
    int ii;
    cout << "Showing constraints [debug]: " << endl;
    for (ss = m_cst.begin (), ii = 0; ss != m_cst.end (); ss++, ii++)
    {
      cout << "Cst " << ii << " : " << *ss << endl; 
    }
  }

  void print_system_variable_list_string ()
  {
    string ret = this->build_system_variable_list_string ();
    cout << "System Variables: " << ret << endl;
  }

  ilp_var ** collect_vars (ilp_var_type_t p_vartype)
  {
    ilp_var ** ret = new ilp_var *[m_vars.size ()+1];
    int ii = 0;
    ilp_var_list_iterator vv;
    if (p_vartype == edge_2gate_bool_e)
    {
      for (vv = m_vars.begin (), ii = 0; vv != m_vars.end (); vv++)
        if ((*vv)->get_type () == edge_bool_e)
      {
        ret[ii++] = (*vv);
      }
      ret[ii] = NULL;
      return ret;
    }
    for (vv = m_vars.begin (), ii = 0; vv != m_vars.end (); vv++)
      if ((*vv)->get_type () == p_vartype)
      {
        ret[ii++] = (*vv);
      }
    ret[ii] = NULL;
    return ret;
  }

  string sum_constraint (ilp_var_type_t p_vartype)
  {
    string ret = "";
    unsigned ii;
    ilp_var ** sel = this->collect_vars (p_vartype);
    for (ii = 0; sel && sel[ii]; ii++)
    {
      if (ret != "")
        ret += " + ";
      ret += sel[ii]->get_name ();
    }
    delete sel;
    return ret;
  }

  string sum_constraint (ilp_var_type_t p_vartype, edge_map & weights, int round_mode)
  {
    string ret = "";
    unsigned ii;
    ilp_var ** sel = this->collect_vars (p_vartype);
    for (ii = 0; sel && sel[ii]; ii++);
    unsigned nterms = ii;
    printf ("Collected %d variables, need %lu\n", nterms, weights.size ());
    for (ii = 0; sel && sel[ii]; ii++)
    {
      cout << "Variable " << ii << " : " << sel[ii]->get_name () << endl;
    }
    assert (nterms == weights.size () && 
      "No. of terms doesn't match No. of weights given."); 
    for (ii = 0; sel && sel[ii]; ii++)
    {
      int edge_id = sel[ii]->get_edge_id ();
      if (ret != "")
        ret += " + ";
      switch (round_mode)
      {
        case GATE2_ERROR:
          ret += std::to_string (round_g2_error (weights[edge_id]));
          break;
        case GATE2_TIME:
          ret += std::to_string (round_g2_time (weights[edge_id]));
          break;
        default:
          ret += std::to_string ((weights[edge_id]));
          assert (0 && "Function designed for use with @edge_map containing floats.");
      }
      ret += " "; 
      ret += sel[ii]->get_name ();
    }
    delete sel;
    return ret;
  }

  void set_bounds (ilp_var_type_t p_vartype, int lb, int ub)
  {
    unsigned ii;
    ilp_var ** sel = this->collect_vars (p_vartype);
    for (ii = 0; sel && sel[ii]; ii++)
    {
      sel[ii]->set_lb (lb);
      sel[ii]->set_ub (ub);
    }
    delete sel;
  }

  void set_upper_bounds (ilp_var_type_t p_vartype, vector<int> & vub)
  {
    unsigned ii;
    ilp_var ** sel = this->collect_vars (p_vartype);
    for (ii = 0; sel && sel[ii]; ii++)
    {
      assert (ii < vub.size () && "ilp.hh: Mismatch in number of entries.");
      sel[ii]->set_ub (vub[ii]);
    }
    delete sel;
  }

  void set_lower_bounds (ilp_var_type_t p_vartype, vector<int> & vlb, bool reverse = false)
  {
    unsigned ii;
    ilp_var ** sel = this->collect_vars (p_vartype);
    if (not reverse)
    {
      for (ii = 0; sel && sel[ii]; ii++)
      {
        assert (ii < vlb.size () && "ilp.hh: Mismatch in number of entries.");
        sel[ii]->set_lb (- vlb[ii]);
      }
    }
    else
    {
      for (ii = 0; sel && sel[ii]; ii++);
      assert (ii == vlb.size () && "ilp.hh: Mismatch in number of entries.");
      int nn = ii;
      for (ii = 0; sel && sel[ii]; ii++)
      {
        cout << "Variable> " + sel[ii]->get_name () << " : " << - vlb[nn - 1 - ii] << endl;
        sel[ii]->set_lb (- vlb[nn - 1 - ii]);
      }
    }
    delete sel;
  }

  ilp_var * add_var_front (string p_name, ilp_var_type_t p_vartype, int p_lb, int p_ub)
  {
    ilp_var * var;
    var = new ilp_var (p_name, p_vartype, p_lb, p_ub);
    m_vars.push_front (var);
    return var;
  }

  ilp_var * add_var_front (string p_name, ilp_var_type_t p_vartype, float p_lb, float p_ub)
  {
    ilp_var * var;
    var = new ilp_var (p_name, p_vartype, p_lb, p_ub);
    m_vars.push_front (var);
    return var;
  }

  ilp_var * add_var_back (string p_name, ilp_var_type_t p_vartype, int p_lb, int p_ub)
  {
    ilp_var * var;
    var = new ilp_var (p_name, p_vartype, p_lb, p_ub);
    m_vars.push_back (var);
    return var;
  }

  ilp_var * add_var_back (string p_name, ilp_var_type_t p_vartype, float p_lb, float p_ub)
  {
    ilp_var * var;
    var = new ilp_var (p_name, p_vartype, p_lb, p_ub);
    m_vars.push_back (var);
    return var;
  }

  void clear_ilp ()
  {
    m_ilp = "";
    m_cst.clear ();
  }

  void add_basic_constraints ()
  {
    if (m_ilp != "")
      m_ilp += " and ";
    m_ilp += build_basic_constraint_string ();
  }

  void add_objective (ilp_var * objv, ilp_var_type_t v_type)
  {
    if (m_ilp != "")
      m_ilp += " and ";

    //string new_cst = objv->get_name () + " = (" + this->sum_constraint (v_type) + ")"; 
    string new_cst;
    new_cst = this->sum_constraint (v_type);
    new_cst += " - ";
    new_cst += objv->get_name ();
    new_cst += objv->get_operator_goal ();
    new_cst += " 0 ";

    m_ilp += new_cst;

    // For cplex
    m_cst.push_back (new_cst);
    cout << "Adding constraint: " << new_cst << endl;
  }

  void add_objective (ilp_var * objv, ilp_var_type_t v_type, int obj_ub)
  {
    if (m_ilp != "")
      m_ilp += " and ";
    string new_cst = "";

    //new_cst += objv->get_name ();
    //new_cst += " = ";
    //new_cst += std::to_string (obj_ub);
    //new_cst += " - (" + this->sum_constraint (v_type) + ")"; 

    new_cst += this->sum_constraint (v_type);

    if (objv->is_non_negative ())
      new_cst += " - ";
    else
      new_cst += " + ";
    new_cst += objv->get_name ();
    new_cst += objv->get_operator_goal ();
    new_cst += std::to_string (obj_ub);

    m_ilp += new_cst;

    // For cplex
    m_cst.push_back (new_cst);
  }

  void add_weighted_objective (
    ilp_var * objv, ilp_var_type_t v_type, edge_map & weights, int round_mode, int obj_ub)
  {
    if (m_ilp != "")
      m_ilp += " and ";
    string new_cst = "";

    //new_cst += objv->get_name ();
    //new_cst += " = ";
    //new_cst += std::to_string (obj_ub);
    //new_cst += " - (" + this->sum_constraint (v_type) + ")"; 

    new_cst += this->sum_constraint (v_type, weights, round_mode);
    if (objv->is_non_negative ())
      new_cst += " - ";
    else
      new_cst += " + ";
    new_cst += objv->get_name ();
    //new_cst += " - ";
    new_cst += objv->get_operator_goal (); // returns >=, <= or =
    new_cst += std::to_string (obj_ub);

    m_ilp += new_cst;

    if (m_options->debug)
    {
      cout << "New weighted objective : " << new_cst << endl;
    }

    // For cplex
    m_cst.push_back (new_cst);
  }

  void add_weighted_objective (
    ilp_var * objv, ilp_var_type_t v_type, edge_map & weights, 
    aquma_qubit_vector & qubits, int round_mode, int obj_ub)
  {
    if (m_ilp != "")
      m_ilp += " and ";
    string new_cst = "";

    //new_cst += objv->get_name ();
    //new_cst += " = ";
    //new_cst += std::to_string (obj_ub);
    //new_cst += " - (" + this->sum_constraint (v_type) + ")"; 

    new_cst += this->sum_constraint (v_type, weights, round_mode);

    // Add penalty
    #if 0 
    new_cst += " + ";
    new_cst += this->get_g2_penalty_objective_string (NULL, qubits, round_mode, 0);
    #endif


    if (objv->is_non_negative ())
      new_cst += " - ";
    else
      new_cst += " + ";
    new_cst += objv->get_name ();
    //new_cst += " - ";
    new_cst += objv->get_operator_goal (); // returns >=, <= or =
    new_cst += std::to_string (obj_ub);
    
    m_ilp += new_cst;

    if (m_options->debug)
    {
      cout << "New weighted objective : " << new_cst << endl;
    }

    // For cplex
    m_cst.push_back (new_cst);
  }



  void add_weighted_objective (
    ilp_var * objv, ilp_var_type_t v_type, edge_map & weights, int round_mode)
  {
    this->add_weighted_objective (objv, v_type, weights, round_mode, 0);
  }

  void add_rscore_objective (aquma_qubit * qubit)
  {
    string cst = "";
    string invalidator = std::to_string (RSCORE_INVAL);
    cst += " -";
    cst += std::to_string (round_rscore (qubit->get_rscore ()));
    cst += " - ";
    cst += invalidator;
    cst += " + ";
    cst += invalidator + this->variable_name (vertex_bool_e, qubit->my_id ());
    cst += " - ";
    cst += this->variable_name (vertex_rscore_e, qubit->my_id ());
    cst += " = ";
    cst += " 0 ";

    if (m_ilp != "")
      m_ilp += " and ";

    m_ilp += cst;

    m_cst.push_back (cst);
  }

  void add_rscore_qubit_bound (ilp_var * objv, aquma_qubit * qubit)
  {
    string cst = "";
    cst += objv->get_name ();
    cst += " - "; // the r-score objv is negative.
    cst += this->variable_name (vertex_rscore_e, qubit->my_id ());
    cst += " >= 0 ";

    if (m_ilp != "")
      m_ilp += " and ";

    m_ilp += cst;

    m_cst.push_back (cst);
  }


  
  /*
  * \Forall vertex v_{k} : v_{k} <= MAX_V . n_{k}
  * This constraint activates the boolean variable of a vertex when its
  * incidence count is greater than zero.
  */
  void add_boolean_to_count_vertex_constraints (int max_V)
  {
    ilp_var ** vbool = this->collect_vars (vertex_bool_e);
    ilp_var ** vcount = this->collect_vars (vertex_count_e);
    int ii;
    for (ii = 0; vbool and vcount and vbool[ii] and vcount[ii]; ii++)
    {
      if (m_ilp != "")
        m_ilp += " and ";

      string new_cst = "";
      //new_cst += vcount[ii]->get_name () + " <= ";
      //new_cst += std::to_string (max_V) + " " + vbool[ii]->get_name ();

      new_cst += std::to_string (max_V) + " " + vbool[ii]->get_name ();
      new_cst += " - ";
      new_cst += vcount[ii]->get_name ();
      new_cst += " >= 0 ";

      m_ilp += new_cst;

      // For cplex
      m_cst.push_back (new_cst);
    }
    delete vbool;
    delete vcount;
  }

  /*
  * \Forall vertex v_{k} : v_{k} <= MAX_V . n_{k}
  * This constraint activates the boolean variable of a vertex when its
  * incidence count is greater than zero.
  */
  void add_boolean_to_count_vertex_constraints (aquma_qubit * qubit)
  {
    if (not qubit->is_2qubit_gate ())
      return;

    string boolvar = this->variable_name (vertex_bool_e, qubit->my_id ());
    string countvar = this->variable_name (vertex_count_e, qubit->my_id ()); 


    string new_cst = "";
    new_cst += std::to_string (qubit->size ());
    new_cst += " ";
    new_cst += boolvar;
    new_cst += " - ";
    new_cst += countvar;
    new_cst += " >= ";
    new_cst += " 0 ";

    if (m_ilp != "")
      m_ilp += " and ";

    m_ilp += new_cst;

    // For cplex
    m_cst.push_back (new_cst);
  }

  /*
  * \Forall edge e_{k} : 
  *   e_{k} <= n_{i}_e{k}
  *   e_{k} <= n_{j}_e{k}
  *   e_{k} <= n_{i}
  *   e_{k} <= n_{j}
  * In English: when an edge e_{k} is chosen for connectivity, we also activate
  * the boolean variables corresponding to the linked qubits (vertices) as well as
  * the boolean variables representing the directed links.
  */
  void add_connectivity_constraints (int q1, int q2, int edge_id)
  {
    if (edge_id < 0)
      return;
    string n1 = this->variable_name (vertex_bool_e, q1);
    string n2 = this->variable_name (vertex_bool_e, q2);
    string edge = this->variable_name (edge_bool_e, q1, q2);
    //string node_edge1 = this->variable_name (edge_bool_e, q1, q2);
    //string node_edge2 = this->variable_name (edge_bool_e, q2, edge_id);
    // if e = <v1,v2>: e <= v1 and e <= v2
    if (m_ilp != "")
      m_ilp += " and ";

    string new_cst1 = n1 + " - " + edge + " >= 0 ";
    string new_cst2 = n2 + " - " + edge + " >= 0 ";
    //string new_cst3 = node_edge1 + " - " + edge + " >= 0 ";
    //string new_cst4 = node_edge2 + " - " + edge + " >= 0 ";

    m_ilp += new_cst1;
    m_ilp += " and ";
    m_ilp += new_cst2;
    //m_ilp += " and ";
    //m_ilp += new_cst3;
    //m_ilp += " and ";
    //m_ilp += new_cst4;

    // For cplex
    m_cst.push_back (new_cst1);
    m_cst.push_back (new_cst2);
    //m_cst.push_back (new_cst3);
    //m_cst.push_back (new_cst4);
  }

  /*
   * v_{k} = \sum n_{k}_e_{i}, s.t  e_{i} >= 0 and n_{k} is in the partition.
   * In English: count the number of edges chosen to link to each vertex.
   */
  void add_incidence_constraint (aquma_qubit* vertex, int * coupling_matrix = NULL)
  {
    if (not vertex->is_2qubit_gate ())
      return;
    string node = this->variable_name (vertex_count_e, vertex->my_id ());
    string node_edge;
    set<int> & adj = vertex->neighbors ();
    string cst = "";
    set<int>::iterator nn;
    for (nn = adj.begin (); nn != adj.end (); nn++)
    {
      int target_qubit = (*nn);
      //int row = vertex->my_id ();
      //int col = (*nn);
      //int edge_id = vertex->my_id () * m_dev_width + (*nn); 
      //int edge_id = coupling_matrix[row * n_qubits + col];
      node_edge = this->variable_name (vertex_edge_bool_e, vertex->my_id (), target_qubit); 
      if (cst != "")
        cst += " + ";
      cst += node_edge; 
    }
    if (cst == "")
      return;

    //cst = node + " = " + cst;
    cst = cst + " - " + node + " >= 0 ";

    m_ilp += " and ";
    m_ilp += cst;

    m_cst.push_back (cst);
  }

  void create_qubit_variables (aquma_qubit_vector & qubits, unsigned target_degree)
  {
    aquma_qubit_vector_iterator qq;
    int expected = 0;
    if (target_degree > 0)
    {
      // Guarantee that we will declare some number of qubits.
      for (qq = qubits.begin (); qq != qubits.end (); qq++)
      {
        aquma_qubit * qu = (aquma_qubit*)(*qq);
        if (qu->is_mapped ())
          continue;
        if (not qu->is_2qubit_gate ())
          continue;
        if (qu->size () >= target_degree)
          expected++;
      }
    }
    if (!expected)
    {
      // If we don't find qubits with sufficient cardinality. Include everything.
      target_degree = 0;
    }
    vector<bool> pending;
    pending.resize (qubits.size (), false);
    set<int> skip;
    for (qq = qubits.begin (); qq != qubits.end (); qq++)
    {
      aquma_qubit * qu = (aquma_qubit*)(*qq);
      if (target_degree > 0 && qu->size () < target_degree)
        continue;

      if (qu->is_mapped ())
        continue;

      pending[qu->my_id ()] = true;
      qu->mark_neighbors_as_pending (pending, skip);
    }
    for (qq = qubits.begin (); qq != qubits.end (); qq++)
      if (pending[(*qq)->my_id ()])
    {
      aquma_qubit * qu = (aquma_qubit*)(*qq);   
      ilp_var * var;
      string varname;

      varname = this->variable_name (vertex_bool_e, qu->my_id ());
      var = new ilp_var (varname, vertex_bool_e, 0, 1, qu->my_id ());
      m_vars.push_back (var);

      varname = this->variable_name (vertex_count_e, qu->my_id ());
      var = new ilp_var (varname, vertex_count_e, 0, qu->size (), qu->my_id ());
      m_vars.push_back (var);
    }
  }

  void create_qubit_variables (aquma_qubit_vector & qubits)
  {
    this->create_qubit_variables (qubits, 0);
  }

  #if 0
  void create_link_variables (edge_map & emap, unsigned dev_width)
  {
    // Create edge variables too.
    edge_map::iterator qq;
    for (qq = emap.begin (); qq != emap.end (); qq++)
    {
      unsigned q1 = qq->first / dev_width;
      unsigned q2 = qq->first % dev_width;
    }
  }
  #endif


  /*
   * Add incidence (aka target degree) constraints for a given value K.
   * Will link all qualifying qubits to constraint.
   * vtd_{k} . k >= \sum v_{i}, where degree (v_{i}) >= K
   * 
   */
  void add_one_vertex_degree_count_constraint (
    aquma_qubit_vector & qubits, unsigned degree_K, int required)
  {
    aquma_qubit_vector_iterator qq;

    string tdcvar = this->variable_name (vertex_target_degree_e, degree_K);
    string cst = "";
    for (qq = qubits.begin (); qq != qubits.end (); qq++)
    {
      if ((*qq)->size () >= degree_K and (*qq)->is_iteration_candidate (degree_K))
      {
        string node = this->variable_name (vertex_count_e, (*qq)->my_id ());
        if (cst != "")
          cst += " + ";
        cst += node;
      }
    }
    // Skip the rest if we didn't find vertices with the requested degree.
    if (cst == "")
      return;
    cst = cst + " + " + std::to_string (degree_K) + " " + tdcvar + " >= 0 ";
    if (m_ilp != "")
      m_ilp += " and ";
    m_ilp += cst;

    // For cplex
    m_cst.push_back (cst);
  }

  /*
   * Add constraints of the type: vtd_{k} . k >= \sum v_{i}, where degree (v_{i}) >= K
   * 
   */
  void add_vertex_degree_count_constraint (
    aquma_qubit_vector & qubits, vector<int> & tdc)
  {
    //aquma_qubit_vector_iterator qq;
    unsigned tt;
    for (tt = 0; tt < tdc.size (); tt++)
    {
      if (tdc[tt] <= 0)
        continue;
      this->add_one_vertex_degree_count_constraint (qubits, tt, tdc[tt]);
      #if 0
      //int FAC_K = tdc[tt];
      string tdcvar = this->variable_name (vertex_target_degree_e, tt);
      string cst = "";
      for (qq = qubits.begin (); qq != qubits.end (); qq++)
      {
        if ((*qq)->size () >= tt)
        {
          string node = this->variable_name (vertex_count_e, (*qq)->my_id ());
          if (cst != "")
            cst += " + ";
          cst += node;
        }
      }
      // Skip the rest if we didn't find vertices with the requested degree.
      if (cst == "")
        continue;
      cst = cst + " + " + std::to_string (tt) + " " + tdcvar + " >= 0 ";
      if (m_ilp != "")
        m_ilp += " and ";
      m_ilp += cst;

      // For cplex
      m_cst.push_back (cst);
      #endif
    }
  }

  string get_g2_penalty_objective_string (
      ilp_var * objv, aquma_qubit_vector & qubits, 
      int round_mode, int obj_ub)
  {
    string cst = "";
    aquma_qubit_vector_iterator qq;
    for (qq = qubits.begin (); qq != qubits.end (); qq++)
    {
      aquma_qubit * qu = (aquma_qubit*)(*qq);

      if (not qu->is_2qubit_gate ())
        continue;

      if (cst != "")
        cst += " + ";

      int rounded_weight = round_g2_error (qu->get_g2_average_error ());
      if (round_mode == GATE2_TIME)
        rounded_weight = round_g2_time (qu->get_g2_average_time ());

      string boolvar = this->variable_name (vertex_bool_e, qu->my_id ());
      string countvar = this->variable_name (vertex_count_e, qu->my_id ()); 

      cst += std::to_string (2 * rounded_weight * qu->size ()); 
      cst += " ";
      cst += boolvar;
      cst += " - ";
      cst += std::to_string (2 * rounded_weight); 
      cst += " ";
      cst += countvar;
    }

    return cst;
  }

  void add_g2_penalty_objective (
      ilp_var * objv, aquma_qubit_vector & qubits, 
      int round_mode, int obj_ub)
  {
    string cst = "";
    cst += get_g2_penalty_objective_string (objv, qubits, round_mode, obj_ub);
    
    cst += " - ";
    cst += objv->get_name ();
    cst += " = ";
    cst += std::to_string (obj_ub);

    if (m_ilp != "")
      m_ilp += " and ";
    m_ilp += cst;

    // For cplex
    m_cst.push_back (cst);
  }

  string assemble_complete_ilp ()
  {
    string ret = "";
    ret += " {";
    ret += " [";
    ret += this->build_system_variable_list_string ();
    ret += " ]";
    ret += " : ";
    ret += m_ilp;
    ret += " }";
    return ret;
  }

  void print_ilp ()
  {
    cout << "ILP: " << this->assemble_complete_ilp () << endl;
  }

  double get_weight_by_vartype (ilp_var * var)
  {
    if (var->is_boolean ())
      return 1.0;
    if (var->get_type () == vertex_edge_count_e)
      return 2;
    if (var->get_type () == vertex_target_degree_e)
      return 3;
    if (var->get_type () == vertex_count_e)
      return 4;
    if (var->get_type () == edge_count_e)
      return 5;
    return 100;
  }

  int count_vertex_degree_variables ()
  {
    ilp_var_list_iterator vv;
    unsigned count = 0;
    for (vv = m_vars.begin (); vv != m_vars.end (); vv++)
    {
      ilp_var * var = *vv;
      count += (var->get_type () == vertex_target_degree_e ? 1 : 0);
    }
    return count;
  }

  void print_ilp_to_cplex_script ()
  {
    list<string>::iterator cc;
    ofstream script;
    assert (m_options && "aquma_options structure not set. Aborting ...\n");
    printf ("Cplex file being used: %s.lp\n", m_options->basename);
    string cplex_file(m_options->basename);
    cplex_file += ".lp";
    script.open (cplex_file, std::fstream::out);
    long long unsigned cid = 1;
    ilp_var_list_iterator vv;
    script << "Minimize " << endl;
    script << "obj1 : ";
    double weight;
    //unsigned n_var = m_vars.size ();
    //unsigned n_obj = 3 + this->count_vertex_degree_variables ();
    unsigned var_pos;
    //unsigned n_modvar = n_var - n_obj;
    bool first = true;

    weight = (1 << 24);
    for (vv = m_vars.begin (), var_pos = 0; vv != m_vars.end (); vv++, var_pos++)
      if ((*vv)->is_objective ())
    {
      ilp_var * var = *vv;
      if (not first)
      {
        if ((*vv)->is_non_negative ())
          script << " - ";
        else
          script << " + ";
      }
      else
      {
        if ((*vv)->is_non_positive ())
          script << " + ";
      }
      first = false;
      /*
      if (var_pos >= n_obj)
        weight = 1.0;
      else if (var_pos >= 3)
        weight = (n_modvar * 2);
      else
        weight = n_modvar + (n_obj - 3) * 2 * n_modvar + n_var * (1 << (4 - var_pos));
        */
      weight = weight / 2.0;
      if (not first)
      {
        float scale;
        bool is_int = (*vv)->is_int ();
        bool is_non_neg = (*vv)->is_non_negative ();
        if (is_int and is_non_neg)
          scale = (*vv)->get_ub_as_int ();
        if (is_int and not is_non_neg)
          scale = - (*vv)->get_lb_as_int ();
        if (not is_int and is_non_neg)
          scale = (*vv)->get_ub_as_float ();
        if (not is_int and not is_non_neg)
          scale = - (*vv)->get_lb_as_float ();
        weight = weight / scale;
      }

      script << weight << var->get_name ();
    }
    script << endl;
    script << "Subject To " << endl;
    for (cc = m_cst.begin (); cc != m_cst.end (); cc++, cid++)
    {
      script << string("c") << cid << string(": ") << *cc << endl;
    }
    // Print binary variables
    string bool_list = "";
    string non_bool_list = "";
    script << "Bound " << endl;
    for (vv = m_vars.begin (); vv != m_vars.end (); vv++)
    {
      ilp_var * var = *vv;
      if (var->is_boolean ())
        bool_list += var->get_name () + " ";
      else
      {
        script << var->get_lb_as_string ();
        script << " <= ";
        script << var->get_name ();
        script << " <= ";
        script << var->get_ub_as_string ();
        script << endl;
        non_bool_list += var->get_name () + " ";
      }
    }
    script << "Binary " << endl;
    script << bool_list << endl;
    script << "General " << endl;
    script << non_bool_list << endl;
    script << "End" << endl;
    // Print non-binary variables
    script.close ();

    cout << "\n[INFO] Saved ILP with " << m_vars.size () << " variables ";
    cout << " and " << m_cst.size () << " non-bounding constraints." << endl;
  }

  void solve_with_cplex ()
  {
    string driver_name(m_options->basename);
    string scriptname(m_options->basename);
    string solfile(m_options->basename);
    string filtered_sol (m_options->basename);

    driver_name += ".cplex";
    scriptname += ".lp";
    solfile += ".xml";
    filtered_sol += ".sol";

    ofstream sout;
    sout.open (driver_name, std::fstream::out);
     
    sout << "read " + scriptname << endl;
    sout << "change problem milp" << endl;
    sout << "set mip tolerance mipgap 0.0000000000000001" << endl;
    sout << "mipopt" << endl;
    sout << "write " << solfile << " sol" << endl;

    sout.close ();

    string strcmd = "";
    strcmd += "cplex -f ";
    strcmd += driver_name;

    int retval;
    retval = system (strcmd.c_str ());
    assert (retval != -1 && retval != 127);

    strcmd = "";
    strcmd += "cat " + solfile;
    strcmd += " |  grep 'variable name' | ";
    strcmd += " sed -e 's/^.*value=\"//g' -e 's/\".*$//g' ";
    strcmd += " > " + filtered_sol;

    retval = system (strcmd.c_str ());
    assert (retval != -1 && retval != 127);

    FILE * ff;
    ff = fopen (filtered_sol.c_str (), "r");

    ilp_var_list_iterator vv;
    //for (vv = m_vars.begin (); vv != m_vars.end ();  vv++)
    unsigned read_count = 0;
    vv = m_vars.begin ();
    while (1)
    {
      if (vv == m_vars.end ())
        break;
      if (read_count == m_vars.size ())
        break;
      int opt_val_int;
      float opt_val_float;
      if ((*vv)->is_int ())
        retval = fscanf (ff, "%d", &opt_val_int);
      else if ((*vv)->is_float ())
        retval = fscanf (ff, "%f", &opt_val_float);
      else
        assert (0 && "Unknown datatype for ILP variable.");
      if (!retval)
        break;
      read_count++;
      if ((*vv)->is_int ())
        (*vv)->set_optimum (opt_val_int);
      else if ((*vv)->is_float ())
        (*vv)->set_optimum (opt_val_float);
      vv++;
    }
    printf ("\n[INFO] Found %u variables in Cplex solution - Expected %lu\n", read_count, m_vars.size ());

    fclose (ff);

    this->print_vars ();
  }

  void print_vars ()
  {
    int dd = 0;
    cout << endl << "--------------------------------------" << endl;
    cout << "ILP Variables : " << endl;
    ilp_var_list_iterator vv;
    for ( vv = m_vars.begin ();
          vv != m_vars.end (); 
          vv++, dd++)
    {
      cout << "[" << dd << "] : ";
      (*vv)->print_info ();
    }
  }

  /*
   * Copy the isl result back to the variables.
   *
   */
  void isl_to_sys (isl_set * sol)
  {
    int ndim = isl_set_dim (sol, isl_dim_set);
    int dd;
    ilp_var_list_iterator vv;
    for ( vv = m_vars.begin (), dd = 0; 
          dd < ndim and vv != m_vars.end (); 
          vv++, dd++)
    {
      isl_val * val;
      val = isl_set_plain_get_val_if_fixed (sol, isl_dim_set, dd);
      int num = isl_val_get_num_si (val);
      int den = isl_val_get_den_si (val);
      if (den > 1)
      {
        cout << "WARNING: Rational solution found [" << num << "/" << den << "]" << endl;
        assert (den == 1);
      }
      isl_val_free (val);
      (*vv)->set_optimum (num);
    }
  }

  void solve_ilp ()
  {
    isl_ctx * ctx;
    ctx = isl_ctx_alloc ();
    isl_set * sys;
    isl_set * sol;
    string ilp_str = this->assemble_complete_ilp ();
    sys = isl_set_read_from_str (ctx, ilp_str.c_str ());
    sol = isl_set_lexmin (sys);
    this->isl_to_sys (sol);
    this->print_vars ();
    fflush (stdout);
    printf ("Solution to ILP: \n");
    fflush (stdout);
    char * solstr = isl_set_to_str (sol);
    printf ("%s\n", solstr);
    free (solstr);
    fflush (stdout);
    isl_set_free (sol);
    isl_ctx_free (ctx);
  }

  /*
  * Translate the allocated qubits in the new layout to a contiguous range
  * of qubit indices. The assignment of qubits will continue from @pre_assigned_q
  * argument. The function returns the number of newly assigned qubits.
  */
  int update_global_layout (vector<int> & global_layout, 
    unsigned pre_assigned_q, set<int> & global_xset)
  {
    ilp_var ** sol = this->collect_vars (vertex_bool_e);
    int ii;
    unsigned next_q = pre_assigned_q;
    cout << endl << "Showing variables updated for global layout \n" << endl;
    for (ii = 0; sol && sol[ii]; ii++)
    {
      ilp_var * var = sol[ii];
      if (var->is_int () and var->get_optimum_as_int () == 1)
      {
        if (global_xset.find (var->get_qubit_id ()) != global_xset.end ())
          continue;
        cout << "Layout var " << var->get_name () << " : " << next_q << endl;
        global_layout[var->get_qubit_id ()] = next_q;
        global_xset.insert (var->get_qubit_id ());
        next_q++;
      }
    }
    printf ("[INFO] @update_global_layout: selected %u qubits\n", next_q);
    delete sol;
    return (next_q - pre_assigned_q);
  }

  int find_selected_qubit ()
  {
    ilp_var ** sol = this->collect_vars (vertex_bool_e);
    int ret = -1;
    int safe_count = 0;
    int ii;
    for (ii = 0; sol && sol[ii]; ii++)
    {
      ilp_var * var = sol[ii];
      if (var->is_int () and var->get_optimum_as_int () == 1)
      {
        ret = var->get_qubit_id ();
        safe_count ++;
      }
    }
    assert (safe_count == 1);
    delete sol;
    return ret;
  }
};



#endif
