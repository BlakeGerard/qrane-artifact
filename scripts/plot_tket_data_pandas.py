from re import sub, findall
from sys import argv
from collections import defaultdict
import pandas as pd
import numpy as np
import os
from numpy import linspace, arange
import matplotlib.pyplot as plt
from more_itertools import grouper

plt.figure()
plt.rcParams.update({
	'font.size': 3,
})

def input_phase(argv):
	if (len(argv) < 5):
		print("<script> path/to/minfuse_results.txt path/to/maxfuse_results.txt path/to/output/file.eps metric(0, 1, 2)")
		exit(1)

# Change m to change metric
# m = 0 -> gate_delta
# m = 1 -> circuit_depth (absolute)
# m = 2 -> tket compilation time 
input_phase(argv)
minfuse_file = argv[1]
maxfuse_file = argv[2]
out_name = argv[3]
m = int(argv[4])

def plot_data(minfuse_data, maxfuse_data):
	#labels = ['minfuse + tket-base', 'maxfuse + tket-base', 
	labels = ['minfuse + tket-opt', 'maxfuse + tket-opt', 'original + tket-opt',
				'minfuse average', 'maxfuse average', 'original average']
	metric_labels = ['Gate Count (gates)', 'Circuit Depth (gates)']

	pretty_print_data(maxfuse_data)

	fig, axes = plt.subplots(1, 1, figsize=(7,1.5))
	x = np.asarray(list(range(0, len(minfuse_data))))

	gates_c1 = []	# minfuse + base
	gates_c2 = []	# maxfuse + base
	gates_c3 = []   # minfuse + opt
	gates_c4 = []   # maxfuse + opt
	gates_c5 = []	# original + opt

	# Get data for the metric at index m
	# m = 0 -> gate_delta
	# m = 1 -> circuit_depth (absolute)
	# m = 2 -> tket compilation time 
	i = 0
	for circ in minfuse_data:
		gates_c1.append(minfuse_data[circ]['qrane_baseline'][m])
		gates_c2.append(maxfuse_data[circ]['qrane_baseline'][m])
		gates_c3.append(minfuse_data[circ]['qrane_fancy'][m])
		gates_c4.append(maxfuse_data[circ]['qrane_fancy'][m])
		gates_c5.append(minfuse_data[circ]['qasm_fancy'][m])
		i += 1

	print(gates_c4)
	print(gates_c5)

	gates_c1 = np.asarray(gates_c1)
	gates_c2 = np.asarray(gates_c2)
	gates_c3 = np.asarray(gates_c3)
	gates_c4 = np.asarray(gates_c4)
	gates_c5 = np.asarray(gates_c5)

	c3_avg = np.average(gates_c3)
	c4_avg = np.average(gates_c4)
	c5_avg = np.average(gates_c5)

	#l1 = axes.scatter(x, gates_c1, marker='+', label=labels[0], color='skyblue')
	#l2 = axes.scatter(x, gates_c2, marker='x', label=labels[1], color='mediumseagreen')
	l3 = axes.scatter(x, gates_c3, s=8, marker='*', label=labels[2], color='deepskyblue')
	l4 = axes.scatter(x, gates_c4, s=8, marker='1', label=labels[3], color='seagreen')
	l5 = axes.scatter(x, gates_c5, s=8, marker='.', label=labels[4], color='mediumpurple')
	l6 = axes.hlines([c3_avg], np.min(x), np.max(x), color='firebrick', linestyle='dotted')
	l7 = axes.hlines([c4_avg], np.min(x), np.max(x), color='peru', linestyle='dashed')
	l8 = axes.hlines([c5_avg], np.min(x), np.max(x), color='hotpink', linestyle='solid')

	axes.set_xlabel('Circuits')
	axes.set_ylabel(metric_labels[m])
	axes.grid()

	fig.tight_layout()
	fig.legend([l3, l4, l5, l6, l7, l8], labels=labels, title='Configuration')
	plt.savefig(out_name, format='eps')

