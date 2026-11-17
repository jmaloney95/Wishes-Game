#ifndef GUARD_WOT_TESTER_H
#define GUARD_WOT_TESTER_H

#ifdef WOT_TESTER

// The Zygarde arc is still first-pass (forced wins, draft dialogue), so build T1 leaves
// it out. Remove this line for the first build that should test it.
#define WOT_TESTER_HOLD_ZYGARDE

enum WotTesterProfile
{
    WOT_TESTER_PROFILE_A,
    WOT_TESTER_PROFILE_B,
    WOT_TESTER_PROFILE_C,
    WOT_TESTER_PROFILE_D,
    WOT_TESTER_PROFILE_E,
    WOT_TESTER_PROFILE_F,
    WOT_TESTER_PROFILE_COUNT,
};

// dest = "T1-A": the build, then the save's profile (G = the tester's own save).
void WotTester_BuildLabel(u8 *dest);
void WotTester_ApplyProfile(enum WotTesterProfile profile);

#endif // WOT_TESTER

#endif // GUARD_WOT_TESTER_H
