#version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;



uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;


//imgui
uniform vec4 imguiColor;
uniform vec4 imguiLightColor;
uniform float lightIntensity;
uniform int lightType;
uniform float lightHeight;
uniform float lightXPos;
uniform float lightZPos;
uniform float lightPower;


vec4 pointLight()
{	
	vec3 lightVec = lightPos - crntPos + vec3(lightXPos, lightHeight, lightZPos);

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = (1.0f / (a * dist * dist + b * dist + 1.0f)) * (lightIntensity * lightPower);

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * imguiLightColor;
}

vec4 direcLight()
{
	// ambient lighting
	float ambient = 1.0f * lightIntensity;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * imguiLightColor;
}

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 2.90f;
	float innerCone = 0.90f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos + vec3(lightXPos, lightHeight, lightZPos));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = (clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f)) * (lightIntensity * lightPower);

	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * imguiLightColor;
}


void main()
{
	if (lightType == 0)
	{
		FragColor = direcLight() * imguiColor;
	}
	if (lightType == 1)
	{
		FragColor = pointLight() * imguiColor;
	}
	if (lightType == 2)
	{
		FragColor = spotLight() * imguiColor;
	}
}