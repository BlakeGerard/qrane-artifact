OPENQASM 2.0;
include "qelib1.inc";
qreg q[8];
creg m6[1];
creg m0[1];
creg m3[1];
creg m1[1];
creg m2[1];
creg m4[1];
creg m5[1];
creg m7[1];
u(3.141592653589793,0,3.141592653589793) q[0];
u(1.5707963267948966,0,3.141592653589793) q[1];
u(1.5707963267948966,0,3.141592653589793) q[1];
u(3.141592653589793,0,3.141592653589793) q[2];
u(1.5707963267948966,0,3.141592653589793) q[2];
u(3.141592653589793,0,3.141592653589793) q[3];
u(3.141592653589793,0,3.141592653589793) q[4];
u(1.5707963267948966,0,3.141592653589793) q[4];
u(3.141592653589793,0,3.141592653589793) q[5];
u(1.5707963267948966,0,3.141592653589793) q[5];
u(1.5707963267948966,0,3.141592653589793) q[7];
u(1.5707963267948966,0,3.141592653589793) q[7];
measure q[6] -> m6[0];
u(1.5707963267948966,0,3.141592653589793) q[6];
measure q[6] -> m6[0];
measure q[0] -> m0[0];
u(3.141592653589793,0,3.141592653589793) q[0];
measure q[0] -> m0[0];
measure q[3] -> m3[0];
u(3.141592653589793,0,3.141592653589793) q[3];
u(1.5707963267948966,0,3.141592653589793) q[3];
measure q[3] -> m3[0];
measure q[1] -> m1[0];
u(1.5707963267948966,0,3.141592653589793) q[1];
u(1.5707963267948966,0,3.141592653589793) q[1];
measure q[1] -> m1[0];
measure q[2] -> m2[0];
u(3.141592653589793,0,3.141592653589793) q[2];
u(1.5707963267948966,0,3.141592653589793) q[2];
u(1.5707963267948966,0,3.141592653589793) q[2];
measure q[2] -> m2[0];
measure q[4] -> m4[0];
u(3.141592653589793,0,3.141592653589793) q[4];
u(1.5707963267948966,0,3.141592653589793) q[4];
u(1.5707963267948966,0,3.141592653589793) q[4];
measure q[4] -> m4[0];
measure q[5] -> m5[0];
u(1.5707963267948966,0,3.141592653589793) q[5];
measure q[5] -> m5[0];
measure q[7] -> m7[0];
u(1.5707963267948966,0,3.141592653589793) q[7];
u(1.5707963267948966,0,3.141592653589793) q[7];
measure q[7] -> m7[0];
