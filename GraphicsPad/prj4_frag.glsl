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
uniform float time;

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
//	vec3 editedPos = objectPos / 2;
	float mytime = time;
	mytime = mytime/5 - floor(mytime/5);

	float sample1Weight = cos(3.14159f*mytime/2.0f);
//	sample1Weight = sqrt(sample1Weight);
	sample1Weight = pow(sample1Weight, 0.7f);
	float sample2Weight = 1 - sample1Weight;

//	mytime = 0.5f * sin(3.14159f*(mytime-0.5f))+0.5f;

	mytime *= 4;
	mytime = pow(2, mytime);


	vec3 samplePos1 = editedPos / mytime;
	vec3 samplePos2 = editedPos * 16.0f / mytime;

	float xValue = cos(objectPos.x * 3.14f / 2);
	xValue = 1-xValue;
	xValue = floor(xValue + edgeBias);

	float yValue = cos(objectPos.y * 3.14f / 2);
	yValue = 1 - yValue;
	yValue = floor(yValue + edgeBias);

	float zValue = cos(objectPos.z * 3.14f / 2);
	zValue = 1-zValue;
	zValue = floor(zValue + edgeBias);
	
	vec2 TexCoordXZ1 = vec2(samplePos1.x, samplePos1.z);
	vec2 TexCoordXZ2 = vec2(samplePos2.x, samplePos2.z);
	vec4 texColorXZ1 = texture(Tex1, TexCoordXZ1);
	vec4 texColorXZ2 = texture(Tex1, TexCoordXZ2);

	vec2 TexCoordXY1 = vec2(samplePos1.x, samplePos1.y);
	vec2 TexCoordXY2 = vec2(samplePos2.x, samplePos2.y);
	vec4 texColorXY1 = texture(Tex1, TexCoordXY1);
	vec4 texColorXY2 = texture(Tex1, TexCoordXY2);

	vec2 TexCoordYZ1 = vec2(samplePos1.y, samplePos1.z);
	vec2 TexCoordYZ2 = vec2(samplePos2.y, samplePos2.z);
	vec4 texColorYZ1 = texture(Tex1, TexCoordYZ1);
	vec4 texColorYZ2 = texture(Tex1, TexCoordYZ2);

	vec4 texColor = 
		(texColorXZ1*sample1Weight + texColorXZ2*sample2Weight) * yValue +
		(texColorXY1*sample1Weight + texColorXY2*sample2Weight) * zValue +
		(texColorYZ1*sample1Weight + texColorYZ2*sample2Weight) * xValue;

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