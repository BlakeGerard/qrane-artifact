from sys import argv
import os
import subprocess as sp
import re
from multiprocessing import Pool

def input_phase(argv):
	if (len(argv) < 3):
		print("Please provide input and output directories")
		exit(1)
	else:
		return argv[1], argv[2]

input_dir, output_dir = input_phase(argv)
qrane_invocation = "./qrane {} -q -d {} -s 256"

def remove_qasm_extension(file_name):
	return file_name.rstrip(".qasm")

def get_register_size(file_name):
	reg_str = re.match("\d+", file_name).group()
	return int(reg_str)

def run_qrane(file_name):
	global input_dir, output_dir
	#depths = [1, 5, 15, 20, 25, 30] # ibmqx max reg size is 16
	depths = [10]

	for depth in depths:
		command = qrane_invocation.format(input_dir + file_name, str(depth))
		output_str = ""
		try:
			result = sp.run(command, shell=True, capture_output=True)
			output_str = str(result.stdout, 'utf-8')
		except Exception as e:
			print(e)
			output_str = "Killed"

		my_output_file = output_dir + file_name + ".d" + str(depth) + ".txt"
		fh = open(my_output_file, "w")
		fh.write(output_str)
		fh.close()

def collect_and_store_output():
	global input_dir
	files = os.listdir(input_dir)
	files = sorted(files, key=lambda f:
			os.stat(os.path.join(input_dir, f)).st_size)
	p = Pool()
	p.map(run_qrane, files)

collect_and_store_output()
