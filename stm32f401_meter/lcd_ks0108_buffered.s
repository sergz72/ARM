	.cpu cortex-m4
	.arch armv7e-m
	.fpu fpv4-sp-d16
	.eabi_attribute 27, 1
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"lcd_ks0108_buffered.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.file 1 "D:\\serg\\my\\Programs\\ARM_CLION\\common_lib\\display\\lcd_ks0108_buffered.c"
	.section	.bss.paint_buffer,"aw",%nobits
	.align	2
	.type	paint_buffer, %object
	.size	paint_buffer, 1024
paint_buffer:
	.space	1024
	.section	.bss.display_buffer,"aw",%nobits
	.align	2
	.type	display_buffer, %object
	.size	display_buffer, 1024
display_buffer:
	.space	1024
	.section	.text.LcdDelay,"ax",%progbits
	.align	1
	.global	LcdDelay
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdDelay, %function
LcdDelay:
.LFB133:
	.loc 1 39 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r7}
	.cfi_def_cfa_offset 4
	.cfi_offset 7, -4
	sub	sp, sp, #12
	.cfi_def_cfa_offset 16
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	.loc 1 40 16
	mov	r3, #500
	str	r3, [r7, #4]
	.loc 1 41 9
	nop
.L2:
	.loc 1 41 11 discriminator 1
	ldr	r3, [r7, #4]
	subs	r2, r3, #1
	str	r2, [r7, #4]
	.loc 1 41 10 discriminator 1
	cmp	r3, #0
	bne	.L2
	.loc 1 42 1
	nop
	nop
	adds	r7, r7, #12
	.cfi_def_cfa_offset 4
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	ldr	r7, [sp], #4
	.cfi_restore 7
	.cfi_def_cfa_offset 0
	bx	lr
	.cfi_endproc
.LFE133:
	.size	LcdDelay, .-LcdDelay
	.section	.text.LcdDelay2,"ax",%progbits
	.align	1
	.global	LcdDelay2
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdDelay2, %function
LcdDelay2:
.LFB134:
	.loc 1 45 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r7}
	.cfi_def_cfa_offset 4
	.cfi_offset 7, -4
	sub	sp, sp, #12
	.cfi_def_cfa_offset 16
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	.loc 1 46 16
	movw	r3, #5000
	str	r3, [r7, #4]
	.loc 1 47 9
	nop
.L4:
	.loc 1 47 11 discriminator 1
	ldr	r3, [r7, #4]
	subs	r2, r3, #1
	str	r2, [r7, #4]
	.loc 1 47 10 discriminator 1
	cmp	r3, #0
	bne	.L4
	.loc 1 48 1
	nop
	nop
	adds	r7, r7, #12
	.cfi_def_cfa_offset 4
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	ldr	r7, [sp], #4
	.cfi_restore 7
	.cfi_def_cfa_offset 0
	bx	lr
	.cfi_endproc
.LFE134:
	.size	LcdDelay2, .-LcdDelay2
	.section	.text.LcdClearScreen,"ax",%progbits
	.align	1
	.global	LcdClearScreen
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdClearScreen, %function
LcdClearScreen:
.LFB135:
	.loc 1 53 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #8
	.cfi_def_cfa_offset 16
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	.loc 1 58 2
	mov	r2, #1024
	movs	r1, #0
	ldr	r0, .L10
	bl	memset
	.loc 1 60 3
	mov	r2, #1024
	movs	r1, #0
	ldr	r0, .L10+4
	bl	memset
	.loc 1 62 9
	movs	r3, #0
	str	r3, [r7, #4]
	.loc 1 62 3
	b	.L6
.L9:
	.loc 1 64 5
	movs	r1, #24
	ldr	r0, [r7, #4]
	bl	LcdSetX
	.loc 1 65 5
	movs	r1, #24
	movs	r0, #0
	bl	LcdSetY
	.loc 1 67 11
	movs	r3, #0
	str	r3, [r7]
	.loc 1 67 5
	b	.L7
.L8:
	.loc 1 68 7 discriminator 3
	movs	r1, #24
	movs	r0, #0
	bl	LcdWriteData
	.loc 1 67 42 discriminator 3
	ldr	r3, [r7]
	adds	r3, r3, #1
	str	r3, [r7]
.L7:
	.loc 1 67 18 discriminator 1
	ldr	r3, [r7]
	cmp	r3, #63
	bls	.L8
	.loc 1 62 35 discriminator 2
	ldr	r3, [r7, #4]
	adds	r3, r3, #1
	str	r3, [r7, #4]
.L6:
	.loc 1 62 16 discriminator 1
	ldr	r3, [r7, #4]
	cmp	r3, #7
	bls	.L9
	.loc 1 71 1
	nop
	nop
	adds	r7, r7, #8
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
.L11:
	.align	2
.L10:
	.word	paint_buffer
	.word	display_buffer
	.cfi_endproc
.LFE135:
	.size	LcdClearScreen, .-LcdClearScreen
	.section	.text.LcdInit,"ax",%progbits
	.align	1
	.global	LcdInit
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdInit, %function
LcdInit:
.LFB136:
	.loc 1 78 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	.loc 1 80 3
	bl	lcd_init
	.loc 1 81 3
	bl	LcdDelay2
	.loc 1 82 3
	movs	r0, #32
	bl	LCD_CONTROL_SET
	.loc 1 88 3
	bl	LcdClearScreen
	.loc 1 91 3
	movs	r1, #24
	movs	r0, #0
	bl	LcdSetStartLine
	.loc 1 93 10
	movs	r3, #1
	.loc 1 94 1
	mov	r0, r3
	pop	{r7, pc}
	.cfi_endproc
.LFE136:
	.size	LcdInit, .-LcdInit
	.section	.text.LcdWait,"ax",%progbits
	.align	1
	.global	LcdWait
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdWait, %function
LcdWait:
.LFB137:
	.loc 1 99 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r7}
	.cfi_def_cfa_offset 4
	.cfi_offset 7, -4
	sub	sp, sp, #12
	.cfi_def_cfa_offset 16
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	.loc 1 100 16
	movs	r3, #50
	str	r3, [r7, #4]
	.loc 1 101 9
	nop
.L15:
	.loc 1 101 11 discriminator 1
	ldr	r3, [r7, #4]
	subs	r2, r3, #1
	str	r2, [r7, #4]
	.loc 1 101 10 discriminator 1
	cmp	r3, #0
	bne	.L15
	.loc 1 102 1
	nop
	nop
	adds	r7, r7, #12
	.cfi_def_cfa_offset 4
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	ldr	r7, [sp], #4
	.cfi_restore 7
	.cfi_def_cfa_offset 0
	bx	lr
	.cfi_endproc
.LFE137:
	.size	LcdWait, .-LcdWait
	.section	.text.LcdCommand,"ax",%progbits
	.align	1
	.global	LcdCommand
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdCommand, %function
LcdCommand:
.LFB138:
	.loc 1 106 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #8
	.cfi_def_cfa_offset 16
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	mov	r3, r0
	str	r1, [r7]
	strb	r3, [r7, #7]
	.loc 1 107 3
	bl	LcdWait
	.loc 1 108 3
	ldr	r3, [r7]
	mov	r0, r3
	bl	LCD_CONTROL_SET
	.loc 1 109 3
	movs	r0, #3
	bl	LCD_CONTROL_CLR
	.loc 1 110 3
	ldr	r3, .L17
	ldr	r3, [r3, #20]
	and	r2, r3, #65280
	ldrb	r3, [r7, #7]	@ zero_extendqisi2
	ldr	r1, .L17
	orrs	r3, r3, r2
	str	r3, [r1, #20]
	.loc 1 111 3
	movs	r0, #4
	bl	LCD_CONTROL_SET
	.loc 1 112 3
	bl	LcdDelay
	.loc 1 113 3
	movs	r0, #4
	bl	LCD_CONTROL_CLR
	.loc 1 114 3
	bl	LcdDelay
	.loc 1 116 3
	ldr	r3, [r7]
	mov	r0, r3
	bl	LCD_CONTROL_CLR
	.loc 1 117 1
	nop
	adds	r7, r7, #8
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
.L18:
	.align	2
.L17:
	.word	1073872896
	.cfi_endproc
.LFE138:
	.size	LcdCommand, .-LcdCommand
	.section	.text.LcdWriteData,"ax",%progbits
	.align	1
	.global	LcdWriteData
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdWriteData, %function
LcdWriteData:
.LFB139:
	.loc 1 120 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #8
	.cfi_def_cfa_offset 16
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	mov	r3, r0
	str	r1, [r7]
	strb	r3, [r7, #7]
	.loc 1 121 3
	bl	LcdWait
	.loc 1 122 25
	ldr	r3, [r7]
	orr	r3, r3, #1
	.loc 1 122 3
	mov	r0, r3
	bl	LCD_CONTROL_SET
	.loc 1 123 3
	movs	r0, #2
	bl	LCD_CONTROL_CLR
	.loc 1 124 3
	ldr	r3, .L20
	ldr	r3, [r3, #20]
	and	r2, r3, #65280
	ldrb	r3, [r7, #7]	@ zero_extendqisi2
	ldr	r1, .L20
	orrs	r3, r3, r2
	str	r3, [r1, #20]
	.loc 1 125 3
	movs	r0, #4
	bl	LCD_CONTROL_SET
	.loc 1 126 3
	bl	LcdDelay
	.loc 1 127 3
	movs	r0, #4
	bl	LCD_CONTROL_CLR
	.loc 1 128 3
	bl	LcdDelay
	.loc 1 130 3
	ldr	r3, [r7]
	mov	r0, r3
	bl	LCD_CONTROL_CLR
	.loc 1 131 1
	nop
	adds	r7, r7, #8
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
.L21:
	.align	2
.L20:
	.word	1073872896
	.cfi_endproc
.LFE139:
	.size	LcdWriteData, .-LcdWriteData
	.section	.text.LcdOn,"ax",%progbits
	.align	1
	.global	LcdOn
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdOn, %function
LcdOn:
.LFB140:
	.loc 1 134 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	.loc 1 135 3
	movs	r1, #24
	movs	r0, #63
	bl	LcdCommand
	.loc 1 136 1
	nop
	pop	{r7, pc}
	.cfi_endproc
.LFE140:
	.size	LcdOn, .-LcdOn
	.section	.text.LcdOff,"ax",%progbits
	.align	1
	.global	LcdOff
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdOff, %function
LcdOff:
.LFB141:
	.loc 1 139 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	.loc 1 140 3
	movs	r1, #24
	movs	r0, #62
	bl	LcdCommand
	.loc 1 141 1
	nop
	pop	{r7, pc}
	.cfi_endproc
.LFE141:
	.size	LcdOff, .-LcdOff
	.section	.text.LcdSetY,"ax",%progbits
	.align	1
	.global	LcdSetY
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdSetY, %function
LcdSetY:
.LFB142:
	.loc 1 144 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #8
	.cfi_def_cfa_offset 16
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	str	r0, [r7, #4]
	str	r1, [r7]
	.loc 1 145 21
	ldr	r3, [r7, #4]
	uxtb	r3, r3
	and	r3, r3, #63
	uxtb	r3, r3
	.loc 1 145 3
	adds	r3, r3, #64
	uxtb	r3, r3
	ldr	r1, [r7]
	mov	r0, r3
	bl	LcdCommand
	.loc 1 146 1
	nop
	adds	r7, r7, #8
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
	.cfi_endproc
.LFE142:
	.size	LcdSetY, .-LcdSetY
	.section	.text.LcdSetX,"ax",%progbits
	.align	1
	.global	LcdSetX
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdSetX, %function
LcdSetX:
.LFB143:
	.loc 1 149 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #8
	.cfi_def_cfa_offset 16
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	str	r0, [r7, #4]
	str	r1, [r7]
	.loc 1 150 21
	ldr	r3, [r7, #4]
	uxtb	r3, r3
	and	r3, r3, #7
	uxtb	r3, r3
	.loc 1 150 3
	subs	r3, r3, #72
	uxtb	r3, r3
	ldr	r1, [r7]
	mov	r0, r3
	bl	LcdCommand
	.loc 1 151 1
	nop
	adds	r7, r7, #8
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
	.cfi_endproc
.LFE143:
	.size	LcdSetX, .-LcdSetX
	.section	.text.LcdSetStartLine,"ax",%progbits
	.align	1
	.global	LcdSetStartLine
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdSetStartLine, %function
LcdSetStartLine:
.LFB144:
	.loc 1 154 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #8
	.cfi_def_cfa_offset 16
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	str	r0, [r7, #4]
	str	r1, [r7]
	.loc 1 155 24
	ldr	r3, [r7, #4]
	uxtb	r3, r3
	and	r3, r3, #63
	uxtb	r3, r3
	.loc 1 155 3
	subs	r3, r3, #64
	uxtb	r3, r3
	ldr	r1, [r7]
	mov	r0, r3
	bl	LcdCommand
	.loc 1 156 1
	nop
	adds	r7, r7, #8
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
	.cfi_endproc
.LFE144:
	.size	LcdSetStartLine, .-LcdSetStartLine
	.section	.text.GetDataAddress,"ax",%progbits
	.align	1
	.global	GetDataAddress
	.syntax unified
	.thumb
	.thumb_func
	.type	GetDataAddress, %function
GetDataAddress:
.LFB145:
	.loc 1 160 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r7}
	.cfi_def_cfa_offset 4
	.cfi_offset 7, -4
	sub	sp, sp, #12
	.cfi_def_cfa_offset 16
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	str	r0, [r7, #4]
	str	r1, [r7]
	.loc 1 161 10
	ldr	r3, [r7, #4]
	lsrs	r3, r3, #3
	lsls	r2, r3, #7
	ldr	r3, [r7]
	add	r3, r3, r2
	ldr	r2, .L29
	add	r3, r3, r2
	.loc 1 162 1
	mov	r0, r3
	adds	r7, r7, #12
	.cfi_def_cfa_offset 4
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	ldr	r7, [sp], #4
	.cfi_restore 7
	.cfi_def_cfa_offset 0
	bx	lr
.L30:
	.align	2
.L29:
	.word	paint_buffer
	.cfi_endproc
.LFE145:
	.size	GetDataAddress, .-GetDataAddress
	.section	.text.internalDrawPixel,"ax",%progbits
	.align	1
	.global	internalDrawPixel
	.syntax unified
	.thumb
	.thumb_func
	.type	internalDrawPixel, %function
internalDrawPixel:
.LFB146:
	.loc 1 165 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #24
	.cfi_def_cfa_offset 32
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	str	r0, [r7, #12]
	str	r1, [r7, #8]
	str	r2, [r7, #4]
	.loc 1 166 16
	ldr	r3, [r7, #8]
	and	r3, r3, #7
	str	r3, [r7, #20]
	.loc 1 167 28
	ldr	r1, [r7, #12]
	ldr	r0, [r7, #8]
	bl	GetDataAddress
	str	r0, [r7, #16]
	.loc 1 168 6
	ldr	r3, [r7, #4]
	cmp	r3, #0
	beq	.L32
	.loc 1 169 5
	ldr	r3, [r7, #16]
	ldrb	r3, [r3]	@ zero_extendqisi2
	sxtb	r2, r3
	.loc 1 169 21
	movs	r1, #1
	ldr	r3, [r7, #20]
	lsl	r3, r1, r3
	.loc 1 169 14
	sxtb	r3, r3
	mvns	r3, r3
	sxtb	r3, r3
	ands	r3, r3, r2
	sxtb	r3, r3
	uxtb	r2, r3
	ldr	r3, [r7, #16]
	strb	r2, [r3]
	.loc 1 172 1
	b	.L34
.L32:
	.loc 1 171 5
	ldr	r3, [r7, #16]
	ldrb	r3, [r3]	@ zero_extendqisi2
	sxtb	r2, r3
	.loc 1 171 19
	movs	r1, #1
	ldr	r3, [r7, #20]
	lsl	r3, r1, r3
	.loc 1 171 14
	sxtb	r3, r3
	orrs	r3, r3, r2
	sxtb	r3, r3
	uxtb	r2, r3
	ldr	r3, [r7, #16]
	strb	r2, [r3]
.L34:
	.loc 1 172 1
	nop
	adds	r7, r7, #24
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
	.cfi_endproc
.LFE146:
	.size	internalDrawPixel, .-internalDrawPixel
	.section	.text.LcdPixelDraw,"ax",%progbits
	.align	1
	.global	LcdPixelDraw
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdPixelDraw, %function
LcdPixelDraw:
.LFB147:
	.loc 1 175 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #16
	.cfi_def_cfa_offset 24
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	str	r0, [r7, #12]
	str	r1, [r7, #8]
	str	r2, [r7, #4]
	.loc 1 176 3
	ldr	r2, [r7, #4]
	ldr	r1, [r7, #8]
	ldr	r0, [r7, #12]
	bl	internalDrawPixel
	.loc 1 177 1
	nop
	adds	r7, r7, #16
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
	.cfi_endproc
.LFE147:
	.size	LcdPixelDraw, .-LcdPixelDraw
	.section	.text.LcdUpdate,"ax",%progbits
	.align	1
	.global	LcdUpdate
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdUpdate, %function
LcdUpdate:
.LFB148:
	.loc 1 180 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #24
	.cfi_def_cfa_offset 32
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	.loc 1 185 8
	ldr	r3, .L52
	str	r3, [r7, #12]
	.loc 1 186 9
	ldr	r3, .L52+4
	str	r3, [r7, #8]
	.loc 1 187 9
	movs	r3, #0
	str	r3, [r7, #20]
	.loc 1 187 3
	b	.L37
.L50:
	.loc 1 189 17
	movs	r3, #1
	strb	r3, [r7, #7]
	.loc 1 189 10
	ldrb	r3, [r7, #7]
	strb	r3, [r7, #6]
	.loc 1 190 11
	movs	r3, #0
	str	r3, [r7, #16]
	.loc 1 190 5
	b	.L38
.L43:
	.loc 1 192 11
	ldr	r3, [r7, #12]
	ldrb	r2, [r3]	@ zero_extendqisi2
	.loc 1 192 20
	ldr	r3, [r7, #8]
	ldrb	r3, [r3]	@ zero_extendqisi2
	.loc 1 192 10
	cmp	r2, r3
	beq	.L39
	.loc 1 194 12
	ldrb	r3, [r7, #7]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L40
	.loc 1 196 14
	ldrb	r3, [r7, #6]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L41
	.loc 1 198 13
	movs	r1, #8
	ldr	r0, [r7, #20]
	bl	LcdSetX
	.loc 1 199 18
	movs	r3, #0
	strb	r3, [r7, #6]
.L41:
	.loc 1 201 11
	movs	r1, #8
	ldr	r0, [r7, #16]
	bl	LcdSetY
	.loc 1 202 16
	movs	r3, #0
	strb	r3, [r7, #7]
.L40:
	.loc 1 204 9
	ldr	r3, [r7, #12]
	ldrb	r3, [r3]	@ zero_extendqisi2
	movs	r1, #8
	mov	r0, r3
	bl	LcdWriteData
	b	.L42
.L39:
	.loc 1 207 14
	movs	r3, #1
	strb	r3, [r7, #7]
.L42:
	.loc 1 208 11 discriminator 2
	ldr	r3, [r7, #12]
	adds	r3, r3, #1
	str	r3, [r7, #12]
	.loc 1 209 12 discriminator 2
	ldr	r3, [r7, #8]
	adds	r3, r3, #1
	str	r3, [r7, #8]
	.loc 1 190 42 discriminator 2
	ldr	r3, [r7, #16]
	adds	r3, r3, #1
	str	r3, [r7, #16]
.L38:
	.loc 1 190 18 discriminator 1
	ldr	r3, [r7, #16]
	cmp	r3, #63
	bls	.L43
	.loc 1 212 10
	movs	r3, #1
	strb	r3, [r7, #7]
	.loc 1 213 11
	movs	r3, #0
	str	r3, [r7, #16]
	.loc 1 213 5
	b	.L44
.L49:
	.loc 1 215 11
	ldr	r3, [r7, #12]
	ldrb	r2, [r3]	@ zero_extendqisi2
	.loc 1 215 20
	ldr	r3, [r7, #8]
	ldrb	r3, [r3]	@ zero_extendqisi2
	.loc 1 215 10
	cmp	r2, r3
	beq	.L45
	.loc 1 217 12
	ldrb	r3, [r7, #7]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L46
	.loc 1 219 14
	ldrb	r3, [r7, #6]	@ zero_extendqisi2
	cmp	r3, #0
	beq	.L47
	.loc 1 221 13
	movs	r1, #16
	ldr	r0, [r7, #20]
	bl	LcdSetX
	.loc 1 222 18
	movs	r3, #0
	strb	r3, [r7, #6]
.L47:
	.loc 1 224 11
	movs	r1, #16
	ldr	r0, [r7, #16]
	bl	LcdSetY
	.loc 1 225 16
	movs	r3, #0
	strb	r3, [r7, #7]
.L46:
	.loc 1 227 9
	ldr	r3, [r7, #12]
	ldrb	r3, [r3]	@ zero_extendqisi2
	movs	r1, #16
	mov	r0, r3
	bl	LcdWriteData
	b	.L48
.L45:
	.loc 1 230 14
	movs	r3, #1
	strb	r3, [r7, #7]
.L48:
	.loc 1 231 11 discriminator 2
	ldr	r3, [r7, #12]
	adds	r3, r3, #1
	str	r3, [r7, #12]
	.loc 1 232 12 discriminator 2
	ldr	r3, [r7, #8]
	adds	r3, r3, #1
	str	r3, [r7, #8]
	.loc 1 213 42 discriminator 2
	ldr	r3, [r7, #16]
	adds	r3, r3, #1
	str	r3, [r7, #16]
.L44:
	.loc 1 213 18 discriminator 1
	ldr	r3, [r7, #16]
	cmp	r3, #63
	bls	.L49
	.loc 1 187 35 discriminator 2
	ldr	r3, [r7, #20]
	adds	r3, r3, #1
	str	r3, [r7, #20]
.L37:
	.loc 1 187 16 discriminator 1
	ldr	r3, [r7, #20]
	cmp	r3, #7
	bls	.L50
	.loc 1 236 2
	ldr	r2, .L52+4
	ldr	r3, .L52
	mov	r0, r2
	mov	r1, r3
	mov	r3, #1024
	mov	r2, r3
	bl	memcpy
	.loc 1 241 10
	movs	r3, #1
	.loc 1 242 1
	mov	r0, r3
	adds	r7, r7, #24
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
.L53:
	.align	2
.L52:
	.word	paint_buffer
	.word	display_buffer
	.cfi_endproc
.LFE148:
	.size	LcdUpdate, .-LcdUpdate
	.section	.text.LcdLineDrawH,"ax",%progbits
	.align	1
	.global	LcdLineDrawH
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdLineDrawH, %function
LcdLineDrawH:
.LFB149:
	.loc 1 275 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #16
	.cfi_def_cfa_offset 24
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	str	r0, [r7, #12]
	str	r1, [r7, #8]
	str	r2, [r7, #4]
	str	r3, [r7]
	.loc 1 276 9
	b	.L55
.L56:
	.loc 1 277 5
	ldr	r3, [r7, #12]
	adds	r2, r3, #1
	str	r2, [r7, #12]
	ldr	r2, [r7]
	ldr	r1, [r7, #4]
	mov	r0, r3
	bl	internalDrawPixel
.L55:
	.loc 1 276 14
	ldr	r2, [r7, #12]
	ldr	r3, [r7, #8]
	cmp	r2, r3
	bls	.L56
	.loc 1 278 1
	nop
	nop
	adds	r7, r7, #16
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
	.cfi_endproc
.LFE149:
	.size	LcdLineDrawH, .-LcdLineDrawH
	.section	.text.LcdLineDrawV,"ax",%progbits
	.align	1
	.global	LcdLineDrawV
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdLineDrawV, %function
LcdLineDrawV:
.LFB150:
	.loc 1 281 1
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #16
	.cfi_def_cfa_offset 24
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	str	r0, [r7, #12]
	str	r1, [r7, #8]
	str	r2, [r7, #4]
	str	r3, [r7]
	.loc 1 282 9
	b	.L58
.L59:
	.loc 1 283 5
	ldr	r3, [r7, #8]
	adds	r2, r3, #1
	str	r2, [r7, #8]
	ldr	r2, [r7]
	mov	r1, r3
	ldr	r0, [r7, #12]
	bl	internalDrawPixel
.L58:
	.loc 1 282 14
	ldr	r2, [r7, #8]
	ldr	r3, [r7, #4]
	cmp	r2, r3
	bls	.L59
	.loc 1 284 1
	nop
	nop
	adds	r7, r7, #16
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
	.cfi_endproc
.LFE150:
	.size	LcdLineDrawV, .-LcdLineDrawV
	.section	.text.LcdRectangle,"ax",%progbits
	.align	1
	.global	LcdRectangle
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdRectangle, %function
LcdRectangle:
.LFB151:
	.loc 1 287 1
	.cfi_startproc
	@ args = 8, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #24
	.cfi_def_cfa_offset 32
	add	r7, sp, #0
	.cfi_def_cfa_register 7
	str	r0, [r7, #12]
	str	r1, [r7, #8]
	str	r2, [r7, #4]
	str	r3, [r7]
	.loc 1 290 22
	ldr	r3, [r7, #36]
	lsrs	r3, r3, #1
	.loc 1 290 10
	ldr	r2, [r7, #12]
	subs	r3, r2, r3
	str	r3, [r7, #12]
	.loc 1 291 19
	ldr	r3, [r7, #36]
	lsrs	r3, r3, #1
	.loc 1 291 7
	ldr	r2, [r7, #8]
	subs	r3, r2, r3
	str	r3, [r7, #8]
	.loc 1 293 10
	movs	r3, #0
	str	r3, [r7, #20]
	.loc 1 293 3
	b	.L61
.L62:
	.loc 1 295 33 discriminator 3
	ldr	r2, [r7, #12]
	ldr	r3, [r7, #4]
	add	r3, r3, r2
	.loc 1 295 5 discriminator 3
	subs	r1, r3, #1
	ldr	r2, [r7, #8]
	ldr	r3, [r7, #20]
	add	r2, r2, r3
	ldr	r3, [r7, #32]
	ldr	r0, [r7, #12]
	bl	LcdLineDrawH
	.loc 1 296 33 discriminator 3
	ldr	r2, [r7, #12]
	ldr	r3, [r7, #4]
	add	r3, r3, r2
	.loc 1 296 5 discriminator 3
	subs	r1, r3, #1
	.loc 1 296 47 discriminator 3
	ldr	r2, [r7, #8]
	ldr	r3, [r7]
	add	r2, r2, r3
	.loc 1 296 56 discriminator 3
	ldr	r3, [r7, #20]
	add	r3, r3, r2
	.loc 1 296 5 discriminator 3
	subs	r2, r3, #1
	ldr	r3, [r7, #32]
	ldr	r0, [r7, #12]
	bl	LcdLineDrawH
	.loc 1 297 5 discriminator 3
	ldr	r2, [r7, #12]
	ldr	r3, [r7, #20]
	adds	r0, r2, r3
	.loc 1 297 39 discriminator 3
	ldr	r2, [r7, #8]
	ldr	r3, [r7]
	add	r3, r3, r2
	.loc 1 297 5 discriminator 3
	subs	r2, r3, #1
	ldr	r3, [r7, #32]
	ldr	r1, [r7, #8]
	bl	LcdLineDrawV
	.loc 1 298 25 discriminator 3
	ldr	r2, [r7, #12]
	ldr	r3, [r7, #4]
	add	r2, r2, r3
	.loc 1 298 34 discriminator 3
	ldr	r3, [r7, #20]
	add	r3, r3, r2
	.loc 1 298 5 discriminator 3
	subs	r0, r3, #1
	.loc 1 298 48 discriminator 3
	ldr	r2, [r7, #8]
	ldr	r3, [r7]
	add	r3, r3, r2
	.loc 1 298 5 discriminator 3
	subs	r2, r3, #1
	ldr	r3, [r7, #32]
	ldr	r1, [r7, #8]
	bl	LcdLineDrawV
	.loc 1 293 30 discriminator 3
	ldr	r3, [r7, #20]
	adds	r3, r3, #1
	str	r3, [r7, #20]
.L61:
	.loc 1 293 17 discriminator 1
	ldr	r2, [r7, #20]
	ldr	r3, [r7, #36]
	cmp	r2, r3
	bcc	.L62
	.loc 1 300 1
	nop
	nop
	adds	r7, r7, #24
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
	.cfi_endproc
.LFE151:
	.size	LcdRectangle, .-LcdRectangle
	.section	.text.LcdPrintf,"ax",%progbits
	.align	1
	.global	LcdPrintf
	.syntax unified
	.thumb
	.thumb_func
	.type	LcdPrintf, %function
LcdPrintf:
.LFB152:
	.loc 1 303 1
	.cfi_startproc
	@ args = 4, pretend = 0, frame = 32
	@ frame_needed = 1, uses_anonymous_args = 1
	push	{r7, lr}
	.cfi_def_cfa_offset 8
	.cfi_offset 7, -8
	.cfi_offset 14, -4
	sub	sp, sp, #48
	.cfi_def_cfa_offset 56
	add	r7, sp, #16
	.cfi_def_cfa 7, 40
	str	r0, [r7, #12]
	str	r1, [r7, #8]
	str	r2, [r7, #4]
	str	r3, [r7]
	.loc 1 308 3
	add	r3, r7, #44
	str	r3, [r7, #20]
	.loc 1 310 3
	ldr	r3, [r7, #20]
	ldr	r2, [r7, #12]
	movs	r1, #40
	ldr	r0, .L64
	bl	snprintf
	.loc 1 315 44
	ldr	r3, [r7, #40]
	cmp	r3, #0
	ite	ne
	movne	r3, #1
	moveq	r3, #0
	uxtb	r3, r3
	.loc 1 315 13
	str	r3, [r7, #28]
	.loc 1 316 42
	ldr	r3, [r7, #40]
	cmp	r3, #0
	ite	eq
	moveq	r3, #1
	movne	r3, #0
	uxtb	r3, r3
	.loc 1 316 11
	str	r3, [r7, #24]
	.loc 1 317 3
	ldr	r3, [r7, #28]
	ldr	r2, [r7, #24]
	movs	r1, #0
	str	r1, [sp, #8]
	str	r2, [sp, #4]
	str	r3, [sp]
	ldr	r3, [r7]
	ldr	r2, .L64
	ldr	r1, [r7, #4]
	ldr	r0, [r7, #8]
	bl	LcdDrawText
	.loc 1 318 1
	nop
	adds	r7, r7, #32
	.cfi_def_cfa_offset 8
	mov	sp, r7
	.cfi_def_cfa_register 13
	@ sp needed
	pop	{r7, pc}
.L65:
	.align	2
.L64:
	.word	buffer.0
	.cfi_endproc
.LFE152:
	.size	LcdPrintf, .-LcdPrintf
	.section	.bss.buffer.0,"aw",%nobits
	.align	2
	.type	buffer.0, %object
	.size	buffer.0, 40
buffer.0:
	.space	40
	.text
.Letext0:
	.file 2 "d:\\serg\\my\\programs\\arm_clion\\common_lib\\display\\font.h"
	.file 3 "c:\\armgcc\\arm-none-eabi\\include\\machine\\_default_types.h"
	.file 4 "c:\\armgcc\\arm-none-eabi\\include\\sys\\_stdint.h"
	.file 5 "D:\\serg\\my\\Programs\\ARM_CLION\\stm32f401_meter\\cmsis\\device/stm32f401xe.h"
	.file 6 "c:\\armgcc\\lib\\gcc\\arm-none-eabi\\12.2.1\\include\\stdarg.h"
	.file 7 "c:\\armgcc\\lib\\gcc\\arm-none-eabi\\12.2.1\\include\\stddef.h"
	.file 8 "d:\\serg\\my\\programs\\arm_clion\\common_lib\\display\\lcd.h"
	.file 9 "c:\\armgcc\\arm-none-eabi\\include\\stdio.h"
	.file 10 "D:\\serg\\my\\Programs\\ARM_CLION\\stm32f401_meter\\src/board.h"
	.file 11 "c:\\armgcc\\arm-none-eabi\\include\\string.h"
	.file 12 "<built-in>"
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0x7b7
	.2byte	0x5
	.byte	0x1
	.byte	0x4
	.4byte	.Ldebug_abbrev0
	.uleb128 0x1b
	.4byte	.LASF76
	.byte	0xc
	.4byte	.LASF77
	.4byte	.LASF78
	.4byte	.LLRL0
	.4byte	0
	.4byte	.Ldebug_line0
	.uleb128 0x13
	.byte	0x4
	.byte	0x8
	.4byte	0x46
	.uleb128 0x4
	.4byte	.LASF0
	.byte	0x9
	.byte	0x11
	.4byte	0x46
	.byte	0
	.uleb128 0x4
	.4byte	.LASF1
	.byte	0xa
	.byte	0x16
	.4byte	0x52
	.byte	0x2
	.byte	0
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.4byte	.LASF2
	.uleb128 0xe
	.4byte	0x46
	.uleb128 0x3
	.byte	0x2
	.byte	0x7
	.4byte	.LASF3
	.uleb128 0xa
	.4byte	.LASF12
	.byte	0x2
	.byte	0xb
	.byte	0x3
	.4byte	0x26
	.uleb128 0xe
	.4byte	0x59
	.uleb128 0x13
	.byte	0x10
	.byte	0xd
	.4byte	0xc6
	.uleb128 0x4
	.4byte	.LASF4
	.byte	0xe
	.byte	0x11
	.4byte	0x46
	.byte	0
	.uleb128 0x4
	.4byte	.LASF5
	.byte	0xf
	.byte	0x8
	.4byte	0xc6
	.byte	0x1
	.uleb128 0x4
	.4byte	.LASF6
	.byte	0x10
	.byte	0x11
	.4byte	0x46
	.byte	0x2
	.uleb128 0x4
	.4byte	.LASF7
	.byte	0x11
	.byte	0x11
	.4byte	0x46
	.byte	0x3
	.uleb128 0x4
	.4byte	.LASF8
	.byte	0x12
	.byte	0x11
	.4byte	0x46
	.byte	0x4
	.uleb128 0x4
	.4byte	.LASF9
	.byte	0x13
	.byte	0x19
	.4byte	0xd2
	.byte	0x8
	.uleb128 0x4
	.4byte	.LASF10
	.byte	0x14
	.byte	0x18
	.4byte	0xd7
	.byte	0xc
	.byte	0
	.uleb128 0x3
	.byte	0x1
	.byte	0x8
	.4byte	.LASF11
	.uleb128 0xe
	.4byte	0xc6
	.uleb128 0xb
	.4byte	0x65
	.uleb128 0xb
	.4byte	0x4d
	.uleb128 0xa
	.4byte	.LASF13
	.byte	0x2
	.byte	0x15
	.byte	0x3
	.4byte	0x6a
	.uleb128 0xe
	.4byte	0xdc
	.uleb128 0x3
	.byte	0x1
	.byte	0x6
	.4byte	.LASF14
	.uleb128 0x3
	.byte	0x2
	.byte	0x5
	.4byte	.LASF15
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.4byte	.LASF16
	.uleb128 0xa
	.4byte	.LASF17
	.byte	0x3
	.byte	0x4f
	.byte	0x19
	.4byte	0x10e
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.4byte	.LASF18
	.uleb128 0x3
	.byte	0x8
	.byte	0x5
	.4byte	.LASF19
	.uleb128 0x3
	.byte	0x8
	.byte	0x7
	.4byte	.LASF20
	.uleb128 0x1c
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x3
	.byte	0x4
	.byte	0x7
	.4byte	.LASF21
	.uleb128 0xa
	.4byte	.LASF22
	.byte	0x4
	.byte	0x30
	.byte	0x14
	.4byte	0x102
	.uleb128 0x14
	.4byte	0x131
	.uleb128 0x1d
	.byte	0x28
	.byte	0x5
	.2byte	0x108
	.byte	0x9
	.4byte	0x1b9
	.uleb128 0xd
	.4byte	.LASF23
	.2byte	0x10a
	.4byte	0x13d
	.byte	0
	.uleb128 0xd
	.4byte	.LASF24
	.2byte	0x10b
	.4byte	0x13d
	.byte	0x4
	.uleb128 0xd
	.4byte	.LASF25
	.2byte	0x10c
	.4byte	0x13d
	.byte	0x8
	.uleb128 0xd
	.4byte	.LASF26
	.2byte	0x10d
	.4byte	0x13d
	.byte	0xc
	.uleb128 0x11
	.ascii	"IDR\000"
	.2byte	0x10e
	.4byte	0x13d
	.byte	0x10
	.uleb128 0x11
	.ascii	"ODR\000"
	.2byte	0x10f
	.4byte	0x13d
	.byte	0x14
	.uleb128 0xd
	.4byte	.LASF27
	.2byte	0x110
	.4byte	0x13d
	.byte	0x18
	.uleb128 0xd
	.4byte	.LASF28
	.2byte	0x111
	.4byte	0x13d
	.byte	0x1c
	.uleb128 0x11
	.ascii	"AFR\000"
	.2byte	0x112
	.4byte	0x1c9
	.byte	0x20
	.byte	0
	.uleb128 0x12
	.4byte	0x13d
	.4byte	0x1c9
	.uleb128 0x15
	.4byte	0x12a
	.byte	0x1
	.byte	0
	.uleb128 0x14
	.4byte	0x1b9
	.uleb128 0x1e
	.4byte	.LASF29
	.byte	0x5
	.2byte	0x113
	.byte	0x3
	.4byte	0x142
	.uleb128 0xa
	.4byte	.LASF30
	.byte	0x6
	.byte	0x28
	.byte	0x1b
	.4byte	0x1e7
	.uleb128 0x1f
	.4byte	.LASF79
	.byte	0x4
	.byte	0xc
	.byte	0
	.4byte	0x1fe
	.uleb128 0x20
	.4byte	.LASF80
	.4byte	0x1fe
	.byte	0
	.byte	0
	.uleb128 0x21
	.byte	0x4
	.uleb128 0xa
	.4byte	.LASF31
	.byte	0x6
	.byte	0x63
	.byte	0x18
	.4byte	0x1db
	.uleb128 0xa
	.4byte	.LASF32
	.byte	0x7
	.byte	0xd6
	.byte	0x17
	.4byte	0x12a
	.uleb128 0xb
	.4byte	0x46
	.uleb128 0xb
	.4byte	0xc6
	.uleb128 0xb
	.4byte	0xcd
	.uleb128 0x12
	.4byte	0x46
	.4byte	0x238
	.uleb128 0x22
	.4byte	0x12a
	.2byte	0x3ff
	.byte	0
	.uleb128 0x7
	.4byte	.LASF33
	.byte	0x1d
	.byte	0x16
	.4byte	0x227
	.uleb128 0x5
	.byte	0x3
	.4byte	paint_buffer
	.uleb128 0x7
	.4byte	.LASF34
	.byte	0x1f
	.byte	0x16
	.4byte	0x227
	.uleb128 0x5
	.byte	0x3
	.4byte	display_buffer
	.uleb128 0x16
	.4byte	.LASF35
	.byte	0x8
	.byte	0x10
	.byte	0xe
	.4byte	0x12a
	.4byte	0x28e
	.uleb128 0x1
	.4byte	0x12a
	.uleb128 0x1
	.4byte	0x12a
	.uleb128 0x1
	.4byte	0x21d
	.uleb128 0x1
	.4byte	0x28e
	.uleb128 0x1
	.4byte	0x12a
	.uleb128 0x1
	.4byte	0x12a
	.uleb128 0x1
	.4byte	0x293
	.byte	0
	.uleb128 0xb
	.4byte	0xe8
	.uleb128 0xb
	.4byte	0x12a
	.uleb128 0x23
	.4byte	.LASF36
	.byte	0x9
	.2byte	0x110
	.byte	0x5
	.4byte	0x123
	.4byte	0x2ba
	.uleb128 0x1
	.4byte	0x21d
	.uleb128 0x1
	.4byte	0x20c
	.uleb128 0x1
	.4byte	0x222
	.uleb128 0x17
	.byte	0
	.uleb128 0x18
	.4byte	.LASF37
	.byte	0x31
	.4byte	0x2ca
	.uleb128 0x1
	.4byte	0x123
	.byte	0
	.uleb128 0x18
	.4byte	.LASF38
	.byte	0x30
	.4byte	0x2da
	.uleb128 0x1
	.4byte	0x123
	.byte	0
	.uleb128 0x24
	.4byte	.LASF81
	.byte	0xa
	.byte	0x32
	.byte	0x6
	.uleb128 0x16
	.4byte	.LASF39
	.byte	0xb
	.byte	0x21
	.byte	0x9
	.4byte	0x1fe
	.4byte	0x302
	.uleb128 0x1
	.4byte	0x1fe
	.uleb128 0x1
	.4byte	0x123
	.uleb128 0x1
	.4byte	0x20c
	.byte	0
	.uleb128 0xf
	.4byte	.LASF47
	.2byte	0x12e
	.4byte	.LFB152
	.4byte	.LFE152-.LFB152
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x3a1
	.uleb128 0x8
	.4byte	.LASF40
	.2byte	0x12e
	.byte	0x1c
	.4byte	0x222
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x8
	.4byte	.LASF41
	.2byte	0x12e
	.byte	0x31
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x2
	.ascii	"row\000"
	.2byte	0x12e
	.byte	0x46
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -36
	.uleb128 0x2
	.ascii	"f\000"
	.2byte	0x12e
	.byte	0x5c
	.4byte	0x28e
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x8
	.4byte	.LASF42
	.2byte	0x12e
	.byte	0x63
	.4byte	0x123
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x17
	.uleb128 0x10
	.4byte	.LASF43
	.2byte	0x130
	.byte	0xf
	.4byte	0x3a1
	.uleb128 0x5
	.byte	0x3
	.4byte	buffer.0
	.uleb128 0x10
	.4byte	.LASF44
	.2byte	0x131
	.byte	0xb
	.4byte	0x200
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x10
	.4byte	.LASF45
	.2byte	0x132
	.byte	0x7
	.4byte	0x123
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x10
	.4byte	.LASF46
	.2byte	0x132
	.byte	0x12
	.4byte	0x123
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x12
	.4byte	0xc6
	.4byte	0x3b1
	.uleb128 0x15
	.4byte	0x12a
	.byte	0x27
	.byte	0
	.uleb128 0xf
	.4byte	.LASF48
	.2byte	0x11e
	.4byte	.LFB151
	.4byte	.LFE151-.LFB151
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x42d
	.uleb128 0x8
	.4byte	.LASF41
	.2byte	0x11e
	.byte	0x20
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x2
	.ascii	"row\000"
	.2byte	0x11e
	.byte	0x35
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x2
	.ascii	"dx\000"
	.2byte	0x11e
	.byte	0x47
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x2
	.ascii	"dy\000"
	.2byte	0x11e
	.byte	0x58
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x8
	.4byte	.LASF49
	.2byte	0x11e
	.byte	0x60
	.4byte	0x123
	.uleb128 0x2
	.byte	0x91
	.sleb128 0
	.uleb128 0x8
	.4byte	.LASF50
	.2byte	0x11e
	.byte	0x74
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 4
	.uleb128 0x25
	.ascii	"i\000"
	.byte	0x1
	.2byte	0x120
	.byte	0x10
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.byte	0
	.uleb128 0xf
	.4byte	.LASF51
	.2byte	0x118
	.4byte	.LFB150
	.4byte	.LFE150-.LFB150
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x47e
	.uleb128 0x2
	.ascii	"lX\000"
	.2byte	0x118
	.byte	0x20
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x2
	.ascii	"lY1\000"
	.2byte	0x118
	.byte	0x31
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x2
	.ascii	"lY2\000"
	.2byte	0x118
	.byte	0x43
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x8
	.4byte	.LASF49
	.2byte	0x118
	.byte	0x4c
	.4byte	0x123
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0xf
	.4byte	.LASF52
	.2byte	0x112
	.4byte	.LFB149
	.4byte	.LFE149-.LFB149
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x4cf
	.uleb128 0x2
	.ascii	"lX1\000"
	.2byte	0x112
	.byte	0x20
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x2
	.ascii	"lX2\000"
	.2byte	0x112
	.byte	0x32
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x2
	.ascii	"lY\000"
	.2byte	0x112
	.byte	0x44
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x8
	.4byte	.LASF49
	.2byte	0x112
	.byte	0x4c
	.4byte	0x123
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.uleb128 0x26
	.4byte	.LASF62
	.byte	0x1
	.byte	0xb3
	.byte	0xe
	.4byte	0x12a
	.4byte	.LFB148
	.4byte	.LFE148-.LFB148
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x53a
	.uleb128 0xc
	.ascii	"x\000"
	.byte	0xb6
	.byte	0x10
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0xc
	.ascii	"y\000"
	.byte	0xb6
	.byte	0x13
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x7
	.4byte	.LASF53
	.byte	0xb7
	.byte	0x11
	.4byte	0x218
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x7
	.4byte	.LASF54
	.byte	0xb7
	.byte	0x18
	.4byte	0x218
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x7
	.4byte	.LASF55
	.byte	0xb7
	.byte	0x1f
	.4byte	0x46
	.uleb128 0x2
	.byte	0x91
	.sleb128 -25
	.uleb128 0x7
	.4byte	.LASF56
	.byte	0xb7
	.byte	0x25
	.4byte	0x46
	.uleb128 0x2
	.byte	0x91
	.sleb128 -26
	.byte	0
	.uleb128 0x9
	.4byte	.LASF57
	.byte	0xae
	.4byte	.LFB147
	.4byte	.LFE147-.LFB147
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x577
	.uleb128 0x5
	.ascii	"lX\000"
	.byte	0xae
	.byte	0x20
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x5
	.ascii	"lY\000"
	.byte	0xae
	.byte	0x31
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x6
	.4byte	.LASF58
	.byte	0xae
	.byte	0x42
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.byte	0
	.uleb128 0x9
	.4byte	.LASF59
	.byte	0xa4
	.4byte	.LFB146
	.4byte	.LFE146-.LFB146
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x5ce
	.uleb128 0x5
	.ascii	"x\000"
	.byte	0xa4
	.byte	0x25
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x5
	.ascii	"y\000"
	.byte	0xa4
	.byte	0x35
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x6
	.4byte	.LASF45
	.byte	0xa4
	.byte	0x45
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x7
	.4byte	.LASF60
	.byte	0xa6
	.byte	0x10
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x7
	.4byte	.LASF61
	.byte	0xa7
	.byte	0x12
	.4byte	0x218
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x27
	.4byte	.LASF63
	.byte	0x1
	.byte	0x9f
	.byte	0x10
	.4byte	0x218
	.4byte	.LFB145
	.4byte	.LFE145-.LFB145
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x605
	.uleb128 0x5
	.ascii	"row\000"
	.byte	0x9f
	.byte	0x2c
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x5
	.ascii	"col\000"
	.byte	0x9f
	.byte	0x3e
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x9
	.4byte	.LASF64
	.byte	0x99
	.4byte	.LFB144
	.4byte	.LFE144-.LFB144
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x636
	.uleb128 0x6
	.4byte	.LASF65
	.byte	0x99
	.byte	0x23
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x6
	.4byte	.LASF66
	.byte	0x99
	.byte	0x36
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x9
	.4byte	.LASF67
	.byte	0x94
	.4byte	.LFB143
	.4byte	.LFE143-.LFB143
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x665
	.uleb128 0x5
	.ascii	"X\000"
	.byte	0x94
	.byte	0x1b
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x6
	.4byte	.LASF66
	.byte	0x94
	.byte	0x2b
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x9
	.4byte	.LASF68
	.byte	0x8f
	.4byte	.LFB142
	.4byte	.LFE142-.LFB142
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x694
	.uleb128 0x5
	.ascii	"Y\000"
	.byte	0x8f
	.byte	0x1b
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0x6
	.4byte	.LASF66
	.byte	0x8f
	.byte	0x2b
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x19
	.4byte	.LASF69
	.byte	0x8a
	.4byte	.LFB141
	.4byte	.LFE141-.LFB141
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x19
	.4byte	.LASF70
	.byte	0x85
	.4byte	.LFB140
	.4byte	.LFE140-.LFB140
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x9
	.4byte	.LASF71
	.byte	0x77
	.4byte	.LFB139
	.4byte	.LFE139-.LFB139
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x6e5
	.uleb128 0x6
	.4byte	.LASF53
	.byte	0x77
	.byte	0x21
	.4byte	0x46
	.uleb128 0x2
	.byte	0x91
	.sleb128 -9
	.uleb128 0x6
	.4byte	.LASF66
	.byte	0x77
	.byte	0x34
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x9
	.4byte	.LASF72
	.byte	0x69
	.4byte	.LFB138
	.4byte	.LFE138-.LFB138
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x716
	.uleb128 0x5
	.ascii	"cmd\000"
	.byte	0x69
	.byte	0x1f
	.4byte	0x46
	.uleb128 0x2
	.byte	0x91
	.sleb128 -9
	.uleb128 0x6
	.4byte	.LASF66
	.byte	0x69
	.byte	0x31
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x1a
	.4byte	.LASF73
	.byte	0x62
	.4byte	.LFB137
	.4byte	.LFE137-.LFB137
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x737
	.uleb128 0xc
	.ascii	"d\000"
	.byte	0x64
	.byte	0x10
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.byte	0
	.uleb128 0x28
	.4byte	.LASF82
	.byte	0x1
	.byte	0x4d
	.byte	0xe
	.4byte	0x12a
	.4byte	.LFB136
	.4byte	.LFE136-.LFB136
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x9
	.4byte	.LASF74
	.byte	0x34
	.4byte	.LFB135
	.4byte	.LFE135-.LFB135
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x77a
	.uleb128 0xc
	.ascii	"x\000"
	.byte	0x37
	.byte	0x10
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.uleb128 0xc
	.ascii	"y\000"
	.byte	0x37
	.byte	0x13
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x1a
	.4byte	.LASF75
	.byte	0x2c
	.4byte	.LFB134
	.4byte	.LFE134-.LFB134
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x79b
	.uleb128 0xc
	.ascii	"d\000"
	.byte	0x2e
	.byte	0x10
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.byte	0
	.uleb128 0x29
	.4byte	.LASF83
	.byte	0x1
	.byte	0x26
	.byte	0x6
	.4byte	.LFB133
	.4byte	.LFE133-.LFB133
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0xc
	.ascii	"d\000"
	.byte	0x28
	.byte	0x10
	.4byte	0x12a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 2
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0x21
	.sleb128 4
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 5
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 17
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 5
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 17
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 2
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 9
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x35
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x18
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 10
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7a
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x55
	.uleb128 0x17
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.uleb128 0x34
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x26
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x27
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7a
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x6
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7a
	.uleb128 0x19
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",%progbits
	.4byte	0xb4
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	.LFB133
	.4byte	.LFE133-.LFB133
	.4byte	.LFB134
	.4byte	.LFE134-.LFB134
	.4byte	.LFB135
	.4byte	.LFE135-.LFB135
	.4byte	.LFB136
	.4byte	.LFE136-.LFB136
	.4byte	.LFB137
	.4byte	.LFE137-.LFB137
	.4byte	.LFB138
	.4byte	.LFE138-.LFB138
	.4byte	.LFB139
	.4byte	.LFE139-.LFB139
	.4byte	.LFB140
	.4byte	.LFE140-.LFB140
	.4byte	.LFB141
	.4byte	.LFE141-.LFB141
	.4byte	.LFB142
	.4byte	.LFE142-.LFB142
	.4byte	.LFB143
	.4byte	.LFE143-.LFB143
	.4byte	.LFB144
	.4byte	.LFE144-.LFB144
	.4byte	.LFB145
	.4byte	.LFE145-.LFB145
	.4byte	.LFB146
	.4byte	.LFE146-.LFB146
	.4byte	.LFB147
	.4byte	.LFE147-.LFB147
	.4byte	.LFB148
	.4byte	.LFE148-.LFB148
	.4byte	.LFB149
	.4byte	.LFE149-.LFB149
	.4byte	.LFB150
	.4byte	.LFE150-.LFB150
	.4byte	.LFB151
	.4byte	.LFE151-.LFB151
	.4byte	.LFB152
	.4byte	.LFE152-.LFB152
	.4byte	0
	.4byte	0
	.section	.debug_rnglists,"",%progbits
.Ldebug_ranges0:
	.4byte	.Ldebug_ranges3-.Ldebug_ranges2
.Ldebug_ranges2:
	.2byte	0x5
	.byte	0x4
	.byte	0
	.4byte	0
.LLRL0:
	.byte	0x7
	.4byte	.LFB133
	.uleb128 .LFE133-.LFB133
	.byte	0x7
	.4byte	.LFB134
	.uleb128 .LFE134-.LFB134
	.byte	0x7
	.4byte	.LFB135
	.uleb128 .LFE135-.LFB135
	.byte	0x7
	.4byte	.LFB136
	.uleb128 .LFE136-.LFB136
	.byte	0x7
	.4byte	.LFB137
	.uleb128 .LFE137-.LFB137
	.byte	0x7
	.4byte	.LFB138
	.uleb128 .LFE138-.LFB138
	.byte	0x7
	.4byte	.LFB139
	.uleb128 .LFE139-.LFB139
	.byte	0x7
	.4byte	.LFB140
	.uleb128 .LFE140-.LFB140
	.byte	0x7
	.4byte	.LFB141
	.uleb128 .LFE141-.LFB141
	.byte	0x7
	.4byte	.LFB142
	.uleb128 .LFE142-.LFB142
	.byte	0x7
	.4byte	.LFB143
	.uleb128 .LFE143-.LFB143
	.byte	0x7
	.4byte	.LFB144
	.uleb128 .LFE144-.LFB144
	.byte	0x7
	.4byte	.LFB145
	.uleb128 .LFE145-.LFB145
	.byte	0x7
	.4byte	.LFB146
	.uleb128 .LFE146-.LFB146
	.byte	0x7
	.4byte	.LFB147
	.uleb128 .LFE147-.LFB147
	.byte	0x7
	.4byte	.LFB148
	.uleb128 .LFE148-.LFB148
	.byte	0x7
	.4byte	.LFB149
	.uleb128 .LFE149-.LFB149
	.byte	0x7
	.4byte	.LFB150
	.uleb128 .LFE150-.LFB150
	.byte	0x7
	.4byte	.LFB151
	.uleb128 .LFE151-.LFB151
	.byte	0x7
	.4byte	.LFB152
	.uleb128 .LFE152-.LFB152
	.byte	0
.Ldebug_ranges3:
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF29:
	.ascii	"GPIO_TypeDef\000"
.LASF57:
	.ascii	"LcdPixelDraw\000"
.LASF0:
	.ascii	"char_width\000"
.LASF35:
	.ascii	"LcdDrawText\000"
.LASF41:
	.ascii	"column\000"
.LASF15:
	.ascii	"short int\000"
.LASF32:
	.ascii	"size_t\000"
.LASF81:
	.ascii	"lcd_init\000"
.LASF37:
	.ascii	"LCD_CONTROL_CLR\000"
.LASF71:
	.ascii	"LcdWriteData\000"
.LASF45:
	.ascii	"textColor\000"
.LASF19:
	.ascii	"long long int\000"
.LASF17:
	.ascii	"__uint32_t\000"
.LASF73:
	.ascii	"LcdWait\000"
.LASF31:
	.ascii	"va_list\000"
.LASF66:
	.ascii	"crystal_id\000"
.LASF9:
	.ascii	"char_info\000"
.LASF12:
	.ascii	"FONT_CHAR_INFO\000"
.LASF65:
	.ascii	"line\000"
.LASF68:
	.ascii	"LcdSetY\000"
.LASF24:
	.ascii	"OTYPER\000"
.LASF46:
	.ascii	"bkColor\000"
.LASF36:
	.ascii	"snprintf\000"
.LASF25:
	.ascii	"OSPEEDR\000"
.LASF40:
	.ascii	"format\000"
.LASF20:
	.ascii	"long long unsigned int\000"
.LASF75:
	.ascii	"LcdDelay2\000"
.LASF26:
	.ascii	"PUPDR\000"
.LASF79:
	.ascii	"__va_list\000"
.LASF39:
	.ascii	"memset\000"
.LASF16:
	.ascii	"long int\000"
.LASF60:
	.ascii	"offset\000"
.LASF27:
	.ascii	"BSRR\000"
.LASF23:
	.ascii	"MODER\000"
.LASF47:
	.ascii	"LcdPrintf\000"
.LASF59:
	.ascii	"internalDrawPixel\000"
.LASF69:
	.ascii	"LcdOff\000"
.LASF13:
	.ascii	"FONT_INFO\000"
.LASF76:
	.ascii	"GNU C99 12.2.1 20221205 -mcpu=cortex-m4 -mfloat-abi"
	.ascii	"=hard -mfpu=fpv4-sp-d16 -mthumb -march=armv7e-m+fp "
	.ascii	"-g -std=gnu99 -ffunction-sections -fdata-sections\000"
.LASF78:
	.ascii	"D:\\serg\\my\\Programs\\ARM_CLION\\stm32f401_meter\000"
.LASF58:
	.ascii	"Value\000"
.LASF64:
	.ascii	"LcdSetStartLine\000"
.LASF2:
	.ascii	"unsigned char\000"
.LASF14:
	.ascii	"signed char\000"
.LASF61:
	.ascii	"address\000"
.LASF22:
	.ascii	"uint32_t\000"
.LASF44:
	.ascii	"vArgs\000"
.LASF34:
	.ascii	"display_buffer\000"
.LASF49:
	.ascii	"color\000"
.LASF18:
	.ascii	"long unsigned int\000"
.LASF51:
	.ascii	"LcdLineDrawV\000"
.LASF38:
	.ascii	"LCD_CONTROL_SET\000"
.LASF3:
	.ascii	"short unsigned int\000"
.LASF1:
	.ascii	"char_offset\000"
.LASF80:
	.ascii	"__ap\000"
.LASF11:
	.ascii	"char\000"
.LASF74:
	.ascii	"LcdClearScreen\000"
.LASF77:
	.ascii	"D:\\serg\\my\\Programs\\ARM_CLION\\common_lib\\disp"
	.ascii	"lay\\lcd_ks0108_buffered.c\000"
.LASF33:
	.ascii	"paint_buffer\000"
.LASF53:
	.ascii	"data\000"
.LASF63:
	.ascii	"GetDataAddress\000"
.LASF83:
	.ascii	"LcdDelay\000"
.LASF43:
	.ascii	"buffer\000"
.LASF56:
	.ascii	"setx\000"
.LASF55:
	.ascii	"sety\000"
.LASF50:
	.ascii	"PenWidth\000"
.LASF5:
	.ascii	"start_character\000"
.LASF70:
	.ascii	"LcdOn\000"
.LASF8:
	.ascii	"character_spacing\000"
.LASF72:
	.ascii	"LcdCommand\000"
.LASF67:
	.ascii	"LcdSetX\000"
.LASF82:
	.ascii	"LcdInit\000"
.LASF10:
	.ascii	"char_bitmaps\000"
.LASF30:
	.ascii	"__gnuc_va_list\000"
.LASF62:
	.ascii	"LcdUpdate\000"
.LASF7:
	.ascii	"character_max_width\000"
.LASF6:
	.ascii	"character_count\000"
.LASF48:
	.ascii	"LcdRectangle\000"
.LASF54:
	.ascii	"data2\000"
.LASF21:
	.ascii	"unsigned int\000"
.LASF4:
	.ascii	"char_height\000"
.LASF28:
	.ascii	"LCKR\000"
.LASF42:
	.ascii	"white_on_black\000"
.LASF52:
	.ascii	"LcdLineDrawH\000"
	.ident	"GCC: (Arm GNU Toolchain 12.2.Rel1 (Build arm-12.24)) 12.2.1 20221205"
