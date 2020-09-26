//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2003 - 3D Realms Entertainment
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

*/
//-------------------------------------------------------------------------

#include "ns.h"	// Must come before everything else!

#include "g_input.h"
#include "duke3d.h"

#include "superfasthash.h"
#include "gamecvars.h"
#include "gamecontrol.h"
#include "c_bind.h"
#include "menu/menu.h"
#include "gstrings.h"
#include "version.h"
#include "names.h"
#include "mapinfo.h"
#include "gamestate.h"
#include "../../glbackend/glbackend.h"


BEGIN_DUKE_NS

//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------

static void Menu_DrawBackground(const DVector2 &origin)
{
	DrawTexture(twod, tileGetTexture(TILE_MENUSCREEN), origin.X + 160, origin.Y + 100, DTA_FullscreenScale, FSMode_Fit320x200, DTA_Color, 0xff808080, DTA_CenterOffset, true, TAG_DONE);
}

//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------

static void Menu_DrawCursor(double x, double y, double scale, bool right)
{
	int mclock = I_GetBuildTime();
	const int frames = isRR() ? 16 : 7;
	int picnum;
	if (!right) picnum = TILE_SPINNINGNUKEICON + ((mclock >> 3) % frames);
	else picnum = TILE_SPINNINGNUKEICON + frames - 1 - ((frames - 1 + (mclock >> 3)) % frames);
	int light = int(224 + 31 * sin(mclock / 20.));
	PalEntry pe(255, light, light, light);
	DrawTexture(twod, tileGetTexture(picnum), x, y, DTA_FullscreenScale, FSMode_Fit320x200, DTA_ScaleX, scale, DTA_ScaleY, scale, DTA_Color, pe, DTA_CenterOffsetRel, true, TAG_DONE);
}

//----------------------------------------------------------------------------
//
// Implements the native looking menu used for the main menu
// and the episode/skill selection screens, i.e. the parts
// that need to look authentic
//
//----------------------------------------------------------------------------

class DukeListMenu : public DListMenu
{
	using Super = DListMenu;
protected:

	void Ticker() override
	{
		// Lay out the menu.
		int y_upper = mDesc->mYpos;
		int y_lower = y_upper + mDesc->mYbotton;
		int y = 0;
		int spacing = 0;
		const int height = 15; // cannot take value from the font because it would be inconsistent

		int totalheight = 0, numvalidentries = mDesc->mItems.Size();

		for (unsigned e = 0; e < mDesc->mItems.Size(); ++e)
		{
			auto entry = mDesc->mItems[e];
			entry->mHidden = false;
			entry->SetHeight(height);
			totalheight += height;
		}
		if (mDesc->mSpacing <= 0) spacing = std::max(0, (y_lower - y_upper - totalheight) / (numvalidentries > 1 ? numvalidentries - 1 : 1));
		if (spacing <= 0) spacing = mDesc->mSpacing;

		int totalHeight;
		for (unsigned e = 0; e < mDesc->mItems.Size(); ++e)
		{
			auto entry = mDesc->mItems[e];
			if (!entry->mHidden)
			{
				entry->SetY(y_upper + y);
				y += height;
				totalHeight = y;
				y += spacing;
			}
		}
	}
};

//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------

class DukeMainMenu : public DukeListMenu
{
	virtual void Init(DMenu* parent = nullptr, FListMenuDescriptor* desc = nullptr) override
	{
		DukeListMenu::Init(parent, desc);
	}

