Ashlands_2_MapScripts::
	.byte 0

@ East seam -> Ashlands (see the note in Ashlands/scripts.inc: different
@ primary tilesets, so this boundary must be a warp, not a connection).
Ashlands_2_EventScript_SeamEast5::
	warp MAP_ASHLANDS, 0, 5
	waitstate
	end

Ashlands_2_EventScript_SeamEast6::
	warp MAP_ASHLANDS, 0, 6
	waitstate
	end

Ashlands_2_EventScript_SeamEast7::
	warp MAP_ASHLANDS, 0, 7
	waitstate
	end

Ashlands_2_EventScript_SeamEast8::
	warp MAP_ASHLANDS, 0, 8
	waitstate
	end
