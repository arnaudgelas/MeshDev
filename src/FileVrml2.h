/***************************************************************************
                                 FileVrml2.h
                             -------------------
    update               : 2003-01-13
    copyright            : (C) 2003 by Michaël Roy
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

#ifndef _FILEVRML2_
#define _FILEVRML2_

#include "Mesh.h"
#include <string>

//--
//
// VRML2.0 file input/ouput
//
//--

// Read VRML2.0 file
extern bool ReadVrml2( Mesh& mesh, const std::string& file_name );

// Write VRML2.0 file
extern bool WriteVrml2( const Mesh& mesh, const std::string& file_name );


#endif // _FILEVVRML2_

