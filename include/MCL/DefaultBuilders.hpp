// Copyright (c) 2016 University of Minnesota
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

#ifndef MCLSCENE_DEFAULTBUILDERS_H
#define MCLSCENE_DEFAULTBUILDERS_H 1

#include <functional> // for std::function
#include "TriMeshBuilder.h"
#include "TetMesh.hpp"
#include "TriangleMesh.hpp"
#include "PointCloud.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "Camera.hpp"
#include "../../deps/pugixml/pugixml.hpp"

namespace mcl {


//
//	The builder types
//
typedef std::function<std::shared_ptr<BaseCamera> ( std::string type, std::vector<Param> &params )> BuildCamCallback;
typedef std::function<std::shared_ptr<BaseObject> ( std::string type, std::vector<Param> &params )> BuildObjCallback;
typedef std::function<std::shared_ptr<BaseLight> ( std::string type, std::vector<Param> &params )> BuildLightCallback;
typedef std::function<std::shared_ptr<BaseMaterial> ( std::string type, std::vector<Param> &params )> BuildMatCallback;


//
//	Default Object Builder: Everything is a trimesh or tetmesh.
//
static std::shared_ptr<BaseObject> default_build_object( std::string type, std::vector<Param> &params ){

	using namespace trimesh;
	type = parse::to_lower(type);

	//
	//	First build the transform and other common params
	//
	xform x_form;
	int material = -1;
	for( int i=0; i<params.size(); ++i ){

		std::string tag = parse::to_lower(params[i].tag);

		if( tag=="translate" ){
			x_form = params[i].as_xform() * x_form;
		}
		else if( tag=="scale" ){
			x_form = params[i].as_xform() * x_form;
		}
		else if( tag=="rotate" ){
			x_form = params[i].as_xform() * x_form;
		}
		else if( tag=="material" ){
			material = params[i].as_int();
		}
	}
	

	//
	//	Sphere
	//
	if( type == "sphere" ){

		std::shared_ptr<TriMesh> tris( new TriMesh() );

		double radius = 1.0;
		vec center(0,0,0);
		int tessellation = 1;

		for( int i=0; i<params.size(); ++i ){
			if( parse::to_lower(params[i].tag)=="radius" ){ radius=params[i].as_double(); }
			else if( parse::to_lower(params[i].tag)=="center" ){ center=params[i].as_vec3(); }
			else if( parse::to_lower(params[i].tag)=="tess" ){ tessellation=params[i].as_int(); }
		}

		make_sphere_polar( tris.get(), tessellation, tessellation );

		// Now scale it by the radius
		xform s_xf = trimesh::xform::scale(radius,radius,radius);
		apply_xform(tris.get(), s_xf);

		// Translate so center is correct
		xform t_xf = trimesh::xform::trans(center[0],center[1],center[2]);
		apply_xform(tris.get(), t_xf);

		tris.get()->need_normals();
		tris.get()->need_tstrips();
		std::shared_ptr<BaseObject> new_obj( new mcl::TriangleMesh(tris) );
		new_obj->apply_xform( x_form );
		if( material >= 0 ){ new_obj->set_material( material ); }
		return new_obj;

	} // end build sphere


	//
	//	Box
	//
	else if( type == "box" || type == "cube" ){


		//
		//	For some reason the make_cube function is broken???
		//	Just use the make beam with 1 chunk for now.
		//

		std::shared_ptr<TriMesh> tris( new TriMesh() );

		int tess = 3;
		int chunks = 1;
		for( int i=0; i<params.size(); ++i ){
			if( parse::to_lower(params[i].tag)=="tess" ){ tess=params[i].as_int(); }
		}
		make_beam( tris.get(), tess, chunks );

		tris.get()->need_normals();
		tris.get()->need_tstrips();
		std::shared_ptr<BaseObject> new_obj( new mcl::TriangleMesh(tris) );
		new_obj->apply_xform( x_form );
		if( material >= 0 ){ new_obj->set_material( material ); }
		return new_obj;

	} // end build box


	//
	//	Plane, 2 or more triangles
	//
	else if( type == "plane" ){

		std::shared_ptr<TriMesh> tris( new TriMesh() );

		int width = 10;
		int length = 10;
		double noise = 0.0;

		for( int i=0; i<params.size(); ++i ){
			if( parse::to_lower(params[i].tag)=="width" ){ width=params[i].as_int(); }
			else if( parse::to_lower(params[i].tag)=="length" ){ length=params[i].as_int(); }
			else if( parse::to_lower(params[i].tag)=="noise" ){ noise=params[i].as_double(); }
		}

		make_sym_plane( tris.get(), width, length );
		if( noise > 0.0 ){ trimesh::noisify( tris.get(), noise ); }

		tris.get()->need_normals();
		tris.get()->need_tstrips();
		std::shared_ptr<BaseObject> new_obj( new mcl::TriangleMesh(tris) );
		new_obj->apply_xform( x_form );
		if( material >= 0 ){ new_obj->set_material( material ); }
		return new_obj;

	} // end build plane


	//
	//	Beam
	//
	else if( type == "beam" ){

		std::shared_ptr<TriMesh> tris( new TriMesh() );

		int tess = 3;
		int chunks = 5;

		for( int i=0; i<params.size(); ++i ){
			if( parse::to_lower(params[i].tag)=="tess" ){ tess=params[i].as_int(); }
			else if( parse::to_lower(params[i].tag)=="chunks" ){ chunks=params[i].as_int(); }
		}


		make_beam( tris.get(), tess, chunks );

		tris.get()->need_normals();
		tris.get()->need_tstrips();
		std::shared_ptr<BaseObject> new_obj( new mcl::TriangleMesh(tris) );
		new_obj->apply_xform( x_form );
		if( material >= 0 ){ new_obj->set_material( material ); }
		return new_obj;

	} // end build beam

	//
	//	Cylinder
	//
	else if( type == "cylinder" ){

		std::shared_ptr<TriMesh> tris( new TriMesh() );

		float radius = 1.f;
		int tess_l=10, tess_c=10;

		for( int i=0; i<params.size(); ++i ){
			if( parse::to_lower(params[i].tag)=="tess_l" ){ tess_l=params[i].as_int(); }
			if( parse::to_lower(params[i].tag)=="tess_c" ){ tess_c=params[i].as_int(); }
			else if( parse::to_lower(params[i].tag)=="radius" ){ radius=params[i].as_float(); }
		}

		trimesh::make_ccyl( tris.get(), tess_l, tess_c, radius );
		tris.get()->need_normals();
		tris.get()->need_tstrips();
		std::shared_ptr<BaseObject> new_obj( new mcl::TriangleMesh(tris) );
		new_obj->apply_xform( x_form );
		if( material >= 0 ){ new_obj->set_material( material ); }
		return new_obj;

	} // end build cylinder



	//
	//	Torus
	//
	else if( type == "torus" ){

		std::shared_ptr<TriMesh> tris( new TriMesh() );

		int tess_th=50, tess_ph=20;
		float inner_rad = 0.25f;
		float outer_rad = 1.f; // doesn't do anything?

		for( int i=0; i<params.size(); ++i ){
			if( parse::to_lower(params[i].tag)=="tess_th" ){ tess_th=params[i].as_int(); }
			else if( parse::to_lower(params[i].tag)=="tess_ph" ){ tess_ph=params[i].as_int(); }
			else if( parse::to_lower(params[i].tag)=="inner_radius" ){ inner_rad=params[i].as_float(); }
//			else if( parse::to_lower(params[i].tag)=="outer_radius" ){ outer_rad=params[i].as_float(); }
		}

		trimesh::make_torus( tris.get(), tess_th, tess_ph, inner_rad, outer_rad );
		tris.get()->need_normals();
		tris.get()->need_tstrips();
		std::shared_ptr<BaseObject> new_obj( new mcl::TriangleMesh(tris) );
		new_obj->apply_xform( x_form );
		if( material >= 0 ){ new_obj->set_material( material ); }
		return new_obj;

	}


	//
	//	Triangle Mesh, 2 or more triangles
	//
	else if( type == "trimesh" || type == "trianglemesh" ){

		std::shared_ptr<TriMesh> tris( new TriMesh() );
		tris->set_verbose(0);

		std::string filename = "";
		for( int i=0; i<params.size(); ++i ){
			if( parse::to_lower(params[i].tag)=="file" ){ filename=params[i].as_string(); }
		}

		// Try to load the trimesh
		if( filename.size() ){
			tris.reset( trimesh::TriMesh::read( filename.c_str() ) );
			if( tris == NULL ){ printf("\n**TriangleMesh Error: failed to load file %s\n", filename.c_str()); }
		}

		std::shared_ptr<BaseObject> new_obj( new mcl::TriangleMesh(tris) );
		new_obj->apply_xform( x_form );
		if( material >= 0 ){ new_obj->set_material( material ); }
		return new_obj;

	} // end build trimesh


	//
	//	Tet Mesh
	//
	else if( type == "tetmesh" ){

		std::shared_ptr<TetMesh> mesh( new TetMesh() );
		std::string filename = "";
		for( int i=0; i<params.size(); ++i ){
			if( parse::to_lower(params[i].tag)=="file" ){ filename=params[i].as_string(); }
		}

		if( filename.size() ){
			if( !mesh->load( filename ) ){ printf("\n**TetMesh Error: failed to load file %s\n", filename.c_str()); }
		}

		std::shared_ptr<BaseObject> new_obj( mesh );
		new_obj->apply_xform( x_form );
		if( material >= 0 ){ new_obj->set_material( material ); }
		return new_obj;

	} // end build tet mesh


	//
	//	Point Cloud
	//
	else if( type == "pointcloud" ){

		std::shared_ptr<PointCloud> cloud( new PointCloud() );
		std::string filename = "";
		bool fill = false;
		for( int i=0; i<params.size(); ++i ){
			if( parse::to_lower(params[i].tag)=="file" ){ filename=params[i].as_string(); }
			if( parse::to_lower(params[i].tag)=="fill" ){ fill=params[i].as_bool(); }
		}

		if( filename.size() ){
			if( !cloud->load( filename, fill ) ){ printf("\n**PointCloud Error: failed to load file %s\n", filename.c_str()); }
		}

		std::shared_ptr<BaseObject> new_obj( cloud );
		new_obj->apply_xform( x_form );
		if( material >= 0 ){ new_obj->set_material( material ); }
		return new_obj;

	} // end build particle cloud


	//
	//	Unknown
	//
	else{
		std::cerr << "**Error: I don't know how to create an object of type " << type << std::endl;
	}


	//
	//	Unknown
	//
	return NULL;

} // end object builder


//
//	Default Material Builder
//
static std::shared_ptr<BaseMaterial> default_build_material( std::string type, std::vector<Param> &params ){

	type = parse::to_lower(type);

	if( type == "blinnphong" ){

		std::shared_ptr<BlinnPhong> mat( new BlinnPhong() );

		// Loop again for a change in params
		for( int i=0; i<params.size(); ++i ){

			std::string tag = parse::to_lower(params[i].tag);

			if( tag=="ambient" ){
				params[i].fix_color();
				mat->ambient=params[i].as_vec3();
			}
			else if( tag=="diffuse" || tag=="color" ){
				params[i].fix_color();
				mat->diffuse=params[i].as_vec3();
			}
			else if( tag=="specular" ){
				params[i].fix_color();
				mat->specular=params[i].as_vec3();
			}
			else if( tag=="texture" ){
				mat->texture_file = params[i].as_string();
			}
			else if( tag=="shininess" || tag=="exponent" ){ mat->shininess=params[i].as_int(); }

		}
		std::shared_ptr<BaseMaterial> new_mat( mat );
		return new_mat;
	}
	else{
		std::cerr << "**Error: I don't know how to create a material of type " << type << std::endl;
	}

	//
	//	Unknown
	//
	return NULL;

} // end build material


//
//	Default Light Builder
//
static std::shared_ptr<BaseLight> default_build_light( std::string type, std::vector<Param> &params ){

	type = parse::to_lower(type);

	//
	//	OpenGL Light
	//
	if( type == "point" ){

		std::shared_ptr<PointLight> light( new PointLight() );
		for( int i=0; i<params.size(); ++i ){
			std::string tag = parse::to_lower(params[i].tag);
			if( tag=="intensity" || tag=="color" ){
				params[i].fix_color();
				light->intensity=params[i].as_vec3();
			}
			else if( tag=="position" ){
				light->position=params[i].as_vec3();
			}
			else if( tag=="falloff" ){
				light->falloff=params[i].as_vec3();
			}
		}
		std::shared_ptr<BaseLight> new_light( light );
		return new_light;
	}
	else{
		std::cerr << "**Error: I don't know how to create a light of type " << type << std::endl;
	}

	//
	//	Unknown
	//
	return NULL;

} // end build light


//
//	Default Camera
//
static std::shared_ptr<BaseCamera> default_build_camera( std::string type, std::vector<Param> &params ){

	type = parse::to_lower(type);

	std::cerr << "**Error: I don't know how to create a camera of type " << type << std::endl;

	//
	//	Unknown
	//
	return NULL;

} // end build camera


} // end namespace mcl

#endif
