#include "include/qrane_deps.hh"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <functional>
#include "qrane_utils.hh"

	// -------------------------------
	// Constructors and Initialization
	// -------------------------------

qrane_deps::qrane_deps() {
};


qrane_deps::qrane_deps(const qrane_options* opt, const qrane_qop_map& qop_map) {
	latest_access = access_map();
	latest_access_write_all = access_map();
	profile_map = qubit_profile_map();
    membership = membership_map();
	construct_dependence_graph(opt, qop_map);
};

qrane_deps::qrane_deps(qrane_deps const& rhs) {
	G_fixed = rhs.G_fixed;
	G_write_all = rhs.G_write_all;
    latest_access = rhs.latest_access;
	latest_access_write_all = rhs.latest_access_write_all;
	profile_map = rhs.profile_map;
    membership = rhs.membership;
};

qrane_deps& qrane_deps::operator=(qrane_deps const& rhs) {
	G_fixed = rhs.G_fixed;
	G_write_all = rhs.G_write_all;
    latest_access = rhs.latest_access;
	latest_access_write_all = rhs.latest_access_write_all;
	profile_map = rhs.profile_map;
    membership = rhs.membership;
    return *this;
};

membership_map qrane_deps::get_membership() {
	return this->membership;
}

	// ------------------------------------------------
	// Dependence Graph and Membership Map Construction
	// ------------------------------------------------

void qrane_deps::construct_dependence_graph(const qrane_options* opt, const qrane_qop_map& qop_map) {
	auto V = std::unordered_set<qop_id >(qop_map.size());
	for (const auto& qop : qop_map) {
		V.insert(qop.first);
		membership[qop.first] = std::make_pair(qop.first, std::vector<unsigned int>());
	}

	// Initialize the qrane_graph objects
	this->G_fixed = qrane_graph<qop_id>(V, true);
	this->G_write_all = qrane_graph<qop_id>(V, true);

	// Add vertices to the graph based on each qop argument
	for (const auto& entry : qop_map) {
		char type;
		for (int j = 0; j < entry.second->num_args(); ++j) {
			if (opt->write_all) { type = 'w';
			} else {
				if (j < entry.second->num_args() - 1) { type = 'r';
        		} else { type = 'w'; }
			}
        	add_vertex(entry.second->get_id(), entry.second->arg(j)->get_index(), 
				type, entry.second->get_operation());
			add_write_all_vertex(entry.second->get_id(), entry.second->arg(j)->get_index(), 
				'w', entry.second->get_operation());
    	}
	}
};

/*
	Add a single vertex to the dependence graph.
	Update the latest_access to reflect presence of new qop.
	Update the qubit access profile to reflect presence of new qop.
*/
void qrane_deps::add_vertex(qop_id  line_num, unsigned int index_accessed, char type, std::string gate_id) {
	auto current_access = std::pair<qop_id , char>(line_num, type);
	std::pair<qop_id , char> queue_member;

	// If the index is already being tracked
	if (latest_access.count(index_accessed)) {

		// If the queue is not empty
		if (latest_access[index_accessed].size() > 0) {

			// If the current access is a read
			if (current_access.second == 'r') {

				// and the latest access is a write (WAR)
				if (latest_access[index_accessed].front().second == 'w') {

					// Generate a dependence only. The access gets pushed onto the queue
					queue_member = latest_access[index_accessed].front();
					if (queue_member.first == current_access.first) { return; }
					this->G_fixed.add_edge(queue_member.first, current_access.first);

				// and the latest access is a read (RAR)
				} else if (latest_access[index_accessed].front().second == 'r') {
					// Don't generate a dependence, just push the acess to the queue
				}

			// If the current access is a write (RAW or WAW)
			} else if (current_access.second == 'w') {

				// Generate dependence for each item and remove it from the queue
				while (!latest_access[index_accessed].empty()) {
    				queue_member = latest_access[index_accessed].front();
					latest_access[index_accessed].pop();
					if (queue_member.first == current_access.first) { continue; }
					this->G_fixed.add_edge(queue_member.first, current_access.first);
  				}
			}
		}
	}
	(profile_map[index_accessed])[gate_id] += 1;
	latest_access[index_accessed].push(current_access);
};

