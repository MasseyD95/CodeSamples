#version 330
struct Light
{
	vec3 position;
	float range;
	vec3 colour;
	float intensity;
};

struct Material
{
	vec3 ambient_diffuse_colour;
	float shininess;
	vec3 specular_colour;
	int use_texture;
	int mat_id;
	int is_shiny;
};

uniform Light lights[22];
uniform int num_of_lights;
uniform Material materials;
uniform int num_of_materials;
uniform sampler2D texture_data;
uniform vec3 ambient_light;

in vec2 varying_texcoord;
in vec3 world_Pos;
in vec3 world_Norm;

out vec4 fragment_colour;

//Function to deal with the calcalation of lights intensity within the equation
vec3 point_light(Light light, vec3 diffuse_col, vec3 tex_colour)
{
	/////////Help with equations at bottom of code//////////
	//WorldPos
	vec3 P = vec3(0, 0, 0);
	//WorldNorm
	vec3 N = vec3(0, 0, 0);
	//Light position
	vec3 L = vec3(0, 0, 0);
	//Reflected light
	vec3 RL = vec3(0, 0, 0);
	//Diffuse colour of material
	vec3 Kd = diffuse_col;
	//Intensity of light and colour
	vec3 IL = vec3(0,0,0);
	//Final output value of equation
	vec3 sumofloop = vec3(0, 0, 0);
	//Distance
	float D = 0;
	//Dot product of LightPos.ReflectedLight
	float LR = 0;
	//Dot product of LightPos.WorldNorm
	float LN = 0;
	//Dot product of LightPos.WorldNorm times by the FD value
	float DP = 0;
	//Final value of distance attenuation smoothstepped
	float FD = 0;
	

	//Lambert Reflection
	P = world_Pos;
	N = normalize(world_Norm);
	L = normalize(light.position - P);
	D = distance(light.position, P);
	//Drops off the itensity after 75% range.
	FD = 1.0 - smoothstep(light.range * 0.75, light.range, D);
	LN = max(0, dot(L, N));
	DP = FD * LN;
	IL = FD * (light.colour * light.intensity);
	//Phone Reflection
	RL = reflect(-L, N);
	LR = max(0, dot(L, RL));

	//Choosing between Lambert or Phong equation
	if (materials.is_shiny == 1 && materials.use_texture == 1)
	{
		sumofloop = IL * ((Kd * tex_colour) * LN + materials.specular_colour * pow(LR, materials.shininess));
	}
	else if(materials.is_shiny == 1 && materials.use_texture == 0)
	{
		sumofloop = IL * (Kd * LN + IL * materials.specular_colour * pow(LR, materials.shininess));
	}
	//Don't need texture information since the changes are done outside the function
	else if(materials.is_shiny == 0)
	{
		sumofloop += DP * (light.colour * light.intensity);
	}
	
	return sumofloop;
}

void main(void)
{
	//Total Diffuse intensity
	vec3 totalDI = vec3(0, 0, 0);
	//Final light intensity
	vec3 light_intensity = vec3(0, 0, 0);
	vec3 tex_colour = vec3(0, 0, 0);
	//Ambient and diffuse colour of material
	vec3 Ka = materials.ambient_diffuse_colour;
	vec3 Kd = materials.ambient_diffuse_colour;
	tex_colour = texture(texture_data, varying_texcoord).rgb;

	//Loop through amount of lights and do the loop of the phong or lambert reflection equation
	for(int i = 0; i < num_of_lights; ++i)
	{
		totalDI += point_light(lights[i], Kd, tex_colour);
	}

	//First set of if's check to see if to include the tex colour in the equation
	//Second set of if's check to see if to do phong or lambert reflection equation
	if (materials.use_texture == 1)
	{
		if (materials.is_shiny == 1)
		{
			light_intensity = ambient_light * (Ka * tex_colour) + totalDI;
		}
		else if(materials.is_shiny == 0)
		{
			light_intensity = ambient_light * (Ka * tex_colour) + (Kd * tex_colour) * totalDI;
		}
		//Final output
		fragment_colour = vec4(tex_colour, 1.0) * vec4(light_intensity, 1.0);
	}
	else if (materials.use_texture == 0)
	{
		if (materials.is_shiny == 1)
		{
			light_intensity = ambient_light * Ka + totalDI;
		}
		else if(materials.is_shiny == 0)
		{
			light_intensity = ambient_light * Ka + Kd * totalDI;
		}	
		//Final output
		fragment_colour = vec4(light_intensity, 1.0);
	}
}

/*	
/////////////////Equation help/////////////////////
			Lambert Reflection
	////// I = I2 * Ka * Kd * Σi,m(L.N * IL) //////////
	I = intensity and colour of light visable to the eye
	I2 = intensity and colour of ambient light
	Ka = ambient colour of mat
	Kd = diffuse colour of mat
	m = number of light sources
	i = light source index
	L = direction to icoming light
	N = normal vector surface
	IL = intensity and colour of incoming light

				Phong Reflection
	////// I = I2 * Ka + Σi,m(IL(Kd * L.N + Ks pow(L.R, n)) //////////
	I = intensity and colour of light visable to the eye
	I2 = intensity and colour of ambient light
	Ka = ambient colour of mat
	m = number of light sources
	i = light source index
	IL = intensity and colour of incoming light
	Kd = diffuse colour of mat
	L = direction to icoming light
	N = normal vector surface
	R = reflection of view vector
	n = shininess for material
	*/