	void PreDraw() override
	{
		DukeListMenu::PreDraw();
		double x = origin.X + 160;
		if (isRRRA())
		{
			DrawTexture(twod, tileGetTexture(TILE_THREEDEE), x-5, origin.Y+57, DTA_FullscreenScale, FSMode_Fit320x200Top, DTA_ScaleX, 0.253, DTA_ScaleY, 0.253, DTA_CenterOffsetRel, true, TAG_DONE);
		}
		else if (isRR())
		{
			DrawTexture(twod, tileGetTexture(TILE_INGAMEDUKETHREEDEE), x+5, origin.Y + 24, DTA_FullscreenScale, FSMode_Fit320x200Top, DTA_ScaleX, 0.36, DTA_ScaleY, 0.36, DTA_CenterOffsetRel, true, TAG_DONE);
		}
		else
		{
			DrawTexture(twod, tileGetTexture(TILE_INGAMEDUKETHREEDEE), x, origin.Y + 29, DTA_FullscreenScale, FSMode_Fit320x200Top, DTA_CenterOffsetRel, true, TAG_DONE);
			if (PLUTOPAK)
			{
				int mclock = I_GetBuildTime();
				int light = 224 + 31 * sin(mclock / 40.);
				PalEntry pe(255, light, light, light);
				DrawTexture(twod, tileGetTexture(TILE_PLUTOPAKSPRITE + 2), x + 100, origin.Y + 36, DTA_FullscreenScale, FSMode_Fit320x200Top, DTA_Color, pe, DTA_CenterOffsetRel, true, TAG_DONE);
			}
		}
		
	}
};


//----------------------------------------------------------------------------
//
// Menu related game interface functions
//
//----------------------------------------------------------------------------

void GameInterface::DrawNativeMenuText(int fontnum, int state, double oxpos, double ypos, float fontscale, const char* text, int flags)
{
	double xpos = oxpos;
	int trans;
	PalEntry pe;

	double scale = isRR() ? 0.4 : 1.;
	if (flags & LMF_Centered) xpos -= BigFont->StringWidth(text) * scale * 0.5;

	if (state == NIT_InactiveState)
	{
		trans = TRANSLATION(Translation_Remap, 1);
		pe = 0xffffffff;
	}
	else if (state == NIT_SelectedState)
	{
		trans = 0;
		int mclock = I_GetBuildTime();
		int light = 224 + 31 * sin(mclock / 20.);
		pe = PalEntry(255, light, light, light);
	}
	else
	{
		trans = 0;
		pe = 0xffa0a0a0;
	}

	DrawText(twod, BigFont, CR_UNDEFINED, xpos, ypos, text, DTA_FullscreenScale, FSMode_Fit320x200, DTA_ScaleX, scale, DTA_ScaleY, scale, DTA_Color, pe,
		DTA_TranslationIndex, trans, TAG_DONE);

	if (state == NIT_SelectedState)
	{
		const int cursorOffset = 110;
		const double cursorScale = isRR() ? 0.2 : 1.0;
		const double ymid = ypos + 7;	// half height must be hardcoded or layouts will break.
		if (flags & LMF_Centered)
		{
			Menu_DrawCursor(oxpos + cursorOffset, ymid, cursorScale, false);
			Menu_DrawCursor(oxpos - cursorOffset, ymid, cursorScale, true);
		}
		else
			Menu_DrawCursor(oxpos - cursorOffset, ymid, cursorScale, false);
	}

}

void GameInterface::MenuOpened()
{
	StopCommentary();
	if (ud.multimode < 2)
	{
		screenpeek = myconnectindex;
	}
}

void GameInterface::MenuSound(EMenuSounds snd)
{
	switch (snd)
	{
	case ActivateSound:
		S_MenuSound();
		break;

	case ChooseSound:
	case CursorSound:
		S_PlaySound(isRR() ? 335 : KICK_HIT, CHAN_AUTO, CHANF_UI);
		break;

	case AdvanceSound:
		S_PlaySound(isRR() ? 341 : PISTOL_BODYHIT, CHAN_AUTO, CHANF_UI);
		break;

	case CloseSound:
	case BackSound:
		S_PlaySound(EXITMENUSOUND, CHAN_AUTO, CHANF_UI);
		break;

	default:
		return;
	}
}

void GameInterface::MenuClosed()
{
}

bool GameInterface::CanSave()
{
	if (ud.recstat == 2 || gamestate != GS_LEVEL) return false;
	auto &myplayer = ps[myconnectindex];
	return (sprite[myplayer.i].extra > 0);
}

