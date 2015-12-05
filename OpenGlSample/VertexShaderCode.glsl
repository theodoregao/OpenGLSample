#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

// ex3
//uniform vec3 dominatingColor;
//uniform float yFlip;

// ex4
//uniform mat4 transformMatrix;
//uniform mat4 rotationMatrix;
//uniform mat4 projectionMatrix;
uniform mat4 matrix;

out vec3 theColor;

void main()
{
	// ex3
    //gl_Position = vec4(position, 1.0);
	//gl_Position.y = gl_Position.y * yFlip;
    //theColor = dominatingColor;		//vertexColor;

	// ex4
	vec4 v = vec4(position, 1.0);
	//gl_Position = projectionMatrix * transformMatrix * rotationMatrix * v;
	gl_Position = matrix * v;
    theColor = vertexColor;
}