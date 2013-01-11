/***************************************************************************
                                FileVrml1.cpp
                             -------------------
    update               : 2003-04-22
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

#include "FileVrml1.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <iostream>

enum AttributeBinding
{
	UNSUPPORTED_BINDING,
	PER_VERTEX_BINDING,
	PER_VERTEX_INDEXED_BINDING
};

//--
//
// ReadVrml1
//
//--
// Read OpenInventor/VRML1.0 file
bool ReadVrml1( Mesh& mesh, const std::string& file_name )
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
	AttributeBinding color_binding(UNSUPPORTED_BINDING);
	AttributeBinding normal_binding(UNSUPPORTED_BINDING);

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
	
	// Try to find #Inventor in the line
	pos = line.find("#Inventor");
	if( pos > line.length() )
	{
		// Else try to find #VRML V1.0 in the line
		pos = line.find("#VRML V1.0");
		if( pos > line.length() )
		{
			return false;
		}
	}
	
	// Verify file is ascii format
	pos = line.find("ascii");
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
			// MATERIALBINDING
			//--
			else if( node[level] == "MaterialBinding" )
			{
				if( previous_word == "value" )
				{
					if( word == "PER_VERTEX" )
					{
						color_binding = PER_VERTEX_BINDING;
					}
					else if( word == "PER_VERTEX_INDEXED" )
					{
						color_binding = PER_VERTEX_INDEXED_BINDING;
					}
					else
					{
						color_binding = UNSUPPORTED_BINDING;
					}
				}
			}
			
			//--
			// NORMALBINDING
			//--
			else if( node[level] == "NormalBinding" )
			{
				if( previous_word == "value" )
				{
					if( word == "PER_VERTEX" )
					{
						normal_binding = PER_VERTEX_BINDING;
					}
					else if( word == "PER_VERTEX_INDEXED" )
					{
						normal_binding = PER_VERTEX_INDEXED_BINDING;
					}
					else
					{
						normal_binding = UNSUPPORTED_BINDING;
					}
   				}
			}

			//--
			// POINT
			//--
			else if( node[level] == "point" )
			{
				if( node[level-1] == "Coordinate3" )
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
				else if( node[level-1] == "TextureCoordinate2" )
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
			else if( node[level] == "diffuseColor" )
			{
				if( node[level-1] == "Material" )
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
			// TEXTURE2
			//--
			else if( node[level] == "Texture2" )
			{
				if( previous_word == "filename" )
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
			
			//--
			// MATERIALINDEX
			//--
			else if( node[level] == "materialIndex" )
			{
				if( node[level-1] == "IndexedFaceSet" )
				{
				}
			}
			
			//--
			// TEXTURECOORDINDEX
			//--
			else if( node[level] == "textureCoordIndex" )
			{
				if( node[level-1] == "IndexedFaceSet" )
				{
				}
			}
			
			//--
			// NORMALINDEX
			//--
			else if( node[level] == "normalIndex" )
			{
				if( node[level-1] == "IndexedFaceSet" )
				{
				}
			}
			
			// Save current word
			previous_word = word;
		}
	}
/*
	// Check vertex normals
	if( (normal_binding!=PER_VERTEX_BINDING) || (mesh.VertexNormalNumber()!=mesh.VertexNumber()) )
	{
		mesh.ClearVertexNormals();
	}

	// Normalize normals
	if( mesh.VertexNormalNumber() != 0 )
	{
		for( int i=0; i<mesh.VertexNormalNumber(); i++ )
		{
			mesh.VertexNormal(i).Normalize();
		}
	}
	
	// Check texture coordinates
	if( (texture_binding!=PER_VERTEX_BINDING) || (mesh.TextureNumber()!=mesh.VertexNumber()) )
	{
		mesh.ClearTextures();
		mesh.ClearTextureName();
	}
	
	// Check colors
	if( (mesh.ColorBinding()==PER_FACE_BINDING) && (mesh.ColorNumber()!=mesh.FaceNumber()) )
	{
		mesh.ClearColors();
		mesh.ColorBinding() = UNKNOWN_BINDING;
	}
	else if( (mesh.ColorBinding()==PER_VERTEX_BINDING) && (mesh.ColorNumber()!=mesh.VertexNumber()) )
	{
		mesh.ClearColors();
		mesh.ColorBinding() = UNKNOWN_BINDING;
	}
*/
	return true;
}

