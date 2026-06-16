Thanks for downloading our GBA Music Pack! These songs are all MIDIs from the Pokemon games and some extras that we've converted to be GBA-ready (i.e. MIDI-0 Format, <12 tracks, loops added).

All of these MIDIs require the All Instruments Patch (or the All Instruments Voicegroup V2) applied to your ROM in order to work. We highly suggest testing any songs you want to use on the title screen first, to make sure they sound right in-game (more details on that below).

Tools required:
All Instruments Patch (or All Instruments Voicegroup V2) - to make these MIDIs work in your ROM.
Sappy - for inserting songs to your ROM
Anvil Studio and MidiEditor - if you need to edit any MIDIs

(All of these can be downloaded from our GBA Tools Pack, linked in the PokeCommunity Thread.)

----------------

CONTENTS:
(A) CONVERTING A MIDI TO A .S FILE (FOR SAPPY INSERTION)
(B) INSERTING A SONG INTO YOUR ROM
(C) FIXING ANY SONGS THAT SOUND FUNNY

----------------

(A) CONVERTING A MIDI TO A .S FILE (FOR SAPPY INSERTION)

1. Put your MIDI in the Mid2Agb folder inside of the Sappy folder.

2. Rename your MIDI so the filename is only one word and contains no special characters. (eg. "BW N's Theme" becomes "bwnstheme")

3. Drag your MIDI onto mid2agb.exe. This will create a .S file with the same name as your MIDI.

4. Open the .S file in Notepad and find where it says "mvl, 127" - this is the volume of the song. Change this to 80 (leaving it at 127 will cause crackling sounds and it will be too loud). 

5. Save the .S file.

----------------

(B) INSERTING A SONG INTO YOUR ROM

1. Open your ROM in Sappy.

2. Select the song from the list that you want to replace. (Some songs are unused in vanilla FR, so you can add a few extras without expanding the song table!)

3. Click "Assemble Song". At the top of the dialogue box, select your .S file (this must be located in the Mid2Agb folder).

4. Where it says "Base Destination Offset", this is where your song will be stored. Make sure to put it in a section of free space with nothing afterwards, or use HMA look for space that's 10000 bytes long. (Each song will take up a few thousand bytes, but the size varies per song.)

5. Change the "Voicegroup offset" to...
0xB30C5C for the All Instruments Patch
0x71A240 for the All Instruments Voicegroup V2 (or if you've manually inserted it, point to that offset)
This selects the new voicegroup, which is required for your songs to work in-game. More info on both patches can be found in their respective PokeCommunity Threads.)

6. Click the "Cook It" button. When the dialogue box appears, click Yes.

7. Your song is now inserted in game. Make sure to test how it sounds!

----------------

(C) FIXING ANY SONGS THAT SOUND FUNNY

Please note that some of the MIDIs may sound funny in-game. For example...

1. Songs with a lot of tracks:
If a song has a lot of instruments playing at once then some bits won't be heard when played in game. This is because the GBA can only play so many tracks at once.
To fix this, open your MIDI in Anvil Studio, give the most important tracks the highest "Channel" numbers, and delete any irrelevant tracks (such as backing tracks that don't add much). Export your MIDI as a "MIDI-Format 0 File".
Alternatively, it's also possible make the game support 12 Direct Sound Tracks as opposed to the default (~6). Read here: https://www.pokecommunity.com/threads/205158/post-9109313

2. Songs with warped notes:
A small number of songs may have distorted notes when inserted in game. To our knowledge, this was caused by "Control Events" in the MIDIs, so we've removed these events from all of the MIDIs.
If your song still has any warped notes, try removing any additional "events" in MidiEditor and export it. (Just don't remove the loops, which are two bits of text - '[' and ']')

3. Songs with any odd-sounding instruments:
Some of the songs may have an odd choice of instrument that could be improved. (This is probably more common with the All Instruments Voicegroup V2 which uses the vanilla instruments, unlike the All Instruments Patch which samples the Windows MIDI instruments).
If a song has any odd-sounding instruments, open the MIDI in Anvil Studio, change the instrument in question, export it as a "MIDI-Format 0 File" and try it again in game.