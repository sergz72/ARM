#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "swim_hal.h"

#define CVR_OFFSET 8
#define CSR_OFFSET 0
#define RVR_OFFSET 4

.syntax unified
.cpu cortex-m0plus
.thumb

.section .text
.global swim_packet_handler
swim_packet_handler:
ldr  r1,=swim_packet_handler_ram+1
bx   r1

.section .time_critical

tmo    .req r9
getv   .req r8
setv   .req r7
sio    .req r6
syst   .req r5
pkt    .req r4
pktend .req r3
table  .req r2

// clr parameter is in r0
swim_packet_handler_ram:
push {r4, r5, r6, r7, lr}
mov  r1,getv
mov  r2,tmo
push {r1, r2}
ldr  r1,get_value
mov  getv,r1
ldr  r1,timeout
mov  tmo,r1
ldr  setv,set_value
ldr  sio,sio_hwp
ldr  syst,systick_hw
ldr  pkt,=swim_packet
//swim_enter_critical_section();
ldr  r1,enter
blx  r1
ldr  pktend,=swim_packet_end
ldr  pktend,[pktend]
ldr  table,=jump_table
//SWIM_CLR(clr);
str  r0,[sio, SIO_GPIO_OUT_CLR_OFFSET]
//SWIM_TIME_COUNTER_START;
ldr  r0,systick_counter_max
str  r0,[syst, RVR_OFFSET]
movs r0, #0
str  r0,[syst, CVR_OFFSET]
movs r0, #5
str  r0,[syst, CSR_OFFSET]
loop:
// load next counter value from packet
ldr  r0,[pkt]
time_wait:
ldr  r1,[syst, CVR_OFFSET]
cmp  r1, r0
bhi  time_wait
//load operation id from packet
ldr  r0,[pkt, #4]
ldr  r0,[table,r0]
bx   r0
swim_wait0:
push {r2}
mov  r1,tmo
mov  r2,getv
wait0:
subs r1,#1
beq  timeout_exit
ldr  r0,[sio, SIO_GPIO_IN_OFFSET]
ands r0,r2
bne  wait0
//reset systick counter
str  r0,[syst, CVR_OFFSET]
pop  {r2}
cont:
adds pkt, #8
cmp  pkt,pktend
blo  loop
exit:
//SWIM_TIME_COUNTER_STOP;
movs r0, #0
str  r0,[syst, CSR_OFFSET]
//swim_leave_critical_section();
ldr  r0,leave
blx  r0
pop  {r1, r2}
mov  getv,r1
mov  tmo,r2
pop  {r4, r5, r6, r7, pc}

swim_set:
str  setv,[sio, SIO_GPIO_OUT_SET_OFFSET]
b    cont

swim_clr:
str  setv,[sio, SIO_GPIO_OUT_CLR_OFFSET]
b    cont

swim_get:
ldr  r0,[sio, SIO_GPIO_IN_OFFSET]
str  r0,[pkt, #4]
b    cont

timeout_exit:
pop  {r2}
movs r0, #0
str  r0,[pkt, #4]
b    exit

.align 4

jump_table:
.word swim_set+1
.word swim_clr+1
.word swim_get+1
.word cont+1
.word swim_wait0+1
enter:
.word swim_enter_critical_section+1
leave:
.word swim_leave_critical_section+1
systick_hw:
.word 0xE000E010
sio_hwp:
.word SIO_BASE
systick_counter_max:
.word SWIM_TIME_COUNTER_MAX
set_value:
.word SWIM_SET_VALUE
get_value:
.word 1<<SWIM_IN_PIN
timeout:
.word 125*20
