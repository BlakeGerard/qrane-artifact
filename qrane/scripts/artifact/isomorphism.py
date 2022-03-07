import pytket
from pytket.utils import Graph
from pytket.qasm import circuit_from_qasm
import networkx
from networkx import is_isomorphic
import os
import subprocess
from collections import defaultdict

exe = "./test --qasm={} --check_qasm={}"

def check_iso(benchmark, path1, path2):
	full = exe.format(path1, path2)
	proc = subprocess.run(full, shell=True, capture_output=True)
	res = str(proc.stdout, 'UTF-8')
	return res

def qasm_path_pairs(path):
	pairs = defaultdict(list)
	for entry in os.scandir(path):
		key = entry.name
		fixed = entry.path
		if (".qrane.qasm" in entry.name):
			key = key.replace(".qrane.qasm", "")
		elif (".qasm" in entry.name):
			key = key.replace(".qasm", "")

		pairs[key].append(fixed)
	return pairs

path = "/home/blake/Projects/axl-blake-copy/axl-blake/qrane/codegen/queko-bigd-minfuse/"
pairs = qasm_path_pairs(path)

with open("minfuse_isomorphism.txt", 'w') as fh:
	for benchmark in pairs:
		if (len(pairs[benchmark]) > 1):
			res = check_iso(benchmark, pairs[benchmark][0], pairs[benchmark][1])
			line = benchmark + " minfuse " + res + "\n"
			fh.write(line)
