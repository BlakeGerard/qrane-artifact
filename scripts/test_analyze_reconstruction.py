import math
from sys import argv
import os
import subprocess as sp
import re
from collections import defaultdict
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

font = {'family' : 'normal',
	'size' : 4}
matplotlib.rc('font', **font)

total_qops_processed = 0
xtitle = "Cardinality (points)"
ytitle = "Number of Domains"

def plot_data(profile, output_dir, benchmark, color):
	num_plots = len(profile)	# One plot per dimension
	fig, axes = plt.subplots(1, num_plots, figsize=(7, 1.32))

	i = 0
	dims_sorted = sorted(profile)
	for dim in dims_sorted:
		#if (dim != '4'): continue
		x = []			# All cards in profile[dim]
		y = []			# All counts in profile[dim][card]
		cards_sorted = sorted(profile[dim], key=lambda e: int(e))

		for card in cards_sorted:
			x.append(card)
			y.append(profile[dim][card])

		axes[i].set_title(dim + "D")
		axes[i].set_xlabel(xtitle)
		axes[i].set_ylabel(ytitle)
		axes[i].set_yscale('log')
		axes[i].bar(x, y, color=color)
		axes[i].set_xticklabels(x, rotation=55)

		i += 1

	output_file = os.path.join(output_dir, benchmark + "-reconstruction.eps")
	print(output_file)
	plt.tight_layout()
	plt.savefig(output_file, format="eps")

def get_qops_count(data):
	global total_qops_processed
	qops_line = re.search("Qops: \d*", data).group()
	number = int(re.search("\d+", qops_line).group())
	return number

# { Card : Count }
def get_domain_sizes(data):
	'''
	dims_size = re.findall("\d+ : \d+", data)
	for val in dims_size:
		left = int(val[:val.index(':')].strip())
		right = int(val[val.index(':')+1:].strip())
		domain_size_profile[left] += right
	'''

# { Dim : Count }
def get_domain_dims(data):
	'''
	global domain_dim_profile
	dims_prof = re.findall("\dD: \d+", data)
	for val in dims_prof:
		colon = val.index(':')
		dim = val[0:colon]
		count = int(val[colon+2:])
		domain_dim_profile[dim] += count
	'''

def get_reconstruction_profile(data):
	profile = defaultdict(lambda: defaultdict(lambda: 0))
	prof_lines = re.findall("\d -> {.+}", data)
	for line in prof_lines:
		line = line.replace(" ", "").split("->")
		dim = line[0]
		hist = line[1]
		hist = hist.strip("{}")
		buckets = hist.split(",")
		for bucket in buckets:
			if bucket != '':
				both = bucket.split(":")
				lhs = both[0]
				rhs = int(both[1])
				profile[dim][lhs] += rhs
	return profile

def valid_file(file_contents):
	if (file_contents == '' or file_contents == 'Killed' or 'Exiting' in file_contents or 'Timeout' in file_contents): 
		return False
	else:
		return True

#
# main_prof = {
# 	"1D" : {"3" : count, "5": count, ...},
#	"2D" : { ... }
# }
#
def get_result_files(input_dir):
	main_prof = defaultdict(lambda: defaultdict(lambda: 0))

	# Go through each result file and catalogue the results in the global variables
	for result_file in os.scandir(input_dir):
		if (result_file.name.endswith(".txt") == False): continue

		with open(result_file, 'r') as fh:
			contents = fh.read()
			#print(contents)
			if (valid_file(contents) == True):
				qops = get_qops_count(contents)
				recon_profile = get_reconstruction_profile(contents)

				for dim in recon_profile:
					for card in recon_profile[dim]:
						main_prof[dim][card] += recon_profile[dim][card]

	return main_prof

def input_phase(argv):
	if (len(argv) < 5):
		print("Please provide input and output directories and a color for plots")
		exit(1)
	else:
		return argv[1], argv[2], argv[3], argv[4]

def get_points_ratio_above_and_below(main_prof, x):
	below = 0
	above = 0
	
	dims = ['2', '3', '4']
	for dim in dims:
		for card in main_prof[dim]:
			if (int(card) < x): 
				print(card + ":", main_prof[dim][card] * int(card))
				below += main_prof[dim][card] * int(card)
			else:
				print(card + ":", main_prof[dim][card] * int(card))
				above += main_prof[dim][card] * int(card)
	print(above, below, below/above)


input_dir, output_dir, benchmark, color = input_phase(argv)
main_prof = get_result_files(input_dir)
#get_points_ratio_above_and_below(main_prof, 4)
plot_data(main_prof, output_dir, benchmark, color)
