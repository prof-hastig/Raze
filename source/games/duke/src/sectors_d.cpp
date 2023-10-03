//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2003 - 3D Realms Entertainment
Copyright (C) 2000, 2003 - Matt Saettler (EDuke Enhancements)
Copyright (C) 2020 - Christoph Oelckers

This file is part of Enhanced Duke Nukem 3D version 1.5 - Atomic Edition

Duke Nukem 3D is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Todd Replogle
Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms

EDuke enhancements integrated: 04/13/2003 - Matt Saettler

Note: EDuke source was in transition.  Changes are in-progress in the
source as it is released.

*/
//-------------------------------------------------------------------------

#include "ns.h"
#include "global.h"
#include "sounds.h"
#include "mapinfo.h"
#include "dukeactor.h"
#include "secrets.h"

// PRIMITIVE
BEGIN_DUKE_NS

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

bool checkaccessswitch_d(int snum, int switchpal, DDukeActor* act, walltype* wwal)
{
	if (getPlayer(snum)->access_incs == 0)
	{
		if (switchpal == 0)
		{
			if ((getPlayer(snum)->got_access & 1))
				getPlayer(snum)->access_incs = 1;
			else FTA(70, getPlayer(snum));
		}

		else if (switchpal == 21)
		{
			if (getPlayer(snum)->got_access & 2)
				getPlayer(snum)->access_incs = 1;
			else FTA(71, getPlayer(snum));
		}

		else if (switchpal == 23)
		{
			if (getPlayer(snum)->got_access & 4)
				getPlayer(snum)->access_incs = 1;
			else FTA(72, getPlayer(snum));
		}

		if (getPlayer(snum)->access_incs == 1)
		{
			if (!act)
				getPlayer(snum)->access_wall = wwal;
			else
				getPlayer(snum)->access_spritenum = act;
		}

		return 1;
	}
	return 0;
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

void activatebysector_d(sectortype* sect, DDukeActor* activator)
{
	int didit = 0;

	DukeSectIterator it(sect);
	while (auto act = it.Next())
	{
		if (isactivator(act))
		{
			operateactivators(act->spr.lotag, nullptr);
			didit = 1;
			//			return;
		}
	}

	if (didit == 0)
		operatesectors(sect, activator);
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

void checkplayerhurt_d(DDukePlayer* p, const Collision& coll)
{
	if (coll.type == kHitSprite)
	{
		CallOnHurt(coll.actor(), p);
		return;
	}

	if (coll.type != kHitWall) return;
	auto wal = coll.hitWall;

	if (p->hurt_delay > 0) p->hurt_delay--;
	else if (wal->cstat & (CSTAT_WALL_BLOCK | CSTAT_WALL_ALIGN_BOTTOM | CSTAT_WALL_MASKED | CSTAT_WALL_BLOCK_HITSCAN))
	{
		int tf = tileflags(wal->overtexture);
		if (tf & TFLAG_ANIMFORCEFIELD)
		{
			p->GetActor()->spr.extra -= 5;

			p->hurt_delay = 16;
			SetPlayerPal(p, PalEntry(32, 32, 0, 0));

			p->vel.XY() = -p->GetActor()->spr.Angles.Yaw.ToVector() * 16;
			S_PlayActorSound(DUKE_LONGTERM_PAIN, p->GetActor());

			checkhitwall(p->GetActor(), wal, p->GetActor()->getPosWithOffsetZ() + p->GetActor()->spr.Angles.Yaw.ToVector() * 2);
		}
		else if (tf & TFLAG_FORCEFIELD)
		{
			p->hurt_delay = 26;
			checkhitwall(p->GetActor(), wal, p->GetActor()->getPosWithOffsetZ() + p->GetActor()->spr.Angles.Yaw.ToVector() * 2);
		}
	}
}

//---------------------------------------------------------------------------
//
// taken out of checksectors to eliminate some gotos.
//
//---------------------------------------------------------------------------

void clearcameras(DDukePlayer* p)
{
	p->GetActor()->restorepos();
	p->newOwner = nullptr;

	updatesector(p->GetActor()->getPosWithOffsetZ(), &p->cursector);

	DukeStatIterator it(STAT_ACTOR);
	while (auto act = it.Next())
	{
		if ((act->flags2 & SFLAG2_CAMERA)) act->spr.yint = 0;
	}
}

//---------------------------------------------------------------------------
//
// 
//
//---------------------------------------------------------------------------

void checksectors_d(int snum)
{
	int i = -1;
	DDukePlayer* p;
	walltype* hitscanwall;
	HitInfo near;

	p = getPlayer(snum);
	auto pact = p->GetActor();

	if (!p->insector()) return;

	switch (p->cursector->lotag)
	{

	case 32767:
		p->cursector->lotag = 0;
		FTA(9, p);
		Level.addSecret(snum);
		SECRET_Trigger(sectindex(p->cursector));
		return;
	case -1:
		p->cursector->lotag = 0;
		setnextmap(false);
		return;
	case -2:
		p->cursector->lotag = 0;
		p->timebeforeexit = 26 * 8;
		p->customexitsound = p->cursector->hitag;
		return;
	default:
		if (p->cursector->lotag >= 10000 && p->cursector->lotag < 16383)
		{
			if (snum == screenpeek || ud.coop == 1)
				S_PlayActorSound(p->cursector->lotag - 10000, pact);
			p->cursector->lotag = 0;
		}
		break;

	}

	//After this point the the player effects the map with space

	if (chatmodeon || p->GetActor()->spr.extra <= 0) return;

	if (ud.cashman && PlayerInput(snum, SB_OPEN))
		lotsofstuff(p->GetActor(), 2, DukeMailClass);

	if (p->newOwner != nullptr)
	{
		if (abs(p->cmd.ucmd.vel.Y) > 0.75 || abs(p->cmd.ucmd.vel.X) > 0.75)
		{
			clearcameras(p);
			return;
		}
		else if (PlayerInput(snum, SB_ESCAPE))
		{
			clearcameras(p);
			return;
		}
	}

	if (!(PlayerInput(snum, SB_OPEN)))
		p->toggle_key_flag = 0;

	else if (!p->toggle_key_flag)
	{
		near.hitActor = nullptr;
		p->toggle_key_flag = 1;
		hitscanwall = nullptr;

		double dist = hitawall(p, &hitscanwall);

		if (hitscanwall != nullptr)
		{
			if (dist < 80 && hitscanwall->overtexture == mirrortex)
				if (hitscanwall->lotag > 0 && S_CheckSoundPlaying(hitscanwall->lotag) == 0 && snum == screenpeek)
				{
					S_PlayActorSound(hitscanwall->lotag, pact);
					return;
				}

			if (hitscanwall != nullptr && (hitscanwall->cstat & CSTAT_WALL_MASKED))
				if (hitscanwall->lotag)
					return;
		}
		if (p->newOwner != nullptr)
			neartag(p->GetActor()->getPrevPosWithOffsetZ(), p->GetActor()->sector(), p->GetActor()->PrevAngles.Yaw, near, 80., NT_Lotag);
		else
		{
			neartag(p->GetActor()->getPosWithOffsetZ(), p->GetActor()->sector(), p->GetActor()->PrevAngles.Yaw, near, 80., NT_Lotag);
			if (near.actor() == nullptr && near.hitWall == nullptr && near.hitSector == nullptr)
				neartag(p->GetActor()->getPosWithOffsetZ().plusZ(8), p->GetActor()->sector(), p->GetActor()->PrevAngles.Yaw, near, 80., NT_Lotag);
			if (near.actor() == nullptr && near.hitWall == nullptr && near.hitSector == nullptr)
				neartag(p->GetActor()->getPosWithOffsetZ().plusZ(16), p->GetActor()->sector(), p->GetActor()->PrevAngles.Yaw, near, 80., NT_Lotag);
			if (near.actor() == nullptr && near.hitWall == nullptr && near.hitSector == nullptr)
			{
				neartag(p->GetActor()->getPosWithOffsetZ().plusZ(16), p->GetActor()->sector(), p->GetActor()->PrevAngles.Yaw, near, 80., NT_Lotag | NT_Hitag);
				if (near.actor() != nullptr)
				{
					if (near.actor()->flags2 & SFLAG2_TRIGGERRESPAWN)
						return;
				}

				near.clearObj();
			}
		}

		if (p->newOwner == nullptr && near.actor() == nullptr && near.hitWall == nullptr && near.hitSector == nullptr)
			if (isanunderoperator(p->GetActor()->sector()->lotag))
				near.hitSector = p->GetActor()->sector();

		if (near.hitSector && (near.hitSector->lotag & 16384))
			return;

		if (near.actor() == nullptr && near.hitWall == nullptr)
			if (p->cursector->lotag == ST_2_UNDERWATER)
			{
				DDukeActor* hit;
				dist = hitasprite(p->GetActor(), &hit);
				if (hit) near.hitActor = hit;
				if (dist > 80) near.hitActor = nullptr;

			}

		auto const neartagsprite = near.actor();
		if (neartagsprite != nullptr)
		{
			if (checkhitswitch(snum, nullptr, neartagsprite)) return;

			if (CallOnUse(neartagsprite, p))
				return;
		}

		if (!PlayerInput(snum, SB_OPEN)) return;
		else if (p->newOwner != nullptr)
		{
			clearcameras(p);
			return;
		}

		if (near.hitWall == nullptr && near.hitSector == nullptr && near.actor() == nullptr)
			if (hits(p->GetActor()) < 32)
			{
				if ((krand() & 255) < 16)
					S_PlayActorSound(DUKE_SEARCH2, pact);
				else S_PlayActorSound(DUKE_SEARCH, pact);
				return;
			}

		if (near.hitWall)
		{
			if (near.hitWall->lotag > 0 && isadoorwall(near.hitWall->walltexture))
			{
				if (hitscanwall == near.hitWall || hitscanwall == nullptr)
					checkhitswitch(snum, near.hitWall, nullptr);
				return;
			}
			else if (p->newOwner != nullptr)
			{
				clearcameras(p);
				return;
			}
		}

		if (near.hitSector && (near.hitSector->lotag & 16384) == 0 && isanearoperator(near.hitSector->lotag))
		{
			DukeSectIterator it(near.hitSector);
			while (auto act = it.Next())
			{
				if (isactivator(act) || ismasterswitch(act))
					return;
			}
			operatesectors(near.hitSector, p->GetActor());
		}
		else if ((p->GetActor()->sector()->lotag & 16384) == 0)
		{
			if (isanunderoperator(p->GetActor()->sector()->lotag))
			{
				DukeSectIterator it(p->GetActor()->sector());
				while (auto act = it.Next())
				{
					if (isactivator(act) || ismasterswitch(act)) return;
				}
				operatesectors(p->GetActor()->sector(), p->GetActor());
			}
			else checkhitswitch(snum, near.hitWall, nullptr);
		}
	}
}





END_DUKE_NS
