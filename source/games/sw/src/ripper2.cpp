//-------------------------------------------------------------------------
/*
Copyright (C) 1997, 2005 - 3D Realms Entertainment

This file is part of Shadow Warrior version 1.2

Shadow Warrior is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

Original Source: 1997 - Frank Maddin and Jim Norwood
Prepared for public release: 03/28/2005 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------
#include "ns.h"
#include "build.h"

#include "names2.h"
#include "panel.h"
#include "game.h"
#include "tags.h"
#include "ai.h"
#include "pal.h"
#include "sprite.h"
#include "misc.h"

BEGIN_SW_NS

DECISION Ripper2Battle[] =
{
    {879, &AF(InitRipper2Charge)},
    {883, &AF(InitActorSetDecide)},
    {900, &AF(InitRipper2Hang)},
    {1024, &AF(InitActorAttack)}
};

DECISION Ripper2Offense[] =
{
    {789, &AF(InitActorMoveCloser)},
    {790, &AF(InitActorSetDecide)},
    {800, &AF(InitRipper2Hang)},
    {1024, &AF(InitActorAttack)}
};

DECISIONB Ripper2Broadcast[] =
{
    {3, attr_ambient},
    {1024, 0}
};

DECISION Ripper2Surprised[] =
{
    {40, &AF(InitRipper2Hang)},
    {701, &AF(InitActorMoveCloser)},
    {1024, &AF(InitActorDecide)}
};

DECISION Ripper2Evasive[] =
{
    {10, &AF(InitActorMoveCloser)},
    {1024, &AF(InitRipper2Charge)}
};

DECISION Ripper2LostTarget[] =
{
    {900, &AF(InitActorFindPlayer)},
    {1024, &AF(InitActorWanderAround)}
};

DECISION Ripper2CloseRange[] =
{
    {1024,  &AF(InitActorAttack)         }
};

PERSONALITY Ripper2Personality =
{
    Ripper2Battle,
    Ripper2Offense,
    Ripper2Broadcast,
    Ripper2Surprised,
    Ripper2Evasive,
    Ripper2LostTarget,
    Ripper2CloseRange,
    Ripper2CloseRange
};

ATTRIBUTE Ripper2Attrib =
{
    {100, 120, 300, 380},               // Speeds
    {5, 0, -2, -4},                     // Tic Adjusts
    3,                                  // MaxWeapons;
    {
        DIGI_RIPPER2AMBIENT, DIGI_RIPPER2ALERT, DIGI_RIPPER2ATTACK,
        DIGI_RIPPER2PAIN, DIGI_RIPPER2SCREAM, DIGI_RIPPER2HEARTOUT,
        0,0,0,0
    }
};

//////////////////////
//
// RIPPER2 RUN
//
//////////////////////

#define RIPPER2_RUN_RATE 16

STATE s_Ripper2Run[5][4] =
{
    {
        {RIPPER2_RUN_R0 + 0, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[0][1]},
        {RIPPER2_RUN_R0 + 1, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[0][2]},
        {RIPPER2_RUN_R0 + 2, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[0][3]},
        {RIPPER2_RUN_R0 + 3, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[0][0]},
    },
    {
        {RIPPER2_RUN_R1 + 0, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[1][1]},
        {RIPPER2_RUN_R1 + 1, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[1][2]},
        {RIPPER2_RUN_R1 + 2, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[1][3]},
        {RIPPER2_RUN_R1 + 3, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[1][0]},
    },
    {
        {RIPPER2_RUN_R2 + 0, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[2][1]},
        {RIPPER2_RUN_R2 + 1, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[2][2]},
        {RIPPER2_RUN_R2 + 2, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[2][3]},
        {RIPPER2_RUN_R2 + 3, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[2][0]},
    },
    {
        {RIPPER2_RUN_R3 + 0, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[3][1]},
        {RIPPER2_RUN_R3 + 1, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[3][2]},
        {RIPPER2_RUN_R3 + 2, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[3][3]},
        {RIPPER2_RUN_R3 + 3, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[3][0]},
    },
    {
        {RIPPER2_RUN_R4 + 0, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[4][1]},
        {RIPPER2_RUN_R4 + 1, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[4][2]},
        {RIPPER2_RUN_R4 + 2, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[4][3]},
        {RIPPER2_RUN_R4 + 3, RIPPER2_RUN_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2Run[4][0]},
    }
};


STATE* sg_Ripper2Run[] =
{
    &s_Ripper2Run[0][0],
    &s_Ripper2Run[1][0],
    &s_Ripper2Run[2][0],
    &s_Ripper2Run[3][0],
    &s_Ripper2Run[4][0]
};

//////////////////////
//
// RIPPER2 RUNFAST
//
//////////////////////

#define RIPPER2_RUNFAST_RATE 14

STATE s_Ripper2RunFast[5][4] =
{
    {
        {RIPPER2_RUNFAST_R0 + 0, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[0][1]},
        {RIPPER2_RUNFAST_R0 + 1, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[0][2]},
        {RIPPER2_RUNFAST_R0 + 2, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[0][3]},
        {RIPPER2_RUNFAST_R0 + 3, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[0][0]},
    },
    {
        {RIPPER2_RUNFAST_R1 + 0, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[1][1]},
        {RIPPER2_RUNFAST_R1 + 1, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[1][2]},
        {RIPPER2_RUNFAST_R1 + 2, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[1][3]},
        {RIPPER2_RUNFAST_R1 + 3, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[1][0]},
    },
    {
        {RIPPER2_RUNFAST_R2 + 0, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[2][1]},
        {RIPPER2_RUNFAST_R2 + 1, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[2][2]},
        {RIPPER2_RUNFAST_R2 + 2, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[2][3]},
        {RIPPER2_RUNFAST_R2 + 3, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[2][0]},
    },
    {
        {RIPPER2_RUNFAST_R3 + 0, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[3][1]},
        {RIPPER2_RUNFAST_R3 + 1, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[3][2]},
        {RIPPER2_RUNFAST_R3 + 2, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[3][3]},
        {RIPPER2_RUNFAST_R3 + 3, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[3][0]},
    },
    {
        {RIPPER2_RUNFAST_R4 + 0, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[4][1]},
        {RIPPER2_RUNFAST_R4 + 1, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[4][2]},
        {RIPPER2_RUNFAST_R4 + 2, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[4][3]},
        {RIPPER2_RUNFAST_R4 + 3, RIPPER2_RUNFAST_RATE | SF_TIC_ADJUST, &AF(DoRipper2Move), &s_Ripper2RunFast[4][0]},
    }
};


STATE* sg_Ripper2RunFast[] =
{
    &s_Ripper2RunFast[0][0],
    &s_Ripper2RunFast[1][0],
    &s_Ripper2RunFast[2][0],
    &s_Ripper2RunFast[3][0],
    &s_Ripper2RunFast[4][0]
};

//////////////////////
//
// RIPPER2 STAND
//
//////////////////////

#define RIPPER2_STAND_RATE 12

STATE s_Ripper2Stand[5][1] =
{
    {
        {RIPPER2_STAND_R0 + 0, RIPPER2_STAND_RATE, &AF(DoRipper2Move), &s_Ripper2Stand[0][0]},
    },
    {
        {RIPPER2_STAND_R1 + 0, RIPPER2_STAND_RATE, &AF(DoRipper2Move), &s_Ripper2Stand[1][0]},
    },
    {
        {RIPPER2_STAND_R2 + 0, RIPPER2_STAND_RATE, &AF(DoRipper2Move), &s_Ripper2Stand[2][0]},
    },
    {
        {RIPPER2_STAND_R3 + 0, RIPPER2_STAND_RATE, &AF(DoRipper2Move), &s_Ripper2Stand[3][0]},
    },
    {
        {RIPPER2_STAND_R4 + 0, RIPPER2_STAND_RATE, &AF(DoRipper2Move), &s_Ripper2Stand[4][0]},
    },
};

STATE* sg_Ripper2Stand[] =
{
    s_Ripper2Stand[0],
    s_Ripper2Stand[1],
    s_Ripper2Stand[2],
    s_Ripper2Stand[3],
    s_Ripper2Stand[4]
};

//////////////////////
//
// RIPPER2 SWIPE
//
//////////////////////

#define RIPPER2_SWIPE_RATE 14

STATE s_Ripper2Swipe[5][8] =
{
    {
        {RIPPER2_SWIPE_R0 + 0, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[0][1]},
        {RIPPER2_SWIPE_R0 + 1, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[0][2]},
        {RIPPER2_SWIPE_R0 + 1, 0 | SF_QUICK_CALL, &AF(InitRipperSlash), &s_Ripper2Swipe[0][3]},
        {RIPPER2_SWIPE_R0 + 2, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[0][4]},
        {RIPPER2_SWIPE_R0 + 3, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[0][5]},
        {RIPPER2_SWIPE_R0 + 3, 0 | SF_QUICK_CALL, &AF(InitRipperSlash), &s_Ripper2Swipe[0][6]},
        {RIPPER2_SWIPE_R0 + 3, 0 | SF_QUICK_CALL, &AF(InitActorDecide), &s_Ripper2Swipe[0][7]},
        {RIPPER2_SWIPE_R0 + 3, RIPPER2_SWIPE_RATE, &AF(DoRipper2Move), &s_Ripper2Swipe[0][7]},
    },
    {
        {RIPPER2_SWIPE_R1 + 0, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[1][1]},
        {RIPPER2_SWIPE_R1 + 1, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[1][2]},
        {RIPPER2_SWIPE_R1 + 1, 0 | SF_QUICK_CALL, &AF(InitRipperSlash), &s_Ripper2Swipe[1][3]},
        {RIPPER2_SWIPE_R1 + 2, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[1][4]},
        {RIPPER2_SWIPE_R1 + 3, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[1][5]},
        {RIPPER2_SWIPE_R1 + 3, 0 | SF_QUICK_CALL, &AF(InitRipperSlash), &s_Ripper2Swipe[1][6]},
        {RIPPER2_SWIPE_R1 + 3, 0 | SF_QUICK_CALL, &AF(InitActorDecide), &s_Ripper2Swipe[1][7]},
        {RIPPER2_SWIPE_R1 + 3, RIPPER2_SWIPE_RATE, &AF(DoRipper2Move), &s_Ripper2Swipe[1][7]},
    },
    {
        {RIPPER2_SWIPE_R2 + 0, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[2][1]},
        {RIPPER2_SWIPE_R2 + 1, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[2][2]},
        {RIPPER2_SWIPE_R2 + 1, 0 | SF_QUICK_CALL, &AF(InitRipperSlash), &s_Ripper2Swipe[2][3]},
        {RIPPER2_SWIPE_R2 + 2, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[2][4]},
        {RIPPER2_SWIPE_R2 + 3, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[2][5]},
        {RIPPER2_SWIPE_R2 + 3, 0 | SF_QUICK_CALL, &AF(InitRipperSlash), &s_Ripper2Swipe[2][6]},
        {RIPPER2_SWIPE_R2 + 3, 0 | SF_QUICK_CALL, &AF(InitActorDecide), &s_Ripper2Swipe[2][7]},
        {RIPPER2_SWIPE_R2 + 3, RIPPER2_SWIPE_RATE, &AF(DoRipper2Move), &s_Ripper2Swipe[2][7]},
    },
    {
        {RIPPER2_SWIPE_R3 + 0, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[3][1]},
        {RIPPER2_SWIPE_R3 + 1, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[3][2]},
        {RIPPER2_SWIPE_R3 + 1, 0 | SF_QUICK_CALL, &AF(InitRipperSlash), &s_Ripper2Swipe[3][3]},
        {RIPPER2_SWIPE_R3 + 2, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[3][4]},
        {RIPPER2_SWIPE_R3 + 3, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[3][5]},
        {RIPPER2_SWIPE_R3 + 3, 0 | SF_QUICK_CALL, &AF(InitRipperSlash), &s_Ripper2Swipe[3][6]},
        {RIPPER2_SWIPE_R3 + 3, 0 | SF_QUICK_CALL, &AF(InitActorDecide), &s_Ripper2Swipe[3][7]},
        {RIPPER2_SWIPE_R3 + 3, RIPPER2_SWIPE_RATE, &AF(DoRipper2Move), &s_Ripper2Swipe[3][7]},
    },
    {
        {RIPPER2_SWIPE_R4 + 0, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[4][1]},
        {RIPPER2_SWIPE_R4 + 1, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[4][2]},
        {RIPPER2_SWIPE_R4 + 1, 0 | SF_QUICK_CALL, &AF(InitRipperSlash), &s_Ripper2Swipe[4][3]},
        {RIPPER2_SWIPE_R4 + 2, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[4][4]},
        {RIPPER2_SWIPE_R4 + 3, RIPPER2_SWIPE_RATE, &AF(NullRipper2), &s_Ripper2Swipe[4][5]},
        {RIPPER2_SWIPE_R4 + 3, 0 | SF_QUICK_CALL, &AF(InitRipperSlash), &s_Ripper2Swipe[4][6]},
        {RIPPER2_SWIPE_R4 + 3, 0 | SF_QUICK_CALL, &AF(InitActorDecide), &s_Ripper2Swipe[4][7]},
        {RIPPER2_SWIPE_R4 + 3, RIPPER2_SWIPE_RATE, &AF(DoRipper2Move), &s_Ripper2Swipe[4][7]},
    }
};


STATE* sg_Ripper2Swipe[] =
{
    &s_Ripper2Swipe[0][0],
    &s_Ripper2Swipe[1][0],
    &s_Ripper2Swipe[2][0],
    &s_Ripper2Swipe[3][0],
    &s_Ripper2Swipe[4][0]
};

//////////////////////
//
// RIPPER2 KONG
//
//////////////////////

#define RIPPER2_MEKONG_RATE 18

STATE s_Ripper2Kong[5][7] =
{
    {
        {RIPPER2_MEKONG_R0 + 0, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[0][1]},
        {RIPPER2_MEKONG_R0 + 0, SF_QUICK_CALL,       &AF(ChestRipper2), &s_Ripper2Kong[0][2]},
        {RIPPER2_MEKONG_R0 + 1, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[0][3]},
        {RIPPER2_MEKONG_R0 + 2, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[0][4]},
        {RIPPER2_MEKONG_R0 + 3, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[0][5]},
        {RIPPER2_MEKONG_R0 + 3, 0 | SF_QUICK_CALL, &AF(InitActorDecide), &s_Ripper2Kong[0][6]},
        {RIPPER2_MEKONG_R0 + 0, RIPPER2_MEKONG_RATE, &AF(DoRipper2Move), &s_Ripper2Kong[0][6]},
    },
    {
        {RIPPER2_MEKONG_R1 + 0, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[1][1]},
        {RIPPER2_MEKONG_R0 + 0, SF_QUICK_CALL,       &AF(ChestRipper2), &s_Ripper2Kong[1][2]},
        {RIPPER2_MEKONG_R1 + 1, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[1][3]},
        {RIPPER2_MEKONG_R1 + 2, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[1][4]},
        {RIPPER2_MEKONG_R1 + 3, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[1][5]},
        {RIPPER2_MEKONG_R1 + 3, 0 | SF_QUICK_CALL, &AF(InitActorDecide), &s_Ripper2Kong[1][6]},
        {RIPPER2_MEKONG_R1 + 0, RIPPER2_MEKONG_RATE, &AF(DoRipper2Move), &s_Ripper2Kong[1][6]},
    },
    {
        {RIPPER2_MEKONG_R2 + 0, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[2][1]},
        {RIPPER2_MEKONG_R0 + 0, SF_QUICK_CALL,       &AF(ChestRipper2), &s_Ripper2Kong[2][2]},
        {RIPPER2_MEKONG_R2 + 1, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[2][3]},
        {RIPPER2_MEKONG_R2 + 2, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[2][4]},
        {RIPPER2_MEKONG_R2 + 3, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[2][5]},
        {RIPPER2_MEKONG_R2 + 3, 0 | SF_QUICK_CALL, &AF(InitActorDecide), &s_Ripper2Kong[2][6]},
        {RIPPER2_MEKONG_R2 + 0, RIPPER2_MEKONG_RATE, &AF(DoRipper2Move), &s_Ripper2Kong[2][6]},
    },
    {
        {RIPPER2_MEKONG_R3 + 0, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[3][1]},
        {RIPPER2_MEKONG_R0 + 0, SF_QUICK_CALL,       &AF(ChestRipper2), &s_Ripper2Kong[3][2]},
        {RIPPER2_MEKONG_R3 + 1, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[3][3]},
        {RIPPER2_MEKONG_R3 + 2, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[3][4]},
        {RIPPER2_MEKONG_R3 + 3, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[3][5]},
        {RIPPER2_MEKONG_R3 + 3, 0 | SF_QUICK_CALL, &AF(InitActorDecide), &s_Ripper2Kong[3][6]},
        {RIPPER2_MEKONG_R3 + 0, RIPPER2_MEKONG_RATE, &AF(DoRipper2Move), &s_Ripper2Kong[3][6]},
    },
    {
        {RIPPER2_MEKONG_R4 + 0, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[4][1]},
        {RIPPER2_MEKONG_R0 + 0, SF_QUICK_CALL,       &AF(ChestRipper2), &s_Ripper2Kong[4][2]},
        {RIPPER2_MEKONG_R4 + 1, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[4][3]},
        {RIPPER2_MEKONG_R4 + 2, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[4][4]},
        {RIPPER2_MEKONG_R4 + 3, RIPPER2_MEKONG_RATE, &AF(NullRipper2), &s_Ripper2Kong[4][5]},
        {RIPPER2_MEKONG_R4 + 3, 0 | SF_QUICK_CALL, &AF(InitActorDecide), &s_Ripper2Kong[4][6]},
        {RIPPER2_MEKONG_R4 + 0, RIPPER2_MEKONG_RATE, &AF(DoRipper2Move), &s_Ripper2Kong[4][6]},
    }
};


STATE* sg_Ripper2Kong[] =
{
    &s_Ripper2Kong[0][0],
    &s_Ripper2Kong[1][0],
    &s_Ripper2Kong[2][0],
    &s_Ripper2Kong[3][0],
    &s_Ripper2Kong[4][0]
};


//////////////////////
//
// RIPPER2 HEART - show players heart
//
//////////////////////

#define RIPPER2_HEART_RATE 20

STATE s_Ripper2Heart[5][4] =
{
    {
        {RIPPER2_HEART_R0 + 0, RIPPER2_HEART_RATE, &AF(DoRipper2StandHeart), &s_Ripper2Heart[0][1]},
        {RIPPER2_HEART_R0 + 1, RIPPER2_HEART_RATE, &AF(DoRipper2StandHeart), &s_Ripper2Heart[0][0]},
    },
    {
        {RIPPER2_HEART_R1 + 0, RIPPER2_HEART_RATE, &AF(DoRipper2StandHeart), &s_Ripper2Heart[1][1]},
        {RIPPER2_HEART_R1 + 1, RIPPER2_HEART_RATE, &AF(DoRipper2StandHeart), &s_Ripper2Heart[1][0]},
    },
    {
        {RIPPER2_HEART_R2 + 0, RIPPER2_HEART_RATE, &AF(DoRipper2StandHeart), &s_Ripper2Heart[2][1]},
        {RIPPER2_HEART_R2 + 1, RIPPER2_HEART_RATE, &AF(DoRipper2StandHeart), &s_Ripper2Heart[2][0]},
    },
    {
        {RIPPER2_HEART_R3 + 0, RIPPER2_HEART_RATE, &AF(DoRipper2StandHeart), &s_Ripper2Heart[3][1]},
        {RIPPER2_HEART_R3 + 1, RIPPER2_HEART_RATE, &AF(DoRipper2StandHeart), &s_Ripper2Heart[3][0]},
    },
    {
        {RIPPER2_HEART_R4 + 0, RIPPER2_HEART_RATE, &AF(DoRipper2StandHeart), &s_Ripper2Heart[4][1]},
        {RIPPER2_HEART_R4 + 1, RIPPER2_HEART_RATE, &AF(DoRipper2StandHeart), &s_Ripper2Heart[4][0]},
    }
};


STATE* sg_Ripper2Heart[] =
{
    &s_Ripper2Heart[0][0],
    &s_Ripper2Heart[1][0],
    &s_Ripper2Heart[2][0],
    &s_Ripper2Heart[3][0],
    &s_Ripper2Heart[4][0]
};

//////////////////////
//
// RIPPER2 HANG
//
//////////////////////

#define RIPPER2_HANG_RATE 14

STATE s_Ripper2Hang[5][4] =
{
    {
        {RIPPER2_HANG_R0, RIPPER2_HANG_RATE, &AF(DoRipper2Hang), &s_Ripper2Hang[0][0]},
    },
    {
        {RIPPER2_HANG_R1, RIPPER2_HANG_RATE, &AF(DoRipper2Hang), &s_Ripper2Hang[1][0]},
    },
    {
        {RIPPER2_HANG_R2, RIPPER2_HANG_RATE, &AF(DoRipper2Hang), &s_Ripper2Hang[2][0]},
    },
    {
        {RIPPER2_HANG_R3, RIPPER2_HANG_RATE, &AF(DoRipper2Hang), &s_Ripper2Hang[3][0]},
    },
    {
        {RIPPER2_HANG_R4, RIPPER2_HANG_RATE, &AF(DoRipper2Hang), &s_Ripper2Hang[4][0]},
    }
};


STATE* sg_Ripper2Hang[] =
{
    &s_Ripper2Hang[0][0],
    &s_Ripper2Hang[1][0],
    &s_Ripper2Hang[2][0],
    &s_Ripper2Hang[3][0],
    &s_Ripper2Hang[4][0]
};


//////////////////////
//
// RIPPER2 PAIN
//
//////////////////////

#define RIPPER2_PAIN_RATE 38

STATE s_Ripper2Pain[5][1] =
{
    {
        {4414 + 0, RIPPER2_PAIN_RATE, &AF(DoRipper2Pain), &s_Ripper2Pain[0][0]},
    },
    {
        {4414 + 0, RIPPER2_PAIN_RATE, &AF(DoRipper2Pain), &s_Ripper2Pain[1][0]},
    },
    {
        {4414 + 0, RIPPER2_PAIN_RATE, &AF(DoRipper2Pain), &s_Ripper2Pain[2][0]},
    },
    {
        {4414 + 0, RIPPER2_PAIN_RATE, &AF(DoRipper2Pain), &s_Ripper2Pain[3][0]},
    },
    {
        {4414 + 0, RIPPER2_PAIN_RATE, &AF(DoRipper2Pain), &s_Ripper2Pain[4][0]},
    }
};

STATE* sg_Ripper2Pain[] =
{
    &s_Ripper2Pain[0][0],
    &s_Ripper2Pain[1][0],
    &s_Ripper2Pain[2][0],
    &s_Ripper2Pain[3][0],
    &s_Ripper2Pain[4][0]
};

//////////////////////
//
// RIPPER2 JUMP
//
//////////////////////

#define RIPPER2_JUMP_RATE 25

STATE s_Ripper2Jump[5][6] =
{
    {
        {RIPPER2_JUMP_R0 + 0, RIPPER2_JUMP_RATE, &AF(NullRipper2), &s_Ripper2Jump[0][1]},
        {RIPPER2_JUMP_R0 + 1, RIPPER2_JUMP_RATE, &AF(DoRipper2MoveJump), &s_Ripper2Jump[0][1]},
    },
    {
        {RIPPER2_JUMP_R1 + 0, RIPPER2_JUMP_RATE, &AF(NullRipper2), &s_Ripper2Jump[1][1]},
        {RIPPER2_JUMP_R1 + 1, RIPPER2_JUMP_RATE, &AF(DoRipper2MoveJump), &s_Ripper2Jump[1][1]},
    },
    {
        {RIPPER2_JUMP_R2 + 0, RIPPER2_JUMP_RATE, &AF(NullRipper2), &s_Ripper2Jump[2][1]},
        {RIPPER2_JUMP_R2 + 1, RIPPER2_JUMP_RATE, &AF(DoRipper2MoveJump), &s_Ripper2Jump[2][1]},
    },
    {
        {RIPPER2_JUMP_R3 + 0, RIPPER2_JUMP_RATE, &AF(NullRipper2), &s_Ripper2Jump[3][1]},
        {RIPPER2_JUMP_R3 + 1, RIPPER2_JUMP_RATE, &AF(DoRipper2MoveJump), &s_Ripper2Jump[3][1]},
    },
    {
        {RIPPER2_JUMP_R4 + 0, RIPPER2_JUMP_RATE, &AF(NullRipper2), &s_Ripper2Jump[4][1]},
        {RIPPER2_JUMP_R4 + 1, RIPPER2_JUMP_RATE, &AF(DoRipper2MoveJump), &s_Ripper2Jump[4][1]},
    }
};


STATE* sg_Ripper2Jump[] =
{
    &s_Ripper2Jump[0][0],
    &s_Ripper2Jump[1][0],
    &s_Ripper2Jump[2][0],
    &s_Ripper2Jump[3][0],
    &s_Ripper2Jump[4][0]
};


//////////////////////
//
// RIPPER2 FALL
//
//////////////////////

#define RIPPER2_FALL_RATE 25

STATE s_Ripper2Fall[5][6] =
{
    {
        {RIPPER2_FALL_R0 + 0, RIPPER2_FALL_RATE, &AF(DoRipper2MoveJump), &s_Ripper2Fall[0][0]},
    },
    {
        {RIPPER2_FALL_R1 + 0, RIPPER2_FALL_RATE, &AF(DoRipper2MoveJump), &s_Ripper2Fall[1][0]},
    },
    {
        {RIPPER2_FALL_R2 + 0, RIPPER2_FALL_RATE, &AF(DoRipper2MoveJump), &s_Ripper2Fall[2][0]},
    },
    {
        {RIPPER2_FALL_R3 + 0, RIPPER2_FALL_RATE, &AF(DoRipper2MoveJump), &s_Ripper2Fall[3][0]},
    },
    {
        {RIPPER2_FALL_R4 + 0, RIPPER2_FALL_RATE, &AF(DoRipper2MoveJump), &s_Ripper2Fall[4][0]},
    }
};


STATE* sg_Ripper2Fall[] =
{
    &s_Ripper2Fall[0][0],
    &s_Ripper2Fall[1][0],
    &s_Ripper2Fall[2][0],
    &s_Ripper2Fall[3][0],
    &s_Ripper2Fall[4][0]
};


//////////////////////
//
// RIPPER2 JUMP ATTACK
//
//////////////////////

#define RIPPER2_JUMP_ATTACK_RATE 35
int DoRipper2BeginJumpAttack(DSWActor* actor);

STATE s_Ripper2JumpAttack[5][6] =
{
    {
        {RIPPER2_JUMP_R0 + 0, RIPPER2_JUMP_ATTACK_RATE, &AF(NullRipper2), &s_Ripper2JumpAttack[0][1]},
        {RIPPER2_JUMP_R0 + 0, 0 | SF_QUICK_CALL, &AF(DoRipper2BeginJumpAttack), &s_Ripper2JumpAttack[0][2]},
        {RIPPER2_JUMP_R0 + 2, RIPPER2_JUMP_ATTACK_RATE, &AF(DoRipper2MoveJump), &s_Ripper2JumpAttack[0][3]},
        {RIPPER2_JUMP_R0 + 1, RIPPER2_JUMP_ATTACK_RATE, &AF(DoRipper2MoveJump), &s_Ripper2JumpAttack[0][3]},
    },
    {
        {RIPPER2_JUMP_R1 + 0, RIPPER2_JUMP_ATTACK_RATE, &AF(NullRipper2), &s_Ripper2JumpAttack[1][1]},
        {RIPPER2_JUMP_R1 + 0, 0 | SF_QUICK_CALL, &AF(DoRipper2BeginJumpAttack), &s_Ripper2JumpAttack[1][2]},
        {RIPPER2_JUMP_R1 + 2, RIPPER2_JUMP_ATTACK_RATE, &AF(DoRipper2MoveJump), &s_Ripper2JumpAttack[1][3]},
        {RIPPER2_JUMP_R1 + 1, RIPPER2_JUMP_ATTACK_RATE, &AF(DoRipper2MoveJump), &s_Ripper2JumpAttack[1][3]},
    },
    {
        {RIPPER2_JUMP_R2 + 0, RIPPER2_JUMP_ATTACK_RATE, &AF(NullRipper2), &s_Ripper2JumpAttack[2][1]},
        {RIPPER2_JUMP_R2 + 0, 0 | SF_QUICK_CALL, &AF(DoRipper2BeginJumpAttack), &s_Ripper2JumpAttack[2][2]},
        {RIPPER2_JUMP_R2 + 2, RIPPER2_JUMP_ATTACK_RATE, &AF(DoRipper2MoveJump), &s_Ripper2JumpAttack[2][3]},
        {RIPPER2_JUMP_R1 + 1, RIPPER2_JUMP_ATTACK_RATE, &AF(DoRipper2MoveJump), &s_Ripper2JumpAttack[2][3]},
    },
    {
        {RIPPER2_JUMP_R3 + 0, RIPPER2_JUMP_ATTACK_RATE, &AF(NullRipper2), &s_Ripper2JumpAttack[3][1]},
        {RIPPER2_JUMP_R3 + 0, 0 | SF_QUICK_CALL, &AF(DoRipper2BeginJumpAttack), &s_Ripper2JumpAttack[3][2]},
        {RIPPER2_JUMP_R3 + 2, RIPPER2_JUMP_ATTACK_RATE, &AF(DoRipper2MoveJump), &s_Ripper2JumpAttack[3][3]},
        {RIPPER2_JUMP_R1 + 1, RIPPER2_JUMP_ATTACK_RATE, &AF(DoRipper2MoveJump), &s_Ripper2JumpAttack[3][3]},
    },
    {
        {RIPPER2_JUMP_R4 + 0, RIPPER2_JUMP_ATTACK_RATE, &AF(NullRipper2), &s_Ripper2JumpAttack[4][1]},
        {RIPPER2_JUMP_R4 + 0, 0 | SF_QUICK_CALL, &AF(DoRipper2BeginJumpAttack), &s_Ripper2JumpAttack[4][2]},
        {RIPPER2_JUMP_R4 + 2, RIPPER2_JUMP_ATTACK_RATE, &AF(DoRipper2MoveJump), &s_Ripper2JumpAttack[4][3]},
        {RIPPER2_JUMP_R1 + 1, RIPPER2_JUMP_ATTACK_RATE, &AF(DoRipper2MoveJump), &s_Ripper2JumpAttack[4][3]},
    }
};


STATE* sg_Ripper2JumpAttack[] =
{
    &s_Ripper2JumpAttack[0][0],
    &s_Ripper2JumpAttack[1][0],
    &s_Ripper2JumpAttack[2][0],
    &s_Ripper2JumpAttack[3][0],
    &s_Ripper2JumpAttack[4][0]
};


//////////////////////
//
// RIPPER2 HANG_JUMP
//
//////////////////////

#define RIPPER2_HANG_JUMP_RATE 20

STATE s_Ripper2HangJump[5][6] =
{
    {
        {RIPPER2_JUMP_R0 + 0, RIPPER2_HANG_JUMP_RATE, &AF(NullRipper2), &s_Ripper2HangJump[0][1]},
        {RIPPER2_JUMP_R0 + 1, RIPPER2_HANG_JUMP_RATE, &AF(DoRipper2HangJF), &s_Ripper2HangJump[0][1]},
    },
    {
        {RIPPER2_JUMP_R1 + 0, RIPPER2_HANG_JUMP_RATE, &AF(NullRipper2), &s_Ripper2HangJump[1][1]},
        {RIPPER2_JUMP_R1 + 1, RIPPER2_HANG_JUMP_RATE, &AF(DoRipper2HangJF), &s_Ripper2HangJump[1][1]},
    },
    {
        {RIPPER2_JUMP_R2 + 0, RIPPER2_HANG_JUMP_RATE, &AF(NullRipper2), &s_Ripper2HangJump[2][1]},
        {RIPPER2_JUMP_R2 + 1, RIPPER2_HANG_JUMP_RATE, &AF(DoRipper2HangJF), &s_Ripper2HangJump[2][1]},
    },
    {
        {RIPPER2_JUMP_R3 + 0, RIPPER2_HANG_JUMP_RATE, &AF(NullRipper2), &s_Ripper2HangJump[3][1]},
        {RIPPER2_JUMP_R3 + 1, RIPPER2_HANG_JUMP_RATE, &AF(DoRipper2HangJF), &s_Ripper2HangJump[3][1]},
    },
    {
        {RIPPER2_JUMP_R4 + 0, RIPPER2_HANG_JUMP_RATE, &AF(NullRipper2), &s_Ripper2HangJump[4][1]},
        {RIPPER2_JUMP_R4 + 1, RIPPER2_HANG_JUMP_RATE, &AF(DoRipper2HangJF), &s_Ripper2HangJump[4][1]},
    }
};


STATE* sg_Ripper2HangJump[] =
{
    &s_Ripper2HangJump[0][0],
    &s_Ripper2HangJump[1][0],
    &s_Ripper2HangJump[2][0],
    &s_Ripper2HangJump[3][0],
    &s_Ripper2HangJump[4][0]
};

//////////////////////
//
// RIPPER2 HANG_FALL
//
//////////////////////

#define RIPPER2_FALL_RATE 25

STATE s_Ripper2HangFall[5][6] =
{
    {
        {RIPPER2_FALL_R0 + 0, RIPPER2_FALL_RATE, &AF(DoRipper2HangJF), &s_Ripper2HangFall[0][0]},
    },
    {
        {RIPPER2_FALL_R1 + 0, RIPPER2_FALL_RATE, &AF(DoRipper2HangJF), &s_Ripper2HangFall[1][0]},
    },
    {
        {RIPPER2_FALL_R2 + 0, RIPPER2_FALL_RATE, &AF(DoRipper2HangJF), &s_Ripper2HangFall[2][0]},
    },
    {
        {RIPPER2_FALL_R3 + 0, RIPPER2_FALL_RATE, &AF(DoRipper2HangJF), &s_Ripper2HangFall[3][0]},
    },
    {
        {RIPPER2_FALL_R4 + 0, RIPPER2_FALL_RATE, &AF(DoRipper2HangJF), &s_Ripper2HangFall[4][0]},
    }
};


STATE* sg_Ripper2HangFall[] =
{
    &s_Ripper2HangFall[0][0],
    &s_Ripper2HangFall[1][0],
    &s_Ripper2HangFall[2][0],
    &s_Ripper2HangFall[3][0],
    &s_Ripper2HangFall[4][0]
};



//////////////////////
//
// RIPPER2 DIE
//
//////////////////////

#define RIPPER2_DIE_RATE 18

STATE s_Ripper2Die[] =
{
    {RIPPER2_DIE + 0, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Die[1]},
    {RIPPER2_DIE + 1, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Die[2]},
    {RIPPER2_DIE + 2, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Die[3]},
    {RIPPER2_DIE + 3, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Die[4]},
    {RIPPER2_DIE + 4, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Die[5]},
    {RIPPER2_DIE + 5, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Die[6]},
    {RIPPER2_DIE + 6, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Die[7]},
    {RIPPER2_DEAD, RIPPER2_DIE_RATE, &AF(DoActorDebris), &s_Ripper2Die[7]},
};

#define RIPPER2_DEAD_RATE 8

STATE s_Ripper2Dead[] =
{
    {RIPPER2_DIE + 0, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Dead[1]},
    {RIPPER2_DIE + 1, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Dead[2]},
    {RIPPER2_DIE + 2, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Dead[3]},
    {RIPPER2_DIE + 3, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Dead[4]},
    {RIPPER2_DIE + 4, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Dead[5]},
    {RIPPER2_DIE + 5, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Dead[6]},
    {RIPPER2_DIE + 6, RIPPER2_DIE_RATE, &AF(NullRipper2), &s_Ripper2Dead[7]},
    {RIPPER2_DEAD, SF_QUICK_CALL, &AF(QueueFloorBlood), &s_Ripper2Dead[8]},
    {RIPPER2_DEAD, RIPPER2_DEAD_RATE, &AF(DoActorDebris), &s_Ripper2Dead[8]},
};

STATE* sg_Ripper2Die[] =
{
    s_Ripper2Die
};

STATE* sg_Ripper2Dead[] =
{
    s_Ripper2Dead
};

STATE s_Ripper2DeathJump[] =
{
    {RIPPER2_DIE + 0, RIPPER2_DIE_RATE, &AF(DoActorDeathMove), &s_Ripper2DeathJump[0]}
};

STATE s_Ripper2DeathFall[] =
{
    {RIPPER2_DIE + 1, RIPPER2_DIE_RATE, &AF(DoActorDeathMove), &s_Ripper2DeathFall[0]}
};


STATE* sg_Ripper2DeathJump[] =
{
    s_Ripper2DeathJump
};

STATE* sg_Ripper2DeathFall[] =
{
    s_Ripper2DeathFall
};


/*
STATE* *Stand[MAX_WEAPONS];
STATE* *Run;
STATE* *Jump;
STATE* *Fall;
STATE* *Crawl;
STATE* *Swim;
STATE* *Fly;
STATE* *Rise;
STATE* *Sit;
STATE* *Look;
STATE* *Climb;
STATE* *Pain;
STATE* *Death1;
STATE* *Death2;
STATE* *Dead;
STATE* *DeathJump;
STATE* *DeathFall;
STATE* *CloseAttack[2];
STATE* *Attack[6];
STATE* *Special[2];
*/


