.text
.syntax   unified

.align 4
.global conv2d_asm
.arm

conv2d_asm:
//load data

vldr       s13, [r0];
vldr       s14, [r1];
vldr       s15, [r2];
pld		   [r1, #256]
pld		   [r2, #256]
//MAC operation
vmul.f32   s14, s15, s14;
vadd.f32   s13, s14, s13;

//store data
vstr      s13, [r0];

//return
bx         lr;
