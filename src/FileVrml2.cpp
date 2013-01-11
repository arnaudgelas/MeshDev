/***************************************************************************
                                FileVrml2.cpp
                             -------------------
    update               : 2003-04-22
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

#include "FileVrml2.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>

//--
//
// ReadVrml2
//
//--
// Read VRML2.0 file
bool ReadVrml2( Mesh& mesh, const std::string& file_name )
{
	std::ifstream file; // File to read
	std::string line, new_line; // Lines to process
	std::string word, previous_word; // Words to process
	std::string::iterator it; // Iterator for lines
	size_t pos; // String position
	int nlbrack(0), nrbrack(0); // Left/Right delimiter counter
	int level(0); // Node level
	int ixyz(0); // Coordinate index
	Vector3d vec3d; // Temp vector 3D (vertex, color, normal)
	Vector3i vec3i; // Temp vector 3I (face)
	Vector2d vec2d; // Temp vector 2D (texture coordinate)
	std::vector<std::string> node(10); // nodes - 10 levels by default

	// Open file
	file.open( file_name.c_str() );
	
	// Test if file is open
	if( file.is_open() == false )
	{
		return false;
	}

	//--
	// Read file header
	//--
	
	// Get first line of the file
	std::getline( file, line );
	
	// Check Vrml2.0 file header
	pos = line.find("#VRML V2.0");
	if( pos > line.length() )
	{
		return false;
	}
	
	// Initialize the mesh
	mesh.ClearAll();

	//--
	// Read the file until the end reading line by line
	//--
	while( std::getline( file, line ) )
	{
		// Replace all comas by space
		std::replace( line.begin(), line.end(), ',', ' ' );

		// Initialize new_line		
		new_line.erase( new_line.begin(), new_line.end() );

		//--
		// Buffer brackets and braces by space and copy in new_line
		//--
		it = line.begin();
		while( it != line.end() )
		{
			
			if( ((*it)=='{') || ((*it)=='}') || ((*it)=='[') || ((*it)==']') || ((*it)=='#') )
			{
				new_line += ' ';
				new_line += *it++;
				new_line += ' ';
			}
			else 
			{
		   		new_line += *it++;
 			}
		}
		
		//--
		// Analyse each word in the line 
		//--
		std::istringstream word_stream( new_line );
		while( word_stream >> word )
		{
			//--
			// Left bracket or brace
			//--
			if( (word == "{") || (word == "[") )
			{
				// Increment left deliminter number
				nlbrack++;
				// Get level number
				level = nlbrack - nrbrack;
				// Save level name
				if( level > (int)node.size() )
				{
					node.push_back( previous_word );
				}
				else
				{
					node[level] = previous_word;
				}
				// Initialize coordinate index
				ixyz = 0;
			}
			
			//--
			// Right bracket or brace
			//--
			else if( (word == "}") || (word == "]") )
			{
				// Increment right deliminter number
				nrbrack++;
				// Get level number
				level = nlbrack - nrbrack;
				// Sanity test
				if( level < 0 )
				{
					mesh.ClearAll();
					return false;
				}
			}
			
			//--
			// Comment
			//--
			else if( word == "#" )
			{
				// Save current word
				previous_word = word;
				// Next line
				break;
			}
			
			//--
			// POINT
			//--
			else if( node[level] == "point" )
			{
				// Geometry
				if( node[level-1] == "Coordinate" )
				{
					// Get current value
					vec3d[ixyz] = atof( word.c_str() );
					// Complete coordinate ?
					if( ixyz == 2 )
					{
						mesh.AddVertex( vec3d );
						ixyz = 0;
					}
					else
					{
						ixyz++;
					}
				}
				// Texture
				else if( node[level-1] == "TextureCoordinate" )
				{
					// Get current value
					vec2d[ixyz] = atof( word.c_str() );
					// Complete coordinate ?
					if( ixyz == 1 )
					{
						mesh.AddTexture( vec2d );
						ixyz = 0;
					}
					else
					{
						ixyz++;
					}
				}
			}
			
			//--
			// DIFFUSECOLOR
			//--
			else if( node[level] == "color" )
			{
				if( node[level-1] == "Color" )
				{
					// Get current value
					vec3d[ixyz] = atof( word.c_str() );
					// Complete coordinate ?
					if( ixyz == 2 )
					{
						mesh.AddColor( vec3d );
						ixyz = 0;
					}
					else
					{
						// Next coordinate
						ixyz++;
					}
				}
			}
			
			//--
			// VECTOR
			//--
			else if( node[level] == "vector" )
			{
				if( node[level-1] == "Normal" )
				{
					// Get current value
					vec3d[ixyz] = atof( word.c_str() );
					// Complete coordinate ?
					if( ixyz == 2 )
					{
						mesh.AddVertexNormal( vec3d );
						ixyz = 0;
					}
					else
					{
						// Next coordinate
						ixyz++;
					}
				}
			}
			
			//--
			// ImageTexture
			//--
			else if( node[level] == "ImageTexture" )
			{
				if( previous_word == "url" )
				{
					if( word.size() > 2 )
					{
						// Remove quotes in filename
						word.erase( word.begin() );
						word.erase( word.end()-1 );
						// Get texture filename
						mesh.TextureName() = word;
					}
				}
			}
			
			//--
			// COORDINDEX
			//--
			else if( node[level] == "coordIndex" )
			{
				if( node[level-1] == "IndexedFaceSet" )
				{
					// -1 value
					if( ixyz == 3 )
					{
						// Next face
						ixyz = 0;
						continue;
					}
					// Get value
					vec3i[ixyz] = atoi( word.c_str() );
					// Complete coordinate ?
					if( ixyz == 2 )
					{
						mesh.AddFace( vec3i );
					}
					// Next coordinate
					ixyz++;
				}
			}
			
			// Save current word
			previous_word = word;
		}
	}
	
	return true;
}

//--
//
// WriteVrml2
//
//--
// Write model to an VRML 2.0 file
bool WriteVrml2( const Mesh& mesh, const std::string& file_name )
{
	int i;
	using std::endl;
	
	// Test if mesh is valid
	if( (mesh.VertexNumber()==0) || (mesh.FaceNumber()==0) )
	{
		return false;
	}

	// Open file for writing
	std::ofstream file( file_name.c_str() );
	
	// Test if the file is open
	if( file.is_open() == false )
	{
		return false;
	}

	// VRML 2.0 file header
	file<<"#VRML V2.0 utf8\n"<<endl;
		
	// Write vertex number (comment)
	file<<"# Vertices: "<<mesh.VertexNumber()<<endl;
	// Write face number (comment)
	file<<"# Faces:    "<<mesh.FaceNumber()<<endl;
	file<<endl;
	
	// Begin 1st transform description (to get first rotation)
	file<<"Transform {"<<endl;
	file<<"  rotation 1 0 0 0"<<endl;

	// Begin 1st transform children description
	file<<"  children ["<<endl;

	// Begin 2nd transform description (to get second rotation)
	file<<"    Transform {"<<endl;
	file<<"      rotation 0 1 0 0"<<endl;

	// Begin 2nd transform children description
	file<<"      children ["<<endl;

	// Begin 3rd transform description (to get second rotation)
	file<<"        Transform {"<<endl;
	file<<"          rotation 0 0 1 0"<<endl;

	// Begin 3rd transform children description
	file<<"          children ["<<endl;

	// Begin shape description
	file<<"            Shape {"<<endl;

	// Begin appearance description
	file<<"              appearance Appearance {"<<endl;

	// Write default material
	file<<"                material Material {"<<endl;
	file<<"                  ambientIntensity  0.54"<<endl;
	file<<"                  diffuseColor      0.55 0.55 0.55"<<endl;
	file<<"                  specularColor     0.5 0.5 0.5"<<endl;
	file<<"                  emissiveColor     0 0 0"<<endl;
	file<<"                  shininess         0.15"<<endl;
	file<<"                  transparency      0"<<endl;
	file<<"                }"<<endl;
	
	// Write texture filename
	if( (mesh.TextureNumber()==mesh.VertexNumber()) && (mesh.TextureName()!="")  )
	{
		file<<"                texture ImageTexture {"<<endl;
		file<<"                  url \""<<mesh.TextureName()<<"\""<<endl;
		file<<"                }"<<endl;
	}

	// End appearance description
	file<<"              }"<<endl;

	// Begin geometry description
	file<<"              geometry IndexedFaceSet  {"<<endl;
	
	// Write  vertex coordinates
	file<<"                coord Coordinate {"<<endl;
	file<<"                  point ["<<endl;
	for( i=0; i<(int)mesh.VertexNumber()-1; i++ )
	{
		file<<"                    "<<mesh.Vertex(i)<<","<<endl;
	}
	file<<"                    "<<mesh.Vertex(mesh.VertexNumber()-1)<<endl;
	file<<"                  ]"<<endl;
	file<<"                }"<<endl;

	// Write colors
	if( mesh.ColorNumber() == mesh.VertexNumber()  )
	{
		file<<"                colorPerVertex TRUE"<<endl;
		file<<"                color Color {"<<endl;
		file<<"                  color ["<<endl;
		for( i=0; i<(int)mesh.ColorNumber()-1; i++ )
		{
			file<<"                    "<<mesh.Color(i)<<","<<endl;
		}
		file<<"                    "<<mesh.Color(mesh.ColorNumber()-1)<<endl;
		file<<"                  ]"<<endl;
		file<<"                }"<<endl;
	}

	// Write vertex normals
	if( mesh.VertexNormalNumber() == mesh.VertexNumber()  )
	{
		file<<"                normalPerVertex TRUE"<<endl;
		file<<"                normal Normal {"<<endl;
		file<<"                  vector ["<<endl;
		for( i=0; i<(int)mesh.VertexNormalNumber()-1; i++ )
		{
			file<<"                    "<<mesh.VertexNormal(i)<<","<<endl;
		}
		file<<"                    "<<mesh.VertexNormal(mesh.VertexNormalNumber()-1)<<endl;
		file<<"                  ]"<<endl;
		file<<"                }"<<endl;
	}

	// Write texture coordinates
	if( (mesh.TextureNumber()==mesh.VertexNumber()) && (mesh.TextureName()!="")  )
	{
		file<<"                texCoord TextureCoordinate {"<<endl;
		file<<"                  point ["<<endl;
		for( i=0; i<(int)mesh.TextureNumber()-1; i++ )
		{
			file<<"                    "<<mesh.Texture(i)<<","<<endl;
		}
		file<<"                    "<<mesh.Texture(mesh.TextureNumber()-1)<<endl;
		file<<"                  ]"<<endl;
		file<<"                }"<<endl;
	}

	// Write face indices
	file<<"                coordIndex ["<<endl;
	for( i=0; i<(int)mesh.FaceNumber()-1; i++ )
	{
		file<<"                  "<<mesh.Face(i,0)<<", "<<mesh.Face(i,1)<<", "<<mesh.Face(i,2)<<", -1,"<<endl;
	}
	file<<"                  "<<mesh.Face(i,0)<<", "<<mesh.Face(i,1)<<", "<<mesh.Face(i,2)<<", -1"<<endl;
	file<<"                ]"<<endl;

	// End geometry description
	file<<"              }"<<endl;

	// End shape description
	file<<"            }"<<endl;
	
	// End 3rd transform children description
	file<<"          ]"<<endl;
	
	// End 3rd transform description
	file<<"        }"<<endl;
	
	// End 2nd transform children description
	file<<"      ]"<<endl;
	
	// End 2nd transform description
	file<<"    }"<<endl;
	
	// End 1st transform children description
	file<<"  ]"<<endl;
	
	// End 1st transform description
	file<<"}"<<endl;
	
	// Close file
	file.close();
	
	return true;
}

