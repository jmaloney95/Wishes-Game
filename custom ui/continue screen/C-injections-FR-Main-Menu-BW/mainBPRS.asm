.gba
.thumb
.open "BPRS.gba","build/rom_BPRS.gba", 0x08000000
//---------------------------------------------------

//hooks
.include "hooks/BPRS/specials_ptr.s"

.align 4
.org insertinto
.importobj "build/linked.o"
.close