//--
//
// WriteVrml1
//
//--
// Write model to an OpenInventor / VRML 1.0 file
bool WriteVrml1( const Mesh& mesh, const std::string& file_name, bool inventor )
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

	//--
	// Write file Header
	//--
	
	// OpenInventor file header
	if( inventor ) file<<"#Inventor V2.1 ascii\n"<<endl;
	// or VRML 1.0 file header
	else file<<"#VRML V1.0 ascii\n"<<endl;
	
	// Write vertex number (comment)
	file<<"# Vertices: "<<mesh.VertexNumber()<<endl;
	// Write face number (comment)
	file<<"# Faces:    "<<mesh.FaceNumber()<<endl;
	file<<endl;
	
	// Begin description
	file<<"Separator {"<<endl;

	//--
	// Write  vertex coordinates
	//--
	file<<"    Coordinate3 {"<<endl;
	file<<"        point ["<<endl;
	for( i=0; i<(int)mesh.VertexNumber()-1; i++ )
	{
		file<<"            "<<mesh.Vertex(i)<<","<<endl;
	}
	file<<"            "<<mesh.Vertex(mesh.VertexNumber()-1)<<endl;
	file<<"        ]"<<endl;
	file<<"    }"<<endl;

	//--
	// Write colors
	//--
	if( mesh.ColorNumber() == mesh.VertexNumber() )
	{
		// Binding
		file<<"    MaterialBinding {"<<endl;
		file<<"        value PER_VERTEX"<<endl;
		file<<"    }"<<endl;
		// Color
		file<<"    Material {"<<endl;
		file<<"        ambientColor  0.54 0.54 0.54"<<endl;
		file<<"        specularColor 0.5 0.5 0.5"<<endl;
		file<<"        emissiveColor 0 0 0"<<endl;
		file<<"        shininess     0.15"<<endl;
		file<<"        transparency  0"<<endl;
		file<<"        diffuseColor ["<<endl;
		for( i=0; i<(int)mesh.ColorNumber()-1; i++ )
		{
			file<<"            "<<mesh.Color(i)<<","<<endl;
		}
		file<<"            "<<mesh.Color(mesh.ColorNumber()-1)<<endl;
		file<<"        ]"<<endl;
		file<<"    }"<<endl;
	}
	else
	{
		// Binding
		file<<"    MaterialBinding {"<<endl;
		file<<"        value OVERALL"<<endl;
		file<<"    }"<<endl;
		// Color
		file<<"    Material {"<<endl;
		file<<"        ambientColor  0.54 0.54 0.54"<<endl;
		file<<"        diffuseColor  0.55 0.55 0.55"<<endl;
		file<<"        specularColor 0.5 0.5 0.5"<<endl;
		file<<"        emissiveColor 0 0 0"<<endl;
		file<<"        shininess     0.15"<<endl;
		file<<"        transparency  0"<<endl;
		file<<"    }"<<endl;
	}

	//--
	// Write vertex normals
	//--
	if( mesh.VertexNormalNumber() == mesh.VertexNumber()  )
	{
		// Binding
		file<<"    NormalBinding {"<<endl;
		file<<"        value PER_VERTEX"<<endl;
		file<<"    }"<<endl;
		// Vertex normals
		file<<"    Normal {"<<endl;
		file<<"        vector ["<<endl;
		for( i=0; i<(int)mesh.VertexNormalNumber()-1; i++ )
		{
			file<<"            "<<mesh.VertexNormal(i)<<","<<endl;
		}
		file<<"            "<<mesh.VertexNormal(mesh.VertexNormalNumber()-1)<<endl;
		file<<"        ]"<<endl;
		file<<"    }"<<endl;
	}

	//--
	// Write texture
	//--
	if( (mesh.TextureNumber()==mesh.VertexNumber()) && (mesh.TextureName()!="")  )
	{
		// Texture file name
		file<<"    Texture2 {"<<endl;
		file<<"        filename \""<<mesh.TextureName()<<"\""<<endl;
		file<<"    }"<<endl;
		// Texture coordinates
		file<<"    TextureCoordinate2 {"<<endl;
		file<<"        point ["<<endl;
		for( i=0; i<(int)mesh.TextureNumber()-1; i++ )
		{
			file<<"            "<<mesh.Texture(i)<<","<<endl;
		}
		file<<"            "<<mesh.Texture(mesh.TextureNumber()-1)<<endl;
		file<<"        ]"<<endl;
		file<<"    }"<<endl;
	}

	//--
	// Write face indices
	//--
	
	// Begin face index block
	file<<"    IndexedFaceSet  {"<<endl;
	
	// Index (same for vertices, colors, texture cooridnates, normals)
	file<<"        coordIndex ["<<endl;
	for( i=0; i<(int)mesh.FaceNumber()-1; i++ )
	{
		file<<"            "<<mesh.Face(i,0)<<", "<<mesh.Face(i,1)<<", "<<mesh.Face(i,2)<<", -1,"<<endl;
	}
	file<<"            "<<mesh.Face(i,0)<<", "<<mesh.Face(i,1)<<", "<<mesh.Face(i,2)<<", -1"<<endl;
	file<<"        ]"<<endl;

	// End face index block
	file<<"     }"<<endl;

	// End description
	file<<"}"<<endl;
	
	// Close file
	file.close();
	
	return true;
}