void qrane_deps::add_write_all_vertex(qop_id  line_num, int index_accessed, char type, std::string gate_id) {
	auto current_access = std::pair<qop_id, char>(line_num, type);
	std::pair<qop_id, char> queue_member;

	// If the index is already being tracked
	if (latest_access_write_all.count(index_accessed)) {

		// If the queue is not empty
		if (latest_access_write_all[index_accessed].size() > 0) {

			// If the current access is a read
			if (current_access.second == 'r') {
				assert(false && "Added a read dependence to ddg_write_all");

				/*
				// and the latest access is a write (WAR)
				if (latest_access_write_all[index_accessed].front().second == 'w') {

					// Generate a dependence only. The access gets pushed onto the queue
					queue_member = latest_access_write_all[index_accessed].front();
					if (queue_member.first == current_access.first) { return; }
					ddg[queue_member.first].push_back(current_access.first);

				// and the latest access is a read (RAR)
				} else if (latest_access[index_accessed].front().second == 'r') {
					// Don't generate a dependence, just push the acess to the queue
				}
				*/

			// If the current access is a write (RAW or WAW)
			} else if (current_access.second == 'w') {

				// Generate dependence for each item and remove it from the queue
				while (!latest_access_write_all[index_accessed].empty()) {
    				queue_member = latest_access_write_all[index_accessed].front();
					latest_access_write_all[index_accessed].pop();
					if (queue_member.first == current_access.first) { continue; }
					this->G_write_all.add_edge(queue_member.first, current_access.first);
  				}
			}
		}
	}
	latest_access_write_all[index_accessed].push(current_access);
};

/*
	TODO: Document
*/
void qrane_deps::set_membership(qop_id  line_num, unsigned int index, unsigned int domain_num) {
    auto rhs = std::pair<qop_id , std::vector<unsigned int>>(domain_num, std::vector<unsigned int>{index});
    membership[line_num] = rhs;
};

/*
	This is only called AFTER the domain has been chosen
*/
void qrane_deps::update_membership_graph_1D_phase(const std::vector<qop_id >& path, unsigned int domain_num) {
	unsigned int index = 0;
	for (const auto point : path) {
		this->membership[point].first = domain_num;
		this->membership[point].second.push_back(index);
		++index;
	}
}

/*
	This takes in domain_num identifiers, NOT inidividual points
*/
void qrane_deps::update_membership_graph_ND_phase(const std::vector<unsigned int>& domains, domain_map_t& domain_map) {
	unsigned int parent_num = domains.front();
	std::vector<qop_id> components = domain_map[parent_num].component_qops;
	
	// Update the points of the parent domain with a zero value for outermost loop variable
	for (auto val : components) {
		membership[val].second.push_back(0);
	}

	// Do the same as above for each subsequent domain, but use mergee_counter instead of zero
	unsigned int mergee_counter = 1;
	for (std::size_t i = 1; i < domains.size(); ++i) {
        components = domain_map[domains[i]].component_qops;

    	for (auto val : components) {
			membership[val].first = parent_num;
         	membership[val].second.push_back(mergee_counter);
		}
        mergee_counter += 1;
    }
};

isl_union_map* qrane_deps::build_implicit_schedule(isl_ctx* ctx) {
	isl_union_map* schedule = isl_union_map_empty_ctx(ctx);

	std::ostringstream strm;
	unsigned int node_count = 0;
	for (const auto& node : this->G_fixed) {
		auto node_info = membership[node.first];

        strm << "{S" << std::to_string(node_info.first) << "[";
        for (std::vector<unsigned int>::reverse_iterator dim = node_info.second.rbegin(); dim != (node_info.second.rend() - 1); ++dim) {
            strm << std::to_string(*dim) << ",";
        }
        strm << std::to_string(node_info.second[0]) << "] -> ";
		strm << "[" << node.first << "]}";

		isl_map* node_map = isl_map_read_from_str(ctx, strm.str().c_str());
		schedule = isl_union_map_add_map(schedule, node_map);
		strm.str(std::string());
		++node_count;
	}
	return isl_union_map_coalesce(schedule);
};

__isl_keep isl_union_map* qrane_deps::generate_validity_map(isl_ctx* ctx) {
    std::ostringstream strm;
    isl_union_map* ret_map = isl_union_map_empty_ctx(ctx);
    isl_map* single_map;

    for (const auto& vertex : this->G_fixed) {
		auto left_info = membership[vertex.first];
		for (const auto neighbor : vertex.second) {
			auto right_info = membership[neighbor];

       	 	strm << "{S" << std::to_string(left_info.first) << "[";
        	for (std::vector<unsigned int>::reverse_iterator dim = left_info.second.rbegin(); dim != (left_info.second.rend() - 1); ++dim) {
            	strm << std::to_string(*dim) << ",";
        	}
        	strm << std::to_string(left_info.second[0]) << "] -> ";

        	strm << "S" << std::to_string(right_info.first) << "[";
        	for (std::vector<unsigned int>::reverse_iterator dim = right_info.second.rbegin(); dim != (right_info.second.rend() - 1); ++dim) {
            	strm << std::to_string(*dim) << ",";
        	}
        	strm << std::to_string(right_info.second[0]) << "]};";

        	single_map = isl_map_read_from_str(ctx, strm.str().c_str());
        	ret_map = isl_union_map_coalesce(isl_union_map_add_map(ret_map, single_map));
        	strm.str(std::string());
		}
    }
	//isl_bool exact = isl_bool_false;
	//isl_bool* exact_ptr = &exact;
	//ret_map = isl_union_map_transitive_closure(ret_map, exact_ptr);
    return isl_union_map_coalesce(ret_map);
};