ACTOR_ACTION_SET Ripper2ActionSet =
{
    sg_Ripper2Stand,
    sg_Ripper2Run,
    sg_Ripper2Jump,
    sg_Ripper2Fall,
    nullptr,                               // sg_Ripper2Crawl,
    nullptr,                               // sg_Ripper2Swim,
    nullptr,                               // sg_Ripper2Fly,
    nullptr,                               // sg_Ripper2Rise,
    nullptr,                               // sg_Ripper2Sit,
    nullptr,                               // sg_Ripper2Look,
    nullptr,                               // climb
    sg_Ripper2Pain,
    sg_Ripper2Die,
    nullptr,                               // sg_Ripper2HariKari,
    sg_Ripper2Dead,
    sg_Ripper2DeathJump,
    sg_Ripper2DeathFall,
    {sg_Ripper2Swipe},
    {1024},
    {sg_Ripper2JumpAttack, sg_Ripper2Kong},
    {500, 1024},
    {sg_Ripper2Heart, sg_Ripper2Hang},
    nullptr,
    nullptr
};

ACTOR_ACTION_SET Ripper2BrownActionSet =
{
    sg_Ripper2Stand,
    sg_Ripper2Run,
    sg_Ripper2Jump,
    sg_Ripper2Fall,
    nullptr,                               // sg_Ripper2Crawl,
    nullptr,                               // sg_Ripper2Swim,
    nullptr,                               // sg_Ripper2Fly,
    nullptr,                               // sg_Ripper2Rise,
    nullptr,                               // sg_Ripper2Sit,
    nullptr,                               // sg_Ripper2Look,
    nullptr,                               // climb
    sg_Ripper2Pain,                      // pain
    sg_Ripper2Die,
    nullptr,                               // sg_Ripper2HariKari,
    sg_Ripper2Dead,
    sg_Ripper2DeathJump,
    sg_Ripper2DeathFall,
    {sg_Ripper2Swipe},
    {1024},
    {sg_Ripper2JumpAttack, sg_Ripper2Kong},
    {400, 1024},
    {sg_Ripper2Heart, sg_Ripper2Hang},
    nullptr,
    nullptr
};

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int SetupRipper2(DSWActor* actor)
{
    if (!(actor->spr.cstat & CSTAT_SPRITE_RESTORE))
    {
        SpawnUser(actor, RIPPER2_RUN_R0, s_Ripper2Run[0]);
        actor->user.Health = HEALTH_RIPPER2;
    }

    ChangeState(actor, s_Ripper2Run[0]);
    actor->user.__legacyState.Attrib = &Ripper2Attrib;
    DoActorSetSpeed(actor, NORM_SPEED);
    actor->user.__legacyState.StateEnd = s_Ripper2Die;
    actor->user.__legacyState.Rot = sg_Ripper2Run;
    actor->clipdist = 32;  // This actor is bigger, needs bigger box.
    actor->spr.scale = DVector2(0.859375, 0.859375);

    if (actor->spr.pal == PALETTE_BROWN_RIPPER)
    {
        EnemyDefaults(actor, &Ripper2BrownActionSet, &Ripper2Personality);
        actor->spr.scale.X += (0.625);
        actor->spr.scale.Y += (0.625);

        if (!(actor->spr.cstat & CSTAT_SPRITE_RESTORE))
            actor->user.Health = HEALTH_MOMMA_RIPPER;

        actor->clipdist += 8;
    }
    else
    {
        EnemyDefaults(actor, &Ripper2ActionSet, &Ripper2Personality);
    }

    actor->user.Flags |= (SPR_XFLIP_TOGGLE);

    return 0;
}

