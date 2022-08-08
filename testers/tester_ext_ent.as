;Tester for .ext and .ent files creation

.extern A1
.extern A2
.extern A3
.extern A4
.extern A5
.extern B

.entry C1
.entry C2
.entry C3
.entry C4

mov A1, B
mov B, A2
mov A3, B
mov B, A4
mov A5, B
prn B

C1: .data 0
C3: .data 0, 1
C2: .data 0, 1, 2
C4: .struct 0, "string"
