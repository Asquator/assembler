;Tester file with mixed correct and wrong code detected in 2nd stage
;Included tests for correct macro expansion

;Wrong structure member in macro m1
macro m1
	mov sct.3, var

endmacro

macro incvar2
inc var
inc var

endmacro

m1
m1
m1

;Invalid case
inc VAR

incvar2
incvar2

.entry NONEXISTENT

;Invalid register number
add x, r8

;Correct data with extra white spaces, no errors produced
	sct:		.struct   10   ,     "asd"
  var:	.data   123
var2: .data   	 -234
	x: .struct 0 	 	,	  "sd"


;Correct instructions with extra spaces, no errors produced
mov		 #-234    ,     r7
				 hlt
add x, x.2

