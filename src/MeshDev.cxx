/***************************************************************************
                                 MeshDev.cxx
                             -------------------
    update               : 2003-02-16
    copyright            : (C) 2002-2003 by Michael ROY
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


#include "MeshDev.h"
#include "Deviation.h"
#include "Mesh.h"
#include "Stopwatch.h"

static const string banner_string =
"             MeshDev 0.5\n"
"       Mesh Comparison Software\n"
" Copyright (C) 2002-2003 Michael Roy\n";

static const string usage_string =
"Usage: MeshDev [options] <file1> <file2>\n"
"\n"
"<file1> <file2>: input meshes (file1 is reference mesh)\n"
"\n"
"Available options:\n"
"\n"
"-d <type>      Deviation type:\n"
"                       0 = geometric [default]\n"
"                       1 = normals\n"
"                       2 = colors\n"
"                       3 = texture\n"
"\n"
"-s <size>      Sample step size:\n"
"                       0              = no sampling [default]\n"
"                       [0.1  - 0.01]  = low\n"
"                       [0.01 - 0.001] = high\n"
"\n"
"-g <size>      Uniform grid cell size:\n"
"                       0.1 = small\n"
"                       0.5 = medium [default]\n"
"                       1.0 = big\n"
"\n"
"-o <file>      Write final model with mapped deviation\n"
"\n"
"-b <value>     Set deviation upper bound value for color\n";

static const string error_string [] =
{
	"Malformed command line",			// 0
	"Unknown option",				// 1
	"Unknown deviation type",			// 2
	"Output file name without any extension",	// 3
	"Unknown output file format",			// 4
	"Deviation computation failed",			// 5
	"Error writing result mesh"			// 6
};

static string mafn;
static string mbfn;
static string ofn;
static FileFormat of = VRML_2_FILE; // Output file format
static Mesh* ma = 0;
static Mesh* mb = 0;
// Compute successed
static bool ComputeOK;
 // The deviations computer
static Deviation* dev;
// A little timer
static Stopwatch timer;
// Sampling Step
static double sample_step=0;
// Uniform Grid Size
static double grid_size=0.5;
// Deviation Type to measure
static int dev_type=0;
static double dev_bound(0);

//========================================================
//
// Help
//
//========================================================
// Display how to use this application
static void Help()
{
    cout<<usage_string<<endl;
}

static void Error( int n, bool line_feed = true )
{
	cerr<<"Error"<<endl;
	cerr<<error_string[n];
	if( line_feed )
		cerr<<endl<<endl;
}

//========================================================
//
// Parameters
//
//========================================================
// Load Option Parameters
static bool Parameters( int argc, char *argv[] )
{
	if( argc == 1 )
	{
	    Help();
		exit( EXIT_SUCCESS );
	}

	int loop = 1;

	//--
	// Options
	//--
	while( loop < argc )
	{
		// Help
		if( (strcmp(argv[loop], "-h") == 0) || (strcmp(argv[loop], "-help") == 0) )
		{
			Help();
 			exit( EXIT_SUCCESS );
		}
		
		// Deviation bound
		else if( strcmp(argv[loop], "-b") == 0 )
		{
			dev_bound = atof( argv[++loop] );
		}
		
		// Deviation type
		else if( strcmp(argv[loop], "-d") == 0 )
		{
			dev_type = atoi( argv[++loop] );
		}
		
		// Sample step size
	   	else if( strcmp(argv[loop], "-s") == 0 )
		{
			sample_step = atof( argv[++loop] );
		}
		
		// Uniform grid size
	   	else if( strcmp(argv[loop], "-g") == 0 )
		{
			grid_size = atof( argv[++loop] );
		}
		
		// Output file name
	   	else if( strcmp(argv[loop], "-o") == 0 )
		{
			ofn = argv[++loop];
		}
		
	    // Input file names
	    else if( loop == (argc - 2) )
	    {
			mafn = argv[loop];
			mbfn = argv[++loop];
	    }
	    
	    // Error
	    else
	    {
			Error( 0 );
			return false;
	    }
	    loop++;
	}

	//-------------------------------------------------------
	//
	// Error tests
	// 
	//-------------------------------------------------------
	if( mafn.empty() || mbfn.empty() )
	{
	    Error( 0 );
	    return false;
	}
	    
	if( (dev_type < 0) || (dev_type > 3) )
	{
		Error( 2 );
		return false;
	}
/*
	if( !ofn.empty() )
	{
		int pos = ofn.find_last_of('.');
		if( pos == -1 )
		{
			Error( 3 );
			return false;
		}
		string ext = ofn.substr( ++pos );
		if( (ext == "smf") || (ext == "SMF") )
		{
			of = SMF_FILE;
		}
		else if( (ext == "iv") || (ext == "IV") )
		{
			of = IV_FILE;
		}
		else if( (ext == "wrl") || (ext == "WRL") )
		{
			of = VRML_1_FILE;
		}
		else
		{
			Error( 4, false );
			cerr<<" \"."<<ext<<"\""<<endl<<endl;
			return false;
		}
	}
*/
	return true;
}