bool GameInterface::StartGame(FNewGameStartup& gs)
{
	if (gs.Episode >= 1)
	{
		if (g_gameType & GAMEFLAG_SHAREWARE)
		{
			M_StartMessage(GStrings("BUYDUKE"), 1, -1);
			return false;
		}
	}

	int32_t skillsound = PISTOL_BODYHIT;

	soundEngine->StopAllChannels();
	
	static const short sounds_d[] = { JIBBED_ACTOR6, BONUS_SPEECH1, DUKE_GETWEAPON2, JIBBED_ACTOR5, JIBBED_ACTOR5 };
	static const short sounds_r[] = { 427, 428, 196, 195, 197 };
	if (gs.Skill >=0 && gs.Skill <= 5) skillsound = isRR()? sounds_r[gs.Skill] : sounds_d[gs.Skill];

	if (menu_sounds && skillsound >= 0 && SoundEnabled() && !netgame)
	{
		S_PlaySound(skillsound, CHAN_AUTO, CHANF_UI);

		while (S_CheckSoundPlaying(skillsound))
		{
			gi->UpdateSounds();
			soundEngine->UpdateSounds(I_GetTime());
			I_GetEvent();
		}
		Net_ClearFifo();
	}
	auto map = FindMapByLevelNum(levelnum(gs.Episode, gs.Level));
	if (map)
	{
		DeferedStartGame(map, gs.Skill);
		return true;
	}
	return false;

}

FSavegameInfo GameInterface::GetSaveSig()
{
	return { SAVESIG_RR, MINSAVEVER_RR, SAVEVER_RR };
}

void GameInterface::DrawMenuCaption(const DVector2& origin, const char* text)
{
	DrawTexture(twod, tileGetTexture(TILE_MENUBAR), origin.X + 160, origin.Y + 19, DTA_FullscreenScale, FSMode_Fit320x200Top, DTA_Color, 0xff808080, DTA_CenterOffsetRel, 1, TAG_DONE);

	FString t = text;
	size_t newlen = t.Len();
	if (t[t.Len() - 1] == ':') newlen--;
	if (newlen > 63) newlen = 63;
	t.Truncate(newlen);
	double scale = isRR() ? 0.4 : 1.0;
	double x = 160 + origin.X - BigFont->StringWidth(t) * scale * 0.5;
	DrawText(twod, BigFont, CR_UNTRANSLATED, x, origin.Y + 12, t, DTA_FullscreenScale, FSMode_Fit320x200Top, DTA_ScaleX, scale, DTA_ScaleY, scale, TAG_DONE);
}

void GameInterface::DrawCenteredTextScreen(const DVector2 &origin, const char *text, int position, bool bg)
{
	if (bg) Menu_DrawBackground(origin);
	else if (!isRR())
	{
		Menu_DrawCursor(160, 130, 1, false);
	}
	::GameInterface::DrawCenteredTextScreen(origin, text, position, bg);
}

void GameInterface::DrawPlayerSprite(const DVector2& origin, bool onteam)
{
	int mclock = I_GetBuildTime();
	int color = TRANSLATION(Translation_Remap, playercolor2lookup(playercolor));
	int tile = isRR() ? 3845 + 36 - ((((8 - (mclock >> 4))) & 7) * 5) : 1441 - ((((4 - (mclock >> 4))) & 3) * 5);
	auto tex = tileGetTexture(tile);
	if (!tex) return;
	double x = origin.X + 260, y = origin.Y + tex->GetDisplayHeight() * (isRR()? 0.25 : 0.5);
	double scale = isRR() ? 0.375 : 0.75;

	DrawTexture(twod, tex, x, y, DTA_FullscreenScale, FSMode_Fit320x200, DTA_TranslationIndex, color, DTA_ScaleX, scale, DTA_ScaleY, scale, TAG_DONE);
}

void GameInterface::QuitToTitle()
{
	gameaction = ga_startup;
}

//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------

static void shadowminitext(int32_t xx, int32_t yy, const char* t, int32_t p)
{
	double x = FixedToFloat(xx);
	double y = FixedToFloat(yy);

	DrawText(twod, SmallFont2, CR_UNDEFINED, x + 1, y + 1, t, DTA_FullscreenScale, FSMode_Fit320x200, DTA_Color, 0xff000000, DTA_Alpha, 0.5, TAG_DONE);
	DrawText(twod, SmallFont2, CR_UNDEFINED, x, y, t, DTA_FullscreenScale, FSMode_Fit320x200, DTA_TranslationIndex, TRANSLATION(Translation_Remap, p), TAG_DONE);
}

