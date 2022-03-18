; 需要烧录的固件程序放在此文件
; 问题:
;    1 bndry 没有起作用，
;    2 如何保证framework一定在程序的最后？
;        链接时，把这个文件放最后

.globl framework

.area CSEG
.bndry 0x100
framework:
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"
.str "0123456789ABCDEF"