def compute_table_means(gates_frame, depth_frame):	
	gates_frame['minfuse_improve'] = gates_frame["minfuse + tket-opt"] < gates_frame["baseline + tket-opt"]
	gates_frame['maxfuse_improve'] = gates_frame["maxfuse + tket-opt"] < gates_frame["baseline + tket-opt"]

	minfuse_effects = gates_frame.groupby('minfuse_improve')
	for name,grp in minfuse_effects:
		s = grp["minfuse + tket-opt"] - grp["baseline + tket-opt"]
		print(name, s.mean(), len(grp))

	maxfuse_effects = gates_frame.groupby('maxfuse_improve')
	for name,grp in maxfuse_effects:
		s = grp["maxfuse + tket-opt"] - grp["baseline + tket-opt"]
		print(name, s.mean(), len(grp))

	'''
	depth_frame['minfuse_improve'] = depth_frame["minfuse + tket-opt"] < depth_frame["baseline + tket-opt"]
	depth_frame['maxfuse_improve'] = depth_frame["maxfuse + tket-opt"] < depth_frame["baseline + tket-opt"]

	minfuse_effects = depth_frame.groupby('minfuse_improve')
	for name,grp in minfuse_effects:
		s = grp["minfuse + tket-opt"] - grp["baseline + tket-opt"]
		print(name, s.mean(), len(grp))

	maxfuse_effects = depth_frame.groupby('maxfuse_improve')
	for name,grp in maxfuse_effects:
		s = grp["maxfuse + tket-opt"] - grp["baseline + tket-opt"]
		print(name, s.mean(), len(grp))
	'''

def plot_frames(gates_frame, depth_frame):
	gates_by_config = gates_frame.groupby('Density Vector').mean()
	depth_by_config = depth_frame.groupby('Density Vector').mean()

	gates_ax = gates_by_config.plot(kind='bar', figsize=(5,1.5), rot=45)
	gates_ax.set_ylabel("Gate Count (gates)")
	plt.tight_layout()
	plt.savefig(os.path.join(out_name, "queko-bigd-abs-gates.eps"), format="eps")

	depth_ax = depth_by_config.plot(kind='bar', figsize=(5,1.5), rot=45)
	depth_ax.set_ylabel("Circuit Depth (gates)")
	plt.tight_layout()
	plt.savefig(os.path.join(out_name, "queko-bigd-abs-depth.eps"), format="eps")

def get_bigd_config(name):
	config = "{}_{}"
	pattern = "\dD\d"
	result = findall(pattern, name)
	config = config.format(result[0], result[1])
	return config

def group_data(minfuse_file, maxfuse_file, sizes):

	gates_data = {
		"circuit" : [],
		"Density Vector" : [],
		"baseline + tket-opt" : [],
		"minfuse + tket-opt" : [],
		"maxfuse + tket-opt" : []
	}

	depth_data = {
		"circuit" : [],
		"Density Vector" : [],
		"baseline + tket-opt" : [],
		"minfuse + tket-opt" : [],
		"maxfuse + tket-opt" : []
	}


	minfuse_circuits = []
	maxfuse_circuits = []

	# Collect circuits with minfuse
	content = open(minfuse_file, "r").readlines()
	for line in content:
		line = line.split(':')
		circuit = line[0]
		minfuse_circuits.append(circuit)

	# Collect circuits with maxfuse
	content = open(maxfuse_file, "r").readlines()
	for line in content:
		line = line.split(':')
		circuit = line[0]
		maxfuse_circuits.append(circuit)
 
	# Minfuse data
	content = open(minfuse_file, "r").readlines()
	for line in content:
		line = line.split(':')
		circuit = line[0]
		if (circuit not in minfuse_circuits or circuit not in maxfuse_circuits): 
			continue

		gates_data['circuit'].append(line[0])
		gates_data['Density Vector'].append(get_bigd_config(circuit))
		gates_data['minfuse + tket-opt'].append(int(line[5]) + sizes[circuit])
		gates_data['baseline + tket-opt'].append(int(line[9]) + sizes[circuit])

		depth_data['circuit'].append(line[0])
		depth_data['Density Vector'].append(get_bigd_config(circuit))
		depth_data['minfuse + tket-opt'].append(int(line[6]))
		depth_data['baseline + tket-opt'].append(int(line[10]))


	# Maxfuse data
	content = open(maxfuse_file, "r").readlines()
	for line in content:
		line = line.split(':')
		circuit = line[0]
		if (circuit not in minfuse_circuits or circuit not in maxfuse_circuits): 
			continue

		gates_data['maxfuse + tket-opt'].append(int(line[5]) + sizes[circuit])
		depth_data['maxfuse + tket-opt'].append(int(line[6]))

	gates_frame = pd.DataFrame(gates_data)
	depth_frame = pd.DataFrame(depth_data)

	return gates_frame, depth_frame

def get_circuit_sizes():
	f = './scripts/info/queko-bigd-gates.txt'
	sizes = defaultdict(int)
	with open(f, 'r') as fh:
		for line in fh:
			line = line.split(":")
			sizes[line[0]] = int(line[1])

	return sizes

