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

#include "fullpipe/fullpipe.h"

#include "fullpipe/objectnames.h"
#include "fullpipe/constants.h"

#include "fullpipe/gameloader.h"
#include "fullpipe/motion.h"
#include "fullpipe/scenes.h"
#include "fullpipe/statics.h"

#include "fullpipe/interaction.h"
#include "fullpipe/behavior.h"


namespace Fullpipe {

struct Bat {
	StaticANIObject *ani;
	int field_4;
	double power;
	int field_10;
	int field_14;
	double currX;
	double currY;
	double powerCos;
	double powerSin;
};

void scene27_initScene(Scene *sc) {
	g_vars->scene27_var01 = 200;
	g_vars->scene27_var02 = 200;
	g_vars->scene27_var03 = 300;
	g_vars->scene27_var04 = 300;
	g_vars->scene27_hitZone = sc->getPictureObjectById(PIC_SC27_HITZONE2, 0);
	g_vars->scene27_driver = sc->getStaticANIObject1ById(ANI_VODILLA, -1);
	g_vars->scene27_maid = sc->getStaticANIObject1ById(ANI_MAID, -1);
	g_vars->scene27_batHandler = sc->getStaticANIObject1ById(ANI_BITAHANDLER, -1);

	g_vars->scene27_balls.numBalls = 0;
	g_vars->scene27_balls.pTail = 0;
	g_vars->scene27_balls.field_8 = 0;
	g_vars->scene27_balls.pHead = 0;
	g_vars->scene27_balls.cPlexLen = 10;

	free(g_vars->scene27_balls.cPlex);
	g_vars->scene27_balls.cPlex = 0;

	g_vars->scene27_bats.clear();
	g_vars->scene27_var07.clear();

	g_vars->scene27_var15 = 1;
	g_vars->scene27_bat = sc->getStaticANIObject1ById(ANI_BITA, -1);

	for (int i = 0; i < 4; i++) {
		StaticANIObject *newbat = new StaticANIObject(g_vars->scene27_bat);

		Ball *runPtr = g_vars->scene27_balls.pTail;
		Ball *lastP = g_vars->scene27_balls.field_8;

		if (!g_vars->scene27_balls.pTail) {
			g_vars->scene27_balls.cPlex = (Ball *)calloc(g_vars->scene27_balls.cPlexLen, sizeof(Ball));

			Ball *p1 = g_vars->scene27_balls.cPlex + (g_vars->scene27_balls.cPlexLen - 1) * sizeof(Ball);

			if (g_vars->scene27_balls.cPlexLen - 1 < 0) {
				runPtr = g_vars->scene27_balls.pTail;
			} else {
				runPtr = g_vars->scene27_balls.pTail;

				for (int j = 0; j < g_vars->scene27_balls.cPlexLen; j++) {
					p1->p1 = runPtr;
					runPtr = p1;

					p1 -= sizeof(Ball);
				}

				g_vars->scene27_balls.pTail = runPtr;
			}
		}

		g_vars->scene27_balls.pTail = runPtr->p0;
		runPtr->p1 = lastP;
		runPtr->p0 = 0;
		runPtr->ani = newbat;

		g_vars->scene27_balls.numBalls++;

		if (g_vars->scene27_balls.field_8)
			g_vars->scene27_balls.field_8->p0 = runPtr;
		else
			g_vars->scene27_balls.pHead = runPtr;

		g_vars->scene27_balls.field_8 = runPtr;

		sc->addStaticANIObject(newbat, 1);
	}

	g_vars->scene27_var08 = 0;
	g_vars->scene27_var09 = 0;
	g_vars->scene27_var10 = 0;
	g_vars->scene27_var11 = 0;
	g_vars->scene27_var12 = 0;
	g_vars->scene27_var13 = 0;
	g_vars->scene27_launchPhase = 0;

	Scene *oldsc = g_fp->_currentScene;
	g_fp->_currentScene = sc;

	if (g_fp->getObjectState(sO_Maid) == g_fp->getObjectEnumState(sO_Maid, sO_WithSwab)) {
		g_vars->scene27_maid->changeStatics2(ST_MID_SWAB2);
	} else if (g_fp->getObjectState(sO_Maid) == g_fp->getObjectEnumState(sO_Maid, sO_WithBroom)) {
		g_vars->scene27_maid->changeStatics2(ST_MID_BROOM);
	} else if (g_fp->getObjectState(sO_Maid) == g_fp->getObjectEnumState(sO_Maid, sO_WithSpade)) {
		g_vars->scene27_maid->changeStatics2(ST_MID_SPADE);
	}

	g_fp->_currentScene = oldsc;

	g_fp->setArcadeOverlay(PIC_CSR_ARCADE7);
}

int scene27_updateCursor() {
	g_fp->updateCursorCommon();

	if (g_vars->scene27_var08) {
		if (g_fp->_cursorId != PIC_CSR_DEFAULT_INV && g_fp->_cursorId != PIC_CSR_ITN_INV)
			g_fp->_cursorId = PIC_CSR_ARCADE7_D;

	} else if (g_fp->_objectIdAtCursor == ANI_MAN) {
		if (g_vars->scene27_var09)
			if (g_fp->_cursorId == PIC_CSR_DEFAULT)
				g_fp->_cursorId = PIC_CSR_ITN;
	}

	return g_fp->_cursorId;
}

} // End of namespace Fullpipe
