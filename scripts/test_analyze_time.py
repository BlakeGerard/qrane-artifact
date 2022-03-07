from sys import argv
import os
import subprocess as sp
import re
from collections import defaultdict, OrderedDict
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

font = {'family' : 'normal',
	'size' : 6}
matplotlib.rc('font', **font)

def plot_data(all, output_dir, benchmark, color):
	fig, ax = plt.subplots(1, 1, figsize=(4, 1.3))

	x = []
	y = []

	# Sort by qops count
	c = sorted(all, key = lambda k: all[k][0])

	for key in c:
		x.append(all[key][0])
		y.append(all[key][1])

	ax.plot(x, y, color=color)
	ax.set_xlabel('Circuit Size (gates)')
	ax.set_ylabel('Reconstruction Time (s)')
	ax.tick_params(rotation=0)
	ax.grid()

	output_file = os.path.join(output_dir, benchmark + ".eps")
	plt.tight_layout()
	plt.savefig(output_file, format="eps")

def killed(data):
	return data == "Killed" or data == ""

def get_qops_count(data):
	qops_line = re.search("Qops: \d*", data).group()
	number = int(re.search("\d+", qops_line).group())
	return number

def get_times(data):
	times = re.findall("\d+ms", data)
	recon = float(times[0].replace("ms", "")) * 0.001
	sched = float(times[1].replace("ms", "")) * 0.001
	return recon, sched

def get_benchmark_name(file_name):
	file_name = file_name[0:]
	fields = file_name.split('.')
	bn = fields[0]
	return bn

def get_result_files(input_dir, output_dir):
	all = defaultdict(tuple)
	for file in os.scandir(input_dir):
		with open(file, 'r') as fh:
			if (file.stat().st_size > 0):
				data = fh.read()
				if (killed(data) == True): continue
				bn = get_benchmark_name(file.name)
				qops = get_qops_count(data)
				recon, sched = get_times(data)
				total = recon + sched
				all[bn] = (qops, total)

	return all

def input_phase(argv):
	if (len(argv) < 3):
		print("<script> <input_dir> <output_dir> <benchmark> <color>")
		exit(1)
	else:
		return argv[1], argv[2], argv[3], argv[4]


input_dir, output_dir, benchmark, color = input_phase(argv)
all = get_result_files(input_dir, output_dir)
plot_data(all, output_dir, benchmark, color)
