; -*- mode:msc51; coding:utf-8 -*-
;;; 对C8051F 系列的单片机烧写，文档参考 C2spec.pdf
;;; Silicon Labs 公司的网页上能找到，
;;; 2015-07-08
;;; c2_data_read,c2_data_write,c2_addr_read,c2_addr_write have issues as below
;;; 等待反馈时，没有检测超时机制, 看pi_inbusy,应该只有这里会死循环
.include "c8051f320.h"
.globl start,framework

C2D			.equ 6
C2CK		.equ 7

OC2D		.equ p2+C2D
OC2CK		.equ p2+C2CK

BUTTON		.equ 5
BU			.equ p2+BUTTON
RED			.equ 2
RED_LED		.equ p2+RED
GREEN		.equ 3
GREEN_LED	.equ p2+GREEN

LED_MDOUT	.equ P2MDOUT
LED_MDIN	.equ P2MDIN
C2_MDIN		.equ P2MDIN
C2_MDOUT	.equ P2MDOUT


; P1.2 P1.3 / P0.6 P0.7
; SDA p0.6, SCL p0.7


FW_PAGES	.equ 1

; }}}

.area BSEG

.area DSEG
hi_addr:			.ds 1
fw_page:			.ds 1

.area CSEG(REL,CON)
	.str "@@@TAG#@@@"
	.db ' '
	.str "date: @@@DATE@@@"
start:
	mov		RSTSRC,#6
	; set Vdd Monitor Contorl
	mov		VDM0CN,#0x80

	clr		ea
	mov		sp,#0xcf
	mov		psw,#0

	acall	init
	acall	init_other
	acall	init_timer
	mov		XBR0,#0
	mov		XBR1,#0x40		; enabel crossbar and weakpud

;ajmp test_buttom_and_led
; ready for burn
	acall	delay_500ms
	setb	GREEN_LED
burn_ready:
	jnb		BU,burn_ready
1$:
	acall	push_button
	jnb		f0,1$
	clr		RED_LED


	cpl		GREEN_LED
	acall	delay_500ms
	cpl		GREEN_LED

	acall	c2_reset

	acall	pi_init
	acall	pi_erase_device
	acall	burn
	acall	verify_target

	setb	GREEN_LED
	sjmp	1$
