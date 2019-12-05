#version 430

out vec4 daColor;
in vec3 theColor;
in vec4 theNormal;
in vec4 viewReflectDirWorld;
in vec4 pointLightDirWorld;
in vec3 objectPos;

uniform vec3 cameraPos;

uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float glossiness;

uniform vec3 directionalLightDir;
uniform vec3 ambientLightColor;

uniform sampler2D Tex1;
uniform sampler2D Tex2;

// x limits
// 3.525 -3

void main()
{
	vec4 normal = normalize(theNormal);
	vec4 viewRefl = normalize(viewReflectDirWorld);
	vec4 dirLightDir = -1 * vec4(directionalLightDir, 0.0);
	dirLightDir = normalize(dirLightDir);
	vec4 pointLightDir = normalize(pointLightDirWorld);

	// directional light values
	float dirDiffBrightness = dot(dirLightDir, normal);
	dirDiffBrightness = clamp(dirDiffBrightness, 0, 1);
	
	float dirSpecBrightness = dot(viewRefl, -1 * dirLightDir);
	dirSpecBrightness = clamp(dirSpecBrightness, 0, 1);
	dirSpecBrightness = pow(dirSpecBrightness, glossiness);

	// point light vallues
	float pointDiffBrightness = dot(pointLightDir, normal);
	pointDiffBrightness = clamp(pointDiffBrightness, 0, 1);

	float pointSpecBrightness = dot(viewRefl, -1 * pointLightDir);
	pointSpecBrightness = clamp(pointSpecBrightness, 0, 1);
	pointSpecBrightness = pow(pointSpecBrightness, glossiness);

//	vec3 color = theColor;
	vec3 color = vec3(1, 1, 1);
	
	float edgeBias = 0.001f;
	vec3 editedPos = objectPos / 2 + vec3(0.5f);

	float xValue = cos(objectPos.x * 3.14f / 2);
	xValue = 1-xValue;
	xValue = floor(xValue + edgeBias);

	float yValue = cos(objectPos.y * 3.14f / 2);
	yValue = 1 - yValue;
	yValue = floor(yValue + edgeBias);

	float zValue = cos(objectPos.z * 3.14f / 2);
	zValue = 1-zValue;
	zValue = floor(zValue + edgeBias);
	
	vec2 TexCoordXZ = vec2(editedPos.x, editedPos.z);
	vec4 texColorXZ = texture(Tex1, TexCoordXZ);

	vec2 TexCoordXY = vec2(editedPos.x, editedPos.y);
	vec4 texColorXY = texture(Tex1, TexCoordXY);

	vec2 TexCoordYZ = vec2(editedPos.y, editedPos.z);
	vec4 texColorYZ = texture(Tex1, TexCoordYZ);

//	vec4 texColor = texColorXZ * yValue + 
//	texColorXY * zValue +
//	texColorYZ * xValue;

	vec4 texColor = texColorXZ * yValue +
	texColorXY * zValue +
	texColorYZ * xValue;

	color = color * (
		(pointDiffBrightness + ambientLightColor) * vec3(texColor) + 
		pointSpecBrightness * specularColor
	);

	// gamma correction
	daColor = vec4(
		pow(color.r,0.454545),
		pow(color.g,0.454545),
		pow(color.b,0.454545),
	1.0);
//	daColor = vec4(color,1.0);
//	daColor = texColor;
}