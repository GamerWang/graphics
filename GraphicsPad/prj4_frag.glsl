#version 430

out vec4 daColor;
in vec3 theColor;
in vec4 theNormal;
in vec4 viewReflectDirWorld;

uniform vec3 cameraPos;

uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float glossiness;

uniform vec3 directionalLightDir;
uniform vec3 ambientLightColor;

void main()
{
	vec4 dirLightDir = -1 * vec4(directionalLightDir, 0.0);
	dirLightDir = normalize(dirLightDir);
	vec4 normal = normalize(theNormal);

	float dirDiffBrightness = dot(dirLightDir, normal);
	dirDiffBrightness = clamp(dirDiffBrightness, 0, 1);
	
	vec4 viewRefl = normalize(viewReflectDirWorld);
	float dirSpecBrightness = dot(viewRefl, normal);
	dirSpecBrightness = clamp(dirSpecBrightness, 0, 1);
	dirSpecBrightness = pow(dirSpecBrightness, glossiness);

	vec3 color = theColor * (
		dirDiffBrightness * diffuseColor +
		dirSpecBrightness * specularColor);
	color += ambientLightColor;

	// gamma correction
	daColor = vec4(
		pow(color.r,0.454545),
		pow(color.g,0.454545),
		pow(color.b,0.454545),
	1.0);
//	daColor = vec4(color,1.0);
}