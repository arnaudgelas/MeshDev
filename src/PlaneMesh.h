/***************************************************************************
                                 PlaneMesh.h
                             -------------------
    update               : 2002-09-25
    copyright            : (C) 2002 by Micha�l ROY
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

#ifndef _PLANEMESH_
#define _PLANEMESH_

#include "Mesh.h"
#include <vector>

// 
// PlaneMesh
// 
class PlaneMesh : public Mesh
{
	//
	// Member Functions
	//
	public :

		//
		// Constructor / Destructor
		// 
		inline PlaneMesh() {
		}
		
		inline ~PlaneMesh() {
		}

		//
		// Face Plane Interface
		//
		inline int FacePlaneNumber() const {
  			return (int)face_planes.size();
     	}
		
		inline std::vector<double>& FacePlanes() {
  			return face_planes;
     	}
     	
		inline const std::vector<double>& FacePlanes() const {
  			return face_planes;
     	}
     	
		inline double& FacePlane(int i) {
			assert( (i>=0) && (i<FacePlaneNumber()) );
  			return face_planes[i];
     	}

		inline const double& FacePlane(int i) const {
			assert( (i>=0) && (i<(int)FacePlaneNumber()) );
  			return face_planes[i];
     	}
     	
		inline void ClearFacePlanes() {
  			face_planes.clear();
		}

		//
        // Computation
        //
		void ComputeFacePlanes();
		
        
	//
	// Member Data
	//
	protected :

		std::vector<double>     face_planes;
};


#endif // _PLANEMESH_

