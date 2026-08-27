MauvilleCity_GameCorner_MapScripts::
	.byte 0

	@ Game Corner prices
	.set TM_DOUBLE_TEAM_COINS, 1500
	.set TM_PSYCHIC_COINS, 3500
	.set TM_FLAMETHROWER_COINS, 4000
	.set TM_THUNDERBOLT_COINS, 4000
	.set TM_ICE_BEAM_COINS, 4000
	.set DOLL_COINS, 1000

	.set COINS_PRICE_50,  1000
	.set COINS_PRICE_500, 10000

MauvilleCity_GameCorner_EventScript_CoinsClerk::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_ThisIsMauvilleGameCorner, MSGBOX_DEFAULT
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NeedCoinCase
	message MauvilleCity_GameCorner_Text_WereYouLookingForCoins
	waitmessage
	showmoneybox 0, 0
	showcoinsbox 1, 6
	goto MauvilleCity_GameCorner_EventScript_ChooseCoinsDefault50

MauvilleCity_GameCorner_EventScript_ChooseCoinsDefault50::
	multichoicedefault 15, 0, MULTI_GAME_CORNER_COINS, 0, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_Buy50Coins
	case 1, MauvilleCity_GameCorner_EventScript_Buy500Coins
	goto MauvilleCity_GameCorner_EventScript_CancelBuyCoins
	end

@ Unused
MauvilleCity_GameCorner_EventScript_ChooseCoinsDefault500::
	multichoicedefault 15, 0, MULTI_GAME_CORNER_COINS, 1, FALSE
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_Buy50Coins
	case 1, MauvilleCity_GameCorner_EventScript_Buy500Coins
	goto MauvilleCity_GameCorner_EventScript_CancelBuyCoins
	end

MauvilleCity_GameCorner_EventScript_Buy50Coins::
	checkcoins VAR_TEMP_1
	goto_if_ge VAR_TEMP_1, (MAX_COINS + 1 - 50), MauvilleCity_GameCorner_EventScript_NoRoomForCoins
	checkmoney COINS_PRICE_50
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NotEnoughMoney
	addcoins 50
	removemoney COINS_PRICE_50
	updatemoneybox
	updatecoinsbox 1, 6
	playse SE_SHOP
	msgbox MauvilleCity_GameCorner_Text_ThankYouHereAreYourCoins, MSGBOX_DEFAULT
	hidemoneybox
	hidecoinsbox 0, 5
	release
	end

MauvilleCity_GameCorner_EventScript_Buy500Coins::
	checkcoins VAR_TEMP_1
	goto_if_ge VAR_TEMP_1, (MAX_COINS + 1 - 500), MauvilleCity_GameCorner_EventScript_NoRoomForCoins
	checkmoney COINS_PRICE_500
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NotEnoughMoney
	addcoins 500
	removemoney COINS_PRICE_500
	updatemoneybox
	updatecoinsbox 1, 6
	playse SE_SHOP
	msgbox MauvilleCity_GameCorner_Text_ThankYouHereAreYourCoins, MSGBOX_DEFAULT
	hidemoneybox
	hidecoinsbox 0, 5
	release
	end

MauvilleCity_GameCorner_EventScript_NeedCoinCase::
	msgbox MauvilleCity_GameCorner_Text_NeedCoinCaseForCoins, MSGBOX_DEFAULT
	release
	end

MauvilleCity_GameCorner_EventScript_NotEnoughMoney::
	msgbox MauvilleCity_GameCorner_Text_DontHaveEnoughMoney, MSGBOX_DEFAULT
	hidemoneybox
	hidecoinsbox 0, 5
	release
	end

MauvilleCity_GameCorner_EventScript_CancelBuyCoins::
	msgbox MauvilleCity_GameCorner_Text_DontNeedCoinsThen, MSGBOX_DEFAULT
	hidemoneybox
	hidecoinsbox 0, 5
	release
	end

MauvilleCity_GameCorner_EventScript_NoRoomForCoins::
	msgbox MauvilleCity_GameCorner_Text_CoinCaseIsFull, MSGBOX_DEFAULT
	hidemoneybox
	hidecoinsbox 0, 5
	release
	end