DEFINE_ACTION_FUNCTION(DSWRipper2, Initialize)
{
    PARAM_SELF_PROLOGUE(DSWActor);
    SetupRipper2(self);
    return 0;
}

//---------------------------------------------------------------------------
//
// HANGING - Jumping/Falling/Stationary
//
//---------------------------------------------------------------------------

int InitRipper2Hang(DSWActor* actor)
{
    // Except for the states this is identical to InitRipperHang. Merge them?
    HitInfo hit{};

    bool Found = false;

    for (DAngle dang = nullAngle; dang < DAngle360; dang += DAngle22_5)
    {
        auto tang = actor->spr.Angles.Yaw + dang;

        FAFhitscan(actor->spr.pos.plusZ(-ActorSizeZ(actor)), actor->sector(), DVector3(tang.ToVector() * 1024, 0), hit, CLIPMASK_MISSILE);

        if (hit.hitSector == nullptr)
            continue;

		double dist = (actor->spr.pos.XY() - hit.hitpos.XY()).Length();

		if (hit.hitWall == nullptr || dist < 125 || dist > 437.5)
        {
            continue;
        }

        Found = true;
        actor->spr.Angles.Yaw = tang;
        break;
    }

    if (!Found)
    {
        InitActorDecide(actor);
        return 0;
    }

    NewStateGroup(actor, sg_Ripper2HangJump);
    actor->user.__legacyState.StateFallOverride = sg_Ripper2HangFall;
    DoActorSetSpeed(actor, FAST_SPEED);

    PickJumpMaxSpeed(actor, -(RandomRange(400)+100));

    actor->user.Flags |= (SPR_JUMPING);
    actor->user.Flags &= ~(SPR_FALLING);

    // set up individual actor jump gravity
    actor->user.jump_grav = 8;

    DoJump(actor);

    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int DoRipper2Hang(DSWActor* actor)
{
    if ((actor->user.WaitTics -= ACTORMOVETICS) > 0)
        return 0;

    NewStateGroup(actor, sg_Ripper2JumpAttack);
    // move to the 2nd frame - past the pause frame
    actor->user.Tics += actor->user.__legacyState.State->Tics;

    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int DoRipper2MoveHang(DSWActor* actor)
{
    // if cannot move the sprite
    if (!move_actor(actor, DVector3(actor->spr.Angles.Yaw.ToVector() * actor->vel.X, 0)))
    {
        if (actor->user.coll.type == kHitWall)
        {
            short hit_wall;
            short w, nw;

            // Don't keep clinging and going ever higher!
            if (abs(actor->spr.pos.Z - actor->user.targetActor->spr.pos.Z) > 250)
                return 0;

            actor->setStateGroup(NAME_Special);
            if (RANDOM_P2(1024<<8)>>8 > 500)
                actor->user.WaitTics = ((RANDOM_P2(2 << 8) >> 8) * 120);
            else
                actor->user.WaitTics = 0; // Double jump

            // hang flush with the wall
			actor->spr.Angles.Yaw = actor->user.coll.hitWall->delta().Angle() - DAngle90;

            return 0;
        }
    }

    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------
int DoRipper2QuickJump(DSWActor* actor);

int DoRipper2HangJF(DSWActor* actor)
{
    if (actor->user.Flags & (SPR_JUMPING | SPR_FALLING))
    {
        if (actor->user.Flags & (SPR_JUMPING))
            DoJump(actor);
        else
            DoFall(actor);
    }

    if (!(actor->user.Flags & (SPR_JUMPING | SPR_FALLING)))
    {
        if (DoRipper2QuickJump(actor))
            return 0;

        InitActorDecide(actor);
    }

    DoRipper2MoveHang(actor);

    return 0;

}

//---------------------------------------------------------------------------
//
// JUMP ATTACK
//
//---------------------------------------------------------------------------

int DoRipper2BeginJumpAttack(DSWActor* actor)
{
    DSWActor* target = actor->user.targetActor;

	// Always jump at player if mad.

	auto vec = (target->spr.pos.XY() - actor->spr.pos.XY()).Unit() * 8;
	Collision coll = move_sprite(actor, DVector3(vec, 0), actor->user.ceiling_dist, actor->user.floor_dist, CLIPMASK_ACTOR, ACTORMOVETICS);

	if (coll.type != kHitNone)
		actor->spr.Angles.Yaw += RandomAngle(DAngle45) + DAngle180 - DAngle22_5;
	else
		actor->spr.Angles.Yaw = vec.Angle();

    DoActorSetSpeed(actor, FAST_SPEED);

    PickJumpMaxSpeed(actor, -(RandomRange(400)+100));

    actor->user.Flags |= (SPR_JUMPING);
    actor->user.Flags &= ~(SPR_FALLING);

    // set up individual actor jump gravity
    actor->user.jump_grav = 8;

    // if I didn't do this here they get stuck in the air sometimes
    DoActorZrange(actor);

    DoJump(actor);

    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int DoRipper2MoveJump(DSWActor* actor)
{
    if (actor->user.Flags & (SPR_JUMPING | SPR_FALLING))
    {
        if (actor->user.Flags & (SPR_JUMPING))
            DoJump(actor);
        else
            DoFall(actor);
    }

    if (!(actor->user.Flags & (SPR_JUMPING | SPR_FALLING)))
    {
        if (DoRipper2QuickJump(actor))
            return 0;

        InitActorDecide(actor);
    }

    DoRipper2MoveHang(actor);
    return 0;
}

//---------------------------------------------------------------------------
//
// STD MOVEMENT
//
//---------------------------------------------------------------------------

int DoRipper2QuickJump(DSWActor* actor)
{
    // Tests to see if ripper2 is on top of a player/enemy and then immediatly
    // does another jump

    DSWActor* low = actor->user.lowActor;
    if (low)
    {
        if ((low->spr.extra & SPRX_PLAYER_OR_ENEMY))
        {
            NewStateGroup(actor, sg_Ripper2JumpAttack);
            // move past the first state
            actor->user.Tics = 30;
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int NullRipper2(DSWActor* actor)
{
    if (actor->user.Flags & (SPR_SLIDING))
        DoActorSlide(actor);

    DoActorSectorDamage(actor);

    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int DoRipper2Pain(DSWActor* actor)
{
    NullRipper2(actor);

    if ((actor->user.WaitTics -= ACTORMOVETICS) <= 0)
        InitActorDecide(actor);
    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int DoRipper2RipHeart(DSWActor* actor)
{
    DSWActor* target = actor->user.targetActor;
    NewStateGroup(actor, sg_Ripper2Heart);
    actor->user.WaitTics = 6 * 120;

    // player face ripper2
    target->spr.Angles.Yaw = (actor->spr.pos - target->spr.pos).Angle();
    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int DoRipper2StandHeart(DSWActor* actor)
{
    NullRipper2(actor);

    if (!SoundValidAndActive(actor, CHAN_RipHeart))
        PlaySound(DIGI_RIPPER2HEARTOUT, actor, v3df_none, CHAN_RipHeart);

    if ((actor->user.WaitTics -= ACTORMOVETICS) <= 0)
        NewStateGroup(actor, sg_Ripper2Run);
    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

void Ripper2Hatch(DSWActor* actor)
{
	const int MAX_RIPPER2S = 1;

    for (int i = 0; i < MAX_RIPPER2S; i++)
    {
        auto actorNew = insertActor(actor->sector(), STAT_DEFAULT);
        ClearOwner(actorNew);
        actorNew->spr.pos = actor->spr.pos;

        actorNew->spr.scale = DVector2(1, 1);
        actorNew->spr.Angles.Yaw = RandomAngle();
        actorNew->spr.pal = 0;
        actorNew->spr.shade = -10;
        SetupRipper2(actorNew);

        // make immediately active
        actorNew->user.Flags |= (SPR_ACTIVE);

        actorNew->setStateGroup(NAME_Jump);
        actorNew->user.ActorActionFunc = AF(DoActorMoveJump);
        DoActorSetSpeed(actorNew, FAST_SPEED);
        PickJumpMaxSpeed(actorNew, -600);

        actorNew->user.Flags |= (SPR_JUMPING);
        actorNew->user.Flags &= ~(SPR_FALLING);

        actorNew->user.jump_grav = 8;

        // if I didn't do this here they get stuck in the air sometimes
        DoActorZrange(actorNew);

        DoJump(actorNew);
    }
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int DoRipper2Move(DSWActor* actor)
{
    if (actor->spr.hitag == TAG_SWARMSPOT && actor->spr.lotag == 1)
        DoCheckSwarm(actor);

    if (actor->user.scale_speed)
    {
        DoScaleSprite(actor);
    }

    if (actor->user.Flags & (SPR_JUMPING | SPR_FALLING))
    {
        if (actor->user.Flags & (SPR_JUMPING))
            DoJump(actor);
        else
            DoFall(actor);
    }

    // if on a player/enemy sprite jump quickly
    if (!(actor->user.Flags & (SPR_JUMPING | SPR_FALLING)))
    {
        if (DoRipper2QuickJump(actor))
            return 0;

        KeepActorOnFloor(actor);
    }

    if (actor->user.Flags & (SPR_SLIDING))
        DoActorSlide(actor);

    if (actor->user.track >= 0)
        ActorFollowTrack(actor, ACTORMOVETICS);
    else
        actor->callAction();

    DoActorSectorDamage(actor);

    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int InitRipper2Charge(DSWActor* actor)
{
    DoActorSetSpeed(actor, FAST_SPEED);

    InitActorMoveCloser(actor);

    NewStateGroup(actor, sg_Ripper2RunFast);

    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

int ChestRipper2(DSWActor* actor)
{
    PlaySound(DIGI_RIPPER2CHEST, actor, v3df_follow);
    return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

#include "saveable.h"


static saveable_data saveable_ripper2_data[] =
{
    SAVE_DATA(Ripper2Personality),

    SAVE_DATA(Ripper2Attrib),

    SAVE_DATA(s_Ripper2Run),
    SAVE_DATA(sg_Ripper2Run),
    SAVE_DATA(s_Ripper2RunFast),
    SAVE_DATA(sg_Ripper2RunFast),
    SAVE_DATA(s_Ripper2Stand),
    SAVE_DATA(sg_Ripper2Stand),
    SAVE_DATA(s_Ripper2Swipe),
    SAVE_DATA(sg_Ripper2Swipe),
    SAVE_DATA(s_Ripper2Kong),
    SAVE_DATA(sg_Ripper2Kong),
    SAVE_DATA(s_Ripper2Heart),
    SAVE_DATA(sg_Ripper2Heart),
    SAVE_DATA(s_Ripper2Hang),
    SAVE_DATA(sg_Ripper2Hang),
    SAVE_DATA(s_Ripper2Pain),
    SAVE_DATA(sg_Ripper2Pain),
    SAVE_DATA(s_Ripper2Jump),
    SAVE_DATA(sg_Ripper2Jump),
    SAVE_DATA(s_Ripper2Fall),
    SAVE_DATA(sg_Ripper2Fall),
    SAVE_DATA(s_Ripper2JumpAttack),
    SAVE_DATA(sg_Ripper2JumpAttack),
    SAVE_DATA(s_Ripper2HangJump),
    SAVE_DATA(sg_Ripper2HangJump),
    SAVE_DATA(s_Ripper2HangFall),
    SAVE_DATA(sg_Ripper2HangFall),
    SAVE_DATA(s_Ripper2Die),
    SAVE_DATA(s_Ripper2Dead),
    SAVE_DATA(sg_Ripper2Die),
    SAVE_DATA(sg_Ripper2Dead),
    SAVE_DATA(s_Ripper2DeathJump),
    SAVE_DATA(s_Ripper2DeathFall),
    SAVE_DATA(sg_Ripper2DeathJump),
    SAVE_DATA(sg_Ripper2DeathFall),

    SAVE_DATA(Ripper2ActionSet),
    SAVE_DATA(Ripper2BrownActionSet),
};

saveable_module saveable_ripper2 =
{
    // code
    nullptr, 0,

    // data
    saveable_ripper2_data,
    SIZ(saveable_ripper2_data)
};
END_SW_NS
