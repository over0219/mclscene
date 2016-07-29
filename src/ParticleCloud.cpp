// Copyright 2016 Matthew Overby.
// 
// MCLSCENE Uses the BSD 2-Clause License (http://www.opensource.org/licenses/BSD-2-Clause)
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other materials
//    provided with the distribution.
// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF MINNESOTA, DULUTH OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// By Matt Overby (http://www.mattoverby.net)

#include "MCL/ParticleCloud.hpp"

using namespace mcl;

namespace parse_helper {
	static std::string get_ext( std::string fname ){
		size_t pos = fname.find_last_of('.');
		return (std::string::npos == pos) ? "" : fname.substr(pos+1,fname.size());
	}
	static std::string to_lower( std::string s ){ std::transform( s.begin(), s.end(), s.begin(), ::tolower ); return s; }
}


std::string ParticleCloud::get_xml( std::string obj_name, int mode ){

	// Save the PLY
	std::stringstream plyfile;
	plyfile << MCLSCENE_BUILD_DIR<< "/" << obj_name << ".ply";
	data->write( plyfile.str() );

	// mclscene
	if( mode == 0 ){
		std::stringstream xml;
		xml << "\t<Object name=\"" << obj_name << "\" type=\"ParticleCloud\" >\n";
		xml << "\t\t<File type=\"string\" value=\"" << plyfile.str() << "\" />\n";
		xml << "\t\t<Material type=\"string\" value=\"" << material << "\" />\n";
		xml << "\t</Object>";
		return xml.str();
	}

	return "";

} // end get xml


bool ParticleCloud::load( std::string file ){

	std::string ext = parse_helper::to_lower( parse_helper::get_ext( file ) );

	// Read the file
	if( ext == "ply" ){
		data.reset( trimesh::TriMesh::read( file ) );
		if( data.get() == NULL ){
			data.reset( new trimesh::TriMesh );
			return false;
		}
	} // end load ply

	else if( ext == "node" ){

		// Load the vertices
		std::ifstream filestream;
		filestream.open( file.c_str() );
		if( !filestream ){ std::cerr << "\n**ParticleCloud::load Error: Could not load " << file << std::endl; return false; }

		std::string header;
		getline( filestream, header );
		std::stringstream headerSS(header);
		int n_nodes = 0; headerSS >> n_nodes;

		vertices.resize( n_nodes );
		std::vector< int > vertex_set( n_nodes, 0 );
		bool starts_with_one = false;

		for( int i=0; i<n_nodes; ++i ){
			std::string line;
			getline( filestream, line );
			std::stringstream lineSS(line);
			double x, y, z;
			int idx;
			lineSS >> idx >> x >> y >> z;

			// Check for 1-indexed
			if( i==0 && idx==1 ){ starts_with_one = true; }
			if( starts_with_one ){ idx -= 1; }

			if( idx > vertices.size() ){
				std::cerr << "\n**ParticleCloud::load Error: Your indices are bad for file " << file << std::endl; return false;
			}

			vertices[idx] = trimesh::point( x, y, z );
			vertex_set[idx] = 1;
		}
		filestream.close();

		for( int i=0; i<vertex_set.size(); ++i ){
			if( vertex_set[i] == 0 ){ std::cerr << "\n**ParticleCloud::load Error: Your indices are bad for file " << file << std::endl; return false; }
		}

	} // end load node

	else {
		std::cerr << "\n**ParticleCloud::load Error: I don't know how to load a file of type \"" << ext << "\"" << std::endl;
		return false;
	}

	// Update the cloud
	update();

	return true;

} // end load ply


void ParticleCloud::bounds( trimesh::vec &bmin, trimesh::vec &bmax ){
	if( !aabb->valid ){
		for( int v=0; v<vertices.size(); ++v ){
			(*aabb) += vertices[v];
		}
	}
	bmin = aabb->min; bmax = aabb->max;
} // end bounds


void ParticleCloud::update(){

	// Update the bounding box
	aabb->valid = false;
	for( int v=0; v<vertices.size(); ++v ){
		(*aabb) += vertices[v];
	}

} // end update