@ =============================================================================
@  WISHES OF TOMORROW -- GAME CORNER PRIZE COUNTERS
@
@  Two counters, both SCROLLING lists (dynmultichoice). A plain multichoice
@  window is sized count*2 tile rows, so anything past about six entries runs
@  off the bottom of the screen -- these tables are 15 and 13 long. Prices are
@  printed inside each row with {CLEAR_TO} so the player can see the whole
@  price list at a glance instead of picking blind.
@
@  Both lists run cheapest first. Coins come out only AFTER the prize is
@  actually handed over, so a full bag or a full party+PC can never charge for
@  nothing. Each purchase returns to the list rather than closing it.
@ =============================================================================

	@ Every prize Pokemon is handed over at this level. They are meant to be
	@ raised -- which is also why the three dragons here suit the 16/36 starter
	@ evolution cadence. Retune them all from this one line.
	.set GAME_CORNER_PRIZE_LEVEL, 5

@ ---- Counter 1: Pokemon ------------------------------------------------------

MauvilleCity_GameCorner_EventScript_PrizeCornerDolls::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_ExchangeCoinsForPrizes, MSGBOX_DEFAULT
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_GameCorner_EventScript_OpenMonPrizes
	release
	end

MauvilleCity_GameCorner_EventScript_OpenMonPrizes::
	showcoinsbox 1, 1
	goto MauvilleCity_GameCorner_EventScript_ChooseMonPrize

MauvilleCity_GameCorner_EventScript_ChooseMonPrize::
	message MauvilleCity_GameCorner_Text_WhichPrize
	waitmessage
	dynmultichoice 0, 0, FALSE, 6, 0, DYN_MULTICHOICE_CB_NONE, WotText_PrizeGimmighoul, WotText_PrizeCharcadet, WotText_PrizeHonedge, WotText_PrizeToxel, WotText_PrizeGoomy, WotText_PrizeJangmoO, WotText_PrizeGreavard, WotText_PrizeDuraludon, WotText_PrizeDreepy, WotText_PrizeFrigibax, WotText_PrizeGible, WotText_PrizeAxew, WotText_PrizeRotom, WotText_PrizeTypeNull, WotText_PrizePorygon, WotText_PrizeNoThanks
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_PrizeGimmighoul
	case 1, MauvilleCity_GameCorner_EventScript_PrizeCharcadet
	case 2, MauvilleCity_GameCorner_EventScript_PrizeHonedge
	case 3, MauvilleCity_GameCorner_EventScript_PrizeToxel
	case 4, MauvilleCity_GameCorner_EventScript_PrizeGoomy
	case 5, MauvilleCity_GameCorner_EventScript_PrizeJangmoO
	case 6, MauvilleCity_GameCorner_EventScript_PrizeGreavard
	case 7, MauvilleCity_GameCorner_EventScript_PrizeDuraludon
	case 8, MauvilleCity_GameCorner_EventScript_PrizeDreepy
	case 9, MauvilleCity_GameCorner_EventScript_PrizeFrigibax
	case 10, MauvilleCity_GameCorner_EventScript_PrizeGible
	case 11, MauvilleCity_GameCorner_EventScript_PrizeAxew
	case 12, MauvilleCity_GameCorner_EventScript_PrizeRotom
	case 13, MauvilleCity_GameCorner_EventScript_PrizeTypeNull
	case 14, MauvilleCity_GameCorner_EventScript_PrizePorygon
	case 15, MauvilleCity_GameCorner_EventScript_CancelMonSelect
	case MULTI_B_PRESSED, MauvilleCity_GameCorner_EventScript_CancelMonSelect
	end

MauvilleCity_GameCorner_EventScript_PrizeGimmighoul::
	setvar VAR_TEMP_1, SPECIES_GIMMIGHOUL
	setvar VAR_TEMP_2, 999
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeCharcadet::
	setvar VAR_TEMP_1, SPECIES_CHARCADET
	setvar VAR_TEMP_2, 2000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeHonedge::
	setvar VAR_TEMP_1, SPECIES_HONEDGE
	setvar VAR_TEMP_2, 2000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeToxel::
	setvar VAR_TEMP_1, SPECIES_TOXEL
	setvar VAR_TEMP_2, 2000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeGoomy::
	setvar VAR_TEMP_1, SPECIES_GOOMY
	setvar VAR_TEMP_2, 2500
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeJangmoO::
	setvar VAR_TEMP_1, SPECIES_JANGMO_O
	setvar VAR_TEMP_2, 3000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeGreavard::
	setvar VAR_TEMP_1, SPECIES_GREAVARD
	setvar VAR_TEMP_2, 3000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeDuraludon::
	setvar VAR_TEMP_1, SPECIES_DURALUDON
	setvar VAR_TEMP_2, 3000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeDreepy::
	setvar VAR_TEMP_1, SPECIES_DREEPY
	setvar VAR_TEMP_2, 4000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeFrigibax::
	setvar VAR_TEMP_1, SPECIES_FRIGIBAX
	setvar VAR_TEMP_2, 5000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeGible::
	setvar VAR_TEMP_1, SPECIES_GIBLE
	setvar VAR_TEMP_2, 5000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeAxew::
	setvar VAR_TEMP_1, SPECIES_AXEW
	setvar VAR_TEMP_2, 5000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeRotom::
	setvar VAR_TEMP_1, SPECIES_ROTOM
	setvar VAR_TEMP_2, 6500
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeTypeNull::
	setvar VAR_TEMP_1, SPECIES_TYPE_NULL
	setvar VAR_TEMP_2, 8000
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizePorygon::
	setvar VAR_TEMP_1, SPECIES_PORYGON
	setvar VAR_TEMP_2, 9999
	goto MauvilleCity_GameCorner_EventScript_ConfirmMonPrize
	end

