#version 430

in layout(location = 0) vec2 position;
in layout(location = 1) vec3 vertexColor;

uniform vec3 dominatingColor;
uniform float yFlip;
uniform vec2 positionOffset;

out vec3 theColor;

void main()
{
	gl_Position = vec4(position, 0.0, 1.0);
	gl_Position.y = gl_Position.y * yFlip;
	gl_Position.x = gl_Position.x + positionOffset.x;
	gl_Position.y = gl_Position.y + positionOffset.y;
	theColor = dominatingColor;
}
