	.align 2
voicegroup229:: @ HGSS main
	voice_keysplit voicegroup233, KeySplitTable35	@ piano
	voice_keysplit voicegroup234, KeySplitTable36	@ rock piano
	voice_keysplit voicegroup235, KeySplitTable37	@ piano octave
	voice_keysplit voicegroup236, KeySplitTable9	@ rhodes
	voice_keysplit voicegroup237, KeySplitTable9	@ rhodes (no rel)
	voice_keysplit voicegroup238, KeySplitTable36	@ vibraphone
	voice_keysplit voicegroup239, KeySplitTable38	@ marimba
	voice_keysplit voicegroup240, KeySplitTable39	@ xylophone
	voice_keysplit voicegroup241, KeySplitTable25	@ tubular bell
	voice_keysplit voicegroup242, KeySplitTable36	@ glockenspiel
	voice_keysplit voicegroup243, KeySplitTable38	@ music box
	voice_keysplit voicegroup244, KeySplitTable40	@ drawbar organ
	voice_keysplit voicegroup245, KeySplitTable40	@ jazz organ
	voice_keysplit voicegroup246, KeySplitTable40	@ accordion
	voice_keysplit voicegroup247, KeySplitTable41	@ nylon guitar
	voice_keysplit voicegroup248, KeySplitTable41	@ martin guitar
	voice_keysplit voicegroup249, KeySplitTable32	@ distortion guitar (no rel)
	voice_keysplit voicegroup250, KeySplitTable32	@ distortion guitar
	voice_keysplit voicegroup251, KeySplitTable41	@ clean guitar
	voice_keysplit voicegroup252, KeySplitTable42	@ upright bass
	voice_keysplit voicegroup253, KeySplitTable43	@ fingered bass
	voice_directsound 60, 0, DirectSoundWaveData_dp_slapbass_c1_16, 255, 0, 255, 197	@ 127/127/127/118/ 64
	voice_keysplit voicegroup254, KeySplitTable44	@ synth bass
	voice_keysplit voicegroup255, KeySplitTable45	@ strings
	voice_keysplit voicegroup256, KeySplitTable46	@ violin
	voice_keysplit voicegroup257, KeySplitTable47	@ cello
	voice_keysplit voicegroup258, KeySplitTable48	@ pizzicato
	voice_keysplit voicegroup259, KeySplitTable40	@ harp
	voice_keysplit voicegroup260, KeySplitTable40	@ timpani
	voice_keysplit voicegroup261, KeySplitTable39	@ trumpets
	voice_keysplit voicegroup262, KeySplitTable47	@ trombones
	voice_keysplit voicegroup263, KeySplitTable49	@ french horn solo
	voice_keysplit voicegroup264, KeySplitTable50	@ french horns
	voice_keysplit voicegroup265, KeySplitTable51	@ oboe
	voice_keysplit voicegroup266, KeySplitTable52	@ clarinet
	voice_keysplit voicegroup267, KeySplitTable53	@ flute
	voice_keysplit voicegroup268, KeySplitTable54	@ bassoon
	voice_keysplit voicegroup269, KeySplitTable55	@ orchestra hit
	voice_keysplit voicegroup270, KeySplitTable38	@ square wave
	voice_keysplit_all voicegroup230				@ drums
	voice_keysplit_all voicegroup231				@ taiko drums
	voice_directsound 60, 0, DirectSoundWaveData_dp_woodbass_d3_16, 255, 251, 0, 178	@ 127/ 60/  1/120/ 64
	voice_keysplit voicegroup271, KeySplitTable56	@ koto
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 242, 40, 197	@ 126/100/ 50/118/ 64
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_wind, 67, 243, 155, 225	@  99/ 99/ 99/110/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_explosion, 67, 255, 0, 246	@  99/ 10/  0/ 80/ 64
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_square_wave, 255, 250, 0, 178	@ 127/ 70/  0/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_bubbles, 255, 0, 255, 0	@ 125/127/127/127/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 242, 25, 0	@ 126/100/ 40/127/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 252, 0, 48	@ 127/ 50/  0/125/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 250, 0, 225	@ 127/ 70/  0/110/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 242, 101, 225	@ 124/100/ 80/110/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 242, 101, 178	@ 124/100/ 80/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_purenoise, 81, 0, 57, 178	@ 110/127/ 60/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_purenoise, 255, 242, 0, 178	@ 125/100/  0/120/ 64
