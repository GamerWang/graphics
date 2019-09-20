#version 430

in layout(location = 0) vec2 position;
in layout(location = 1) vec3 vertexColor;

uniform vec3 dominatingColor;
uniform float yFlip;
uniform vec2 positionOffset;

out vec3 theColor;

void main()
{
	gl_Position = vec4(position + positionOffset, 0.0, 1.0);
	theColor = dominatingColor;
}
