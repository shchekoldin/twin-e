/** @file animations.c
	@brief
	This file contains 3D actors animations routines
	
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animations.h"
#include "lbaengine.h"
#include "resources.h"
#include "scene.h"
#include "actor.h"
#include "renderer.h"

/** Preload all animations */
void preload_animations()
{
	int32 i;
	int32 numEntries = hqr_num_entries(HQR_ANIM_FILE)-1;

	for(i=0; i<numEntries; i++)
	{
		animSizeTable[i] = hqr_getalloc_entry(&animTable[i],HQR_ANIM_FILE,i);
	}
}

int set_anim_at_keyframe(int32 keyframeIdx, uint8 *anim, uint8 *body, AnimTimerDataStruct* animTimerDataPtr)
{
	int16 numOfKeyframeInAnim;
	int16 numOfBonesInAnim;
	uint8 *ptrToData;
	uint8 *ptrToDataBackup;
	uint8 *ptrToBodyData;
	int16 bodyHeader;
	int16 numOfElementInBody;
	int16 numOfPointInBody;
	int32 i;

	numOfKeyframeInAnim = *(int16 *)(anim);

	if (keyframeIdx >= numOfKeyframeInAnim)
		return (numOfKeyframeInAnim);

	numOfBonesInAnim = *(int16 *) (anim + 2);

	ptrToData = (uint8 *) ((numOfBonesInAnim * 8 + 8) * keyframeIdx + anim + 8);

	bodyHeader = *(int16 *)(body);

	if (!(bodyHeader & 2))
		return (0);

	ptrToBodyData = body + 14;

	animTimerDataPtr->ptr = ptrToData;
	animTimerDataPtr->time = lbaTime;

	ptrToBodyData = ptrToBodyData + *(int16 *)(ptrToBodyData) + 2; 

	numOfElementInBody = *(int16 *)(ptrToBodyData);

	ptrToBodyData = ptrToBodyData + numOfElementInBody * 6 + 12; 

	numOfPointInBody = *(int16 *)(ptrToBodyData - 10); // num elements

	if (numOfBonesInAnim > numOfPointInBody)
	{
		numOfBonesInAnim = numOfPointInBody;
	}

	ptrToDataBackup = ptrToData;

	ptrToData += 8;

	do
	{
		for (i = 0; i < 8; i++)
		{
			*(ptrToBodyData++) = *(ptrToData++);
		}

		ptrToBodyData += 30;

	}while (--numOfBonesInAnim);

	ptrToData = ptrToDataBackup + 2;

	currentX = *(int16 *)(ptrToData);
	currentY = *(int16 *)(ptrToData + 2);
	currentZ = *(int16 *)(ptrToData + 4);

	processActorVar5     = *(int16 *)(ptrToData + 6);
	processActorSub2Var0 = *(int16 *)(ptrToData + 8);
	processActorVar6     = *(int16 *)(ptrToData + 10);
	processActorSub2Var1 = *(int16 *)(ptrToData + 12);

	return (1);
}

int32 get_num_keyframes(uint8 *ptr)
{
  return (*(int16 *)(ptr));
}

int get_start_keyframe(uint8 *ptr)
{
  return (*(int16 *)(ptr + 4));
}

void apply_anim_steprotation(uint8 **ptr, int32 bp, int32 bx)
{
	int16 *dest;
	int16 lastAngle;
	int16 newAngle;
	int16 angleDif;
	int16 computedAngle;

	lastAngle = *(int16 *)(lastKeyFramePtr);
	lastKeyFramePtr += 2;

	newAngle = *(int16 *)(keyFramePtr);
	keyFramePtr += 2;

	lastAngle &= 0x3FF;
	newAngle &= 0x3FF;

	angleDif = newAngle - lastAngle;

	if (angleDif)
	{
		if (angleDif < -0x200)
		{
			angleDif += 0x400;
		}
		else if (angleDif > 0x200)
		{
			angleDif -= 0x400;
		}

		computedAngle = lastAngle + (angleDif * bp) / bx;
	}
	else
	{
		computedAngle = lastAngle;
	}

	dest = (int16 *) *(ptr);
	*dest = computedAngle & 0x3FF;
	*(ptr) = *(ptr) + 2;
}

