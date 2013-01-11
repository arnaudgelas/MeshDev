/***************************************************************************
                                 FileVrml1.h
                             -------------------
    update               : 2003-01-13
    copyright            : (C) 2002-2003 by Michaël Roy
    email                : michaelroy@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _FILEVRML1_
#define _FILEVRML1_

#include "Mesh.h"
#include <string>

//--
//
// OpenInventor / VRML1.0 file input/ouput
//
//--

// Read OpenInventor/VRML1.0 file
extern bool ReadVrml1( Mesh& mesh, const std::string& file_name );

// Write OpenInventor/VRML1.0 file
extern bool WriteVrml1( const Mesh& mesh, const std::string& file_name, bool inventor = false );


#endif // _FILEVRML1_