MauvilleCity_GameCorner_EventScript_ConfirmMonPrize::
	bufferspeciesname STR_VAR_1, VAR_TEMP_1
	msgbox WotText_GameCornerConfirmPrize, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MauvilleCity_GameCorner_EventScript_ChooseMonPrize
	checkcoins VAR_RESULT
	goto_if_lt VAR_RESULT, VAR_TEMP_2, MauvilleCity_GameCorner_EventScript_NotEnoughCoinsForMon
	@ Porygon is the only prize that arrives holding something.
	goto_if_eq VAR_TEMP_1, SPECIES_PORYGON, MauvilleCity_GameCorner_EventScript_GivePorygonPrize
	givemon VAR_TEMP_1, GAME_CORNER_PRIZE_LEVEL
	goto MauvilleCity_GameCorner_EventScript_CheckReceivedPrizeMon
	end

MauvilleCity_GameCorner_EventScript_GivePorygonPrize::
	givemon SPECIES_PORYGON, GAME_CORNER_PRIZE_LEVEL, item=ITEM_DUBIOUS_DISC
	goto MauvilleCity_GameCorner_EventScript_CheckReceivedPrizeMon
	end

MauvilleCity_GameCorner_EventScript_CheckReceivedPrizeMon::
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PARTY, MauvilleCity_GameCorner_EventScript_PrizeMonToParty
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, MauvilleCity_GameCorner_EventScript_PrizeMonToPC
	@ MON_CANT_GIVE: nothing was created, so charge nothing.
	msgbox gText_NoMoreRoomForPokemon, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeMonToParty::
	call MauvilleCity_GameCorner_EventScript_PayAndAnnounceMon
	getpartysize
	subvar VAR_RESULT, 1
	copyvar VAR_0x8004, VAR_RESULT
	call Common_EventScript_NameReceivedPartyMon
	goto MauvilleCity_GameCorner_EventScript_ChooseMonPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeMonToPC::
	call MauvilleCity_GameCorner_EventScript_PayAndAnnounceMon
	msgbox gText_PkmnTransferredSomeonesPC, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseMonPrize
	end

MauvilleCity_GameCorner_EventScript_PayAndAnnounceMon::
	removecoins VAR_TEMP_2
	updatecoinsbox 0, 5
	bufferspeciesname STR_VAR_1, VAR_TEMP_1
	playfanfare MUS_OBTAIN_ITEM
	message gText_PlayerObtainedTheMon
	waitmessage
	waitfanfare
	return

MauvilleCity_GameCorner_EventScript_NotEnoughCoinsForMon::
	msgbox MauvilleCity_GameCorner_Text_NotEnoughCoins, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseMonPrize
	end

MauvilleCity_GameCorner_EventScript_CancelMonSelect::
	msgbox MauvilleCity_GameCorner_Text_OhIsThatSo, MSGBOX_DEFAULT
	hidecoinsbox 0, 0
	release
	end

@ ---- Counter 2: TMs and evolution items --------------------------------------

MauvilleCity_GameCorner_EventScript_PrizeCornerTMs::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_ExchangeCoinsForPrizes, MSGBOX_DEFAULT
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_GameCorner_EventScript_OpenGoodsPrizes
	release
	end

MauvilleCity_GameCorner_EventScript_OpenGoodsPrizes::
	showcoinsbox 1, 1
	goto MauvilleCity_GameCorner_EventScript_ChooseGoodsPrize