//========================================================
//
// LoadModels
//
//========================================================
// Load Model Files
static bool LoadModels()
{
	ma = new Mesh;
	mb = new Mesh;

	assert( ma != 0 );
	assert( mb != 0 );

	if( !ma->ReadFile(mafn) ) return false;
 	if( !mb->ReadFile(mbfn) ) return false;

	return true;
}


static void OutputLog()
{
	cout<<"Mesh 1"<<endl;
	cout<<"   "<<mafn<<endl;
	cout<<"   Vertices:    "<<ma->VertexNumber()<<endl;
	cout<<"   Faces:       "<<ma->FaceNumber()<<endl;
//	cout<<"   Area:        "<<ma->Area()<<endl;
	cout<<endl;
	cout<<"Mesh 2"<<endl;
	cout<<"   "<<mbfn << endl;
	cout<<"   Vertices:    "<<mb->VertexNumber()<<endl;
	cout<<"   Faces:       "<<mb->FaceNumber()<<endl;
//	cout<<"   Area:        "<<mb->Area()<< endl;
	cout<<endl;
	switch( dev_type )
	{
		case 0 :
			cout<<"Geometric Deviation"<<endl;
			break;
		case 1 :
			cout<<"Normal Deviation"<<endl;
			break;
		case 2 :
			cout<<"Color Deviation"<<endl;
			break;
		case 3 :
			cout<<"Texture Deviation"<<endl;
			break;
	}
	cout<<"   Minimum:     "<<dev->Min()<<endl;
	cout<<"   Maximum:     "<<dev->Max()<<endl;
	cout<<"   Mean:        "<<dev->Mean()<<endl;
	cout<<"   RMS:         "<<dev->Rms()<<endl;
	cout<<"   Variance:    "<<dev->Variance()<<endl;
//	cout<<"   Median:      "<<dev->Median()<<endl;
	if( sample_step )
		cout<<"   Samples:     "<<dev->SampleNumber()<<endl;
	cout<<endl;
}

//========================================================
//
// main
//
//========================================================
// Main Program
int main(int argc, char *argv[])
{
	cout<<endl<<banner_string<<endl;
	if( Parameters(argc, argv) == false ) return EXIT_FAILURE;

	timer.Reset();

	cout<<"Model load...      "<<flush;
	timer.Start();
	if( !LoadModels() ) return EXIT_FAILURE;
 	timer.Stop();
	cout<<std::setw(5)<<timer.Intermediate()<<endl;

	cout<<"Initialization...  "<<flush;
	timer.Start();
	// Initialize member data
	dev = new Deviation;
	if( dev_type == 1 )
	{
		if( ma->VertexNormalNumber() != ma->VertexNumber() )
		{
			ma->ComputeFaceNormals();
			ma->ComputeVertexNormals();
		}
		if( mb->VertexNormalNumber() != mb->VertexNumber() )
		{
			mb->ComputeFaceNormals();
			mb->ComputeVertexNormals();
		}
	}
	ComputeOK = false;

	dev->Initialization( ma, mb, sample_step, grid_size );
	dev->SetDeviationColorBound( dev_bound );
	
  	timer.Stop();
	cout<<setw(5)<<timer.Intermediate()<<endl;

	cout<<"Processing...      "<<flush;
	timer.Start();
	switch(dev_type )
	{
		case 0 :
			ComputeOK = dev->Compute( GEOMETRIC_DEVIATION );
			break;

		case 1 :
			ComputeOK = dev->Compute( NORMAL_DEVIATION );
			break;

		case 2 :
			ComputeOK = dev->Compute( COLOR_DEVIATION );
			break;

		case 3 :
			ComputeOK = dev->Compute( TEXTURE_DEVIATION );
			break;
	}

	if( !ComputeOK )
	{
		Error( 5 );
		delete dev;
		return EXIT_FAILURE;
	}

  	timer.Stop();
	cout<<std::setw(5)<<timer.Intermediate()<<endl;

	// Write result file
	if(  !ofn.empty() )
	{
		bool write_ok = false;
		cout<<"Writing result...  "<<flush;
		timer.Start();
		write_ok = ma->WriteFile(ofn.c_str(), of );
		if( !write_ok )
		{
			Error( 6 );
			return EXIT_FAILURE;
		}
	  	timer.Stop();
		cout<<setw(5)<<timer.Intermediate()<<endl;
	}

	// Total time
	cout<<"Total time...      "<<setw(5)<<timer.Total()<<endl<<endl;

	// Write output log file
	OutputLog();

	// Free member data
	delete dev;

	// Finish !
	return EXIT_SUCCESS;
}
