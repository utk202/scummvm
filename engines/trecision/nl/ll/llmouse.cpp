/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/scummsys.h"
#include "common/translation.h"
#include "trecision/nl/3d/3dinc.h"
#include "trecision/nl/struct.h"
#include "trecision/nl/ll/llinc.h"
#include "trecision/nl/extern.h"
#include "trecision/nl/define.h"
#include "trecision/nl/message.h"
#include "trecision/trecision.h"

#include "common/config-manager.h"
#include "common/file.h"
#include "common/savefile.h"
#include "common/str.h"
#include "common/system.h"
#include "graphics/cursorman.h"
#include "graphics/scaler.h"
#include "gui/saveload.h"
#include "trecision/graphics.h"
#include "trecision/video.h"

namespace Trecision {

uint16 BlinkLastDTextChar = MASKCOL;

/*-----------------17/02/95 09.53-------------------
 TextLength - Compute string length from character 0 to num
--------------------------------------------------*/
uint16 TextLength(const char *sign, uint16 num) {
	if (sign == nullptr)
		return 0;

	uint16 len = (num == 0) ? strlen(sign) : num;

	uint16 retVal = 0;
	for (uint16 c = 0; c < len; c++)
		retVal += g_vm->Font[(uint8)sign[c] * 3 + 2];

	return retVal;
}

void SDText::set(SDText org) {
	set(org.x, org.y, org.dx, org.dy, org.l[0], org.l[1], org.l[2], org.l[3], org.tcol, org.scol, org.sign);
}

void SDText::set(uint16 px, uint16 py, uint16 pdx, uint16 pdy, uint16 pl0, uint16 pl1, uint16 pl2, uint16 pl3, uint16 ptcol, uint16 pscol, const char *psign) {
	x = px;
	y = py;
	dx = pdx;
	dy = pdy;
	l[0] = pl0;
	l[1] = pl1;
	l[2] = pl2;
	l[3] = pl3;
	tcol = ptcol;
	scol = pscol;
	sign = psign;
}

void SDText::clear() {
	set(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr);
}
/*-------------------------------------------------------------
   checkDText - Computes and returns the dy of the given DText
--------------------------------------------------------------*/
uint16 SDText::checkDText() {
	if (sign == nullptr)
		return 0;

	uint8 curLine = 0;
	if (TextLength(sign, 0) <= dx) {
		strcpy((char *)DTextLines[curLine], sign);
		return CARHEI;
	}

	uint16 a = 0;
	uint16 tmpDy = 0;
	uint16 lastSpace = 0;
	uint16 curInit = 0;

	while (a < strlen(sign)) {
		a++;
		if (sign[a] == ' ') {
			if (TextLength(sign + curInit, a - curInit) <= dx)
				lastSpace = a;
			else if (TextLength(sign + curInit, lastSpace - curInit) <= dx) {
				uint16 b;
				for (b = curInit; b < lastSpace; b++)
					DTextLines[curLine][b - curInit] = sign[b];

				DTextLines[curLine][b - curInit] = '\0';
				curLine++;

				curInit = lastSpace + 1;

				tmpDy += CARHEI;
				a = curInit;
			} else
				return 0;
		} else if (sign[a] == '\0') {
			if (TextLength(sign + curInit, a - curInit) <= dx) {
				uint16 b;
				for (b = curInit; b < a; b++)
					DTextLines[curLine][b - curInit] = sign[b];
				DTextLines[curLine][b - curInit] = '\0';

				tmpDy += CARHEI;

				return tmpDy;
			}

			if (TextLength(sign + curInit, lastSpace - curInit) <= dx) {
				uint16 b;
				for (b = curInit; b < lastSpace; b++)
					DTextLines[curLine][b - curInit] = sign[b];

				DTextLines[curLine][b - curInit] = '\0';
				curLine++;

				curInit = lastSpace + 1;
				tmpDy += CARHEI;

				if (curInit < strlen(sign)) {
					for (b = curInit; b < strlen(sign); b++)
						DTextLines[curLine][b - curInit] = sign[b];

					DTextLines[curLine][b - curInit] = '\0';

					tmpDy += CARHEI;
				}
				return tmpDy;
			}
			return 0;
		}
	}
	return 0;
}

/*-----------------10/12/95 15.43-------------------
						DText
--------------------------------------------------*/
void SDText::DText() {
	uint16 tmpTCol = tcol;
	uint16 tmpSCol = scol;
	g_vm->_graphicsMgr->updatePixelFormat(&tmpTCol, 1);
	if (scol != MASKCOL)
		g_vm->_graphicsMgr->updatePixelFormat(&tmpSCol, 1);

	if (sign == nullptr)
		return;

	uint16 curDy = checkDText();

	for (uint16 b = 0; b < (curDy / CARHEI); b++) {
		char *curSign = (char *)DTextLines[b];

		uint16 inc = (dx - TextLength(curSign, 0)) / 2;

		uint16 len = strlen(curSign);

		if (len >= MAXCHARS) {
			strcpy(curSign, g_vm->_sysText[kMessageError]);
			len = strlen(curSign);
		}

		for (uint16 c = 0; c < len; c++) {
			uint8 curCar = curSign[c]; /* legge prima parte del font */

			uint16 carSco = (uint16)(g_vm->Font[curCar * 3]) + (uint16)(g_vm->Font[curCar * 3 + 1] << 8);
			uint16 DataSco = 768;                       /* Scostamento */
			uint16 CarWid = g_vm->Font[curCar * 3 + 2]; /* Larghezza   */

			if ((c == (len - 1)) && (BlinkLastDTextChar != MASKCOL))
				tmpTCol = BlinkLastDTextChar;

			for (uint16 a = (b * CARHEI); a < ((b + 1)*CARHEI); a++) {
				uint16 CarCounter = 0;
				uint16 CurColor = tmpSCol;

				while (CarCounter <= (CarWid - 1)) {
					if ((a >= l[1]) && (a < l[3])) {
						if ((CurColor != MASKCOL) && (g_vm->Font[carSco + DataSco])) {
							uint16 firstLim = inc + CarCounter;
							uint16 lastLim = firstLim + g_vm->Font[carSco + DataSco];
							uint16 *dst1 = g_vm->_screenBuffer + (x + firstLim) + (y + a) * SCREENLEN;
							uint16 *dst2 = g_vm->_screenBuffer + (x + l[0]) + (y + a) * SCREENLEN;
							uint16 *dst = nullptr;
							uint16 size = 0;
							
							if (firstLim >= l[0] && lastLim < l[2]) {
								dst = dst1;
								size = lastLim - firstLim;
							} else if (firstLim < l[0] && lastLim < l[2] && lastLim > l[0]) {
								dst = dst2;
								size = lastLim - l[0];
							} else if (firstLim >= l[0] && lastLim >= l[2] && l[2] > firstLim) {
								dst = dst1;
								size = l[2] - firstLim;
							} else if (firstLim < l[0] && lastLim >= l[2] && l[2] > firstLim) {
								dst = dst2;
								size = l[2] - l[0];
							}

							if (dst && size > 0) {
								uint16 *d = (uint16 *)dst;
								for (uint32 i = 0; i < size; i++)
									*d++ = CurColor;
							}
						}
					}

					CarCounter += g_vm->Font[carSco + DataSco];
					DataSco++;

					if (CurColor == tmpSCol)
						CurColor = 0;
					else if (CurColor == 0)
						CurColor = tmpTCol;
					else if (CurColor == tmpTCol)
						CurColor = tmpSCol;
				}
			}
			inc += CarWid;
		}
	}
}

#define MAXSAVEFILE		12
/* -----------------21/01/98 16.38-------------------
 * 						IconSnapShot
 * --------------------------------------------------*/
void IconSnapShot() {
	uint16 blackPixel = 0x0000;
	int a;

	for (int b = 0; b < ICONDY; b++) {
		for (a = 0; a < (ICONDX - 1); a++)
			g_vm->Icone[(READICON + 13) * ICONDX * ICONDY + b * ICONDX + a] = g_vm->_graphicsMgr->restorePixelFormat(g_vm->_screenBuffer[SCREENLEN * b * 10 + a * (SCREENLEN / ICONDX)]);
		g_vm->Icone[(READICON + 13)*ICONDX * ICONDY + b * ICONDX + a] = blackPixel;
	}

	::createThumbnailFromScreen(&g_vm->_thumbnail);
}

Graphics::Surface *convertScummVMThumbnail(Graphics::Surface *thumbnail) {
	Graphics::Surface *thumbnailConverted = thumbnail->convertTo(g_system->getScreenFormat());
	Graphics::Surface *result = thumbnailConverted->scale(ICONDX, ICONDY);

	thumbnailConverted->free();
	delete thumbnailConverted;
	thumbnail->free();
	delete thumbnail;

	return result;
}

void loadSaveSlots(Common::StringArray &saveNames) {
	Common::SaveFileManager *saveFileMan = g_engine->getSaveFileManager();
	
	for (int i = 0; i < g_vm->_inventorySize; i++) {
		Common::String saveFileName = g_vm->getSaveStateName(i + 1);
		Common::InSaveFile *saveFile = saveFileMan->openForLoading(saveFileName);
		ExtendedSavegameHeader header;

		if (!saveFile) {
			saveNames.push_back(g_vm->_sysText[kMessageEmptySpot]);
			g_vm->_inventory[i] = iEMPTYSLOT;
			continue;
		}

		const byte version = saveFile->readByte();

		if (saveFile && version == SAVE_VERSION_ORIGINAL) {
			// Original saved game, convert
			char buf[40];
			saveFile->read(buf, 40);
			buf[39] = '\0';
			saveNames.push_back(buf);
			
			uint16 *thumbnailBuf = g_vm->Icone + (READICON + 1 + i) * ICONDX * ICONDY;
			saveFile->read((void *)thumbnailBuf, ICONDX * ICONDY * sizeof(uint16));
			g_vm->_graphicsMgr->updatePixelFormat(thumbnailBuf, ICONDX * ICONDY);

			g_vm->_inventory[i] = LASTICON + i;
		} else if (saveFile && version == SAVE_VERSION_SCUMMVM) {
			const bool headerRead = MetaEngine::readSavegameHeader(saveFile, &header, false);
			if (headerRead) {
				saveNames.push_back(header.description);

				Graphics::Surface *thumbnail = convertScummVMThumbnail(header.thumbnail);
				uint16 *thumbnailBuf = g_vm->Icone + (READICON + 1 + i) * ICONDX * ICONDY;
				memcpy(thumbnailBuf, thumbnail->getPixels(), ICONDX * ICONDY * 2);
				thumbnail->free();
				delete thumbnail;

				g_vm->_inventory[i] = LASTICON + i;
			} else {
				saveNames.push_back(g_vm->_sysText[kMessageEmptySpot]);
				g_vm->_inventory[i] = iEMPTYSLOT;
			}
		} else {
			saveNames.push_back(g_vm->_sysText[kMessageEmptySpot]);
			g_vm->_inventory[i] = iEMPTYSLOT;
		}

		delete saveFile;
	}

	g_vm->refreshInventory(0, 0);
}

/* -----------------25/10/97 15.16-------------------
						DataSave
 --------------------------------------------------*/
bool DataSave() {
	uint8 OldInv[MAXICON], OldIconBase, OldInvLen;
	char ch;
	Common::StringArray saveNames;
	saveNames.reserve(MAXSAVEFILE);
	uint16 posx, LenText;
	bool ret = true;
	
	actorStop();
	nextStep();

	if (!ConfMan.getBool("originalsaveload")) {
		GUI::SaveLoadChooser *dialog = new GUI::SaveLoadChooser(_("Save game:"), _("Save"), true);
		int saveSlot = dialog->runModalWithCurrentTarget();
		Common::String saveName = dialog->getResultString();
		bool skipSave = saveSlot == -1;
		delete dialog;

		// Remove the mouse click event from the save/load dialog
		g_vm->eventLoop();
		g_vm->_mouseLeftBtn = g_vm->_mouseRightBtn = false;

		if (!skipSave)
			g_vm->saveGameState(saveSlot, saveName);

		return !skipSave;
	}

	for (int a = 0; a < TOP; a++)
		memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

	SDText SText;
	SText.set(0, TOP - 20, SCREENLEN, CARHEI, 0, 0, SCREENLEN, CARHEI, 0x7FFF, MASKCOL, g_vm->_sysText[kMessageSavePosition]);
	SText.DText();

	g_vm->_graphicsMgr->copyToScreen(0, 0, MAXX, TOP);

	for (int a = TOP + AREA; a < AREA + 2 * TOP; a++)
		memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);
	g_vm->_graphicsMgr->copyToScreen(0, TOP + AREA, MAXX, TOP);