MauvilleCity_GameCorner_EventScript_ChooseGoodsPrize::
	message MauvilleCity_GameCorner_Text_WhichPrize
	waitmessage
	dynmultichoice 0, 0, FALSE, 6, 0, DYN_MULTICHOICE_CB_NONE, WotText_PrizePlayRough, WotText_PrizeScaleShot, WotText_PrizeKnockOff, WotText_PrizeLeadersCrest, WotText_PrizeScorchingSands, WotText_PrizeEarthPower, WotText_PrizeMetalAlloy, WotText_PrizeAuspiciousArmor, WotText_PrizeMaliciousArmor, WotText_PrizeSwordsDance, WotText_PrizeNastyPlot, WotText_PrizeDracoMeteor, WotText_PrizeCloseCombat, WotText_PrizeNoThanks
	switch VAR_RESULT
	case 0, MauvilleCity_GameCorner_EventScript_PrizePlayRough
	case 1, MauvilleCity_GameCorner_EventScript_PrizeScaleShot
	case 2, MauvilleCity_GameCorner_EventScript_PrizeKnockOff
	case 3, MauvilleCity_GameCorner_EventScript_PrizeLeadersCrest
	case 4, MauvilleCity_GameCorner_EventScript_PrizeScorchingSands
	case 5, MauvilleCity_GameCorner_EventScript_PrizeEarthPower
	case 6, MauvilleCity_GameCorner_EventScript_PrizeMetalAlloy
	case 7, MauvilleCity_GameCorner_EventScript_PrizeAuspiciousArmor
	case 8, MauvilleCity_GameCorner_EventScript_PrizeMaliciousArmor
	case 9, MauvilleCity_GameCorner_EventScript_PrizeSwordsDance
	case 10, MauvilleCity_GameCorner_EventScript_PrizeNastyPlot
	case 11, MauvilleCity_GameCorner_EventScript_PrizeDracoMeteor
	case 12, MauvilleCity_GameCorner_EventScript_PrizeCloseCombat
	case 13, MauvilleCity_GameCorner_EventScript_CancelGoodsSelect
	case MULTI_B_PRESSED, MauvilleCity_GameCorner_EventScript_CancelGoodsSelect
	end

MauvilleCity_GameCorner_EventScript_PrizePlayRough::
	setvar VAR_TEMP_1, ITEM_TM_PLAY_ROUGH
	setvar VAR_TEMP_2, 3000
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeScaleShot::
	setvar VAR_TEMP_1, ITEM_TM_SCALE_SHOT
	setvar VAR_TEMP_2, 3500
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeKnockOff::
	setvar VAR_TEMP_1, ITEM_TM_KNOCK_OFF
	setvar VAR_TEMP_2, 3500
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeLeadersCrest::
	setvar VAR_TEMP_1, ITEM_LEADERS_CREST
	setvar VAR_TEMP_2, 3500
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeScorchingSands::
	setvar VAR_TEMP_1, ITEM_TM_SCORCHING_SANDS
	setvar VAR_TEMP_2, 4000
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeEarthPower::
	setvar VAR_TEMP_1, ITEM_TM_EARTH_POWER
	setvar VAR_TEMP_2, 4000
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeMetalAlloy::
	setvar VAR_TEMP_1, ITEM_METAL_ALLOY
	setvar VAR_TEMP_2, 4000
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeAuspiciousArmor::
	setvar VAR_TEMP_1, ITEM_AUSPICIOUS_ARMOR
	setvar VAR_TEMP_2, 4000
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeMaliciousArmor::
	setvar VAR_TEMP_1, ITEM_MALICIOUS_ARMOR
	setvar VAR_TEMP_2, 4000
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeSwordsDance::
	setvar VAR_TEMP_1, ITEM_TM_SWORDS_DANCE
	setvar VAR_TEMP_2, 6000
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeNastyPlot::
	setvar VAR_TEMP_1, ITEM_TM_NASTY_PLOT
	setvar VAR_TEMP_2, 6000
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeDracoMeteor::
	setvar VAR_TEMP_1, ITEM_TM_DRACO_METEOR
	setvar VAR_TEMP_2, 8000
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_PrizeCloseCombat::
	setvar VAR_TEMP_1, ITEM_TM_CLOSE_COMBAT
	setvar VAR_TEMP_2, 8000
	goto MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_ConfirmGoodsPrize::
	bufferitemname STR_VAR_1, VAR_TEMP_1
	msgbox WotText_GameCornerConfirmPrize, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MauvilleCity_GameCorner_EventScript_ChooseGoodsPrize
	checkcoins VAR_RESULT
	goto_if_lt VAR_RESULT, VAR_TEMP_2, MauvilleCity_GameCorner_EventScript_NotEnoughCoinsForGoods
	checkitemspace VAR_TEMP_1, 1
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForGoods
	additem VAR_TEMP_1, 1
	removecoins VAR_TEMP_2
	updatecoinsbox 0, 5
	bufferitemname STR_VAR_1, VAR_TEMP_1
	playfanfare MUS_OBTAIN_ITEM
	message MauvilleCity_GameCorner_Text_HereYouGo
	waitmessage
	waitfanfare
	goto MauvilleCity_GameCorner_EventScript_ChooseGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_NotEnoughCoinsForGoods::
	msgbox MauvilleCity_GameCorner_Text_NotEnoughCoins, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_ChooseGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_NoRoomForGoods::
	call Common_EventScript_BagIsFull
	goto MauvilleCity_GameCorner_EventScript_ChooseGoodsPrize
	end