__isl_keep isl_union_map* qrane_deps::generate_write_all_dependences(isl_ctx* ctx) {
    std::ostringstream strm;
    isl_union_map* ret_map = isl_union_map_empty_ctx(ctx);
    isl_map* single_map;

    for (const auto& vertex : this->G_write_all) {
		auto left_info = membership[vertex.first];
		for (const auto neighbor : vertex.second) {
			auto right_info = membership[neighbor];

       	 	strm << "{S" << std::to_string(left_info.first) << "[";
        	for (std::vector<unsigned int>::reverse_iterator dim = left_info.second.rbegin(); dim != (left_info.second.rend() - 1); ++dim) {
            	strm << std::to_string(*dim) << ",";
        	}
        	strm << std::to_string(left_info.second[0]) << "] -> ";

        	strm << "S" << std::to_string(right_info.first) << "[";
        	for (std::vector<unsigned int>::reverse_iterator dim = right_info.second.rbegin(); dim != (right_info.second.rend() - 1); ++dim) {
            	strm << std::to_string(*dim) << ",";
        	}
        	strm << std::to_string(right_info.second[0]) << "]};";

        	single_map = isl_map_read_from_str(ctx, strm.str().c_str());
        	ret_map = isl_union_map_coalesce(isl_union_map_add_map(ret_map, single_map));
        	strm.str(std::string());
		}
    }
	//isl_bool exact = isl_bool_false;
	//isl_bool* exact_ptr = &exact;
	//ret_map = isl_union_map_transitive_closure(ret_map, exact_ptr);
    return isl_union_map_coalesce(ret_map);
};

	// ------------------------------------
	// Dependence Graph Frontier Processing
	// ------------------------------------

/*
	This is handled within qrane_graph
*/
void qrane_deps::initialize_frontier_processing() {
	std::cout << "Graph vertices: " << this->G_fixed.get_vertex_set().size() << std::endl;
	this->G_processing = this->G_fixed;
};

/*
	TODO: Document
*/
std::vector<qop_id> qrane_deps::get_dependence_frontier() {
	std::unordered_set<qop_id> frontier = this->G_processing.get_frontier();
	std::vector<qop_id> ret(std::make_move_iterator(frontier.begin()),
							std::make_move_iterator(frontier.end()));
	return ret;
};

/*
	TODO: Document
*/
void qrane_deps::remove_frontier_subset(std::vector<qop_id>& path) {
	for (const auto& v : path) {
		this->G_processing.remove_vertex(v);
	}
};

	// ------------------------
	// Look-ahead Functionality
	// ------------------------

/*
	TODO: Document
*/
std::array<unsigned int, 2> qrane_deps::dependence_graph_lookahead_breadth(
	std::vector<qop_id>& frontier, unsigned int num_qops, 
	unsigned int depth, const qrane_qop_map& qops) {

	std::array<unsigned int, 2> all_counts = {0,0};
	for (std::size_t i = 0; i < frontier.size(); ++i) {
		const auto& neighbors = this->G_processing.get_neighbors(frontier[i]);

        // No nodes to look ahead
		if (neighbors.size() == 0) { continue; }

		std::size_t args_count;
		for (const auto& n : neighbors) {
			args_count = qops[n]->num_args();
			all_counts[args_count - 1] += 1;
		}
	}
	return all_counts;
};

/*
	TODO: Document
*/
std::array<unsigned int, 2> qrane_deps::dependence_graph_lookahead_depth(
	std::vector<qop_id>& frontier, unsigned int num_qops, unsigned int depth,
	const qrane_qop_map& qops) {
	std::array<unsigned int, 2> count = {0, 0};

	for (const auto& f : frontier) {
		std::size_t args = qops[f]->num_args();
		count[args - 1] += 1;
		dependence_graph_lookahead_depth_main(f, num_qops, depth, count, qops);
	}
	return count;
};

