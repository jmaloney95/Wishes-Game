Route129_MapScripts::
	.byte 0

@ ============================================================================
@  WoT: Route 129 is the open transit sea between Shin-Tokyo (north) and
@  Celebi Island (south). Full-width trigger rows on y=0 / y=39 carry the
@  surfing player across (plain warps never fire on water). The old Hoenn
@  connections + Kyogre abnormal-weather hooks are removed with them.
@ ============================================================================
Route129_EventScript_SurfNorthToCity::
	lockall
	warp MAP_SHIN_TOKYO, 26, 51
	waitstate
	releaseall
	end

Route129_EventScript_SurfSouthToIsland::
	lockall
	warp MAP_CELEBI_ISLAND, 10, 43
	waitstate
	releaseall
	end

Route129_EventScript_Chase::
	trainerbattle_single TRAINER_CHASE, Route129_Text_ChaseIntro, Route129_Text_ChaseDefeat
	msgbox Route129_Text_ChasePostBattle, MSGBOX_AUTOCLOSE
	end

Route129_EventScript_Allison::
	trainerbattle_single TRAINER_ALLISON, Route129_Text_AllisonIntro, Route129_Text_AllisonDefeat
	msgbox Route129_Text_AllisonPostBattle, MSGBOX_AUTOCLOSE
	end

Route129_EventScript_Reed::
	trainerbattle_single TRAINER_REED, Route129_Text_ReedIntro, Route129_Text_ReedDefeat
	msgbox Route129_Text_ReedPostBattle, MSGBOX_AUTOCLOSE
	end

Route129_EventScript_Tisha::
	trainerbattle_single TRAINER_TISHA, Route129_Text_TishaIntro, Route129_Text_TishaDefeat
	msgbox Route129_Text_TishaPostBattle, MSGBOX_AUTOCLOSE
	end

Route129_EventScript_Clarence::
	trainerbattle_single TRAINER_CLARENCE, Route129_Text_ClarenceIntro, Route129_Text_ClarenceDefeat
	msgbox Route129_Text_ClarencePostBattle, MSGBOX_AUTOCLOSE
	end

