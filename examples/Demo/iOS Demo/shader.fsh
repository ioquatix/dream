//
//  Shader.fsh
//  foobar
//
//  Created by Samuel Williams on 20/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

varying lowp vec4 vertex_color;
varying lowp vec4 whacky;

void main()
{
    gl_FragColor = vertex_color * vec4(whacky.x, 1.0, 1.0, 1.0);
}