MauvilleCity_GameCorner_EventScript_CancelGoodsSelect::
	msgbox MauvilleCity_GameCorner_Text_OhIsThatSo, MSGBOX_DEFAULT
	hidecoinsbox 0, 0
	release
	end

@ ---- Prize list rows ---------------------------------------------------------

WotText_PrizeGimmighoul:
	.string "Gimmighoul{CLEAR_TO 0x48}{FONT_SMALL}  999 COINS$"
WotText_PrizeCharcadet:
	.string "Charcadet{CLEAR_TO 0x48}{FONT_SMALL}2,000 COINS$"
WotText_PrizeHonedge:
	.string "Honedge{CLEAR_TO 0x48}{FONT_SMALL}2,000 COINS$"
WotText_PrizeToxel:
	.string "Toxel{CLEAR_TO 0x48}{FONT_SMALL}2,000 COINS$"
WotText_PrizeGoomy:
	.string "Goomy{CLEAR_TO 0x48}{FONT_SMALL}2,500 COINS$"
WotText_PrizeJangmoO:
	.string "Jangmo-o{CLEAR_TO 0x48}{FONT_SMALL}3,000 COINS$"
WotText_PrizeGreavard:
	.string "Greavard{CLEAR_TO 0x48}{FONT_SMALL}3,000 COINS$"
WotText_PrizeDuraludon:
	.string "Duraludon{CLEAR_TO 0x48}{FONT_SMALL}3,000 COINS$"
WotText_PrizeDreepy:
	.string "Dreepy{CLEAR_TO 0x48}{FONT_SMALL}4,000 COINS$"
WotText_PrizeFrigibax:
	.string "Frigibax{CLEAR_TO 0x48}{FONT_SMALL}5,000 COINS$"
WotText_PrizeGible:
	.string "Gible{CLEAR_TO 0x48}{FONT_SMALL}5,000 COINS$"
WotText_PrizeAxew:
	.string "Axew{CLEAR_TO 0x48}{FONT_SMALL}5,000 COINS$"
WotText_PrizeRotom:
	.string "Rotom{CLEAR_TO 0x48}{FONT_SMALL}6,500 COINS$"
WotText_PrizeTypeNull:
	.string "Type: Null{CLEAR_TO 0x48}{FONT_SMALL}8,000 COINS$"
WotText_PrizePorygon:
	.string "Porygon{CLEAR_TO 0x48}{FONT_SMALL}9,999 COINS$"
WotText_PrizePlayRough:
	.string "TM54 Play Rough{CLEAR_TO 0x70}{FONT_SMALL}3,000 COINS$"
WotText_PrizeScaleShot:
	.string "TM53 Scale Shot{CLEAR_TO 0x70}{FONT_SMALL}3,500 COINS$"
WotText_PrizeKnockOff:
	.string "TM59 Knock Off{CLEAR_TO 0x70}{FONT_SMALL}3,500 COINS$"
WotText_PrizeLeadersCrest:
	.string "Leader's Crest{CLEAR_TO 0x70}{FONT_SMALL}3,500 COINS$"
WotText_PrizeScorchingSands:
	.string "TM57 Scorching Sands{CLEAR_TO 0x70}{FONT_SMALL}4,000 COINS$"
WotText_PrizeEarthPower:
	.string "TM58 Earth Power{CLEAR_TO 0x70}{FONT_SMALL}4,000 COINS$"
WotText_PrizeMetalAlloy:
	.string "Metal Alloy{CLEAR_TO 0x70}{FONT_SMALL}4,000 COINS$"
