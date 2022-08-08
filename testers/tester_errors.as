;Tester file for different types of errors detected in the first stage
entry LOOP
.entry LENGTH NONSENSE
.extern L3 NONSENSE
.extern W

.data
.entry
.extern

MAIN: mov S1.1, W (NONSENSE ENDING)
		add r2, STR
LOOP:	jmp W
		prn INVALID_ADDR
		sub r1, r4
		inc K

		mov S1.3, r3
		bne L3
END: 	aliens here?
STR:	.string 5555
LENGTH:.data 6,-9, "not a number"
K:		.data 22 123123123123
S1:		.struct 8,"ab"

.data 423.23

S2: .struct -123, "NONSTRING!

just nonsense line
more nonsense

;correct comment (skipped as blank line)

;wrong addressing:
hlt STR
mov #34, #43
