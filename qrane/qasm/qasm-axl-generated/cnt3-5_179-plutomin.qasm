OPENQASM 2.0;
include "qelib1.inc";
qreg qureg[19];
cx qureg[15],qureg[18];
ccx qureg[16],qureg[17],qureg[15];
cx qureg[12],qureg[14];
cx qureg[9],qureg[11];
cx qureg[6],qureg[8];
cx qureg[3],qureg[5];
cx qureg[0],qureg[2];
cx qureg[15],qureg[18];
ccx qureg[13],qureg[15],qureg[12];
cx qureg[12],qureg[14];
ccx qureg[10],qureg[12],qureg[9];
cx qureg[9],qureg[11];
ccx qureg[7],qureg[9],qureg[6];
cx qureg[6],qureg[8];
ccx qureg[4],qureg[6],qureg[3];
cx qureg[3],qureg[5];
ccx qureg[1],qureg[3],qureg[0];
cx qureg[0],qureg[2];
ccx qureg[14],qureg[15],qureg[13];
ccx qureg[11],qureg[12],qureg[10];
ccx qureg[8],qureg[9],qureg[7];
ccx qureg[5],qureg[6],qureg[4];
ccx qureg[2],qureg[3],qureg[1];
cx qureg[15],qureg[14];
cx qureg[12],qureg[11];
cx qureg[9],qureg[8];
cx qureg[6],qureg[5];
cx qureg[3],qureg[2];
ccx qureg[16],qureg[18],qureg[17];
cx qureg[16],qureg[18];
