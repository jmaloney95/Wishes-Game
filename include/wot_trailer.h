#ifndef GUARD_WOT_TRAILER_H
#define GUARD_WOT_TRAILER_H

// Debug only: scenes replayed for trailer capture (see wot_trailer.c).
// Release builds have no debug menu; the testers' build (WOT_TESTER) has one,
// so the trailer scenes are left out of it too.
#if DEBUG_OVERWORLD_MENU && !defined(WOT_TESTER)
#define WOT_TRAILER_CAPTURE TRUE
#else
#define WOT_TRAILER_CAPTURE FALSE
#endif

// WotTrailer_StartParkTour's VAR_0x8004.
#define WOT_TRAILER_PARK_CLIP   0 // no message boxes, timed like the trailer clip
#define WOT_TRAILER_PARK_TEXT   1 // the guide's lines, A to advance
#define WOT_TRAILER_PARK_PORTAL 2 // as CLIP, but no one in the park, a portal on the shore

struct ObjectEventTemplate;

bool32 WotTrailer_IsGrayMap(void);
bool32 WotTrailer_IsGraySpriteTag(u16 tag);
bool32 WotTrailer_IsEmptyPark(void);
void WotTrailer_SpawnParkObject(const struct ObjectEventTemplate *template, s16 cameraX, s16 cameraY);
void WotTrailer_StartParkTour(void);
void WotTrailer_EndParkTour(void);

#endif // GUARD_WOT_TRAILER_H