void apply_anim_step(uint8 **ptr, int32 bp, int32 bx)
{
	int16 *dest;
	int16 lastAngle;
	int16 newAngle;
	int16 angleDif;
	int16 computedAngle;

	lastAngle = *(int16 *) lastKeyFramePtr;
	lastKeyFramePtr += 2;

	newAngle = *(int16 *) keyFramePtr;
	keyFramePtr += 2;

	angleDif = newAngle - lastAngle;

	if (angleDif)
	{
		computedAngle = lastAngle + (angleDif * bp) / bx;
	}
	else
	{
		computedAngle = lastAngle;
	}

	dest = (int16 *) *(ptr);
	*dest = computedAngle;
	*(ptr) = *(ptr) + 2;
}

int32 get_anim_mode(uint8 **ptr)
{
	int16 *lptr;
	int16 opcode;

	lptr = (int16 *) *ptr;

	opcode = *(int16 *)(keyFramePtr);
    *(int16 *)(lptr) = opcode;

	keyFramePtr += 2;
	*(ptr) = *(ptr) + 2;
	lastKeyFramePtr += 2;

	return (opcode);
}

int32 set_model_animation(int32 animState, uint8 *animData, uint8 *body, AnimTimerDataStruct* animTimerDataPtr)
{
	int16 animOpcode;

	int16 bodyHeader;

	uint8 *edi;
	uint8 *ebx;
	int32 ebp;
	int32 eax;
	int32 keyFrameLength;
	int32 numOfPointInBody;
	int32 numOfPointInAnim;
	uint8 *keyFramePtrOld;

	numOfPointInAnim = *(int16 *)(animData + 2);

	keyFramePtr = ((numOfPointInAnim * 8 + 8) * animState) + animData + 8;

	keyFrameLength = *(int16 *)(keyFramePtr);

	bodyHeader = *(int16 *)(body);

	if (!(bodyHeader & 2))
	{
		return (0);
	}

	edi = body + 16;

	animVar1 = edi;

	ebx = animTimerDataPtr->ptr;
	ebp = animTimerDataPtr->time;

	if (!ebx)
	{
		ebx = keyFramePtr;
		ebp = keyFrameLength;
	}
/*	else
	{
		assert_ptr(ebx);
	}*/

	lastKeyFramePtr = ebx;

	eax = *(int16 *)(edi - 2);
	edi += eax;

	eax = *(int16 *)(edi);
	eax = eax + eax * 2;
	edi = edi + eax * 2 + 12;

	numOfPointInBody = *(int16 *)(edi - 10);

	if (numOfPointInAnim > numOfPointInBody)
	{
		numOfPointInAnim = numOfPointInBody;
	}

	eax = lbaTime - ebp;

	if (eax >= keyFrameLength)
	{
		int32 *destPtr; // keyFrame
		int32 *sourcePtr;

		sourcePtr = (int32 *) (keyFramePtr + 8);
		destPtr = (int32 *) edi;

		do
		{
			*(destPtr++) = *(sourcePtr++);
			*(destPtr++) = *(sourcePtr++);
			destPtr = (int32 *) (((int8 *)destPtr) + 30);
		}while (--numOfPointInAnim);

		animTimerDataPtr->ptr = keyFramePtr;
		animTimerDataPtr->time = lbaTime;

		currentX = *(int16 *)(keyFramePtr + 2);
		currentY = *(int16 *)(keyFramePtr + 4);
		currentZ = *(int16 *)(keyFramePtr + 6);

		processActorVar5     = *(int16 *)(keyFramePtr + 8);
		processActorSub2Var0 = *(int16 *)(keyFramePtr + 10);
		processActorVar6     = *(int16 *)(keyFramePtr + 12);
		processActorSub2Var1 = *(int16 *)(keyFramePtr + 14);

		return (1);
	}
	else
	{
		keyFramePtrOld = keyFramePtr;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		processActorVar5     = *(int16 *)(keyFramePtr);
		processActorSub2Var0 = (*(int16 *)(keyFramePtr + 2) * eax) / keyFrameLength;
		processActorVar6     = (*(int16 *)(keyFramePtr + 4) * eax) / keyFrameLength;
		processActorSub2Var1 = (*(int16 *)(keyFramePtr + 6) * eax) / keyFrameLength;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		edi += 38;

		if (--numOfPointInAnim)
		{
			animVar4 = numOfPointInAnim;

			do
			{
				animOpcode = get_anim_mode(&edi);

				switch (animOpcode)
				{
					case 0:	// allow global rotate
					{
						apply_anim_steprotation(&edi, eax, keyFrameLength);
						apply_anim_steprotation(&edi, eax, keyFrameLength);
						apply_anim_steprotation(&edi, eax, keyFrameLength);
						break;
					}
					case 1:	// dissallow global rotate
					{
						apply_anim_step(&edi, eax, keyFrameLength);
						apply_anim_step(&edi, eax, keyFrameLength);
						apply_anim_step(&edi, eax, keyFrameLength);
						break;
					}
					case 2:	// dissallow global rotate + hide
					{
						apply_anim_step(&edi, eax, keyFrameLength);
						apply_anim_step(&edi, eax, keyFrameLength);
						apply_anim_step(&edi, eax, keyFrameLength);
						break;
					}
					default:
					{
						printf("Unsupported animation rotation mode %d!\n", animOpcode);
						exit(1);
					}
				}

				edi += 30;
			}while (--animVar4);
		}

		currentX = ( *(int16 *)(keyFramePtrOld + 2) * eax) / keyFrameLength;
		currentY = ( *(int16 *)(keyFramePtrOld + 4) * eax) / keyFrameLength;
		currentZ = ( *(int16 *)(keyFramePtrOld + 6) * eax) / keyFrameLength;
	}

	return (0);
}

