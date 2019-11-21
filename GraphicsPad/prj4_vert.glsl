#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normal;

uniform mat4 objectToWorldMatrix;
uniform mat4 worldToClampMatrix;
uniform vec3 cameraPos;
uniform vec3 pointLightPos;
uniform vec3 directionalLightDir;

out vec3 theColor;
out vec4 theNormal;
out vec4 viewReflectDirWorld;
out vec4 pointLightDirWorld;
//out vec4 theDirectional;

void main()
{
	theNormal = objectToWorldMatrix * vec4(normal, 0.0);
	
	vec4 vertex = vec4(position, 1.0);
	vertex = objectToWorldMatrix * vertex;

	viewReflectDirWorld = reflect(
		vec4(cameraPos, 2.0) - vertex, 
		theNormal);

	pointLightDirWorld = vec4(pointLightPos, 2.0) - vertex;

	vertex = worldToClampMatrix * vertex;
	gl_Position = vertex;

//	float dirBrightNess = dot(-1 * directionalLightDir, normal);
//	dirBrightNess = clamp(dirBrightNess, 0, 1);
//	theColor = dirBrightNess * vertexColor;
	theColor = vertexColor;

}