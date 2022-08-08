;Tester for preprocessor errors

;Invalid macro names
macro

endmacro

macro 123
		hlt
endmacro

macro extern
endmacro

macro mov
bne X
endmacro

macro macro
endmacro

macro name (SOME MESS)
	;code
endmacro

;endmacro directive missing

macro MyMacro
rts