static void mgametextcenter(int32_t xx, int32_t yy, const char* t)
{
	double x = FixedToFloat(xx) + 160. - SmallFont->StringWidth(t) * 0.5;
	double y = FixedToFloat(yy);

	DrawText(twod, SmallFont, CR_UNDEFINED, x, y + 2, t, DTA_FullscreenScale, FSMode_Fit320x200, TAG_DONE);
}

//----------------------------------------------------------------------------
//
// allows the front end to override certain fullscreen image menus
// with custom implementations.
//
// This is needed because the credits screens in Duke Nukem
// are either done by providing an image or by printing text, based on the version used.
//
//----------------------------------------------------------------------------

bool GameInterface::DrawSpecialScreen(const DVector2& origin, int tilenum)
{
	// Older versions of Duke Nukem create the credits screens manually.
	// On the latest version there's real graphics for this.
	bool haveCredits = !(g_gameType & GAMEFLAG_DUKE) || (VOLUMEALL && PLUTOPAK);

	int32_t m, l;
	if (!haveCredits)
	{
		if (tilenum == 2504)
		{
			Menu_DrawBackground(origin);
			DrawMenuCaption(origin, GStrings("MNU_CREDITS"));
			m = int(origin.X * 65536) + (20 << 16);
			l = int(origin.Y * 65536) + (33 << 16);

			shadowminitext(m, l, "Original Concept", 12); l += 7 << 16;
			shadowminitext(m, l, "Todd Replogle and Allen H. Blum III", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Produced & Directed By", 12); l += 7 << 16;
			shadowminitext(m, l, "Greg Malone", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Executive Producer", 12); l += 7 << 16;
			shadowminitext(m, l, "George Broussard", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "BUILD Engine", 12); l += 7 << 16;
			shadowminitext(m, l, "Ken Silverman", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Game Programming", 12); l += 7 << 16;
			shadowminitext(m, l, "Todd Replogle", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "3D Engine/Tools/Net", 12); l += 7 << 16;
			shadowminitext(m, l, "Ken Silverman", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Network Layer/Setup Program", 12); l += 7 << 16;
			shadowminitext(m, l, "Mark Dochtermann", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Map Design", 12); l += 7 << 16;
			shadowminitext(m, l, "Allen H. Blum III", 12); l += 7 << 16;
			shadowminitext(m, l, "Richard Gray", 12); l += 7 << 16;

			m = int(origin.X * 65536) + (180 << 16);
			l = int(origin.Y * 65536) + (33 << 16);

			shadowminitext(m, l, "3D Modeling", 12); l += 7 << 16;
			shadowminitext(m, l, "Chuck Jones", 12); l += 7 << 16;
			shadowminitext(m, l, "Sapphire Corporation", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Artwork", 12); l += 7 << 16;
			shadowminitext(m, l, "Dirk Jones, Stephen Hornback", 12); l += 7 << 16;
			shadowminitext(m, l, "James Storey, David Demaret", 12); l += 7 << 16;
			shadowminitext(m, l, "Douglas R. Wood", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Sound Engine", 12); l += 7 << 16;
			shadowminitext(m, l, "Jim Dose", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Sound & Music Development", 12); l += 7 << 16;
			shadowminitext(m, l, "Robert Prince", 12); l += 7 << 16;
			shadowminitext(m, l, "Lee Jackson", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Voice Talent", 12); l += 7 << 16;
			shadowminitext(m, l, "Lani Minella - Voice Producer", 12); l += 7 << 16;
			shadowminitext(m, l, "Jon St. John as \"Duke Nukem\"", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Graphic Design", 12); l += 7 << 16;
			shadowminitext(m, l, "Packaging, Manual, Ads", 12); l += 7 << 16;
			shadowminitext(m, l, "Robert M. Atkins", 12); l += 7 << 16;
			shadowminitext(m, l, "Michael Hadwin", 12); l += 7 << 16;
			return true;
		}
		else if (tilenum == 2505)
		{
			Menu_DrawBackground(origin);
			DrawMenuCaption(origin, GStrings("MNU_CREDITS"));
			m = int(origin.X * 65536) + (20 << 16);
			l = int(origin.Y * 65536) + (33 << 16);

			shadowminitext(m, l, "Special Thanks To", 12); l += 7 << 16;
			shadowminitext(m, l, "Steven Blackburn, Tom Hall", 12); l += 7 << 16;
			shadowminitext(m, l, "Scott Miller, Joe Siegler", 12); l += 7 << 16;
			shadowminitext(m, l, "Terry Nagy, Colleen Compton", 12); l += 7 << 16;
			shadowminitext(m, l, "HASH, Inc., FormGen, Inc.", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "The 3D Realms Beta Testers", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Nathan Anderson, Wayne Benner", 12); l += 7 << 16;
			shadowminitext(m, l, "Glenn Brensinger, Rob Brown", 12); l += 7 << 16;
			shadowminitext(m, l, "Erik Harris, Ken Heckbert", 12); l += 7 << 16;
			shadowminitext(m, l, "Terry Herrin, Greg Hively", 12); l += 7 << 16;
			shadowminitext(m, l, "Hank Leukart, Eric Baker", 12); l += 7 << 16;
			shadowminitext(m, l, "Jeff Rausch, Kelly Rogers", 12); l += 7 << 16;
			shadowminitext(m, l, "Mike Duncan, Doug Howell", 12); l += 7 << 16;
			shadowminitext(m, l, "Bill Blair", 12); l += 7 << 16;

			m = int(origin.X * 65536) + (160 << 16);
			l = int(origin.Y * 65536) + (33 << 16);

			shadowminitext(m, l, "Company Product Support", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "The following companies were cool", 12); l += 7 << 16;
			shadowminitext(m, l, "enough to give us lots of stuff", 12); l += 7 << 16;
			shadowminitext(m, l, "during the making of Duke Nukem 3D.", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Altec Lansing Multimedia", 12); l += 7 << 16;
			shadowminitext(m, l, "for tons of speakers and the", 12); l += 7 << 16;
			shadowminitext(m, l, "THX-licensed sound system.", 12); l += 7 << 16;
			shadowminitext(m, l, "For info call 1-800-548-0620", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Creative Labs, Inc.", 12); l += 7 << 16;
			l += 3 << 16;
			shadowminitext(m, l, "Thanks for the hardware, guys.", 12); l += 7 << 16;
			return true;
		}
		else if (tilenum == 2506)
		{
			Menu_DrawBackground(origin);
			DrawMenuCaption(origin, GStrings("MNU_CREDITS"));
			mgametextcenter(int(origin.X * 65536), int(origin.Y * 65536) + (50 << 16), "Duke Nukem 3D is a trademark of");
			mgametextcenter(int(origin.X * 65536), int(origin.Y * 65536) + (59 << 16), "3D Realms Entertainment");
			mgametextcenter(int(origin.X * 65536), int(origin.Y * 65536) + (77 << 16), "Duke Nukem 3D");
			mgametextcenter(int(origin.X * 65536), int(origin.Y * 65536) + (86 << 16), "(C) 1996 3D Realms Entertainment");

			if (VOLUMEONE)
			{
				mgametextcenter(int(origin.X * 65536), int(origin.Y * 65536) + (106 << 16), "Please read LICENSE.DOC for shareware");
				mgametextcenter(int(origin.X * 65536), int(origin.Y * 65536) + (115 << 16), "distribution grants and restrictions.");
			}
			mgametextcenter(int(origin.X * 65536), int(origin.Y * 65536) + ((VOLUMEONE ? 134 : 115) << 16), "Made in Dallas, Texas USA");
			return true;
		}
	}
	return false;
}

END_DUKE_NS

//----------------------------------------------------------------------------
//
// Class registration
//
//----------------------------------------------------------------------------


static TMenuClassDescriptor<Duke3d::DukeMainMenu> _mm("Duke.MainMenu");
static TMenuClassDescriptor<Duke3d::DukeListMenu> _lm("Duke.ListMenu");

static TMenuClassDescriptor<DImageScrollerMenu> _ism("Duke.ImageScrollerMenu"); // does not implement a new class, we only need the descriptor.

void RegisterDuke3dMenus()
{
	menuClasses.Push(&_mm);
	menuClasses.Push(&_lm);
	menuClasses.Push(&_ism);
}
