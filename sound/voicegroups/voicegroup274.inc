	.align 2
voicegroup274:: @ BW basic
	voice_keysplit voicegroupInst1, KeySplitTable90	@ piano
	voice_keysplit voicegroup234, KeySplitTable36	@ rock piano
	voice_keysplit voicegroup235, KeySplitTable37	@ piano octave
	voice_keysplit voicegroupInst3, KeySplitTable36	@ rhodes
	voice_keysplit voicegroupInst4, KeySplitTable36	@ rhodes (lo rel)
	voice_keysplit voicegroup238, KeySplitTable36	@ vibraphone
	voice_keysplit voicegroupInst7, KeySplitTable40	@ marimba
	voice_keysplit voicegroup240, KeySplitTable39	@ xylophone
	voice_keysplit voicegroup241, KeySplitTable25	@ tubular bell
	voice_directsound 60, 0, DirectSoundWaveData_bw_glockenspiel_c5, 255, 249, 0, 208	@ 127/ 75/  1/116/ 64
	voice_keysplit voicegroup243, KeySplitTable38	@ music box
	voice_keysplit voicegroup244, KeySplitTable40	@ drawbar organ
	voice_keysplit voicegroup245, KeySplitTable40	@ jazz organ
	voice_keysplit voicegroupInst14, KeySplitTable79	@ accordion
	voice_keysplit voicegroupInst15, KeySplitTable87	@ nylon guitar
	voice_keysplit voicegroup248, KeySplitTable41	@ folk guitar
	voice_keysplit voicegroup249, KeySplitTable32	@ distortion guitar
	voice_keysplit voicegroupInst17, KeySplitTable73	@ marschall guitar
	voice_keysplit voicegroup251, KeySplitTable41	@ clean guitar
	voice_keysplit voicegroup252, KeySplitTable42	@ upright bass
	voice_keysplit voicegroup253, KeySplitTable43	@ fingered bass
	voice_directsound 60, 0, DirectSoundWaveData_dp_slapbass_c1_16, 255, 0, 255, 197	@ 127/127/127/118/ 64
	voice_keysplit voicegroup254, KeySplitTable44	@ synth bass
	voice_keysplit voicegroup255, KeySplitTable45	@ strings
	voice_keysplit voicegroup256, KeySplitTable46	@ violin
	voice_keysplit voicegroup257, KeySplitTable47	@ cello
	voice_keysplit voicegroup258, KeySplitTable48	@ pizzicato
	voice_keysplit voicegroup259, KeySplitTable40	@ harp 1
	voice_keysplit voicegroup260, KeySplitTable40	@ timpani
	voice_keysplit voicegroup261, KeySplitTable39	@ trumpets
	voice_keysplit voicegroup262, KeySplitTable47	@ trombones
	voice_keysplit voicegroup264, KeySplitTable78	@ french horns
	voice_keysplit voicegroup264, KeySplitTable50	@ french horns
	voice_keysplit voicegroup265, KeySplitTable51	@ oboe
	voice_keysplit voicegroup266, KeySplitTable52	@ clarinet
	voice_keysplit voicegroup267, KeySplitTable53	@ flute 1
	voice_keysplit voicegroupInst37, KeySplitTable88	@ flute 2
	voice_directsound 60, 0, DirectSoundWaveData_bw_orch_hit_c5, 255, 250, 2, 208	@ 127/ 70/ 10/116/ 64
	voice_keysplit voicegroupInst27, KeySplitTable115	@ harp 2
	voice_keysplit_all voicegroupDrum1				@ standard kit
	voice_keysplit_all voicegroupDrum2				@ tr-909 kit
	voice_keysplit_all voicegroupDrum3				@ room kit
	voice_directsound 60, 0, DirectSoundWaveData_bw_french_horn_ensemble_c3, 255, 216, 219, 208	@ 127/117/118/116/ 64
	voice_keysplit voicegroupInst34, KeySplitTable22	@ tuba
	voice_keysplit voicegroup265, KeySplitTable101	@ oboe 2
	voice_keysplit voicegroupInst22, KeySplitTable95	@ slap bass
	voice_directsound 60, 0, DirectSoundWaveData_dp_timpany_as_16, 255, 249, 0, 208	@ 127/ 80/  1/116/ 64
	voice_directsound 60, 0, DirectSoundWaveData_bw_slap_bass_g2, 255, 64, 227, 178	@ 127/125/120/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 242, 25, 0	@ 126/100/ 40/127/ 64
	voice_keysplit voicegroupInst47, KeySplitTable90	@ piano (longer decay)
	voice_keysplit voicegroupInst40, KeySplitTable93	@ saxophone
	voice_directsound 60, 0, DirectSoundWaveData_bw_trombone_stab_cs2, 255, 197, 227, 208	@ 127/120/120/116/ 64
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_directsound 60, 0, DirectSoundWaveData_purenoise, 255, 0, 255, 178	@ 125/127/127/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_bw_ocean_waves_sfx_c4, 255, 0, 255, 0	@ 127/127/127/127/ 64
	voice_keysplit voicegroupInst24, KeySplitTable96	@ strings 2
	voice_keysplit voicegroupInst57, KeySplitTable80	@ underwater sfx
	voice_keysplit voicegroupInst44, KeySplitTable35	@ piano 2
	voice_keysplit voicegroup247, KeySplitTable41	@ nylon guitar 2
	voice_directsound 60, 0, DirectSoundWaveData_bw_electronic_organ_c5, 255, 0, 255, 208	@ 127/127/127/116/ 64
	voice_directsound 60, 0, DirectSoundWaveData_bw_solo_trumpet_c5, 255, 0, 255, 178	@ 127/127/127/120/ 64
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_directsound 60, 0, DirectSoundWaveData_bw_suspended_cymbal_e2, 255, 0, 255, 237	@ 127/127/127/100/ 64
	voice_keysplit voicegroupInst41, KeySplitTable85	@ choir aahs
	voice_keysplit voicegroupInst60, KeySplitTable63	@ female choir
	voice_keysplit voicegroupInst48, KeySplitTable62	@ male choir
	voice_keysplit voicegroupInst5, KeySplitTable81	@ harpsichord
	voice_directsound 60, 0, DirectSoundWaveData_bw_erhu_c3, 42, 233, 159, 208	@  80/110/100/116/ 64
	voice_keysplit voicegroupInst6, KeySplitTable100	@ vibraphone 2
	voice_directsound 60, 0, DirectSoundWaveData_bw_vibraphone_soft_c5, 255, 249, 2, 208	@ 127/ 80/ 10/116/ 64
	voice_directsound 60, 0, DirectSoundWaveData_bw_gong1_c4, 255, 252, 0, 237	@ 127/ 45/  1/100/ 64
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_keysplit voicegroupInst64, KeySplitTable86	@ pipe organ
	voice_keysplit voicegroupInst43, KeySplitTable81	@ sitar
	voice_directsound 60, 0, DirectSoundWaveData_bw_steel_drum_g4, 255, 252, 0, 208	@ 127/ 50/  1/116/ 64
	voice_keysplit voicegroupInst65, KeySplitTable74	@ clarinet ensemble
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_keysplit voicegroupInst62, KeySplitTable38	@ resonant synth
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_keysplit voicegroupInst50, KeySplitTable106	@ french horn ensemble soft
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_directsound 60, 0, DirectSoundWaveData_bw_electric_bass_e2, 255, 233, 191, 197	@ 127/110/110/118/ 64
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_directsound 60, 0, DirectSoundWaveData_bw_whistle, 255, 0, 255, 178	@ 127/127/127/120/ 64
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
	voice_directsound 60, 0, DirectSoundWaveData_dp_808snare_16, 255, 219, 0, 178	@ 127/116/  0/120/ 64