	g_vm->_gameQueue.initQueue();
	g_vm->_animQueue.initQueue();
	g_vm->_characterQueue.initQueue();

	FreeKey();

	mleft = mright = false;
	Mouse(MCMD_UPDT);
	while (mleft || mright)
		Mouse(MCMD_UPDT);

	// Reset the inventory and turn it into save slots
	memcpy(OldInv, g_vm->_inventory, MAXICON);
	memset(g_vm->_inventory, 0, MAXICON);
	OldIconBase = g_vm->_iconBase;
	g_vm->_iconBase = 0;
	OldInvLen = g_vm->_inventorySize;
	g_vm->_inventorySize = MAXSAVEFILE;

insave:

	int8 CurPos = -1;
	int8 OldPos = -1;
	bool skipSave = false;
	ch = 0;

	loadSaveSlots(saveNames);
	
	for (;;) {
		g_vm->checkSystem();
		Mouse(MCMD_UPDT);

		GetKey();

		if ((my >= FIRSTLINE) &&
				(my < (FIRSTLINE + ICONDY)) &&
				(mx >= ICONMARGSX) &&
				(mx < (SCREENLEN - ICONMARGDX))) {
			OldPos = CurPos;
			CurPos = ((mx - ICONMARGSX) / ICONDX);

			if (OldPos != CurPos) {
				for (int a = FIRSTLINE + ICONDY + 10; a < FIRSTLINE + ICONDY + 10 + CARHEI; a++)
					memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

				posx    = ICONMARGSX + ((CurPos) * (ICONDX)) + ICONDX / 2;
				LenText  = TextLength(saveNames[CurPos].c_str(), 0);

				posx = CLIP(posx - (LenText / 2), 2, SCREENLEN - 2 - LenText);
				SText.set(posx, FIRSTLINE + ICONDY + 10, LenText, CARHEI, 0, 0, LenText, CARHEI, 0x7FFF, MASKCOL, saveNames[CurPos].c_str());
				SText.DText();

				g_vm->_graphicsMgr->copyToScreen(0, FIRSTLINE + ICONDY + 10, MAXX, CARHEI);
			}

			if (mleft)
				break;
		} else {
			if (OldPos != -1) {
				for (int a = FIRSTLINE + ICONDY + 10; a < FIRSTLINE + ICONDY + 10 + CARHEI; a++)
					memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

				g_vm->_graphicsMgr->copyToScreen(0, FIRSTLINE + ICONDY + 10, MAXX, CARHEI);
			}

			OldPos = -1;
			CurPos = -1;

			if (mleft || mright) {
				skipSave = true;
				break;
			}
		}
	}

