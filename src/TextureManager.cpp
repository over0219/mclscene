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

#include "MCL/TextureManager.hpp"

using namespace mcl;


bool TextureManager::load( std::string name, std::string file ){

	if( bindActive ){
		std::cerr << "**TextureManager Warning: Bind already active!" << std::endl;
	}

	sf::Texture img;
	if( !img.loadFromFile( file ) ){
		std::cerr << "**TextureManager Error: Could not load texture [" << file << "]" << std::endl;
		return false;
	}

	textures.insert( std::pair< std::string, sf::Texture >( name, img ) );

	return true;
}


void TextureManager::bind( std::string name ){
//	if( !textures.count(name) ){
//		std::unordered_map< std::string, sf::Texture >::iterator it = textures.begin();
//		for( it; it != textures.end(); ++it ){
//			std::cout << it->first << std::endl;
//		}
//	}
	sf::Texture::bind( &textures[ name ] );
	bindActive = true;
}


void TextureManager::unbind(){
	sf::Texture::bind( NULL );
	bindActive = false;
}