; {{{ function push_button
push_button:
	clr		f0
	jnb		BU,0$
	ret
0$:
	setb	TR2
	jnb		BU,.
	jnb		TF2H,push_bad
	clr		TF2H
	setb	f0
push_bad:
	clr		TR2
	mov		TMR2H,#0
	ret
; }}}
; {{{ function verify_target
verify_target:
	mov		fw_page,#FW_PAGES
	mov		hi_addr,#0
0$:
	mov		a,fw_page
	clr		c
	subb	a,hi_addr
	jz		1$
	mov		dptr,#framework
	mov		a,dph
	add		a,hi_addr
	mov		dph,a
	mov		EMI0CN,#1
	acall	load256
	mov		EMI0CN,#0
	acall	pi_read256
	acall	compare
	inc		hi_addr
	cpl		GREEN_LED
	sjmp	0$
1$:
	setb	GREEN_LED
	ret
compare:
	mov		r0,#0
	mov		dptr,#0x0100
0$:
	movx	a,@dptr
	mov		r2,a
	movx	a,@r0
	clr		c
	subb	a,r2
	jnz		verify_error
	inc		r0
	inc		dptr
	mov		a,r0
	jnz		0$
	ret

verify_error:
	mov		sp,#0xcf
	setb	RED_LED
	acall	delay_500ms
	clr		RED_LED
	acall	delay_500ms
	sjmp	verify_error
; }}}
; {{{ function burn
burn:
	setb	GREEN_LED
	mov		fw_page,#FW_PAGES
	mov		hi_addr,#0
0$:
	mov		a,fw_page
	clr		c
	subb	a,hi_addr
	jz		1$
	mov		dptr,#framework
	mov		a,dph
	add		a,hi_addr
	mov		dph,a
	acall	load256
	acall	pi_write256
	inc		hi_addr
	cpl		GREEN_LED
	sjmp	0$
1$:
	setb	GREEN_LED
	ret
; }}}
; {{{ functions delay
delay_500ms:
	mov		r4,#9
1$:
	acall	delay_5000us
	djnz	r4,1$
delay_5000us:
	mov		r3,#249
1$:
	acall	delay_20us
	djnz	r3, 1$
delay_20us:
	mov		r2,#0
	djnz	r2, .
	ret
delay_2us:
	mov		r2,#25
	djnz	r2, .
	ret
; }}}
; {{{ function init,init_other
init:
	; set Internal oscillator
	; OSCICN [IOSCEN|IFRDY|SUSPEND|STSYNC|-|-|IFCN1|IFCN0]
	; IOSCEN 1:Internal High-Frequency Oscillator enabled
	; IFCN[1:0] 11:SYSCLK DERIVED FROM INTERNAL H-F OSCILLATOR divided by 1
	mov		OSCICN,#0x83
	; SET oscillator source, use internal H-F Oscilator
	mov		CLKSEL,#0

	; TURN OFF WATCHDOG
	; PCA0MD[CIDL|WDTE|WDLCK|-|CPS2|CPS1+CPS-|ECF]
	; WDTE was set to 0
	anl		PCA0MD,#0b10111111
	mov		a,#0xff
	mov		P0SKIP,a
	mov		P1SKIP,a
	mov		P2SKIP,a
	clr		a
	mov		p2,a
	ret
init_other:

	mov		a,#0xff				; all pin configured as digital port
;	setb	acc+C2CK			; is N/A for push-pull
	mov		C2_MDIN,a
	mov		a,#0xff - ((1<<C2D)+(1<<BUTTON))	; C2D and button set to open-drain
	mov		C2_MDOUT,a
	setb	OC2CK
	setb	BU				; set Port Latch for enable weak pull-up
	ret

init_timer:
	; T2 Timer use prescale clock and prescale clock is system clock divided by 12
	; T2MH 和后面 T3XCLK = 00 设定T2H 用sysclk/12
	; Don't care T0,T1,T3 timer
	clr		a
	mov		CKCON,a
	; T2SPLIT = 1 8bit mode
	; T2XCLK和前面的T2MH, T2高位用 sysclk/12
	mov		TMR2CN,#0b00001000

; T2低字节不能停止 。因此使用高字节
	mov		TMR2H,a
	mov		TMR2RLH,a
	mov		TMR2RLL,a
	clr		TR2
	; 因为T2L 无法控制停止。考虑用于烧写时的超时计算
	ret
; }}}
; C2 interface routines {{{
; r2 会被改变。 A 作为参数，或者结果
; {{{ function c2_strobe
c2_strobe:
	clr		OC2CK
	nop
	setb	OC2CK
	ret
; }}}
; {{{ function c2_data_write
c2_data_write:
	acall	c2_strobe		; start
	setb	OC2D
	acall	c2_strobe
	clr		OC2D
	acall	c2_strobe
	; length
;	clr		OC2D
	acall	c2_strobe
;	clr		OC2D
	acall	c2_strobe
	mov		r2,#8
1$:
	rrc		a
	mov		OC2D,c
	acall	c2_strobe
	djnz	r2,1$

	setb	OC2D		; PULL-UP
2$:
	acall	c2_strobe	; wait
	jnb		OC2D,2$

	acall	c2_strobe	; STOP
	ret
; }}}
; {{{ function c2_data_read
c2_data_read:
	acall	c2_strobe

	;ins
	clr		OC2D
	acall	c2_strobe
;	clr		OC2D
	acall	c2_strobe
	; length
	clr		OC2D
	acall	c2_strobe
;	clr		OC2D
	acall	c2_strobe

	setb	OC2D		;enable pull-up
1$:
	acall	c2_strobe

	jnb		OC2D,1$	; wait

	mov		r2,#8
2$:
	acall	c2_strobe
	mov		c,OC2D
	rrc		a
	djnz	r2,2$
	clr		OC2CK		; stop
	nop
	setb	OC2CK

	ret
; }}}
; {{{ function c2_addr_write
c2_addr_write:
	acall	c2_strobe		; frame start
	setb	OC2D
	acall	c2_strobe
;	setb	OC2D
	acall	c2_strobe
	mov		r2,#8
1$:
	rrc		a
	mov		OC2D,c
	acall	c2_strobe
	djnz	r2,1$
	clr		OC2CK		; STOP
	setb	OC2CK
	ret
; }}}
; {{{ function c2_addr_read
c2_addr_read:
	acall	c2_strobe
	clr		OC2D		; ins[0]=0
	acall	c2_strobe
	setb	OC2D		; ins[1]=1
	acall	c2_strobe
	mov		r2,#8
;	setb	OC2D		; for enable weak pull-up
1$:
	acall	c2_strobe
	mov		c,OC2D
	rrc		a
	djnz	r2, 1$
	clr		OC2D
	clr		OC2CK		; stop
	nop
	setb	OC2CK

	ret
; }}}
; {{{ function c2_reset
c2_reset:
	; reset
	clr		OC2D			; disable c2d?
	clr		OC2CK
	; switch to bank1
	setb	rs0
	acall	delay_20us		; T_rd
	setb	OC2CK
	;; acall	delay_2us			;T_sd
	clr		rs0
	ret
; }}}
; }}}
; {{{ PI routines
; {{{ function pi_inbusy
;;; 参考 AN127.PDF C2 Address Register Status Bits
;;; 进入死循环应该是这里。FIXME
pi_inbusy:
	acall	c2_addr_read
	jb		acc.1,pi_inbusy
	ret
; }}}
; {{{ function pi_outready
pi_outready:
	acall	c2_addr_read
	jnb		acc.0,pi_outready
	ret
; }}}
; {{{ function pi_init
;;; 在文档 AN127.PDF中要求要对地址输入 0x02,0x04,0x01,才能允许编程
;;; 在C8051Fxx 的文档中只要 0x02,0x01 就可以了
pi_init:
	mov		a,#2
	acall	c2_addr_write
	mov		a,#2
	acall	c2_data_write
	mov		a,#1
	acall	c2_data_write
;;; 文档中描述，如果进入了编程状态，就必须用一个系统RESET的方法，才能
;;; 让单片机回复到正常运行状态
;;; 文档AN127.PDF中描述需要20ms的延时。
	setb	rs0
	acall	delay_5000us
	clr		rs0

	ret
; }}}
; {{{ function pi_read256
; use variable "hi_addr" target address high byte,
pi_read256:
	mov		a,#0xb4
	acall	c2_addr_write
	mov		a,#6				; block read command
	acall	c2_data_write
	acall	pi_inbusy
	acall	result_check
	mov		a,hi_addr				; address high
	acall	c2_data_write
	acall	pi_inbusy

	clr		a					; address low
	acall	c2_data_write
	acall	pi_inbusy

	clr		a					; length of data
	acall	c2_data_write
	acall	pi_inbusy
	acall	result_check

	mov		r1,#0
2$:
	acall	c2_data_read
	movx	@r1,a
	inc		r1
	mov		a,r1
	jnz		2$
	ret
; }}}
; function pi_write256 {{{
; hi_addr target address high
pi_write256:
	mov		a,#0xb4
	acall	c2_addr_write
	mov		a,#7				; block write command
	acall	c2_data_write
	acall	pi_inbusy
	acall	result_check
	mov		a,hi_addr			; address high byte
	acall	c2_data_write
	acall	pi_inbusy
	clr		a					; address low byte
	acall	c2_data_write
	acall	pi_inbusy
	clr		a					; data length 256
	acall	c2_data_write
	acall	pi_inbusy
	mov		r1,#0
1$:
	movx	a,@r1
	acall	c2_data_write
	acall	pi_inbusy
	inc		r1
	mov		a,r1
	jnz		1$
	acall	pi_inbusy
	sjmp	result_check

; }}}
; {{{ function pi_erase_device
;;; an127.pdf page 23
pi_erase_device:
;;; FPDAT address
	mov		a,#0xb4
	acall	c2_addr_write
;;; Send Device Erase Command
	mov		a,#3				; erase device command
	acall	c2_data_write
	acall	pi_inbusy
;;; Read PI Command Status
	acall	result_check
;;; Write 1st Byte of the Arming Sequence
	mov		a,#0xde
	acall	c2_data_write
	acall	pi_inbusy
;;; Write 2nd Byte of the Arming Sequence
	mov		a,#0xad
	acall	c2_data_write
	acall	pi_inbusy
;;; Write 3rd Byte of the Arming Sequence
	mov		a,#0xa5
	acall	c2_data_write
	acall	pi_inbusy
;;; Read PI Command Status
	sjmp	result_check
; }}}
result_check:
	acall	pi_outready
	acall	c2_data_read
	cjne	a,#0x0d,1$	; error
	ret
1$:
	clr		GREEN_LED
	setb	RED_LED
	; 这里有些返回是从函数中直接跳回来。因此。需要重新复位一下堆栈
	mov		sp,#0xcf
	mov		psw,#0
	ajmp	burn_ready
; }}}
; {{{ function load256
load256:
	clr		a
	mov		r0,a
1$:
	movc	a,@a+dptr
	movx	@r0,a
	inc		r0
	mov		a,r0
	jnz		1$
	ret
; }}}

test_buttom_and_led:
	mov		c,BU
	mov		GREEN_LED,c
	cpl		c
	mov		RED_LED,c
	sjmp	test_buttom_and_led
test_data_read:
	acall	c2_reset
	acall	c2_data_read
	mov		0x30,a
	sjmp	.
test1:
	acall	c2_reset



