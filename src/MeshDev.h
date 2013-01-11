/***************************************************************************
                                  MeshDev.h
                             -------------------
    update               : 2002-09-03
    copyright            : (C) 2002 by Michaël ROY
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

#ifndef _MESHDEV_
#define _MESHDEV_

// C++ Standard Library
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <valarray>
#include <vector>

// C Standard Library
#include <math.h>		// sqrt, ceil
#include <stdlib.h>		// EXIT_SUCCESS, EXIT_FAILURE
#include <sys/timeb.h>	// timeb, ftime
#include <time.h>	    // time

// Use C++ Standard Library namespace
using namespace std;

// Linear Interpolation
template<class Type>
inline Type Lerp( const Type& a, const Type& b, const double& r )
{
	return a + (b - a) * r;
}

// Square
template<class Type>
inline Type Sqr( const Type& x )
{
	return x * x;
}


#endif // _MESHDEV_
