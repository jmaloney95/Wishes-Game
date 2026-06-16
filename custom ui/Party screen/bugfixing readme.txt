Fixed an issue where certain numbers were not being updated correctly after healing your party.

The content of the modified ROM is a little big to include in the form of a patch.
To fix this bug you will need to use a bit of free space, therefore, I only updated the 2 BW styled party patches which can safely secure free space.

Instructions:

BPRE(FireRed)
Insert the contents of the "bpre.bin" file into free space.

0x1217c2	[3D 68 38 7A 29 7A 6A 7A 04 32 AB 7A EC 7A 00 94 01 4C 04 E0 00 00 XX XX XX XX]
0x1219BC	[35 68 30 7A 29 7C 6A 7C 04 32 AB 7C EC 7C 00 94 00 4C 04 E0 XX XX XX XX]
0x121A5C	[35 68 30 7A 29 7D 6A 7D 04 32 AB 7D EC 7D 00 94 00 4C 05 E0 XX XX XX XX]
0x459EF6	[10]


BPEE(Emerald)
Insert the contents of the "bpee.bin" file into free space.

0x1B2AFA	[3D 68 38 7A 29 7A 6A 7A 04 32 AB 7A EC 7A 00 94 01 4C 04 E0 00 00 XX XX XX XX]
0x1B2CF4	[35 68 30 7A 29 7C 6A 7C 05 32 AB 7C EC 7C 00 94 00 4C 04 E0 XX XX XX XX]
0x1B2D94	[35 68 30 7A 29 7D 6A 7D 04 32 AB 7D EC 7D 00 94 00 4C 05 E0 XX XX XX XX]
0x6156F6	[10]

Note: XX XX XX XX = Pointer to the offset where the content of "bpre.bin" or "bpee.bin" was inserted.