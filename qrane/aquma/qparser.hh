#ifndef _AQUMA_QASM_PARSER_H_

#define _AQUMA_QASM_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

enum qasm_op
{
  op_error = -1,
  op_cx, // 2
  op_h, // 1
  op_rzz, // 3: rzz(angle) q[x],q[y]
  op_t, // 1 
  op_s, // 1 
  op_tdg, // 2
  op_rz, // 2: rz(angle) q[x]
  op_rx, // 2: same as rz
  op_x, // 1
  op_z, // 1
};

static const char * qasm_op_str[] = {
  "cx",
  "h",
  "rzz",
  "t",
  "s",
  "tdg",
  "rz",
  "rx",
  "x",
  "z",
  NULL
};


typedef struct 
{
  qasm_op opcode;
  float arg1;
  int arg2;
  int arg3;
} qasm_t;

class aquma_qparser 
{
  vector<string>  m_prologue; // declarations
  ifstream m_istream;   // input file
  ofstream m_ostream;   // output file
  vector<qasm_t*>  m_ops;  // list of quantum ops
  string m_regname;
  int m_device_width;  // number of qubits in quantum device
  int m_2q_ops; // number of 2Q-ops

  public:

  aquma_qparser (const char * filename, int dev_size)
  {
    m_istream.open (filename);
    string outname (filename);
    m_regname = "";
    int pos = outname.find (".qasm");
    outname.replace (pos, 5, ".fqm.qasm");
    m_ostream.open (outname);
    cout << "Output file will be: " << outname << endl;
    m_device_width = dev_size;
    m_2q_ops = 0;
  }

  ~aquma_qparser ()
  {
    m_istream.close ();
    m_ostream.close ();    
    // TODO: free m_ops
  }

  qasm_op get_opcode (const char * op)
  {
    int ii;
    char temp[20];
    for (ii = 0; isalpha(op[ii]); ii++)
      temp[ii] = op[ii];
    temp[ii] = '\0';
    for (ii = 0; qasm_op_str[ii]; ii++)
      if (!strcmp (qasm_op_str[ii], temp))
        return (qasm_op)(ii);
    return op_error;
  }

  float get_angle (const char * op)
  {
    int ii;
    for (ii = 0; op[ii] != '('; ii++);
    char temp[20];
    ii++;
    int kk;
    for (kk=0 ; op[ii] != ')'; ii++,kk++)
      temp[kk] = op[ii];
    temp[kk] = '\0';
    float ret;
    sscanf (temp,"%f", &ret);
    return ret;
  }

  bool is_2Q_gate (qasm_op theop)
  {
    if (theop == op_cx)
      return true;
    if (theop == op_rzz)
      return true;
    return false;
  }

  int parse ()
  {
    int ret;
    assert (m_istream.is_open ());
    int ii;
    for (ii = 0; ii < 3; ii++)
    {
      string line;
      getline (m_istream, line);
      m_prologue.push_back (line);
    }
    string op_str;
    ii = 1;
    while (m_istream >> op_str)
    {
      ret = this->parse_line (op_str);
      if (ret)
      {
        cout << "Error reading Q_OP #" << ii << " -- [" << op_str << "]" << endl;
        return 1;
      }
      ii++;
    }
    return 0;
  }

  void parse_qubit (const char * qstr, char * regname, int & index1, int &index2)
  {
    char * ptr = (char*)(qstr);
    //printf ("Received string: --%s--\n", ptr);
    //fflush (stdout);
    int ii;
    while (!isalnum (ptr[ii])) ii++;
    ii = 0;
    //printf ("Remaining string (1): --%s--\n", &ptr[ii]); fflush (stdout);
    int jj = 0;
    while (isalnum (ptr[ii]))
    {
      regname[jj++] = ptr[ii++];
    }
    regname[jj] = '\0';
    //printf ("Remaining string (2): --%s--\n", &ptr[ii]); fflush (stdout);
    while (isspace (ptr[ii]) || ptr[ii] == '[') ii++;
    //printf ("Remaining string (3): --%s--\n", &ptr[ii]); fflush (stdout);
    sscanf (&ptr[ii],"%d", &index1);
    while (ptr[ii] != ',' && ptr[ii] != ';') ii++;
    printf ("Current position: %s\n", &ptr[ii]);
    if (ptr[ii] == ',')
    {
      while (!isdigit(ptr[ii]) && ptr[ii] != ';') ii++;
      printf ("Current position 2: %s\n", &ptr[ii]);
      sscanf (&ptr[ii],"%d", &index2);
    }
  }


