/** @file lbaengine.c
	@brief
	This file contains the main game engine routines

	Prequengine: a Little Big Adventure engine

	Copyright (C) 2008 Prequengine team \n
	Copyright (C) 2002-2007 The TwinEngine team \n

	This program is free software; you can redistribute it and/or \n
	modify it under the terms of the GNU General Public License \n
	as published by the Free Software Foundation; either version 2 \n
	of the License, or (at your option) any later version. \n

	This program is distributed in the hope that it will be useful, \n
	but WITHOUT ANY WARRANTY; without even the implied warranty of \n
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n
	GNU General Public License for more details. \n

	You should have received a copy of the GNU General Public License \n
	along with this program; if not, write to the Free Software \n
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

	$URL$
	$Id$
*/

#include "lbaengine.h"
#include "main.h"
#include "sdlengine.h"
#include "images.h"
#include "grid.h"
#include "scene.h"
#include "menu.h"
#include "interface.h"
#include "dialogues.h"
#include "redraw.h"
#include "hqrdepack.h"
#include "resources.h"
#include "renderer.h"
#include "animations.h"
#include "movements.h"
#include "keyboard.h"

#ifdef GAMEMOD
#include "debug.h"
#endif

int32 isTimeFreezed = 0;
int32 saveFreezedTime = 0;

void freeze_time() {
	if (!isTimeFreezed)
		saveFreezedTime = lbaTime;
	isTimeFreezed++;
}

void unfreeze_time() {
	--isTimeFreezed;
	if (isTimeFreezed == 0)
		lbaTime = saveFreezedTime;
}



void recenter_screen() {
	if ((loopPressedKey & 2) && disableScreenRecenter == 0) {
		newCameraX = sceneActors[currentlyFollowedActor].X >> 9;
		newCameraY = sceneActors[currentlyFollowedActor].Y >> 8;
		newCameraZ = sceneActors[currentlyFollowedActor].Z >> 9;
		reqBgRedraw = 1;
		//needChangeRoom = 119;
		//needChangeRoom=currentRoom+1;
	}
}

/** Game engine main loop
	@return true if we want to show credit sequence */
int32 run_game_engine() { // mainLoopInteration
	int32 a;
	int16 pKey; // current pressed key
	read_keys();

	key  = pressedKey;
	pKey = skipIntro; // mainLoopVar67
	loopPressedKey = skipedKey;

	if (process_giveup_menu())
		return 0; // give up

	process_options_menu(pKey);

	recenter_screen();

#ifdef GAMEMOD
	process_debug(pKey);
#endif

	if (needChangeScene > -1)
		change_scene();

	for (a = 0; a < sceneNumActors; a++) {
		ActorStruct *actor = &sceneActors[a];

		process_actor_movements(a);
		// TODO: process_track_script(a);
		process_actor_animations(a);
	}

	redraw_engine_actions(reqBgRedraw);

	needChangeScene = -1;
	reqBgRedraw = 0;

	return 0;
}

/** Game engine main loop
	@return true if we want to show credit sequence */
int32 game_engine_loop() { // mainLoop
	uint32 start;

	quitGame = 1;
	//requestBackgroundRedraw = 1;
	//lockPalette = 1;
	//setActorAngle(0, -256, 5, &mainLoopVar1);

	while (quitGame) {
		start = SDL_GetTicks();

		while (SDL_GetTicks() < start + cfgfile.Fps) {
			if (run_game_engine())
				return 1;
			SDL_Delay(cfgfile.Fps);
		}
		lbaTime++;
	}
	return 0;
}
