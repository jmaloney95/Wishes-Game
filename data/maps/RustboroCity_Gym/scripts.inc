RustboroCity_Gym_MapScripts::
	.byte 0

RustboroCity_Gym_EventScript_Roxanne::
	setspeaker SP_NAME_RED_FATALITY
	trainerbattle_single TRAINER_ROXANNE_1, RustboroCity_Gym_Text_RoxanneIntro, RustboroCity_Gym_Text_RoxanneDefeat, RustboroCity_Gym_EventScript_RoxanneDefeated, NO_MUSIC
	setspeaker SP_NAME_RED_FATALITY
	goto_if_set FLAG_HIDE_SHINKANSEN_GOON, RustboroCity_Gym_EventScript_PostBattleKnowsTrain
	msgbox RustboroCity_Gym_Text_PostBattleNorth, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_PostBattleKnowsTrain::
	msgbox RustboroCity_Gym_Text_RoxannePostBattle, MSGBOX_DEFAULT
	release
	end

@ Badge ceremony hook: if the player already boarded the SHINKANSEN and beat the
@ goon (FLAG_HIDE_SHINKANSEN_GOON), RedFatality acknowledges the frozen train and
@ vows to thaw it. Otherwise he just points the player north past the SHINKANSEN
@ toward the mountain. (The track-clear itself is gated on FLAG_BADGE01_GET only.)
RustboroCity_Gym_EventScript_RoxanneDefeated::
	message RustboroCity_Gym_Text_ReceivedEmberBadge
	waitmessage
	call Common_EventScript_PlayGymBadgeFanfare
	setspeaker SP_NAME_RED_FATALITY
	msgbox RustboroCity_Gym_Text_EmberBadgeInfo, MSGBOX_DEFAULT
	setflag FLAG_DEFEATED_RUSTBORO_GYM
	setflag FLAG_BADGE01_GET
	setvar VAR_RUSTBORO_CITY_STATE, 1
	call_if_quest_active SIDE_QUEST_FROZEN_TRACKS, RustboroCity_Gym_EventScript_CompleteFrozenTracks
	setspeaker SP_NAME_RED_FATALITY
	goto_if_set FLAG_HIDE_SHINKANSEN_GOON, RustboroCity_Gym_EventScript_KnownTrainHook
	msgbox RustboroCity_Gym_Text_HeadNorthHook, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_CompleteFrozenTracks::
	completequest SIDE_QUEST_FROZEN_TRACKS
	return

RustboroCity_Gym_EventScript_KnownTrainHook::
	msgbox RustboroCity_Gym_Text_FrozenTrainHook, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_Josh::
	setspeaker SP_NAME_MIKMANC
	trainerbattle_single TRAINER_JOSH, RustboroCity_Gym_Text_JoshIntro, RustboroCity_Gym_Text_JoshDefeat
	setspeaker SP_NAME_MIKMANC
	msgbox RustboroCity_Gym_Text_JoshPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Tommy::
	setspeaker SP_NAME_MINISTER
	trainerbattle_single TRAINER_TOMMY, RustboroCity_Gym_Text_TommyIntro, RustboroCity_Gym_Text_TommyDefeat
	setspeaker SP_NAME_MINISTER
	msgbox RustboroCity_Gym_Text_TommyPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_Marc::
	setspeaker SP_NAME_YIFFER
	trainerbattle_single TRAINER_MARC, RustboroCity_Gym_Text_MarcIntro, RustboroCity_Gym_Text_MarcDefeat
	setspeaker SP_NAME_YIFFER
	msgbox RustboroCity_Gym_Text_MarcPostBattle, MSGBOX_AUTOCLOSE
	end

RustboroCity_Gym_EventScript_GymGuide::
	lock
	faceplayer
	goto_if_set FLAG_DEFEATED_RUSTBORO_GYM, RustboroCity_Gym_EventScript_GymGuidePostVictory
	msgbox RustboroCity_Gym_Text_GymGuideAdvice, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_GymGuidePostVictory::
	msgbox RustboroCity_Gym_Text_GymGuidePostVictory, MSGBOX_DEFAULT
	release
	end

