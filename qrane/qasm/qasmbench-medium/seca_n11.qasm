OPENQASM 2.0;
include "qelib1.inc";
qreg q[11];
creg c[11];
u(0,0,3.141592653589793) q[0];
u(1.5707963267948966,0,3.141592653589793) q[0];
cx q[0],q[3];
cx q[0],q[6];
u3(1.5707963267948966,0,3.141592653589793) q[3];
cx q[0],q[3];
u3(1.5707963267948966,0,3.141592653589793) q[3];
u(1.5707963267948966,0,3.141592653589793) q[3];
u(0,0,3.141592653589793) q[3];
cx q[3],q[4];
cx q[3],q[5];
u3(1.5707963267948966,0,3.141592653589793) q[4];
cx q[3],q[4];
u3(1.5707963267948966,0,3.141592653589793) q[4];
u3(1.5707963267948966,0,3.141592653589793) q[5];
cx q[3],q[5];
u3(1.5707963267948966,0,3.141592653589793) q[5];
u3(1.5707963267948966,0,3.141592653589793) q[6];
cx q[0],q[6];
u(1.5707963267948966,0,3.141592653589793) q[0];
u(0,0,3.141592653589793) q[0];
cx q[0],q[1];
cx q[0],q[2];
u3(1.5707963267948966,0,3.141592653589793) q[1];
cx q[0],q[1];
u3(1.5707963267948966,0,3.141592653589793) q[1];
u3(1.5707963267948966,0,3.141592653589793) q[2];
cx q[0],q[2];
u3(1.5707963267948966,0,3.141592653589793) q[2];
u3(1.5707963267948966,0,3.141592653589793) q[6];
u(1.5707963267948966,0,3.141592653589793) q[6];
u(0,0,3.141592653589793) q[6];
cx q[6],q[7];
cx q[6],q[8];
u3(1.5707963267948966,0,3.141592653589793) q[7];
cx q[6],q[7];
u3(1.5707963267948966,0,3.141592653589793) q[7];
u3(1.5707963267948966,0,3.141592653589793) q[8];
cx q[6],q[8];
u3(1.5707963267948966,0,3.141592653589793) q[8];
u(1.5707963267948966,0,3.141592653589793) q[9];
cx q[9],q[10];
cx q[0],q[9];
u(1.5707963267948966,0,3.141592653589793) q[0];
measure q[0] -> c[0];
measure q[9] -> c[9];
cx q[9],q[10];
u3(1.5707963267948966,0,3.141592653589793) q[10];
cx q[0],q[10];
u3(1.5707963267948966,0,3.141592653589793) q[10];
cx q[10],q[1];
u3(1.5707963267948966,0,3.141592653589793) q[1];
cx q[10],q[2];
cx q[10],q[1];
u3(1.5707963267948966,0,3.141592653589793) q[1];
u3(1.5707963267948966,0,3.141592653589793) q[2];
cx q[10],q[2];
u3(1.5707963267948966,0,3.141592653589793) q[10];
u3(1.5707963267948966,0,3.141592653589793) q[2];
cx q[2],q[10];
u3(0,0,-0.7853981633974483) q[10];
cx q[1],q[10];
u3(0,0,0.7853981633974483) q[10];
cx q[2],q[10];
u3(0,0,-0.7853981633974483) q[10];
cx q[1],q[10];
u3(0,0,0.7853981633974483) q[10];
u3(1.5707963267948966,0,3.141592653589793) q[10];
u3(0,0,0.7853981633974483) q[2];
cx q[1],q[2];
u3(0,0,0.7853981633974483) q[1];
u3(0,0,-0.7853981633974483) q[2];
cx q[1],q[2];
cx q[3],q[4];
cx q[3],q[5];
u3(1.5707963267948966,0,3.141592653589793) q[4];
cx q[3],q[4];
u3(1.5707963267948966,0,3.141592653589793) q[4];
u3(1.5707963267948966,0,3.141592653589793) q[5];
cx q[3],q[5];
u3(1.5707963267948966,0,3.141592653589793) q[3];
cx q[4],q[3];
u3(0,0,-0.7853981633974483) q[3];
u3(1.5707963267948966,0,3.141592653589793) q[5];
cx q[5],q[3];
u3(0,0,0.7853981633974483) q[3];
cx q[4],q[3];
u3(0,0,-0.7853981633974483) q[3];
u3(0,0,0.7853981633974483) q[4];
cx q[5],q[3];
u3(0,0,0.7853981633974483) q[3];
u3(1.5707963267948966,0,3.141592653589793) q[3];
cx q[5],q[4];
u3(0,0,-0.7853981633974483) q[4];
u3(0,0,0.7853981633974483) q[5];
cx q[5],q[4];
cx q[6],q[7];
cx q[6],q[8];
u3(1.5707963267948966,0,3.141592653589793) q[7];
cx q[6],q[7];
u3(1.5707963267948966,0,3.141592653589793) q[7];
u3(1.5707963267948966,0,3.141592653589793) q[8];
cx q[6],q[8];
u3(1.5707963267948966,0,3.141592653589793) q[6];
cx q[7],q[6];
u3(0,0,-0.7853981633974483) q[6];
u3(1.5707963267948966,0,3.141592653589793) q[8];
cx q[8],q[6];
u3(0,0,0.7853981633974483) q[6];
cx q[7],q[6];
u3(0,0,-0.7853981633974483) q[6];
u3(0,0,0.7853981633974483) q[7];
cx q[8],q[6];
u3(0,0,0.7853981633974483) q[6];
u3(1.5707963267948966,0,3.141592653589793) q[6];
cx q[8],q[7];
u3(0,0,-0.7853981633974483) q[7];
u3(0,0,0.7853981633974483) q[8];
cx q[8],q[7];
u(1.5707963267948966,0,3.141592653589793) q[10];
u3(1.5707963267948966,0,3.141592653589793) q[10];
cx q[2],q[10];
u3(0,0,-0.7853981633974483) q[10];
cx q[1],q[10];
u3(0,0,0.7853981633974483) q[10];
cx q[2],q[10];
u3(0,0,-0.7853981633974483) q[10];
cx q[1],q[10];
u3(0,0,0.7853981633974483) q[10];
u3(1.5707963267948966,0,3.141592653589793) q[10];
u(1.5707963267948966,0,3.141592653589793) q[10];
u3(0,0,0.7853981633974483) q[2];
cx q[1],q[2];
u3(0,0,0.7853981633974483) q[1];
u3(0,0,-0.7853981633974483) q[2];
cx q[1],q[2];
u(1.5707963267948966,0,3.141592653589793) q[3];
u3(1.5707963267948966,0,3.141592653589793) q[3];
cx q[4],q[3];
u3(0,0,-0.7853981633974483) q[3];
cx q[5],q[3];
u3(0,0,0.7853981633974483) q[3];
cx q[4],q[3];
u3(0,0,-0.7853981633974483) q[3];
u3(0,0,0.7853981633974483) q[4];
cx q[5],q[3];
u3(0,0,0.7853981633974483) q[3];
u3(1.5707963267948966,0,3.141592653589793) q[3];
u(1.5707963267948966,0,3.141592653589793) q[3];
cx q[5],q[4];
u3(0,0,-0.7853981633974483) q[4];
u3(0,0,0.7853981633974483) q[5];
cx q[5],q[4];
u(1.5707963267948966,0,3.141592653589793) q[6];
u3(1.5707963267948966,0,3.141592653589793) q[6];
cx q[7],q[6];
u3(0,0,-0.7853981633974483) q[6];
cx q[8],q[6];
u3(0,0,0.7853981633974483) q[6];
cx q[7],q[6];
u3(0,0,-0.7853981633974483) q[6];
u3(0,0,0.7853981633974483) q[7];
cx q[8],q[6];
u3(0,0,0.7853981633974483) q[6];
u3(1.5707963267948966,0,3.141592653589793) q[6];
u(1.5707963267948966,0,3.141592653589793) q[6];
cx q[8],q[7];
u3(0,0,-0.7853981633974483) q[7];
u3(0,0,0.7853981633974483) q[8];
cx q[8],q[7];
u(1.5707963267948966,0,3.141592653589793) q[10];
u(0,0,3.141592653589793) q[10];
u(1.5707963267948966,0,3.141592653589793) q[3];
u(0,0,3.141592653589793) q[3];
cx q[10],q[3];
u3(1.5707963267948966,0,3.141592653589793) q[3];
u(1.5707963267948966,0,3.141592653589793) q[6];
u(0,0,3.141592653589793) q[6];
cx q[10],q[6];
cx q[10],q[3];
u3(1.5707963267948966,0,3.141592653589793) q[3];
u3(1.5707963267948966,0,3.141592653589793) q[6];
cx q[10],q[6];
u3(1.5707963267948966,0,3.141592653589793) q[10];
u3(1.5707963267948966,0,3.141592653589793) q[6];
cx q[6],q[10];
u3(0,0,-0.7853981633974483) q[10];
cx q[3],q[10];
u3(0,0,0.7853981633974483) q[10];
cx q[6],q[10];
u3(0,0,-0.7853981633974483) q[10];
cx q[3],q[10];
u3(0,0,0.7853981633974483) q[10];
u3(1.5707963267948966,0,3.141592653589793) q[10];
u(1.5707963267948966,0,3.141592653589793) q[10];
u3(1.5707963267948966,0,3.141592653589793) q[10];
u3(0,0,0.7853981633974483) q[6];
cx q[3],q[6];
u3(0,0,0.7853981633974483) q[3];
u3(0,0,-0.7853981633974483) q[6];
cx q[3],q[6];
cx q[6],q[10];
u3(0,0,-0.7853981633974483) q[10];
cx q[3],q[10];
u3(0,0,0.7853981633974483) q[10];
cx q[6],q[10];
u3(0,0,-0.7853981633974483) q[10];
cx q[3],q[10];
u3(0,0,0.7853981633974483) q[10];
u3(1.5707963267948966,0,3.141592653589793) q[10];
u(1.5707963267948966,0,3.141592653589793) q[10];
u3(0,0,0.7853981633974483) q[6];
cx q[3],q[6];
u3(0,0,0.7853981633974483) q[3];
u3(0,0,-0.7853981633974483) q[6];
cx q[3],q[6];
u(1.5707963267948966,0,3.141592653589793) q[10];
u(0,0,3.141592653589793) q[10];