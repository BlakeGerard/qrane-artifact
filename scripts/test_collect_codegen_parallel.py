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
sched = ""
threads = ""

def run_qrane(file_name):
	if (file_name.endswith(".bak") or 'fqm' in file_name): return
	benchmark = file_name.rstrip(".qasm")
	command = ["./qrane/qrane", 
		"--qasm={}".format(os.path.join(input_dir, file_name)),
		"--codegen={}".format(os.path.join(codegen_dir, benchmark + "." + sched + ".qasm")),
		"--{}".format(sched)]

	print(command)
	output_str = ""
	try:
		result = sp.run(command, capture_output=True, timeout=900)
		output_str = str(result.stdout, 'utf-8')
	except:
		output_str = "Timeout"

	out_file = open(os.path.join(stats_dir, benchmark + "-" + sched + "-c{}-s{}.txt".format(chunk, search)), "w")
	out_file.write(output_str)
	out_file.close()

def collect_and_store_output():
	all_files = os.listdir(input_dir)
	p = Pool(int(threads))
	p.map(run_qrane, all_files)

def input_phase(argv):
	if (len(argv) < 7):
		print("<script> <input_dir> <stats_dir> <codegen_dir> <chunk> <search> <minfuse|maxfuse> <threads>")
		exit(1)
	else:
		return argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]

input_dir, stats_dir, codegen_dir, chunk, search, sched, threads = input_phase(argv)
collect_and_store_output()
