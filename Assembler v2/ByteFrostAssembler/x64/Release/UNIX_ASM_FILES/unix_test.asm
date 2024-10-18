.define 4 Banana 0xdeadbeef
:main
LDR R1, Banana[1]
OUT Banana[1], A
CALL :end
:end
BRK
