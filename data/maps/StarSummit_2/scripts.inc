@ The Act-1 finale plays HERE now (Torii's summit warps land on this map).
@ The trigger events reuse the ORIGINAL StarSummit cutscene labels -- script
@ labels are global and this duplicate's object local ids match 1:1
@ (1 Jirachi / 2 Prof / 3 the masked leader / 4-5 the top goons).
StarSummit_2_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, StarSummit_2_OnFrame
	.byte 0

@ Setting foot on the summit completes the Act 1 main quest -- same
@ idempotent shape as the original map's hook.
StarSummit_2_OnFrame:
	map_script_2 VAR_TEMP_1, 0, StarSummit_2_EventScript_ReachedSummit
	.2byte 0

StarSummit_2_EventScript_ReachedSummit::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_quest_active QUEST_WAKING_MOUNTAIN, StarSummit_2_EventScript_CompleteWakingMountain
	releaseall
	end

StarSummit_2_EventScript_CompleteWakingMountain::
	completequest QUEST_WAKING_MOUNTAIN
	releaseall
	end
