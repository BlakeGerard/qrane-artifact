OPENQASM 2.0;
include "qelib1.inc";
qreg q[9];

h q[0];
h q[1];
x q[2];
h q[2];
x q[3];
h q[3];

cx q[0],q[1];
cx q[1],q[2];
cx q[2],q[3];

ccx q[0],q[1],q[2];

h q[0];
h q[1];
h q[2];

z q[2];

h q[0];
h q[1];
h q[2];
h q[3];

z q[1];

h q[0];
h q[1];
h q[2];