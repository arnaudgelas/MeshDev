/***************************************************************************
                                  Deviation.h
                             -------------------
    update               : 2003-01-09
    copyright            : (C) 2002-2003 by Micha�l ROY
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

#ifndef _DEVIATION_
#define _DEVIATION_

#include "MeshDev.h"
#include "Mesh.h"
#include "Stopwatch.h"
#include "Sample.h"
#include "BoundingBox.h"
#include "UniformGrid.h"

// Available deviation types
enum DeviationType
{
	GEOMETRIC_DEVIATION,
	NORMAL_DEVIATION,
	COLOR_DEVIATION,
	TEXTURE_DEVIATION
};


class Deviation
{
	public :

		Deviation();
		~Deviation();

		bool Initialization( Mesh* a, Mesh* b, double SampleStep = 0, double GridSize = 0.5 );
		bool Compute( DeviationType type ); 

		double Mean() { return meandev; }
		double Min() { return mindev;}
		double Max() { return maxdev; }
		double Median() { return meddev; }
		double Rms() {
			return rmsdev;
		}
		double Variance() { return vardev; }

		int SampleNumber() { return snum; }
		
		inline void SetDeviationColorBound( const double& v ) {
			dev_bound = v;
		}

	private :

		void Deviation2Material();

		bool Statistics();
		void StatisticsSample();

		bool GeometricDeviation();
		bool GeometricDeviationSample();

		template<class Type>
		bool MeshDeviation( vector<Type>& aa, vector<Type>& ab );
		template<class Type>
		bool MeshDeviationSample( vector<Type>& aa, vector<Type>& ab );


		Vector3d Deviation2Color( const double& d );
		void SampleFace( int face );

		Mesh* ma;
		Mesh* mb;

		int mavn;
		int mbvn;

		int mafn;
		int mbfn;

		valarray<double> dev; // Deviation

		double mindev;
		double maxdev;
		double vardev;
		double meandev;
		double meddev;
		double rmsdev;
		double dev_bound;

		BoundingBox3d* bb;
		UniformGrid* ug;

		double step;
		Vector3d sampleu, samplev; // Vectors for sampling
		Sample* samples;
		int snum;

};



#endif // _DEVIATION_

