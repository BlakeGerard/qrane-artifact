import sys
import re

if (len(sys.argv) < 2):
  print ("Target circuit not given. Aborting...")
  sys.exit (42)

circname = sys.argv[1]

try:
  ff = open (circname)
except FileNotFoundError:
  print ("File {} not found. Aborting ...".format (circname))
  sys.exit (41)

parts = circname.split('.')
npart = len(parts)
if (parts[npart-1].find ("qasm") < 0 ):
  print ("Circuit given not in QASM format. Aborting ...")
  sys.exit (40)

alllines = ff.readlines ()
nops = len (alllines)
print (nops-3)

for line in alllines:
  line = line.strip ()
  is2gate = line.find (",") >= 0
  q1 = re.search (r'\[[0-9][0-9]*\]', line)
  if (q1 == None):
    continue
  if (not is2gate):
    q1 = q1.group()[1:-1]
    print ("1 {}".format (q1))
  else:
    q1 = q1.group()[1:-1]
    q2 = line.split (',')
    q2 = re.search (r'\[[0-9][0-9]*\]', q2[1])
    q2 = q2.group()[1:-1]
    print ("2 {} {}".format (q1,q2))

ff.close ()
