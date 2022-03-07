OPENQASM 2.0;
include "qelib1.inc";
qreg q[27];
creg c[27];
creg meas[27];
u(-0.7853981633974483,0,0) q[0];
u3(1.5707963267948966,0,3.141592653589793) q[0];
u(-0.95531662,0,0) q[1];
u3(1.5707963267948966,0,3.141592653589793) q[1];
u(-1.0471975511965976,0,0) q[2];
u3(1.5707963267948966,0,3.141592653589793) q[2];
u(-1.1071487,0,0) q[3];
u3(1.5707963267948966,0,3.141592653589793) q[3];
u(-1.150262,0,0) q[4];
u3(1.5707963267948966,0,3.141592653589793) q[4];
u(-1.1831996,0,0) q[5];
u3(1.5707963267948966,0,3.141592653589793) q[5];
u(-1.2094292,0,0) q[6];
u3(1.5707963267948966,0,3.141592653589793) q[6];
u(-1.2309594,0,0) q[7];
u3(1.5707963267948966,0,3.141592653589793) q[7];
u(-1.2490458,0,0) q[8];
u3(1.5707963267948966,0,3.141592653589793) q[8];
u(-1.264519,0,0) q[9];
u3(1.5707963267948966,0,3.141592653589793) q[9];
u(-1.2779536,0,0) q[10];
u3(1.5707963267948966,0,3.141592653589793) q[10];
u(-1.2897614,0,0) q[11];
u3(1.5707963267948966,0,3.141592653589793) q[11];
u(-1.3002466,0,0) q[12];
u3(1.5707963267948966,0,3.141592653589793) q[12];
u(-1.3096389,0,0) q[13];
u3(1.5707963267948966,0,3.141592653589793) q[13];
u(-1.3181161,0,0) q[14];
u3(1.5707963267948966,0,3.141592653589793) q[14];
u(-1.3258177,0,0) q[15];
u3(1.5707963267948966,0,3.141592653589793) q[15];
u(-1.3328552,0,0) q[16];
u3(1.5707963267948966,0,3.141592653589793) q[16];
u(-1.339319,0,0) q[17];
u3(1.5707963267948966,0,3.141592653589793) q[17];
u(-1.3452829,0,0) q[18];
u3(1.5707963267948966,0,3.141592653589793) q[18];
u(-1.3508083,0,0) q[19];
u3(1.5707963267948966,0,3.141592653589793) q[19];
u(-1.3559465,0,0) q[20];
u3(1.5707963267948966,0,3.141592653589793) q[20];
u(-1.3607406,0,0) q[21];
u3(1.5707963267948966,0,3.141592653589793) q[21];
u(-1.3652274,0,0) q[22];
u3(1.5707963267948966,0,3.141592653589793) q[22];
u(-1.3694384,0,0) q[23];
u3(1.5707963267948966,0,3.141592653589793) q[23];
u(-1.3734008,0,0) q[24];
u3(1.5707963267948966,0,3.141592653589793) q[24];
u(-1.377138,0,0) q[25];
u3(1.5707963267948966,0,3.141592653589793) q[25];
u(3.141592653589793,0,3.141592653589793) q[26];
cx q[26],q[25];
u3(1.5707963267948966,0,3.141592653589793) q[25];
u(1.377138,0,0) q[25];
cx q[25],q[24];
u3(1.5707963267948966,0,3.141592653589793) q[24];
u(1.3734008,0,0) q[24];
cx q[24],q[23];
u3(1.5707963267948966,0,3.141592653589793) q[23];
u(1.3694384,0,0) q[23];
cx q[23],q[22];
u3(1.5707963267948966,0,3.141592653589793) q[22];
u(1.3652274,0,0) q[22];
cx q[22],q[21];
u3(1.5707963267948966,0,3.141592653589793) q[21];
u(1.3607406,0,0) q[21];
cx q[21],q[20];
u3(1.5707963267948966,0,3.141592653589793) q[20];
u(1.3559465,0,0) q[20];
cx q[20],q[19];
u3(1.5707963267948966,0,3.141592653589793) q[19];
u(1.3508083,0,0) q[19];
cx q[19],q[18];
u3(1.5707963267948966,0,3.141592653589793) q[18];
u(1.3452829,0,0) q[18];
cx q[18],q[17];
u3(1.5707963267948966,0,3.141592653589793) q[17];
u(1.339319,0,0) q[17];
cx q[17],q[16];
u3(1.5707963267948966,0,3.141592653589793) q[16];
u(1.3328552,0,0) q[16];
cx q[16],q[15];
u3(1.5707963267948966,0,3.141592653589793) q[15];
u(1.3258177,0,0) q[15];
cx q[15],q[14];
u3(1.5707963267948966,0,3.141592653589793) q[14];
u(1.3181161,0,0) q[14];
cx q[14],q[13];
u3(1.5707963267948966,0,3.141592653589793) q[13];
u(1.3096389,0,0) q[13];
cx q[13],q[12];
u3(1.5707963267948966,0,3.141592653589793) q[12];
u(1.3002466,0,0) q[12];
cx q[12],q[11];
u3(1.5707963267948966,0,3.141592653589793) q[11];
u(1.2897614,0,0) q[11];
cx q[11],q[10];
u3(1.5707963267948966,0,3.141592653589793) q[10];
u(1.2779536,0,0) q[10];
cx q[10],q[9];
cx q[25],q[26];
cx q[24],q[25];
cx q[23],q[24];
cx q[22],q[23];
cx q[21],q[22];
cx q[20],q[21];
cx q[19],q[20];
cx q[18],q[19];
cx q[17],q[18];
cx q[16],q[17];
cx q[15],q[16];
cx q[14],q[15];
cx q[13],q[14];
cx q[12],q[13];
cx q[11],q[12];
cx q[10],q[11];
u3(1.5707963267948966,0,3.141592653589793) q[9];
u(1.264519,0,0) q[9];
cx q[9],q[8];
u3(1.5707963267948966,0,3.141592653589793) q[8];
u(1.2490458,0,0) q[8];
cx q[8],q[7];
u3(1.5707963267948966,0,3.141592653589793) q[7];
u(1.2309594,0,0) q[7];
cx q[7],q[6];
u3(1.5707963267948966,0,3.141592653589793) q[6];
u(1.2094292,0,0) q[6];
cx q[6],q[5];
u3(1.5707963267948966,0,3.141592653589793) q[5];
u(1.1831996,0,0) q[5];
cx q[5],q[4];
u3(1.5707963267948966,0,3.141592653589793) q[4];
u(1.150262,0,0) q[4];
cx q[4],q[3];
u3(1.5707963267948966,0,3.141592653589793) q[3];
u(1.1071487,0,0) q[3];
cx q[3],q[2];
u3(1.5707963267948966,0,3.141592653589793) q[2];
u(1.0471975511965976,0,0) q[2];
cx q[2],q[1];
u3(1.5707963267948966,0,3.141592653589793) q[1];
u(0.95531662,0,0) q[1];
cx q[1],q[0];
u3(1.5707963267948966,0,3.141592653589793) q[0];
u(0.7853981633974483,0,0) q[0];
cx q[9],q[10];
cx q[8],q[9];
cx q[7],q[8];
cx q[6],q[7];
cx q[5],q[6];
cx q[4],q[5];
cx q[3],q[4];
cx q[2],q[3];
cx q[1],q[2];
cx q[0],q[1];