	if (!skipSave) {
		if (g_vm->_inventory[CurPos] == iEMPTYSLOT) {
			saveNames[CurPos].clear();

			for (int a = FIRSTLINE + ICONDY + 10; a < FIRSTLINE + ICONDY + 10 + CARHEI; a++)
				memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

			g_vm->_graphicsMgr->copyToScreen(0, FIRSTLINE + ICONDY + 10, MAXX, CARHEI);
		}

		for (;;) {
			g_vm->_keybInput = true;
			g_vm->checkSystem();
			ch = GetKey();
			FreeKey();

			Mouse(MCMD_UPDT);
			g_vm->_keybInput = false;

			if (ch == 0x1B) {
				ch = 0;
				for (int a = FIRSTLINE + ICONDY + 10; a < FIRSTLINE + ICONDY + 10 + CARHEI; a++)
					memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

				g_vm->_graphicsMgr->copyToScreen(0, FIRSTLINE + ICONDY + 10, MAXX, CARHEI);

				goto insave;
			}

			if (ch == 8)	// Backspace
				saveNames[CurPos].deleteLastChar();
			else if (ch == 13)	// Enter
				break;
			else if (saveNames[CurPos].size() < 39 && Common::isPrint(ch))
				saveNames[CurPos] += ch;
			
			for (int a = FIRSTLINE + ICONDY + 10; a < FIRSTLINE + ICONDY + 10 + CARHEI; a++)
				memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

			saveNames[CurPos] += '_';	// add blinking cursor
			
			posx    = ICONMARGSX + ((CurPos) * (ICONDX)) + ICONDX / 2;
			LenText  = TextLength(saveNames[CurPos].c_str(), 0);

			posx = CLIP(posx - (LenText / 2), 2, SCREENLEN - 2 - LenText);
			SText.set(posx, FIRSTLINE + ICONDY + 10, LenText, CARHEI, 0, 0, LenText, CARHEI, 0x7FFF, MASKCOL, saveNames[CurPos].c_str());

			if ((ReadTime() / 8) & 1)
				BlinkLastDTextChar = 0x0000;

			SText.DText();
			BlinkLastDTextChar = MASKCOL;

			saveNames[CurPos].deleteLastChar();	// remove blinking cursor
			
			g_vm->_graphicsMgr->copyToScreen(0, FIRSTLINE + ICONDY + 10, MAXX, CARHEI);
		}

		for (int a = FIRSTLINE; a < MAXY; a++)
			memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

		ret = false;

		// Restore the inventory
		memcpy(g_vm->_inventory, OldInv, MAXICON);
		g_vm->_curInventory = 0;
		g_vm->_iconBase = OldIconBase;
		g_vm->_inventorySize = OldInvLen;
		
		g_vm->saveGameState(CurPos + 1, saveNames[CurPos]);
	}

