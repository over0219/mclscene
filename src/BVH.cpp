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

#include "MCL/BVH.hpp"

using namespace mcl;


void BVHNode::get_edges( std::vector<trimesh::vec> &edges ){
	aabb->get_edges( edges );
	if( left_child != NULL ){ left_child->get_edges( edges ); }
	if( right_child != NULL ){ right_child->get_edges( edges ); }

	for( int i=0; i<m_objects.size(); ++i ){
		m_objects[i]->get_edges( edges );
	}
}


void BVHNode::make_tree( const std::vector< std::shared_ptr<BaseObject> > objects, int split_axis, int max_depth ) {

	using namespace trimesh;	

	split_axis = (split_axis+1)%3;
	m_split = split_axis;
	max_depth--;

	// Create the aabb
	std::vector< point > obj_centers; // store the centers for later lookup
	for( int i=0; i<objects.size(); ++i ){
		vec bmin, bmax; objects[i]->get_aabb( bmin, bmax );
		*aabb += bmin;
		*aabb += bmax;
		obj_centers.push_back( (bmin+bmax)*0.5f );
	}
	point center = aabb->center();

	// If num faces == 1, we're done
	if( objects.size()==0 ){ return; }
	else if( objects.size()==1 || max_depth <= 0 ){
		m_objects = objects;
/*
		for( int o=0; o<objects.size(); ++o ){
			if( objects[o]->get_type() == "trimesh" ){
				std::shared_ptr<TriangleMesh> mesh = std::static_pointer_cast<TriangleMesh>(objects[o]);
				left_child = std::shared_ptr<BVHNode>( new BVHNode() );
				left_child->make_tree( mesh->faces, mesh->vertices, mesh->normals, split_axis, max_depth );
			}
			else if( objects[o]->get_type() == "tetmesh" ){
				std::shared_ptr<TetMesh> mesh = std::static_pointer_cast<TetMesh>(objects[o]);
				left_child = std::shared_ptr<BVHNode>( new BVHNode() );
				left_child->make_tree( mesh->faces, mesh->vertices, mesh->normals, split_axis, max_depth );
			}
			else{ m_objects.push_back( objects[o] ); }
		}
*/
		return;
	}

	// Split faces
	std::vector< std::shared_ptr<BaseObject> > left_objs, right_objs;
	for( int i=0; i<objects.size(); ++i ){
		double oc = obj_centers[i][split_axis];
		if( oc <= center[ split_axis ] ){ left_objs.push_back( objects[i] ); }
		else if( oc > center[ split_axis ] ){ right_objs.push_back( objects[i] ); }
	}

	// Check to make sure things got sorted. Sometimes small meshes fail.
	if( left_objs.size()==0 ){ left_objs.push_back( right_objs.back() ); right_objs.pop_back(); }
	if( right_objs.size()==0 ){ right_objs.push_back( left_objs.back() ); left_objs.pop_back(); }

	// Create the children
	left_child = std::shared_ptr<BVHNode>( new BVHNode() );
	right_child = std::shared_ptr<BVHNode>( new BVHNode() );
	left_child->make_tree( left_objs, split_axis, max_depth );
	right_child->make_tree( right_objs, split_axis, max_depth );
}


// Make a BVH tree from a triangle mesh
void BVHNode::make_tree( const std::vector<trimesh::TriMesh::Face> &faces,
	const std::vector<trimesh::point> &vertices, const std::vector<trimesh::vec> &normals,
	int split_axis, int max_depth ) {
/*
	using namespace trimesh;

	split_axis = (split_axis+1)%3;
	m_split = split_axis;
	max_depth--;

	// Create the aabb
	for( int i=0; i<faces.size(); ++i ){
		*aabb += vertices[ faces[i][0] ];
		*aabb += vertices[ faces[i][1] ];
		*aabb += vertices[ faces[i][2] ];
	}
	point center = aabb->center();

	// If num faces == 1, we're done
	if( faces.size()==0 ){ return; }
	else if( faces.size()==1 || max_depth <= 0 ){
		std::shared_ptr<BaseObject> face_list( new FaceList( faces, vertices, normals) );
		m_objects.push_back( face_list );
		return;
	}

	// Split faces
	std::vector<TriMesh::Face> left_faces, right_faces;
	for( int i=0; i<faces.size(); ++i ){
		double fc = helper::face_center( faces[i], vertices )[ split_axis ];
		if( fc <= center[ split_axis ] ){ left_faces.push_back( faces[i] ); }
		else if( fc > center[ split_axis ] ){ right_faces.push_back( faces[i] ); }
	}

	// Check to make sure things got sorted. Sometimes small meshes fail.
	if( left_faces.size()==0 ){ left_faces.push_back( right_faces.back() ); right_faces.pop_back(); }
	if( right_faces.size()==0 ){ right_faces.push_back( left_faces.back() ); left_faces.pop_back(); }

	// Create the children
	left_child = std::shared_ptr<BVHNode>( new BVHNode() );
	right_child = std::shared_ptr<BVHNode>( new BVHNode() );
	left_child->make_tree( left_faces, vertices, normals, split_axis, max_depth );
	right_child->make_tree( right_faces, vertices, normals, split_axis, max_depth );
*/
}


