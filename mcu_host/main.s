;;; 对C8051F 系列的单片机烧写，文档参考 C2spec.pdf
;;; Silicon Labs 公司的网页上能找到，
;;; 2015-07-08
;;; c2_data_read,c2_data_write,c2_addr_read,c2_addr_write have issues as below
;;; 等待反馈时，没有检测超时机制, 看pi_inbusy,应该只有这里会死循环
.include "c8051f320.h"
.globl init_start


.area VECT(ABS,OVR)
.org 0
    ljmp	init_start
.org 0x03
    reti	;ljmp	ext0_int
.org  0x0b
    reti	;ljmp    t0_overflow_int
.org 0x13
    reti	;	ljmp    ext1_int
.org 0x1b
    reti	;	ljmp	t1_overflow_int
.org 0x23
    reti	;	ljmp	usart0_int
.org 0x2b
    reti	;ljmp	t2_overflow_int
.org 0x33
    reti	;ljmp	spi0_int
.org 0x3b
    reti	;ljmp	smb0_int
.org 0x43
    reti	;ljmp	usb0_int
.org 0x4b
    reti	;ljmp	adc0_win_int
.org 0x53
    reti	;ljmp	adc0_cc_int
.org 0x5b
    reti	;ljmp	pca_int
.org 0x63
    reti	;ljmp	acompare0_int
.org 0x6b
    reti	;ljmp	acompare1_int
.org 0x73
    reti	;ljmp	t3_overflow_int
.org 0x7b
    reti	;	ljmp	vbus_level_int
