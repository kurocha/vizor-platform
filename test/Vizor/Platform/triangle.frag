#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 albedo;
layout(location = 1) in vec2 mapping;

layout(location = 0) out vec4 color;

vec3 hsv2rgb(vec3 color)
{
	if (color[1] == 0.0) return vec3(0, 0, 0);

	int i = int(floor(color[0] * 6.0));

	float f = (color[0] * 6.0) - i;
	float p = color[2] * (1.0 - color[1]);
	float q = color[2] * (1.0 - color[1] * f);
	float t = color[2] * (1.0 - color[1] * (1.0 - f));
	float v = color[2];

	switch (i % 6) {
		case 0: return vec3(v, t, p);
		case 1: return vec3(q, v, p);
		case 2: return vec3(p, v, t);
		case 3: return vec3(p, q, v);
		case 4: return vec3(t, p, v);
		case 5: return vec3(v, p, q);
	}

	return vec3(0, 0, 0);
}

float powi(float z, int i)
{
	float r = 1;
	
	while(i > 0) {
		r *= z;
		i--;
	}
	
	return r;
}

vec2 f(vec2 z)
{
	return vec2(z.x * z.x - z.y * z.y, 2.0 * (z.x * z.y));
}

float magnitude(vec2 z)
{
	return sqrt(z.x * z.x + z.y * z.y);
}

const int max_iterations = 1000;
const int max_magnitude = 100;

int iterate(vec2 c)
{
	int n = 0;
	vec2 z = vec2(0,0);
	
	while (magnitude(z) < max_magnitude && n < max_iterations)
	{
		z = f(z) + c;
		n++;
	}
	
	return n;
}

void main()
{
	int iterations = iterate(mapping);
	
	if (iterations == max_iterations) {
		color = vec4(0.0);
	} else {
		float s = float(iterations) / float(max_iterations);
		color = vec4(hsv2rgb(vec3(s/4.0, 0.9, s)), 1.0);
	}
}
