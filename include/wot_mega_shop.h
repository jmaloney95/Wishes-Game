#ifndef GUARD_WOT_MEGA_SHOP_H
#define GUARD_WOT_MEGA_SHOP_H

void WotGetPartyLeaderMegaStone(void);

// The species a mon should be SHOWN as on trophy screens: its Mega, if it is
// holding the stone for one. Display only -- nothing about battle changes.
u16 WotGetDisplaySpecies(struct Pokemon *mon);

#endif // GUARD_WOT_MEGA_SHOP_H
