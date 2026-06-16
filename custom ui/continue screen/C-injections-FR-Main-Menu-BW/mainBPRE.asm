.gba
.thumb
.open "BPRE0.gba","build/test.gba", 0x08000000
//---------------------------------------------------

//hooks
.include "hooks/BPRE/hooks.s"

.align 4
.org insertinto
.importobj "build/linked.o"
.close