WotText_PrizeAuspiciousArmor:
	.string "Auspicious Armor{CLEAR_TO 0x70}{FONT_SMALL}4,000 COINS$"
WotText_PrizeMaliciousArmor:
	.string "Malicious Armor{CLEAR_TO 0x70}{FONT_SMALL}4,000 COINS$"
WotText_PrizeSwordsDance:
	.string "TM55 Swords Dance{CLEAR_TO 0x70}{FONT_SMALL}6,000 COINS$"
WotText_PrizeNastyPlot:
	.string "TM56 Nasty Plot{CLEAR_TO 0x70}{FONT_SMALL}6,000 COINS$"
WotText_PrizeDracoMeteor:
	.string "TM51 Draco Meteor{CLEAR_TO 0x70}{FONT_SMALL}8,000 COINS$"
WotText_PrizeCloseCombat:
	.string "TM52 Close Combat{CLEAR_TO 0x70}{FONT_SMALL}8,000 COINS$"
WotText_PrizeNoThanks:
	.string "No thanks$"

WotText_GameCornerConfirmPrize:
	.string "So your choice is {STR_VAR_1}?$"

MauvilleCity_GameCorner_EventScript_Woman2::
	msgbox MauvilleCity_GameCorner_Text_CoinsAreNeededToPlay, MSGBOX_NPC
	end

MauvilleCity_GameCorner_EventScript_Gentleman::
	msgbox MauvilleCity_GameCorner_Text_RouletteOnlyLuck, MSGBOX_NPC
	end

MauvilleCity_GameCorner_EventScript_Girl::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_STARTER_DOLL, MauvilleCity_GameCorner_EventScript_ReceivedStarterDoll
	msgbox MauvilleCity_GameCorner_Text_GotTwoOfSameDollWantOne, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MauvilleCity_GameCorner_EventScript_DeclineStarterDoll
	switch VAR_STARTER_MON
	case 0, MauvilleCity_GameCorner_EventScript_GiveTreeckoDoll
	case 1, MauvilleCity_GameCorner_EventScript_GiveTorchicDoll
	case 2, MauvilleCity_GameCorner_EventScript_GiveMudkipDoll
	end

MauvilleCity_GameCorner_EventScript_GiveTreeckoDoll::
	bufferdecorationname STR_VAR_2, DECOR_TREECKO_DOLL
	checkdecorspace DECOR_TREECKO_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForStarterDoll
	msgbox MauvilleCity_GameCorner_Text_HereYouGo2, MSGBOX_DEFAULT
	givedecoration DECOR_TREECKO_DOLL
	setflag FLAG_RECEIVED_STARTER_DOLL
	goto MauvilleCity_GameCorner_EventScript_ReceivedStarterDoll
	end

MauvilleCity_GameCorner_EventScript_GiveTorchicDoll::
	bufferdecorationname STR_VAR_2, DECOR_TORCHIC_DOLL
	checkdecorspace DECOR_TORCHIC_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForStarterDoll
	msgbox MauvilleCity_GameCorner_Text_HereYouGo2, MSGBOX_DEFAULT
	givedecoration DECOR_TORCHIC_DOLL
	setflag FLAG_RECEIVED_STARTER_DOLL
	goto MauvilleCity_GameCorner_EventScript_ReceivedStarterDoll
	end

MauvilleCity_GameCorner_EventScript_GiveMudkipDoll::
	bufferdecorationname STR_VAR_2, DECOR_MUDKIP_DOLL
	checkdecorspace DECOR_MUDKIP_DOLL
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoRoomForStarterDoll
	msgbox MauvilleCity_GameCorner_Text_HereYouGo2, MSGBOX_DEFAULT
	givedecoration DECOR_MUDKIP_DOLL
	setflag FLAG_RECEIVED_STARTER_DOLL
	goto MauvilleCity_GameCorner_EventScript_ReceivedStarterDoll
	end

MauvilleCity_GameCorner_EventScript_NoRoomForStarterDoll::
	call Common_EventScript_NoRoomForDecor
	msgbox MauvilleCity_GameCorner_Text_YouWantItButNotNow, MSGBOX_DEFAULT
	release
	end

MauvilleCity_GameCorner_EventScript_DeclineStarterDoll::
	msgbox MauvilleCity_GameCorner_Text_DontBeNegative, MSGBOX_DEFAULT
	release
	end