RustboroCity_Gym_EventScript_LeftGymStatue::
	lockall
	goto_if_set FLAG_BADGE01_GET, RustboroCity_Gym_EventScript_GymStatueCertified
	goto RustboroCity_Gym_EventScript_GymStatue
	end

RustboroCity_Gym_EventScript_RightGymStatue::
	lockall
	goto_if_set FLAG_BADGE01_GET, RustboroCity_Gym_EventScript_GymStatueCertified
	goto RustboroCity_Gym_EventScript_GymStatue
	end

RustboroCity_Gym_EventScript_GymStatueCertified::
	msgbox RustboroCity_Gym_Text_GymStatueCertified, MSGBOX_DEFAULT
	releaseall
	end

RustboroCity_Gym_EventScript_GymStatue::
	msgbox RustboroCity_Gym_Text_GymStatue, MSGBOX_DEFAULT
	releaseall
	end

RustboroCity_Gym_EventScript_RegisterRoxanne::
	lockall
	pokenavcall RustboroCity_Gym_Text_RoxanneRegisterCall
	waitmessage
	delay 30
	playfanfare MUS_REGISTER_MATCH_CALL
	msgbox RustboroCity_Gym_Text_RegisteredRoxanne, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	delay 30
	setflag FLAG_ENABLE_ROXANNE_MATCH_CALL
	clearflag FLAG_ENABLE_ROXANNE_FIRST_CALL
	releaseall
	end

RustboroCity_Gym_Text_GymGuideAdvice:
	.string "Hey, challenger! Cold enough\n"
	.string "for you in here?\p"
	.string "Welcome to the FROSTWOOD GYM.\n"
	.string "The LEADER, RedFatality, is an ICE-type\l"
	.string "master through and through.\p"
	.string "ICE POKéMON can't stand FIRE-,\n"
	.string "FIGHTING-, ROCK-, or STEEL-type\l"
	.string "moves, so come prepared!\p"
	.string "One more thing… RedFatality has a soft\n"
	.string "spot for rare POKéMON.\p"
	.string "Don't go assuming you've got him\n"
	.string "all figured out.\p"
	.string "Now go earn that EMBER BADGE!$"

RustboroCity_Gym_Text_GymGuidePostVictory:
	.string "Whoa! You actually toppled RedFatality\p"
	.string "That EMBER BADGE is the genuine\n"
	.string "article.\p"
	.string "You've taken your first big step\n"
	.string "toward the top. Feels great, huh champ?$"

RustboroCity_Gym_Text_JoshIntro:
	.string "Snow on the ground doesn't mean\n"
	.string "everything's frozen solid!\p"
	.string "My POKéMON grow strong even in\n"
	.string "the bitter cold!$"

RustboroCity_Gym_Text_JoshDefeat:
	.string "Guess my little garden needs\n"
	.string "more sun…$"

RustboroCity_Gym_Text_JoshPostBattle:
	.string "RedFatality scouts rare POKéMON from\n"
	.string "all over the region.\p"
	.string "You're about to find out what\n"
	.string "I mean.$"

RustboroCity_Gym_Text_TommyIntro:
	.string "The big thaw's been turning up\n"
	.string "POKéMON nobody's seen 'round\l"
	.string "FROSTWOOD in ages!$"

RustboroCity_Gym_Text_TommyDefeat:
	.string "Damn! You're tough.$"

RustboroCity_Gym_Text_TommyPostBattle:
	.string "Strange types keep crawling out\n"
	.string "since the ice started melting.\p"
	.string "Wild times, huh?$"

RustboroCity_Gym_Text_MarcIntro:
	.string "Yiff yiff!$"

RustboroCity_Gym_Text_MarcDefeat:
	.string "yiff…$"

