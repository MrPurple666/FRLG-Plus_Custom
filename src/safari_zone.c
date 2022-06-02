#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "event_scripts.h"
#include "overworld.h"
#include "pokedex.h"
#include "script.h"
#include "event_data.h"
#include "field_screen_effect.h"
#include "constants/map_types.h"

EWRAM_DATA u8 gNumSafariBalls = 0;
EWRAM_DATA u16 gSafariZoneStepCounter = 0;

bool32 GetSafariZoneFlag(void)
{
    return FlagGet(FLAG_SYS_SAFARI_MODE);
}

void SetSafariZoneFlag(void)
{
    FlagSet(FLAG_SYS_SAFARI_MODE);
}

void ResetSafariZoneFlag(void)
{
    FlagClear(FLAG_SYS_SAFARI_MODE);
}

void EnterSafariMode(void)
{
    IncrementGameStat(GAME_STAT_ENTERED_SAFARI_ZONE);
    SetSafariZoneFlag();
    gNumSafariBalls = 30;
    gSafariZoneStepCounter = 600;
}

void ExitSafariMode(void)
{
    ResetSafariZoneFlag();
    gNumSafariBalls = 0;
    gSafariZoneStepCounter = 0;
}

bool8 SafariZoneTakeStep(void)
{
    if (GetSafariZoneFlag() == FALSE)
        return FALSE;
    if (gMapHeader.mapType == MAP_TYPE_INDOOR)
        return FALSE;
    gSafariZoneStepCounter--;
    if (gSafariZoneStepCounter == 0)
    {
        ScriptContext1_SetupScript(SafariZone_EventScript_TimesUp);
        return TRUE;
    }
    return FALSE;
}

void SafariZoneRetirePrompt(void)
{
    ScriptContext1_SetupScript(SafariZone_EventScript_RetirePrompt);
}

void CB2_EndSafariBattle(void)
{
    if(gSaveBlock1Ptr->keyFlags.nuzlocke == 1 || gSaveBlock1Ptr->keyFlags.nuzlocke == 2)
    {   
        if(!IsWildMonNuzlockeDupe(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES)))
        {
            NuzlockeFlagSet(GetCurrentRegionMapSectionId());
        }
    }
    if (gNumSafariBalls != 0)
    {
        SetMainCallback2(CB2_ReturnToField);
    }
    else if (gBattleOutcome == B_OUTCOME_NO_SAFARI_BALLS)
    {
        ScriptContext2_RunNewScript(SafariZone_EventScript_OutOfBallsMidBattle);
        WarpIntoMap();
        gFieldCallback = FieldCB_SafariZoneRanOutOfBalls;
        SetMainCallback2(CB2_LoadMap);
    }
    else if (gBattleOutcome == B_OUTCOME_CAUGHT)
    {
        ScriptContext1_SetupScript(SafariZone_EventScript_OutOfBalls);
        ScriptContext1_Stop();
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
    }
}

void SafariZoneRefillSteps(void)
{
    gSafariZoneStepCounter = 600;
}

void SafariZoneCheckSteps(void)
{
    gSpecialVar_Result = gSafariZoneStepCounter;
}
