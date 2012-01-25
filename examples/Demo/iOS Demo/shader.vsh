//
//  Shader.vsh
//  foobar
//
//  Created by Samuel Williams on 20/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

uniform vec2 p;

attribute vec4 position;
attribute vec4 colour;

varying vec4 vertex_color;
varying vec4 whacky;

void main()
{
    vertex_color = colour;
	
	whacky = vec4(sin(p.x * 2.0), sin(p.y * 2.0), 0.0, 0.0) + position;
    gl_Position = vec4(p, 0.0, 0.0) + position;
}
