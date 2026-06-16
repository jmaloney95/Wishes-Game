1a. BW HP Bars Fixed FR
These have a few minor bugs from the original C: Injection:

- The first digit of your HP doesn't update when dropping below 100
- The left opponent's Level won't always show in Double Battles
  (Going to the Party Menu or Bag fixes both the above)

- The text changing on the Marowak Ghost Battle is bugged
  (The event will need to be changed to a normal scripted encounter or removed)

- The Safari Zone HP Bars aren't supported
  (You'll need to change the Safari Zone to use normal battles)

As a result, we recommend using the B2W2 HP Bars instead, as these have no bugs.



3. BW Main Menu
To point to the BW Options Menu instead of the vanilla options menu, search for any instances of 89 83 08 08 and replace them with 2D 7D C3 08



4. BW Start Menu
To point to the BW Options Menu instead of the vanilla options menu, search for any instances of 89 83 08 08 and replace them with 2D 7D C3 08



7b. BW Summary Screen (New)
This should not be used with HUBOL/DPE, as it will display the wrong abilities for your party and bug out when viewing Pokémon from the PC. It should only be used with normal FR.
As a result, we recommend using the old BW Summary Screen instead, as this has no bugs.

To fix CFRU Ability Data:
As the CFRU does some table offsets dynamically, you'll need to replace the static ones in BPRE.ld to the actual locations in your CFRU/ROM.

https://https//github.com/Skeli789/Complete-Fire-Red-Upgrade
Generates dynamic offsets
->
BPRE.ld
gAbilityNames = 0x????????

https://github.com/Shiny-Miner/New-BW-summary-screen
Resolves abilities using the following static offsets
->
BPRE.ld
gAbilityNames = 0x0824FC40;                @ i.e. this is not correct for HUBOL/DPE base
gAbilityDescriptionPointers = 0x0824FB08;  @ likewise



9. BW Options Menu
If you're using the BW Main Menu, BW Start Menu, or any other modifications to these menus, you'll need to make them point to the BW Options Menu instead of the vanilla options menu.
To do this, search for any instances of 89 83 08 08 and replace them with 2D 7D C3 08



13b. B2W2 Naming Screen
The "_______'s Nickname?" text has been removed to ensure compatibility with HUBOL/DPE.
The Rival's Overworld Sprites don't appear when naming them - this is intentional as the C: Injection was designed to always use the vanilla sprites, which you'll probably change.



14a. BW EV-IV Screen
This works with the vanilla Summary Screen and old BW Summary Screen, but not the new BW Summary Screen.
Therefore, if you're using the new BW Summary Screen, you can only call the EV-IV Screen using an item with the fieldeffect pointer set to <C3C9F1>

If used with HUBOL/DPE, this screen will bug out when viewing Pokémon from the PC Box.
As a result, we recommend using the new BW EV-IV Screen instead, as this has no bugs.



14b. BW EV-IV Screen (New)
To call the EV-IV Screen in game, make an item with the fieldeffect pointer set to <C3C9F1>


-------------------------------------------------------------------------------------------


The original C: Injections are always the best way to implement features where possible. These will let you place the data at any offset you want, and are open-source to allow for any edits to the graphics etc. If you'd prefer to use the C: injections, please see the links below:

(Note: this may be complicated unless you're familiar with Path and Command Prompt)

The Code Mining Hub Discord: https://discord.gg/2vRGgnqw7T
Pre-requirements: https://www.mediafire.com/file/bdq6eept3e00bb8/pret-tools.rar/file
ARMIPS: https://github.com/Kingcom/armips/releases
DevkitPro: https://github.com/devkitPro/installer/releases
Python: https://www.python.org/downloads/

All of the C: Injections can be found in our ROM Hacking Patches Pack: https://www.pokecommunity.com/threads/540279/