MauvilleCity_GameCorner_EventScript_ReceivedStarterDoll::
	msgbox MauvilleCity_GameCorner_Text_CantWinJackpot, MSGBOX_DEFAULT
	release
	end

MauvilleCity_GameCorner_EventScript_PokefanM::
	lock
	faceplayer
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, TRUE, MauvilleCity_GameCorner_EventScript_TryGive20Coins
	msgbox MauvilleCity_GameCorner_Text_NeedCoinCaseGoNextDoor, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_TryGive20Coins::
	goto_if_set FLAG_RECEIVED_20_COINS, MauvilleCity_GameCorner_EventScript_PokefanMNormal
	checkcoins VAR_TEMP_1
	goto_if_ge VAR_TEMP_1, 1, MauvilleCity_GameCorner_EventScript_PokefanMNormal @ Only give 20 coins if player has no coins
	setflag FLAG_RECEIVED_20_COINS
	addcoins 20
	msgbox MauvilleCity_GameCorner_Text_LuckOnlyLastSoLongTakeCoins, MSGBOX_DEFAULT
	playse SE_SHOP
	goto MauvilleCity_GameCorner_EventScript_PokefanMNormal
	end

MauvilleCity_GameCorner_EventScript_PokefanMNormal::
	msgbox MauvilleCity_GameCorner_Text_MauvilleSomethingForEveryone, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_OldMan::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_RouletteTablesDifferentRates, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_Cook::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_EasyToLoseTrackOfTime, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_Man::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_UpTo3CoinsCanBeUsed, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_NPCReturnToSlots::
	closemessage
	applymovement VAR_LAST_TALKED, Common_Movement_FaceOriginalDirection
	waitmovement 0
	release
	end

MauvilleCity_GameCorner_EventScript_Maniac::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_DifficultToStopOn7, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_Woman::
	lock
	faceplayer
	msgbox MauvilleCity_GameCorner_Text_HeresSomeSlotsInfo, MSGBOX_DEFAULT
	goto MauvilleCity_GameCorner_EventScript_NPCReturnToSlots
	end

MauvilleCity_GameCorner_EventScript_SlotMachine0::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 0
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine1::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 1
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine2::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 2
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine3::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 3
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine4::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 4
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine5::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 5
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine6::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 6
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine7::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 7
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine8::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 8
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine9::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 9
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine10::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 10
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_SlotMachine11::
	lockall
	checkitem ITEM_COIN_CASE
	goto_if_eq VAR_RESULT, FALSE, MauvilleCity_GameCorner_EventScript_NoCoinCase
	setvar VAR_0x8004, 11
	specialvar VAR_RESULT, GetSlotMachineId
	playslotmachine VAR_RESULT
	releaseall
	end

MauvilleCity_GameCorner_EventScript_NoCoinCase::
	msgbox MauvilleCity_GameCorner_Text_CantPlayWithNoCoinCase, MSGBOX_DEFAULT
	releaseall
	end

MauvilleCity_GameCorner_Text_ThisIsMauvilleGameCorner:
	.string "This is MAUVILLE GAME CORNER.$"

MauvilleCity_GameCorner_Text_NeedCoinCaseForCoins:
	.string "Okay, you wanted some COINS for\n"
	.string "the games?\p"
	.string "But you don't have a COIN CASE for\n"
	.string "stowing the COINS.$"

MauvilleCity_GameCorner_Text_WereYouLookingForCoins:
	.string "Were you looking for COINS?\p"
	.string "It's ¥1000 for 50 COINS.\n"
	.string "Would you like some?$"

MauvilleCity_GameCorner_Text_ThankYouHereAreYourCoins:
	.string "Thank you very much!\n"
	.string "Here are your COINS!$"

MauvilleCity_GameCorner_Text_DontHaveEnoughMoney:
	.string "Um… You don't appear to have\n"
	.string "enough money…$"

MauvilleCity_GameCorner_Text_CoinCaseIsFull:
	.string "Oh?\n"
	.string "Your COIN CASE is full.$"

MauvilleCity_GameCorner_Text_DontNeedCoinsThen:
	.string "Oh… You don't need COINS, then?\n"
	.string "Good luck on your adventure!$"

MauvilleCity_GameCorner_Text_ExchangeCoinsForPrizes:
	.string "Welcome.\p"
	.string "You can exchange your COINS for\n"
	.string "prizes here.$"

MauvilleCity_GameCorner_Text_WhichPrize:
	.string "Which prize would you like?$"