	for (int a = FIRSTLINE; a < MAXY; a++)
		memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

	g_vm->_graphicsMgr->copyToScreen(0, FIRSTLINE, MAXX, TOP);

	for (int a = TOP - 20; a < TOP - 20 + CARHEI; a++)
		memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

	g_vm->_graphicsMgr->copyToScreen(0, 0, MAXX, TOP);

	// Restore the inventory
	memcpy(g_vm->_inventory, OldInv, MAXICON);
	g_vm->_curInventory = 0;
	g_vm->_iconBase = OldIconBase;
	g_vm->_inventorySize = OldInvLen;
	
	mleft = mright = false;
	Mouse(MCMD_UPDT);
	while (mleft || mright)
		Mouse(MCMD_UPDT);

	return ret;
}

/*-----------------09/02/96 20.57-------------------
					DataLoad
--------------------------------------------------*/
bool DataLoad() {
	uint8 OldInv[MAXICON], OldIconBase, OldInvLen;
	Common::StringArray saveNames;
	saveNames.reserve(MAXSAVEFILE);
	bool retval = true;
	Common::SaveFileManager *saveFileMan = g_engine->getSaveFileManager();

	if (!ConfMan.getBool("originalsaveload")) {
		GUI::SaveLoadChooser *dialog = new GUI::SaveLoadChooser(_("Load game:"), _("Load"), false);
		int saveSlot = dialog->runModalWithCurrentTarget();
		bool skipLoad = saveSlot == -1;
		delete dialog;

		// Remove the mouse click event from the save/load dialog
		g_vm->eventLoop();
		g_vm->_mouseLeftBtn = g_vm->_mouseRightBtn = false;

		performLoad(saveSlot - 1, skipLoad);
		
		return !skipLoad;
	}

	for (int a = 0; a < TOP; a++)
		memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

	if (!g_vm->_flagMouseEnabled) {
		g_vm->_flagMouseEnabled = true;
		Mouse(MCMD_ON);
	}

	SDText SText;
	SText.set(0, TOP - 20, SCREENLEN, CARHEI, 0, 0, SCREENLEN, CARHEI, 0x7FFF, MASKCOL, g_vm->_sysText[kMessageLoadPosition]);
	SText.DText();

	g_vm->_graphicsMgr->copyToScreen(0, 0, MAXX, TOP);

	for (int a = TOP + AREA; a < AREA + 2 * TOP; a++)
		memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);
	g_vm->_graphicsMgr->copyToScreen(0, TOP + AREA, MAXX, TOP);

	g_vm->_gameQueue.initQueue();
	g_vm->_animQueue.initQueue();
	g_vm->_characterQueue.initQueue();

	FreeKey();

	mleft = mright = false;
	Mouse(MCMD_UPDT);
	while (mleft || mright)
		Mouse(MCMD_UPDT);

	// Reset the inventory and turn it into save slots
	memcpy(OldInv, g_vm->_inventory, MAXICON);
	memset(g_vm->_inventory, 0, MAXICON);
	OldIconBase = g_vm->_iconBase;
	g_vm->_iconBase = 0;
	OldInvLen = g_vm->_inventorySize;
	g_vm->_inventorySize = MAXSAVEFILE;

	loadSaveSlots(saveNames);

	bool skipLoad = false;
	int8 CurPos = -1;
	int8 OldPos = -1;

	for (;;) {
		g_vm->checkSystem();
		Mouse(MCMD_UPDT);

		GetKey();

		if ((my >= FIRSTLINE) &&
				(my < (FIRSTLINE + ICONDY)) &&
				(mx >= ICONMARGSX) &&
				(mx < (SCREENLEN - ICONMARGDX))) {
			OldPos = CurPos;
			CurPos = ((mx - ICONMARGSX) / ICONDX);

			if (OldPos != CurPos) {
				for (int a = FIRSTLINE + ICONDY + 10; a < FIRSTLINE + ICONDY + 10 + CARHEI; a++)
					memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

				uint16 posX = ICONMARGSX + ((CurPos) * (ICONDX)) + ICONDX / 2;
				uint16 lenText = TextLength(saveNames[CurPos].c_str(), 0);
				if (posX - (lenText / 2) < 2)
					posX = 2;
				else
					posX = posX - (lenText / 2);
				if ((posX + lenText) > SCREENLEN - 2)
					posX = SCREENLEN - 2 - lenText;

				SText.set(posX, FIRSTLINE + ICONDY + 10, lenText, CARHEI, 0, 0, lenText, CARHEI, 0x7FFF, MASKCOL, saveNames[CurPos].c_str());
				SText.DText();

				g_vm->_graphicsMgr->copyToScreen(0, FIRSTLINE + ICONDY + 10, MAXX, CARHEI);
			}

			if (mleft && (g_vm->_inventory[CurPos] != iEMPTYSLOT))
				break;
		} else {
			if (OldPos != -1) {
				for (int a = FIRSTLINE + ICONDY + 10; a < FIRSTLINE + ICONDY + 10 + CARHEI; a++)
					memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

				g_vm->_graphicsMgr->copyToScreen(0, FIRSTLINE + ICONDY + 10, MAXX, CARHEI);
			}

			OldPos = -1;
			CurPos = -1;

			if (mleft || mright) {
				retval = false;
				skipLoad = true;
				break;
			}
		}
	}

	performLoad(CurPos, skipLoad);

	if (skipLoad) {
		// Restore the inventory
		memcpy(g_vm->_inventory, OldInv, MAXICON);
		g_vm->_curInventory = 0;
		g_vm->_iconBase = OldIconBase;
		g_vm->_inventorySize = OldInvLen;
	}
	
	return retval;
}

