/** @file holomap.h
	@brief
	This file contains holomap routines

	TwinEngine: a Little Big Adventure engine
	
	Copyright (C) 2013 The TwinEngine team \n
	Copyright (C) 2008-2013 Prequengine team \n
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

#ifndef HOLOMAP_H
#define HOLOMAP_H

#include "sys.h"

/** Set Holomap location position
	@location Scene where position must be set */
void setHolomapPosition(int32 location);

/** Clear Holomap location position
	@location Scene where position must be cleared */
void clearHolomapPosition(int32 location);


#endif