/*
BVHNode::BVHNode( std::vector< std::shared_ptr<BVHNode> > bvhnodes, int split_axis ){
	using namespace trimesh;

	left_child = NULL;
	right_child = NULL;
	split_axis = (split_axis+1)%3;
	m_split = split_axis;


	// Create the aabb
	aabb = std::shared_ptr<AABB>( new AABB );
	for( int i=0; i<bvhnodes.size(); ++i ){
		*aabb += *bvhnodes[i]->bounds();
	}
	point center = aabb->center();

	// Special cases for numbers
	if( bvhnodes.size()==0 ){ return; }
	else if( bvhnodes.size()==1 ){
		left_child = bvhnodes[0];
		return;
	}
	else if( bvhnodes.size()==2 ){
		left_child = bvhnodes[0];
		right_child = bvhnodes[1];
		return;
	}

	// Otherwise, sort and store
	std::vector< std::shared_ptr<BVHNode> > left_nodes, right_nodes;
	for( int i=0; i<bvhnodes.size(); ++i ){
		double fc = bvhnodes[i]->bounds()->center()[ split_axis ];
		if( fc <= center[ split_axis ] ){ left_nodes.push_back( bvhnodes[i] ); }
		else if( fc > center[ split_axis ] ){ right_nodes.push_back( bvhnodes[i] ); }
	}

	// Check to make sure things got sorted. Sometimes small meshes fail.
	if( left_nodes.size()==0 ){ left_nodes.push_back( right_nodes.back() ); right_nodes.pop_back(); }
	if( right_nodes.size()==0 ){ right_nodes.push_back( left_nodes.back() ); left_nodes.pop_back(); }

	// Create the children
	left_child = std::shared_ptr<BVHNode>( new BVHNode( left_nodes, split_axis ) );
	right_child = std::shared_ptr<BVHNode>( new BVHNode( right_nodes, split_axis ) );
}

*/
/*
bool BVHTraversal::ray_intersect( std::shared_ptr<BVHNode> node, const trimesh::vec &origin, const trimesh::vec &dir,
		double &t_min, double &t_max, ray_payload &payload ) {

	payload.curr_depth++;

	// See if we even hit the box
	if( !node->aabb->ray_intersect( origin, dir, t_min, t_max ) ){ return false; }

	// If we have children, progress down the tree
	if( node->left_child != NULL || node->right_child != NULL ){

		ray_payload payload_l=payload; ray_payload payload_r=payload;
		double t_min_l = t_min; double t_min_r = t_min;
		double t_max_l = t_max; double t_max_r = t_max;
		bool left_hit=false, right_hit=false;
		if( node->left_child != NULL ){ left_hit = ray_intersect( node->left_child, origin, dir, t_min_l, t_max_l, payload_l ); }
		if( node->right_child != NULL ){ right_hit = ray_intersect( node->right_child, origin, dir, t_min_r, t_max_r, payload_r ); }

		// See which child is closer
		if( left_hit && right_hit ){
			if( t_max_r < t_max_l ){
				t_max = t_max_r;
				payload = payload_r;
				return true;
			}
			t_max = t_max_l;
			payload = payload_l;
			return true;
		}
		else if( right_hit ){
			t_max = t_max_r;
			payload = payload_r;
			return true;
		}
		else if( left_hit ){
			t_max = t_max_l;
			payload = payload_l;
			return true;
		}

	} // end ray_intersect children

	// Otherwise it's a leaf node, check objects
	else{
		bool obj_hit = false;
		for( int i=0; i<node->m_objects.size(); ++i ){
			if( node->m_objects[i]->ray_intersect( origin, dir, t_min, t_max, payload ) ){ obj_hit=true; }
		}
		return obj_hit;
	} // end ray_intersect objects

	return false;

} // end ray intersect

*/