int32 get_body_anim_index(int32 anim, int16 actorNumber)
{
	int8 type;
	uint16 var1;
	uint8 *bodyPtr;
	uint8 *ptr, *ptr2;
	uint8 *costumePtr = NULL;
	ActorStruct *localActor;

	localActor = &sceneActors[actorNumber];
	bodyPtr = localActor->entityDataPtr;

	do
	{
		type = *(bodyPtr++);

		if (type == -1)
		{
			currentActorAnimExtraData = NULL;
			return (-1);
		}

		ptr = (bodyPtr + 1);

		if (type == 3)
		{
			if (anim == *bodyPtr)
			{
				ptr++;
				var1 = *(int16 *)(ptr);
				ptr += 2;
				ptr2 = ptr;
				ptr++;
				if (*ptr2 != 0)
				{
					costumePtr = ptr - 1;
				}
				currentActorAnimExtraData = costumePtr;
				return (var1);
			}
		}

		bodyPtr = *ptr + ptr;

	}while (1);

	return (0);
}


int32 stock_animation(uint8 *lBufAnim, uint8 *lBody, AnimTimerDataStruct* animTimerDataPtr)	// copy the next keyFrame from a different buffer
{
	int32 temp;
	uint8 *ptr;
	int32 *edi;
	int32 *esi;
	int32 var0;
	int32 var1;
	int32 var2;
	int32 counter;

	//assert_ptr(lBufAnim);
	//assert_ptr(lBody);

	temp = *(int16 *)(lBody);

	if (temp & 2)
	{
		ptr = (lBody + 0x10);

		animTimerDataPtr->time = lbaTime;
		animTimerDataPtr->ptr = lBufAnim;

		var0 = *(int16 *)(ptr - 2);
		ptr = ptr + var0;

		var1 = *(int16 *)(ptr);
		var1 = var1 + var1 * 2;

		ptr = ptr + var1 * 2 + 2;

		var2 = *(int16 *)(ptr);
		counter = var2;
		var2 = (var2 * 8) + 8;

		edi = (int32 *) (lBufAnim + 8);
		esi = (int32 *) (ptr + 10);

		do
		{
			*(edi++) = *(esi++);
			*(edi++) = *(esi++);

			esi = (int32 *) (((int8 *) esi) + 30);
		}while (--counter);

		return (var2);
	}
	return (0);
}

