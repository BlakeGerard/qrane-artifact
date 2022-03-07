OPENQASM 2.0;
include "qelib1.inc";
qreg carry[2];
qreg a[8];
qreg b[8];
creg ans[8];
creg carryout[1];
u(3.141592653589793,0,3.141592653589793) a[0];
u(3.141592653589793,0,3.141592653589793) b[0];
cx a[0],b[0];
cx a[0],carry[0];
h a[0];
cx b[0],a[0];
tdg a[0];
cx carry[0],a[0];
t a[0];
cx b[0],a[0];
tdg a[0];
cx carry[0],a[0];
t a[0];
h a[0];
t b[0];
cx carry[0],b[0];
t carry[0];
tdg b[0];
cx carry[0],b[0];
u(3.141592653589793,0,3.141592653589793) b[1];
cx a[1],b[1];
cx a[1],a[0];
h a[1];
cx b[1],a[1];
tdg a[1];
cx a[0],a[1];
t a[1];
cx b[1],a[1];
tdg a[1];
cx a[0],a[1];
t a[1];
h a[1];
t b[1];
cx a[0],b[1];
t a[0];
tdg b[1];
cx a[0],b[1];
u(3.141592653589793,0,3.141592653589793) b[2];
cx a[2],b[2];
cx a[2],a[1];
h a[2];
cx b[2],a[2];
tdg a[2];
cx a[1],a[2];
t a[2];
cx b[2],a[2];
tdg a[2];
cx a[1],a[2];
t a[2];
h a[2];
t b[2];
cx a[1],b[2];
t a[1];
tdg b[2];
cx a[1],b[2];
u(3.141592653589793,0,3.141592653589793) b[3];
cx a[3],b[3];
cx a[3],a[2];
h a[3];
cx b[3],a[3];
tdg a[3];
cx a[2],a[3];
t a[3];
cx b[3],a[3];
tdg a[3];
cx a[2],a[3];
t a[3];
h a[3];
cx a[3],carry[1];
h a[3];
t b[3];
cx a[2],b[3];
t a[2];
tdg b[3];
cx a[2],b[3];
cx b[3],a[3];
tdg a[3];
cx a[2],a[3];
t a[3];
cx b[3],a[3];
tdg a[3];
cx a[2],a[3];
t a[3];
h a[3];
t b[3];
cx a[2],b[3];
t a[2];
tdg b[3];
cx a[2],b[3];
cx a[3],a[2];
cx a[2],b[3];
h a[2];
cx b[2],a[2];
tdg a[2];
cx a[1],a[2];
t a[2];
cx b[2],a[2];
tdg a[2];
cx a[1],a[2];
t a[2];
h a[2];
t b[2];
cx a[1],b[2];
t a[1];
tdg b[2];
cx a[1],b[2];
cx a[2],a[1];
cx a[1],b[2];
h a[1];
cx b[1],a[1];
tdg a[1];
cx a[0],a[1];
t a[1];
cx b[1],a[1];
tdg a[1];
cx a[0],a[1];
t a[1];
h a[1];
t b[1];
cx a[0],b[1];
t a[0];
tdg b[1];
cx a[0],b[1];
cx a[1],a[0];
cx a[0],b[1];
h a[0];
cx b[0],a[0];
tdg a[0];
cx carry[0],a[0];
t a[0];
cx b[0],a[0];
tdg a[0];
cx carry[0],a[0];
t a[0];
h a[0];
t b[0];
cx carry[0],b[0];
t carry[0];
tdg b[0];
cx carry[0],b[0];
cx a[0],carry[0];
cx carry[0],b[0];
u(3.141592653589793,0,3.141592653589793) b[4];
cx a[4],b[4];
cx a[4],carry[1];
h a[4];
cx b[4],a[4];
tdg a[4];
cx carry[1],a[4];
t a[4];
cx b[4],a[4];
tdg a[4];
cx carry[1],a[4];
t a[4];
h a[4];
t b[4];
cx carry[1],b[4];
t carry[1];
tdg b[4];
cx carry[1],b[4];
u(3.141592653589793,0,3.141592653589793) b[5];
cx a[5],b[5];
cx a[5],a[4];
h a[5];
cx b[5],a[5];
tdg a[5];
cx a[4],a[5];
t a[5];
cx b[5],a[5];
tdg a[5];
cx a[4],a[5];
t a[5];
h a[5];
t b[5];
cx a[4],b[5];
t a[4];
tdg b[5];
cx a[4],b[5];
u(3.141592653589793,0,3.141592653589793) b[6];
u(3.141592653589793,0,3.141592653589793) b[6];
cx a[6],b[6];
cx a[6],a[5];
h a[6];
cx b[6],a[6];
tdg a[6];
cx a[5],a[6];
t a[6];
cx b[6],a[6];
tdg a[6];
cx a[5],a[6];
t a[6];
h a[6];
t b[6];
cx a[5],b[6];
t a[5];
tdg b[6];
cx a[5],b[6];
u(3.141592653589793,0,3.141592653589793) b[7];
cx a[7],b[7];
cx a[7],a[6];
h a[7];
cx b[7],a[7];
tdg a[7];
cx a[6],a[7];
t a[7];
cx b[7],a[7];
tdg a[7];
cx a[6],a[7];
t a[7];
h a[7];
cx a[7],carry[0];
h a[7];
t b[7];
cx a[6],b[7];
t a[6];
tdg b[7];
cx a[6],b[7];
cx b[7],a[7];
tdg a[7];
cx a[6],a[7];
t a[7];
cx b[7],a[7];
tdg a[7];
cx a[6],a[7];
t a[7];
h a[7];
t b[7];
cx a[6],b[7];
t a[6];
tdg b[7];
cx a[6],b[7];
cx a[7],a[6];
cx a[6],b[7];
h a[6];
cx b[6],a[6];
tdg a[6];
cx a[5],a[6];
t a[6];
cx b[6],a[6];
tdg a[6];
cx a[5],a[6];
t a[6];
h a[6];
t b[6];
cx a[5],b[6];
t a[5];
tdg b[6];
cx a[5],b[6];
cx a[6],a[5];
cx a[5],b[6];
h a[5];
cx b[5],a[5];
tdg a[5];
cx a[4],a[5];
t a[5];
cx b[5],a[5];
tdg a[5];
cx a[4],a[5];
t a[5];
h a[5];
t b[5];
cx a[4],b[5];
t a[4];
tdg b[5];
cx a[4],b[5];
cx a[5],a[4];
cx a[4],b[5];
h a[4];
cx b[4],a[4];
tdg a[4];
cx carry[1],a[4];
t a[4];
cx b[4],a[4];
tdg a[4];
cx carry[1],a[4];
t a[4];
h a[4];
t b[4];
cx carry[1],b[4];
t carry[1];
tdg b[4];
cx carry[1],b[4];
cx a[4],carry[1];
cx carry[1],b[4];