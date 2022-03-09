.globl start
.area CSEG(CODE)
msg:
    .str "@@@TAG#@@@"
    .db ' '
    .str "date: @@@DATE@@@"
start:
    sjmp .