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

#ifndef MCLSCENE_RENDERGL_H
#define MCLSCENE_RENDERGL_H 1

#include <GL/glew.h>
//#include <GL/gl3w.h>
#include "MCL/Shader.hpp"
#include "MCL/RenderUtils.hpp"
#include "MCL/SceneManager.hpp"

namespace mcl {

class RenderGL  {
public:
	struct AppCamera {
		trimesh::XForm<float> model, view, projection;
	};

	// Initialize shaders. Must be called after
	// OpenGL context has been created.
	bool init( mcl::SceneManager *scene_, AppCamera *cam_ );

	// Draws a specific object with aspecificgiven material. If material is null,
	// a default one is used (lambertian red). The object must have get_TriMesh()
	// function implemented, otherwise nothing is drawn.
	void draw( std::shared_ptr<BaseObject> obj, std::shared_ptr<BaseMaterial> mat=NULL );

	// Draws all objects in the SceneManager
	void draw_objects();

	// Draws all lights in the SceneManager that have a shape
	// (I.e., point lights as a sphere, spot lights as a cone).
	void draw_lights();

private:
	std::unique_ptr<Shader> blinnphong;
	mcl::SceneManager *scene;
	AppCamera *camera;

	std::vector< std::shared_ptr<OGLLight> > lights; // resized at init

}; // end class RenderGL


} // end namespace mcl

#endif