from sys import argv
import os
import subprocess as sp
import re
from multiprocessing import Pool

input_dir = ""
stats_dir = ""
codegen_dir = ""
chunk = ""
search = ""
threads = ""

def run_qrane(file_name):
	if (file_name.endswith(".bak") or 'fqm' in file_name): return
	command = ["./qrane/qrane", 
		"--qasm={}".format(os.path.join(input_dir, file_name)), 
		"--chunk={}".format(chunk), 
		"--search={}".format(search)]

	print(command)
	output_str = ""
	try:
		result = sp.run(command, capture_output=True)
		output_str = str(result.stdout, 'utf-8')
	except Exception as e:
		print(e)
		output_str = "Exception raised"

	benchmark = file_name.rstrip(".qasm")
	out_file = open(os.path.join(stats_dir, benchmark + "-c{}-s{}.txt".format(chunk, search)), "w")
	out_file.write(output_str)
	out_file.close()

def collect_and_store_output():
	all_files = os.listdir(input_dir)
	p = Pool(int(threads))
	p.map(run_qrane, all_files)

def input_phase(argv):
	if (len(argv) < 6):
		print("<script> <input_dir> <stats_dir> <codegen_dir> <chunk> <search> <threads>")
		exit(1)
	else:
		return argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]

input_dir, stats_dir, codegen_dir, chunk, search, threads = input_phase(argv)
collect_and_store_output()
