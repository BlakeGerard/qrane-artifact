OPENQASM 2.0;
include "qelib1.inc";
qreg q[3];
h q[0];
cp(pi/2) q[1], q[0];
h q[1];
cp(pi/4) q[2], q[0];
cp(pi/2) q[2], q[1];
h q[2];