  int parse_line (string op_str)
  {
    int ret = 0;
    qasm_t * op;
    op = new qasm_t;
    op->opcode = op_x;
    op->arg1 = 0.0;
    op->arg2 = -1;
    op->arg3 = -1;
    op->opcode = get_opcode (op_str.c_str ());
    string rest;
    std::getline (m_istream,rest);
    const char * reststr = rest.c_str ();
    int len = strlen(reststr);
    char temp[256];
    strncpy (temp, (char*)(reststr), len+1);
    if (strstr (temp,"qureg"))
    {
      m_regname = "qureg";
    }
    int ii;
    for (ii = 0; ii < len; ii++)
    {
      if (temp[ii] == '(')
        temp[ii] = ' ';
      if (temp[ii] == ')')
        temp[ii] = ' ';
      if (temp[ii] == ',')
        temp[ii] = ' ';
      if (temp[ii] == ';')
        temp[ii] = ' ';
      if (temp[ii] == 'q')
      {
        temp[ii] = ' ';
        m_regname = "q";
      }
      if (temp[ii] == '[')
        temp[ii] = ' ';
      if (temp[ii] == ']')
        temp[ii] = ' ';
    }
    #ifdef DEBUG_Q_PARSER
    cout << "Op found is : " << op->opcode << " - " << op_str << endl; fflush (stdout);
    #endif
    if (op->opcode < 0)
      return 1;
    char * ptr = temp;
    #ifdef DEBUG_Q_PARSER
    cout << "Rest of line is: [" << rest << "]" << endl;
    printf ("simplified rest: [%s]\n", ptr);
    #endif
    if (op->opcode == op_rzz || op->opcode == op_rz || op->opcode == op_rx)
    {
      //m_istream >> op->arg1;
      //sscanf (ptr,"%f", &op->arg1);
      op->arg1 = this->get_angle (op_str.c_str ());
      cout << "Argument 1 read: " << op->arg1 << endl; fflush (stdout);
      //while (*ptr != '.') ptr++;
      //while (*ptr != '.') ptr++;
      //printf ("remaining string after advancing 1: [%s]\n", ptr);
      //while (! isspace (*ptr)) ptr++;
      //printf ("remaining string after advancing 2: [%s]\n", ptr);
    }
    if (this->is_2Q_gate (op->opcode))
    {
      #ifdef DEBUG_Q_PARSER
      printf ("remaining string after advancing 3: [%s]\n", ptr);
      #endif
      int q1, q2;
      sscanf (ptr,"%d %d", &q1, &q2);
      op->arg2 = q1;
      op->arg3 = q2;

      m_2q_ops++; // Count number of 2Q-operations
    }
    else
    {
      int q1;
      sscanf (ptr,"%d", &q1);
      op->arg2 = q1;
    }
    m_ops.push_back (op);
    return ret;
  }

