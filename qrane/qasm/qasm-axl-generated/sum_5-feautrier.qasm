OPENQASM 2.0;
include "qelib1.inc";
qreg qureg[11];
cx qureg[10],qureg[1];
cx qureg[10],qureg[2];
cx qureg[10],qureg[3];
cx qureg[10],qureg[4];
cx qureg[10],qureg[5];
cx qureg[10],qureg[6];
cx qureg[10],qureg[7];
cx qureg[10],qureg[8];
cx qureg[0],qureg[10];
cx qureg[10],qureg[2];
ccx qureg[1],qureg[2],qureg[10];
cx qureg[10],qureg[4];
ccx qureg[3],qureg[4],qureg[10];
cx qureg[10],qureg[1];
cx qureg[10],qureg[6];
cx qureg[10],qureg[3];
ccx qureg[5],qureg[6],qureg[10];
cx qureg[10],qureg[5];
cx qureg[10],qureg[8];
cx qureg[10],qureg[7];
ccx qureg[7],qureg[8],qureg[10];
ccx qureg[1],qureg[2],qureg[0];
ccx qureg[3],qureg[4],qureg[0];
ccx qureg[5],qureg[6],qureg[0];
cx qureg[0],qureg[7];
ccx qureg[5],qureg[6],qureg[0];
cx qureg[0],qureg[5];
ccx qureg[3],qureg[4],qureg[0];
ccx qureg[1],qureg[2],qureg[0];
cx qureg[0],qureg[3];
cx qureg[0],qureg[1];
cx qureg[1],qureg[2];
cx qureg[3],qureg[4];
cx qureg[5],qureg[6];
cx qureg[7],qureg[8];
cx qureg[9],qureg[10];