/*
	TODO: Document
*/
void qrane_deps::dependence_graph_lookahead_depth_main(
	qop_id src, unsigned int num_qops, 
	unsigned int depth, std::array<unsigned int, 2>& count, 
	const qrane_qop_map& qops) {

	// The frame is <neighbor node, current_depth>
	auto traversal = std::stack<std::pair<qop_id, unsigned int>>();
	auto completed = std::vector<bool>(num_qops, false);
	traversal.push(std::make_pair(src, 1));

	unsigned int v;
	unsigned int d;
	while (!traversal.empty()) {
		auto frame = traversal.top();
		v = frame.first;
		d = frame.second;
		traversal.pop();

		// If the vertex has not been counted, count it and mark it as counted
		if (!completed[v]) { 
			completed[v] = true;
			std::size_t args = qops[v]->num_args();
			count[args - 1] += 1;
		}

		if (d < depth) {
			for (const auto& n : this->G_processing.get_neighbors(v)) {
				traversal.push(std::make_pair(n, d+1));
			}
		}
	}
};

qrane_graph<qop_id> qrane_deps::get_dependence_graph() {
	return this->G_fixed;
};


	// -----------------------------
	// Printing and String Retrieval
	// -----------------------------

qubit_profile_map qrane_deps::get_qubit_access_profile_map() {
	return this->profile_map;
}

/*
	TODO: Document
*/
std::string qrane_deps::get_networkx_edge_list_str() {
	std::ostringstream strm;
	for (const auto& entry : this->G_fixed) {
		for (const auto neighbor : entry.second) {
			strm << entry.first << " " << neighbor << std::endl;
		}
	}
	return strm.str();
};

std::string qrane_deps::get_gates_networkx_edge_list_str(const qrane_qop_map& qops) {
	std::ostringstream strm;
	std::string lhs_id;
	std::string rhs_id;
	for (const auto& entry : this->G_fixed) {
		lhs_id = qops[entry.first]->to_string();
		lhs_id.erase(std::remove(lhs_id.begin(), lhs_id.end(), ' '), lhs_id.end());
		for (const auto& neighbor : entry.second) {
			rhs_id = qops[neighbor]->to_string();
			rhs_id.erase(std::remove(rhs_id.begin(), rhs_id.end(), ' '), rhs_id.end());
			strm << lhs_id << " " << rhs_id << std::endl;
		}
	}
	return strm.str();
};

/*
	TODO: Document
*/
std::string qrane_deps::get_qubit_access_profile_str() {
	std::ostringstream strm;
	unsigned long long num_qubit_expr = 0;
	for (const auto &index_map : profile_map) {

		strm << index_map.first << ": {";
		for (const auto &gate_count_pair : index_map.second) {
			strm << " " << gate_count_pair.first << " (" << gate_count_pair.second << ")";
			num_qubit_expr += gate_count_pair.second;
		}
		strm << " }" << std::endl;
	}
	strm << "Index expression count: " << num_qubit_expr << std::endl;
	return strm.str();
};

/*
	TODO: Document
*/
void qrane_deps::time_dependence_graph_print() {
	std::ostringstream strm;
	strm << "{\n";
	for (const auto& pair : this->G_fixed) {
		strm << "\t" << pair.first << " -> [ ";
		for (auto val : pair.second) {
			strm << val << " ";
		}
		strm << "];\n";
	}
	strm << "};";
	std::cout << strm.str() << std::endl;
};

/*
	TODO: Document
*/
void qrane_deps::time_dependence_graph_print_given(qrane_graph<qop_id>& G) {
	std::ostringstream strm;
	strm << "{\n";
	for (auto& pair : G) {
		strm << "\t" << pair.first << " -> [ ";
		for (auto val : pair.second) {
			strm << val << " ";
		}
		strm << "];\n";
	}
	strm << "};";
	std::cout << strm.str() << std::endl;
};

/*
	TODO: Document
*/
void qrane_deps::print_membership() {
    std::ostringstream stream;

    stream << "Membership: (line_num) -> (domain_num, [k, j, ...])" << std::endl;
    for (const auto &entry : membership) {
        stream << "(" << entry.first << ") -> ";
        stream << "(" << entry.second.first << ", [";

		for (auto r = entry.second.second.rbegin(); r != entry.second.second.rend(); ++r) {
			stream << *r << " ";
		}
        stream << "])" << std::endl;
    }
    std::cout << stream.str() << std::endl << std::flush;
};

/*
	TODO: Document
*/
void qrane_deps::print_membership_given(membership_map& given) {
	std::ostringstream stream;

    stream << "Membership: (line_num) -> (domain_num, [i, j, ...])" << std::endl;
    for (const auto &entry : given) {
        stream << "(" << entry.first << ") -> ";
        stream << "(" << entry.second.first << ", [";
        for (auto r = entry.second.second.rbegin(); r != entry.second.second.rend(); ++r) {
			stream << *r << " ";
		}
        stream << "])" << std::endl;
    }
    std::cout << stream.str() << std::endl;
}