  int parse_line_old (string op_str)
  {
    int ret = 0;
    qasm_t op = {op_x,0.0,-1,-1};
    op.opcode = get_opcode (op_str.c_str ());
    cout << "Op found is : " << op.opcode << " - " << op_str << endl; fflush (stdout);
    if (op.opcode < 0)
      return 1;
    if (op.opcode == op_rzz || op.opcode == op_rz)
    {
      //m_istream >> op.arg1;
      op.arg1 = this->get_angle (op_str.c_str ());
      cout << "Argument 1 read: " << op.arg1 << endl; fflush (stdout);
    }

    string qubit_str;

    // Read the first qubit
    m_istream >> qubit_str;
    //cout << "Argument 2 read: " << qubit_str << endl;
    const char * raw_str = qubit_str.c_str ();
    char regname[64];
    this->parse_qubit (raw_str, regname, op.arg2, op.arg3);
    cout << "Regname found : " << regname << endl;;
    cout << "Qubit index 1 found : " << op.arg2 << endl;
    cout << "Qubit index 2 found : " << op.arg3 << endl;
    if (m_regname == "")
    {
      m_regname = string (regname);
    }

    if (this->is_2Q_gate (op.opcode) and op.arg3 < 0)
    {
      do {
        m_istream >> qubit_str;
      } while (qubit_str.length () < 2);
      //cout << "Argument 3 read: " << qubit_str << endl;
      const char * raw_str = qubit_str.c_str ();
      char regname[64];
      int discard;
      this->parse_qubit (raw_str, regname, op.arg3, discard);
    }
     
    m_ops.push_back (&op);
    return ret; 
  }

  void print_qubit_arg (int & index)
  {
    m_ostream << m_regname << "[" << index << "]"; 
  }

  void pretty_print_op (qasm_t * op)
  {
    m_ostream << qasm_op_str[op->opcode];
    if (op->opcode == op_rzz || op->opcode == op_rz || op->opcode == op_rx)
    {
      m_ostream << "(" << op->arg1 << ")";
    }
    m_ostream << " ";
    this->print_qubit_arg (op->arg2);
    if (this->is_2Q_gate (op->opcode))
    {
      m_ostream << ", ";
      this->print_qubit_arg (op->arg3);
    }
    m_ostream << ";" << endl; 
  }

  void pretty_print ()
  {
    unsigned ii;
    for (ii = 0; ii < 2; ii++)
    {
      m_ostream << m_prologue[ii] << endl;
    }
    if (m_device_width > 0)
    {
      m_ostream << "qreg " << m_regname << "[" << m_device_width << "];" << endl;
    }
    else
    {
      m_ostream << m_prologue[2] << endl;
    }
    for (ii = 0; ii < this->m_ops.size (); ii++)
    {
      this->pretty_print_op ((this->m_ops[ii]));
    }
  }

  void update_layout (vector<int> & layout, int device_width)
  {
    vector<int>::iterator ii;
    unsigned kk = 0;
    #if 0
    for (kk = 0, ii = layout.begin (); ii != layout.end (); ii++, kk++)
    {
      printf ("In qparser -- qubit %d --> %d\n", kk, layout[kk]);
    }
    #endif
    vector<qasm_t*>::iterator qq;
    //m_device_width = device_width;
    for (kk = 0; kk < this->m_ops.size (); kk++)
    {
      int old_qubit = this->m_ops[kk]->arg2;
      this->m_ops[kk]->arg2 = layout[old_qubit];
      if (this->is_2Q_gate (this->m_ops[kk]->opcode))
      {
        old_qubit = this->m_ops[kk]->arg3;
        this->m_ops[kk]->arg3 = layout[old_qubit];
      }
    }
  }

  inline
  int convert_qpair_to_qedge (int src, int dst)
  {
    return  src * m_device_width + dst;
  }


  void build_qubit_string (vector<int> & qstr)
  {
    cout << "Number of 2Q-gates found : " << m_2q_ops << endl;
    qstr.resize (m_2q_ops);
    unsigned ii;
    unsigned pp;
    for (ii = 0, pp = 0; ii < m_ops.size (); ii++)
      if (this->is_2Q_gate (this->m_ops[ii]->opcode))
    {
      int q1 = this->m_ops[ii]->arg2;
      int q2 = this->m_ops[ii]->arg3;
      qstr[pp] = this->convert_qpair_to_qedge (q1, q2);
      pp++;
    }
  }
};

#endif