RustboroCity_Gym_Text_MarcPostBattle:
	.string "Yiff yiff...$"

RustboroCity_Gym_Text_RoxanneIntro:
	.string "Welcome to the FROSTWOOD GYM.\p"
	.string "That's right, I'm RedFatality.\p"
	.string "And yeah, the rumors are true.\n"
	.string "I was a Halo 3 MLG pro.\p"
	.string "If you caught nothing but ICE types\n"
	.string "like the others, you're in for a\p"
	.string "rude awakening, slugger.\p"
	.string "Show me what you've got!$"

RustboroCity_Gym_Text_RoxanneDefeat:
	.string "…Heh.\p"
	.string "You fought all the way down to my\n"
	.string "ace and still came out on top.\p"
	.string "Not many TRAINERS can say that.\p"
	.string "You've earned this.$"

RustboroCity_Gym_Text_ReceivedEmberBadge:
	.string "{PLAYER} received the EMBER BADGE\n"
	.string "from RedFatality.$"

RustboroCity_Gym_Text_EmberBadgeInfo:
	.string "The EMBER BADGE raises the ATTACK\n"
	.string "power of your POKéMON.\p"
	.string "Wear it with pride.$"

RustboroCity_Gym_Text_FrozenTrainHook:
	.string "One more thing before you go.\p"
	.string "The SENNEN LINE off the north\n"
	.string "edge of town?\p"
	.string "The thaw stalled halfway and the\n"
	.string "whole train's locked under solid\l"
	.string "ice. Nobody's getting through.\p"
	.string "Tell you what.\p"
	.string "Arcanine and I will head up there\n"
	.string "and burn it loose ourselves.\p"
	.string "The tracks will be clear, \n"
	.string "and your road forward with them.\p"
	.string "Go on, challenger.\n"
	.string "The region's waiting for you.$"

RustboroCity_Gym_Text_HeadNorthHook:
	.string "One more thing before you go.\p"
	.string "Your road forward runs north, past\n"
	.string "the SHINKANSEN -- the bullet\l"
	.string "train parked on the SENNEN LINE.\p"
	.string "Beyond those tracks rises the\n"
	.string "mountain… and the challengers\l"
	.string "waiting on it.\p"
	.string "If anything's blocking the rails,\n"
	.string "Arcanine and I will burn it loose.\p"
	.string "Go on, challenger.\n"
	.string "The region's waiting for you.$"

RustboroCity_Gym_Text_RoxannePostBattle:
	.string "The train's thawing as we speak.\p"
	.string "Keep climbing -- and if you swing\n"
	.string "by my folks' place, tell my mom\l"
	.string "I'm eating just fine.$"

RustboroCity_Gym_Text_PostBattleNorth:
	.string "The rails north of SENNEN should\n"
	.string "be clear for you now.\p"
	.string "Keep climbing, and if you swing\n"
	.string "by my folks' place, tell my mom\l"
	.string "I'm eating just fine.$"

RustboroCity_Gym_Text_GymStatue:
	.string "FROSTWOOD POKéMON GYM$"

RustboroCity_Gym_Text_GymStatueCertified:
	.string "FROSTWOOD POKéMON GYM\p"
	.string "RedFatality's CERTIFIED TRAINERS:\n"
	.string "{PLAYER}$"

RustboroCity_Gym_Text_RoxanneRegisterCall:
	.string "… … … … … …\n"
	.string "… … … … … Beep!\p"
	.string "RedFatality: Hey, {PLAYER}!\n"
	.string "It's RedFatality, from FROSTWOOD.\p"
	.string "Word is you're climbing the ranks\n"
	.string "at a serious clip.\p"
	.string "I'll keep my GYM sharp so you can\n"
	.string "challenge me again sometime.\p"
	.string "Looking forward to it!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

RustboroCity_Gym_Text_RegisteredRoxanne:
	.string "Registered GYM LEADER RedFatality\n"
	.string "in the POKéNAV.$"