void performLoad(int slot, bool skipLoad) {
	if (!skipLoad) {
		for (int a = FIRSTLINE; a < MAXY; a++)
			memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

		g_vm->loadGameState(slot + 1);

		FlagNoPaintScreen = true;
		g_vm->_curStack = 0;
		g_vm->_flagscriptactive = false;

		g_vm->_oldRoom = g_vm->_curRoom;
		doEvent(MC_SYSTEM, ME_CHANGEROOM, MP_SYSTEM, g_vm->_curRoom, 0, 0, 0);
	}

	actorStop();
	nextStep();
	g_vm->checkSystem();

	for (int a = FIRSTLINE; a < MAXY; a++)
		memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

	g_vm->_graphicsMgr->copyToScreen(0, FIRSTLINE, MAXX, TOP);

	for (int a = TOP - 20; a < TOP - 20 + CARHEI; a++)
		memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

	g_vm->_graphicsMgr->copyToScreen(0, 0, MAXX, TOP);

	mleft = mright = false;
	Mouse(MCMD_UPDT);
	while (mleft || mright)
		Mouse(MCMD_UPDT);

	if (g_vm->_flagscriptactive) {
		g_vm->_flagMouseEnabled = false;
		Mouse(MCMD_OFF);
	}
}

/*-----------------09/02/96 20.57-------------------
					QuitGame
--------------------------------------------------*/
bool QuitGame() {
	for (int a = 0; a < TOP; a++)
		memcpy(g_vm->ZBuffer + a * SCREENLEN, g_vm->_screenBuffer + SCREENLEN * a, SCREENLEN * 2);

	for (int a = 0; a < TOP; a++)
		memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

	SDText SText;
	SText.set(0, TOP - 20, SCREENLEN, CARHEI, 0, 0, SCREENLEN, CARHEI, 0x7FFF, MASKCOL, g_vm->_sysText[kMessageConfirmExit]);
	SText.DText();

	g_vm->_graphicsMgr->copyToScreen(0, 0, SCREENLEN, TOP);

	FreeKey();

	mleft = mright = false;
	Mouse(MCMD_UPDT);
	while (mleft || mright)
		Mouse(MCMD_UPDT);

	char ch = waitKey();

	bool exitFl = ((ch == 'y') || (ch == 'Y'));

	for (int a = 0; a < TOP; a++)
		memcpy(g_vm->_screenBuffer + SCREENLEN * a, g_vm->ZBuffer + a * SCREENLEN, SCREENLEN * 2);

	g_vm->_graphicsMgr->copyToScreen(0, 0, SCREENLEN, TOP);

	return exitFl;
}

/*-----------------09/02/96 20.57-------------------
					DemoOver
--------------------------------------------------*/
void DemoOver() {
	for (int a = 0; a < TOP; a++)
		memset(g_vm->_screenBuffer + SCREENLEN * a, 0, SCREENLEN * 2);

	SDText SText;
	SText.set(0, TOP - 20, SCREENLEN, CARHEI, 0, 0, SCREENLEN, CARHEI, 0x7FFF, MASKCOL, g_vm->_sysText[kMessageDemoOver]);
	SText.DText();

	g_vm->_graphicsMgr->copyToScreen(0, 0, SCREENLEN, TOP);

	FreeKey();

	mleft = mright = false;
	Mouse(MCMD_UPDT);
	while (mleft || mright)
		Mouse(MCMD_UPDT);

	waitKey();

	CloseSys(nullptr);
}

} // End of namespace Trecision