int32 init_anim(int8 newAnim, int16 arg_4, uint8 arg_8, int16 actorNum)
{
	ActorStruct *localActor;
	int32 animIndex;

	localActor = &sceneActors[actorNum];

	if (localActor->entity == -1)
		return (0);

	if (localActor->staticFlags.bIsSpriteActor) // si c'est un sprite
		return (0);

	if (newAnim == localActor->anim && localActor->previousAnimIdx != -1) // le costume est deja loade
		return (1);

	if (arg_8 == 255 && localActor->field_78 != 2)
		arg_8 = localActor->anim;

	animIndex = get_body_anim_index(newAnim, actorNum);

	if (animIndex == -1)
		animIndex = get_body_anim_index(0, actorNum);

	if (arg_4 != 4 && localActor->field_78 == 2)
	{
		localActor->animExtra = newAnim;
		return (0);
	}

	if (arg_4 == 3)
	{
		arg_4 = 2;

		arg_8 = localActor->anim;

		if (arg_8 == 15 || arg_8 == 7 || arg_8 == 8 || arg_8 == 9)
		{
			arg_8 = 0;
		}
	}

	if (arg_4 == 4)
		arg_4 = 2;

	if (localActor->previousAnimIdx == -1)	// if no previous animation
	{
		//setAnimAtKeyFrame(0, HQR_Get(HQR_Anims, animIndex), bodyTable[lactor->costumeIndex], &lactor->animTimerData);	// set animation directly to first keyFrame
	}
	else // interpolation between animations
	{
		animBuffer2 += stock_animation(animBuffer2, bodyTable[localActor->entity], &localActor->animTimerData);
		if (animBuffer1 + 4488 > animBuffer2)
			animBuffer2 = animBuffer1;
	}

	localActor->previousAnimIdx = animIndex;
	localActor->anim = newAnim;
	localActor->animExtra = arg_8;
	localActor->animExtraData = currentActorAnimExtraData;
	localActor->field_78 = arg_4;
	localActor->animPosition = 0;
	localActor->dynamicFlags.bIsHitting = 0;
	localActor->dynamicFlags.bAnimEnded = 0;
	localActor->dynamicFlags.bAnimFrameReached = 1;

	if (localActor->animExtraData)
	{
		//GereAnimAction(lactor, actorNum);
	}

	localActor->lastRotationSpeed = 0;
	localActor->lastX = 0;
	localActor->lastY = 0;
	localActor->lastZ = 0;

	return (1);
}

int32 verify_anim_at_keyframe(int32 animPos, uint8 *animData, uint8 *body, AnimTimerDataStruct* animTimerDataPtr)
{
	int16 bodyHeader;

	uint8 *edi;
	uint8 *ebx;
	int32 ebp;
	int32 eax;
	int32 keyFrameLength;
	int32 numOfPointInAnim = -1;
	uint8 *keyFramePtrOld;

	numOfPointInAnim = *(int16 *)(animData + 2);

	keyFramePtr = ((numOfPointInAnim * 8 + 8) * animPos) + animData + 8;

	keyFrameLength = *(int16 *)(keyFramePtr);

	bodyHeader = *(int16 *)(body);

	if (!(bodyHeader & 2))
	{
		return (0);
	}

	edi = body + 16;

	animVar1 = edi;

	ebx = animTimerDataPtr->ptr;
	ebp = animTimerDataPtr->time;

	if (!ebx)
	{
		ebx = keyFramePtr;
		ebp = keyFrameLength;
	}

	lastKeyFramePtr = ebx;

	eax = lbaTime - ebp;

	if (eax >= keyFrameLength)
	{
		animTimerDataPtr->ptr = keyFramePtr;
		animTimerDataPtr->time = lbaTime;

		currentX = *(int16 *)(keyFramePtr + 2);
		currentY = *(int16 *)(keyFramePtr + 4);
		currentZ = *(int16 *)(keyFramePtr + 6);

		processActorVar5     = *(int16 *)(keyFramePtr + 8);
		processActorSub2Var0 = *(int16 *)(keyFramePtr + 10);
		processActorVar6     = *(int16 *)(keyFramePtr + 12);
		processActorSub2Var1 = *(int16 *)(keyFramePtr + 14);

		return (1);
	}
	else
	{
		keyFramePtrOld = keyFramePtr;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		processActorVar5     = *(int16 *)(keyFramePtr);
		processActorSub2Var0 = (*(int16 *)(keyFramePtr + 2) * eax) / keyFrameLength;
		processActorVar6     = (*(int16 *)(keyFramePtr + 4) * eax) / keyFrameLength;
		processActorSub2Var1 = (*(int16 *)(keyFramePtr + 6) * eax) / keyFrameLength;

		lastKeyFramePtr += 8;
		keyFramePtr += 8;

		currentX = ( *(int16 *)(keyFramePtrOld + 2) * eax) / keyFrameLength;
		currentY = ( *(int16 *)(keyFramePtrOld + 4) * eax) / keyFrameLength;
		currentZ = ( *(int16 *)(keyFramePtrOld + 6) * eax) / keyFrameLength;
	}

	return (0);
}

