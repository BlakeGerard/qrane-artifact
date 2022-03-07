#ifndef QRANE_UTILS_H
#define QRANE_UTILS_H

#include <string>
#include <iostream>
#include <stack>
#include "isl/mat.h"
#include "isl/val.h"
#include "qrane_statementlist.hh"
#include "qrane_shared.hh"

#if defined USE_AQUMA
#include "aquma.hh"
#endif

namespace qrane_utils {

	// qrane_statement splicing 
	qrane_statementlist splice_stmtlist(qrane_statementlist stmts, 
		std::vector<unsigned int>& locs, unsigned int len);
	std::vector<qrane_statementlist> split_into_n_components(qrane_statementlist stmts, unsigned int chunks);
	
	// For generating compileable C file to produce OpenQASM
	int generate_c_test_file(std::string c_name, std::string qasm_name, std::string codegen_c_str, std::string qreg_decls);

	// legacy stride_graph still in use
	// Need to convert to qrane_graph
	void adj_list_remove_vertex(stride_graph& G, qop_id v);
	std::vector<qop_id> adj_list_topological_ordering(stride_graph G);
	void adj_list_topological_ordering_util(qop_id start, stride_graph& G, std::unordered_map<qop_id, bool>& visited, std::stack<qop_id>& ordering);
	
	// Some misc helpers
	bool queue_size_predicate(const std::deque<unsigned int>& lhs, const std::deque<unsigned int>& rhs);
	bool is_integer(const float m);

	// Matrix methods
	std::vector<int> vectorize_isl_mat(__isl_keep isl_mat* mat);
	isl_mat* back_substitute_isl_mat(isl_ctx* ctx, __isl_keep isl_mat* mat, int args, int dim);
	__isl_give isl_mat* zero_matrix(isl_ctx* ctx, isl_size rows, isl_size cols);
	std::vector<int> find_zero_cols(__isl_keep isl_mat* mat, int col_lb, int col_ub);
	__isl_give isl_mat* drop_zero_cols(__isl_take isl_mat* mat, std::vector<int> cols_to_drop);
	void print_isl_mat(isl_mat* mat);
};

#endif
