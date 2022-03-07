#ifndef _AQUMA_UTILS_HH_

#define _AQUMA_UTILS_HH_

#include <set>
#include <assert.h>
#include "isl/set.h"
#include "isl/map.h"
#include "isl/polynomial.h"
#include "barvinok/isl.h"

using namespace std;

class aquma_utils {
  public:

  static isl_set * intersect_sets (__isl_keep isl_set * s1, __isl_keep isl_set * s2)
  {
    isl_set * ret;
    ret = isl_set_intersect (
      isl_set_copy (s1),
      isl_set_copy (s2));
    return ret;
  }
  

  static isl_pw_qpolynomial * get_expression_from_set (__isl_take isl_set * aset)
  {
    // NOTE: isl_set_card "takes" the set given as argument.
    isl_pw_qpolynomial * ret = isl_set_card (aset);
    //isl_pw_qpolynomial_dump (ret);
    return ret;
  }

  /*
    Destructive get_set_size()
  */
  static long get_set_size (__isl_take isl_set * aset)
  {
    isl_pw_qpolynomial * pwqp = get_expression_from_set (aset);
    isl_qpolynomial * qp = NULL;
    long ret = -1;
    if (isl_pw_qpolynomial_isa_qpolynomial (pwqp) == isl_bool_true)
    {
      qp = isl_pw_qpolynomial_as_qpolynomial (pwqp);
      isl_val * val = isl_qpolynomial_get_constant_val (qp);
      ret = isl_val_get_num_si (val);
      long denom = isl_val_get_den_si (val);
      assert (denom == 1);
      isl_qpolynomial_free (qp);
      isl_val_free (val);
    }
    return ret;
  }

  static long get_set_size_nd (__isl_keep isl_set * aset)
  {
    isl_set * temp = isl_set_copy (aset);
    return get_set_size (temp);
  }

  static isl_stat add_index_to_set (__isl_take isl_point * pnt, void * usr)
  {
    isl_val * val = isl_point_get_coordinate_val (pnt, isl_dim_set, 0); 
    set<unsigned> * data = (set<unsigned>*)(usr);
    int si = (unsigned)isl_val_get_num_si (val);
    isl_point_free (pnt);
    isl_val_free (val);
    data->insert (si);
    return isl_stat_ok;
  }

  static set<unsigned> * isl_set_to_unsigned_set (__isl_keep isl_set * aset)
  {
    isl_stat res;
    set<unsigned> * qbset = new set<unsigned>;
    isl_set * temp_set = isl_set_coalesce (isl_set_copy (aset));
    temp_set = isl_set_detect_equalities (temp_set);
    res = isl_set_foreach_point (temp_set, & add_index_to_set, (void*)(qbset));
    isl_set_free (temp_set);
    assert (res == isl_stat_ok);
    return qbset;
  }


  static __isl_give isl_set * add_to_set (__isl_take isl_set * left, __isl_keep isl_set * right)
  {
    /*
    fprintf (stderr,"here: \n");
    isl_set_dump (left);
    isl_set_dump (right);
    fflush (stdout);
    */
    left = isl_set_union (left, isl_set_copy (right));
    return left;
  }

  static unsigned count_common_items (set<int> & left, set<int> & right)
  {
    set<int>::iterator aa;
    set<int>::iterator bb;
    unsigned ret = 0;
    for (aa = left.begin (); aa != left.end (); aa++)
      for (bb = right.begin (); bb != right.end (); bb++)
        if (*bb == *aa)
    {
      ret++;
      break;
    }
    return ret;
  }

};



#endif
