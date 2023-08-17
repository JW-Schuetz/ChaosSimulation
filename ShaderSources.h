#pragma once


string axesVertexBody =
"layout (location = 0) in vec3 aPos;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4( aPos, 1.0 );\n"
"}\n";


string axesFragmentBody =
"out vec4 color;\n"
"uniform vec3 axesColor;\n"
"\n"
"void main()\n"
"{\n"
"	color = vec4( axesColor, 1.0 );\n"
"}\n";


string pointVertexBody =
"layout (location = 0) in vec3 aPos;\n"
"\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4( aPos, 1.0 );\n"
"}\n";


string pointFragmentBody =
"out vec4 color;\n"
"uniform vec3 pointColor;\n"
"\n"
"void main()\n"
"{\n"
"	color = vec4( pointColor, 1.0 );\n"
"}\n";


string pointTraceVertexBody =
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aCol;\n"
"out vec3 pointColor;\n"		// specify a color output to the fragment shader
"\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4( aPos, 1.0 );\n"
"	pointColor  = aCol;\n"
"}\n";


string pointTraceFragmentBody =
"layout (location = 0) in vec3 aPos;\n"
"in vec3 pointColor;\n"	// input variable from vertexShader (same name and type)
"out vec4 color;\n"
"\n"
"void main()\n"
"{\n"
"	color = vec4( pointColor, 1.0 );\n"
"}\n";


string glyphsVertexBody =
"layout (location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * vec4( vertex.xy, 0.0, 1.0 );\n"
"	TexCoords   = vertex.zw;\n"
"}\n";


string glyphsFragmentBody =
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D text;\n"
"uniform vec3 glyphsColor;\n"
"\n"
"void main()\n"
"{\n"
"	vec4 sampled = vec4( 1.0, 1.0, 1.0, texture( text, TexCoords ).r );\n"
"	color        = vec4( glyphsColor, 1.0 ) * sampled;\n"
"}\n";


string cubeVertexBody =
"layout (location = 0) in vec3 aPos;\n"
"\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4( aPos, 1.0 );\n"
"}\n";


string cubeFragmentBody =
"out vec4 color;\n"
"uniform vec3 cubeColor;\n"
"\n"
"void main()\n"
"{\n"
"	color = vec4( cubeColor, 1.0 );\n"
"}\n";