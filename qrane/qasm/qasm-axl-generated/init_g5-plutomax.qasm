OPENQASM 2.0;
include "qelib1.inc";
qreg qureg[11];
ccx qureg[0],qureg[1],qureg[10];
cx qureg[10],qureg[3];
cx qureg[10],qureg[2];
x qureg[1];
ccx qureg[2],qureg[3],qureg[10];
cx qureg[10],qureg[5];
cx qureg[10],qureg[4];
ccx qureg[2],qureg[3],qureg[0];
ccx qureg[4],qureg[5],qureg[10];
cx qureg[10],qureg[7];
cx qureg[10],qureg[6];
ccx qureg[4],qureg[5],qureg[0];
ccx qureg[3],qureg[4],qureg[5];
ccx qureg[6],qureg[7],qureg[10];
cx qureg[10],qureg[9];
cx qureg[10],qureg[8];
ccx qureg[6],qureg[7],qureg[0];
cx qureg[0],qureg[9];
cx qureg[0],qureg[8];
ccx qureg[7],qureg[6],qureg[0];
cx qureg[0],qureg[7];
cx qureg[0],qureg[6];
ccx qureg[5],qureg[4],qureg[0];
cx qureg[0],qureg[5];
cx qureg[0],qureg[4];
ccx qureg[3],qureg[2],qureg[0];
ccx qureg[0],qureg[1],qureg[9];
ccx qureg[0],qureg[1],qureg[8];
ccx qureg[0],qureg[1],qureg[7];
ccx qureg[0],qureg[1],qureg[6];
ccx qureg[0],qureg[1],qureg[5];
ccx qureg[0],qureg[1],qureg[4];
x qureg[1];
ccx qureg[0],qureg[1],qureg[3];
ccx qureg[0],qureg[1],qureg[2];