MauvilleCity_GameCorner_Text_SoYourChoiceIsTheTMX:
	.string "So your choice is\n"
	.string "the {STR_VAR_1} {STR_VAR_2}?$"

MauvilleCity_GameCorner_Text_SendToYourHomePC:
	.string "Thank you!\n"
	.string "We'll send it to your PC at home.$"

MauvilleCity_GameCorner_Text_NotEnoughCoins:
	.string "You don't have enough COINS.$"

@ Unused
MauvilleCity_GameCorner_Text_NoRoomForPlacingDecor:
	.string "There isn't any room available for\n"
	.string "placing {STR_VAR_1}.$"

MauvilleCity_GameCorner_Text_OhIsThatSo:
	.string "Oh, is that so? \n"
	.string "You need to save some COINS before\l"
	.string "coming back here.$"

MauvilleCity_GameCorner_Text_SoYourChoiceIsX:
	.string "So your choice is {STR_VAR_1}?$"

MauvilleCity_GameCorner_Text_HereYouGo:
	.string "Here you go!$"

@ Unused
MauvilleCity_GameCorner_Text_CantCarryAnyMore:
	.string "Oh, you can't carry any more than that.$"

MauvilleCity_GameCorner_Text_GotTwoOfSameDollWantOne:
	.string "I made a mistake and got two of\n"
	.string "the same DOLLS.\p"
	.string "Would you like one of them?$"

MauvilleCity_GameCorner_Text_HereYouGo2:
	.string "Here you go!$"

MauvilleCity_GameCorner_Text_YouWantItButNotNow:
	.string "Huh?\n"
	.string "You want it, but not right now?$"

MauvilleCity_GameCorner_Text_DontBeNegative:
	.string "Oh, don't be so negative!\n"
	.string "You can have this!$"

MauvilleCity_GameCorner_Text_CantWinJackpot:
	.string "There's a prize I want, but I can't win\n"
	.string "the jackpot.$"

MauvilleCity_GameCorner_Text_NeedCoinCaseGoNextDoor:
	.string "Hey, kid, if you want to play here,\n"
	.string "you need a COIN CASE.\p"
	.string "I think the young lady next door\n"
	.string "had one. Go see her!$"

MauvilleCity_GameCorner_Text_LuckOnlyLastSoLongTakeCoins:
	.string "My luck can only last so long.\n"
	.string "This is too much for me.\l"
	.string "Here, take some COINS!$"

MauvilleCity_GameCorner_Text_MauvilleSomethingForEveryone:
	.string "MAUVILLE has something for\n"
	.string "everyone.\p"
	.string "For me, it's the GAME CORNER.$"

MauvilleCity_GameCorner_Text_RouletteTablesDifferentRates:
	.string "The ROULETTE tables have different\n"
	.string "rates.\p"
	.string "Check your COINS if you're going to\n"
	.string "pick a table.$"

MauvilleCity_GameCorner_Text_EasyToLoseTrackOfTime:
	.string "It's easy to lose track of time in here. \n"
	.string "I should get back to work.$"

MauvilleCity_GameCorner_Text_CoinsAreNeededToPlay:
	.string "COINS are needed to play here\n"
	.string "in the GAME CORNER.$"

MauvilleCity_GameCorner_Text_RouletteOnlyLuck:
	.string "This ROULETTE thing…\n"
	.string "It's rather demanding.\p"
	.string "Win or lose, it's only by luck.$"

MauvilleCity_GameCorner_Text_UpTo3CoinsCanBeUsed:
	.string "Up to three COINS can be used to play\n"
	.string "the SLOTS.$"

MauvilleCity_GameCorner_Text_DifficultToStopOn7:
	.string "It's very difficult to make it stop\n"
	.string "right on “7.”\p"
	.string "If it stops on “7” during the REEL TIME\n"
	.string "bonus game, you'll receive extra COINS.$"

MauvilleCity_GameCorner_Text_HeresSomeSlotsInfo:
	.string "Here's some information for you\n"
	.string "about the SLOTS.\p"
	.string "The more lightning bolts you stock,\n"
	.string "the more REEL TIME chances you get.\p"
	.string "In a game with the maximum five\n"
	.string "REEL TIME chances…\p"
	.string "It is possible to receive four regular\n"
	.string "bonuses, then a big bonus.\p"
	.string "That would total 660 COINS, but it's\n"
	.string "very difficult to get.$"

MauvilleCity_GameCorner_Text_CantPlayWithNoCoinCase:
	.string "You can't play if you don't have\n"
	.string "a COIN CASE.$"