def get_improvement_averages(minfuse_data, maxfuse_data):
	tket_sum = 0
	tket_count = 0
	depth_imp_sum = 0
	depth_imp_count = 0
	good_circs = []
	for circ in minfuse_data:
		qrane_depth = minfuse_data[circ]['qrane_fancy'][m]
		qasm_depth = minfuse_data[circ]['qasm_fancy'][m]
		tket_sum += qasm_depth
		tket_count += 1
		if (qrane_depth < qasm_depth):
			good_circs.append((circ, 'minfuse', qrane_depth, qasm_depth))
			depth_imp_sum += qasm_depth - qrane_depth
			depth_imp_count += 1

	tket_avg = tket_sum / tket_count
	depth_imp_avg = depth_imp_sum / depth_imp_count
	print("tket avg: ", tket_avg)
	print("minfuse improvement avg: ", depth_imp_avg)
	print("circuits_improved_minfuse: ", depth_imp_count)

	depth_wrs_sum = 0
	depth_wrs_count = 0
	for circ in minfuse_data:
		qrane_depth = minfuse_data[circ]['qrane_fancy'][m]
		qasm_depth = minfuse_data[circ]['qasm_fancy'][m]
		if (qrane_depth > qasm_depth):
			depth_wrs_sum += qrane_depth - qasm_depth
			depth_wrs_count += 1

	depth_wrs_avg = depth_wrs_sum / depth_wrs_count
	print("minfuse deterioration avg: ", depth_wrs_avg)
	print("circuits_deteriorate_minfuse: ", depth_wrs_count)

	depth_imp_sum = 0
	depth_imp_count = 0
	for circ in maxfuse_data:
		qrane_depth = maxfuse_data[circ]['qrane_fancy'][m]
		qasm_depth = maxfuse_data[circ]['qasm_fancy'][m]
		if (qrane_depth < qasm_depth):
			good_circs.append((circ, 'maxfuse', qrane_depth, qasm_depth))
			depth_imp_sum += qasm_depth - qrane_depth
			depth_imp_count += 1

	#ordered = sorted(good_circs, key=lambda e: e[0])
	#for e in ordered:
	#	print(e[0], e[1], e[2], e[3])

	depth_imp_avg = depth_imp_sum / depth_imp_count
	print("maxfuse improvement avg: ", depth_imp_avg)
	print("circuits_imp_maxfuse: ", depth_imp_count)

	depth_wrs_sum = 0
	depth_wrs_count = 0
	for circ in maxfuse_data:
		qrane_depth = maxfuse_data[circ]['qrane_fancy'][m]
		qasm_depth = maxfuse_data[circ]['qasm_fancy'][m]
		if (qrane_depth > qasm_depth):
			depth_wrs_sum += qrane_depth - qasm_depth
			depth_wrs_count += 1

	depth_wrs_avg = depth_wrs_sum / depth_wrs_count
	print("maxfuse deterioration avg: ", depth_wrs_avg)
	print("circuits_wrs_maxfuse: ", depth_wrs_count)

sizes = get_circuit_sizes()
minfuse_data, maxfuse_data = group_data(minfuse_file, maxfuse_file, sizes)
plot_frames(minfuse_data, maxfuse_data)
#compute_table_means(minfuse_data, maxfuse_data)
#get_improvement_averages(minfuse_data, maxfuse_data)


'''
		minfuse_data['circuit'].append(line[0])
		minfuse_data['Density Vector'].append(get_bigd_config(circuit))
		minfuse_data['gates_qrane_baseline'].append(int(line[1]) + sizes[circuit])
		minfuse_data['gates_qrane_fancy'].append(int(line[5]) + sizes[circuit])
		minfuse_data['gates_tket_fancy'].append(int(line[9]) + sizes[circuit])
		minfuse_data['depth_qrane_baseline'].append(int(line[2]))
		minfuse_data['depth_qrane_fancy'].append(int(line[6]))
		minfuse_data['depth_tket_fancy'].append(int(line[10]))
		minfuse_data['time_qrane_baseline'].append(float(line[3]))
		minfuse_data['time_qrane_fancy'].append(float(line[7]))
		minfuse_data['time_tket_fancy'].append(float(line[11]))

		maxfuse_data['circuit'].append(line[0])
		maxfuse_data['Density Vector'].append(get_bigd_config(circuit))
		maxfuse_data['gates_qrane_baseline'].append(int(line[1]) + sizes[circuit])
		maxfuse_data['gates_qrane_fancy'].append(int(line[5]) + sizes[circuit])
		maxfuse_data['gates_tket_fancy'].append(int(line[9]) + sizes[circuit])
		maxfuse_data['depth_qrane_baseline'].append(int(line[2]))
		maxfuse_data['depth_qrane_fancy'].append(int(line[6]))
		maxfuse_data['depth_tket_fancy'].append(int(line[10]))
		maxfuse_data['time_qrane_baseline'].append(float(line[3]))
		maxfuse_data['time_qrane_fancy'].append(float(line[7]))
		maxfuse_data['time_tket_fancy'].append(float(line[11]))
'''