/** Process main loop actor animations */
void process_actor_animations(int32 actorIdx) // DoAnim
{
	int16 numKeyframe;
	int8 *animPtr;
	ActorStruct *actor; 

	actor = &sceneActors[actorIdx];

    currentlyProcessedActorNum = actorIdx;
    processActorVar1 = actor;

    if (actor->entity == -1)
		return;

	processActorVar2 = actor->X;
    processActorVar3 = actor->Y;
    processActorVar4 = actor->Z;

	if (actor->staticFlags.bIsSpriteActor) // is sprite actor
    {
		if (actor->strengthOfHit)
		{
			actor->dynamicFlags.bIsHitting = 1;
		}

		processActorX = actor->X;
		processActorY = actor->Y;
		processActorZ = actor->Z;

		// TODO: update sprite actors
	}
	else // 3D actor
	{
		if (actor->previousAnimIdx != -1)
		{
			int32 keyFramePassed;
			animPtr = animTable[actor->previousAnimIdx];
      
			keyFramePassed = verify_anim_at_keyframe(actor->animPosition, animPtr,(int8*)bodyTable[actor->entity], &actor->animTimerData); 

			if (processActorVar5)
			{
				actor->dynamicFlags.bIsRotationByAnim = 1;
			}
			else
			{
				actor->dynamicFlags.bIsRotationByAnim = 0;
			}

			actor->angle = (actor->angle + processActorVar6 - actor->lastRotationSpeed) & 0x3FF;
			actor->lastRotationSpeed = processActorVar6;

			rotate_actor(currentX, currentZ, actor->angle);

			currentX = destX; // dest
			currentZ = destZ;

			processActorX = actor->X + currentX - actor->lastX;
			processActorY = actor->Y + currentY - actor->lastY;
			processActorZ = actor->Z + currentZ - actor->lastZ;

			actor->lastX = currentX;
			actor->lastY = currentY;
			actor->lastZ = currentZ;

			actor->dynamicFlags.bAnimEnded = 0;
			actor->dynamicFlags.bAnimFrameReached = 0;

			if(keyFramePassed)  // if keyFrame
			{
				actor->animPosition++;

				if (actor->animExtraData) // if actor have animation actions to process
				{
					//TODO process_anim_actions - GereAnimAction(actor, actorIdx);
				}

				numKeyframe = actor->animPosition;
				if (numKeyframe == get_num_keyframes(animPtr)) // last anim keyframe
				{
					actor->dynamicFlags.bIsHitting = 0;

					if (actor->field_78 == 0)
					{
						actor->animPosition = get_start_keyframe(animPtr);
					}
					else
					{
						actor->anim = actor->animExtra;
						actor->previousAnimIdx = get_body_anim_index(actor->anim, actorIdx);

						if (actor->previousAnimIdx == -1)
						{
							actor->previousAnimIdx = get_body_anim_index(0, actorIdx);
							actor->anim = 0;
						}

						actor->animExtraData = currentActorAnimExtraData;

						actor->field_78 = 0;
						actor->animPosition = 0;
						actor->strengthOfHit = 0;
					}

					if (actor->animExtraData)
					{
						//TODO process_anim_actions - GereAnimAction(actor, actorIdx);
					}

					actor->dynamicFlags.bAnimEnded = 1;
				}

				actor->lastRotationSpeed = 0;

				actor->lastX = 0;
				actor->lastY = 0;
				actor->lastZ = 0;
			}
		}
	}

	// TODO: actor standing of object
    // TODO: actor falling Y speed.
	// TODO: actor collisions
	// TODO: check actor damage
	// TODO: check actor bounding position

	actor->X = processActorX;
	actor->Y = processActorY;
	actor->Z = processActorZ;
}
