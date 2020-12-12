
layout (location = 0) vec2 NewVelocity;

uniform sampler2DRect VelocityTexture;
uniform sampler2DRect TemperatureTexture;
uniform sampler2DRect DensityTexture;

uniform int GridWidth;
uniform int GridHeight;

uniform float AmbientTemperature;
uniform float Kappa;
uniform float Sigma;
uniform float dt;

void main(void)
{
	ivec2 T = ivec2(gl_FragCoord.xy);

	float t = texelFetch(TemperatureTexture, T).x;
	float buoyMag = (t - AmbientTemperature) * Sigma;
	
	float dens = texelFetch(DensityTexture, T).x;
	buoyMag -= (Kappa * dens);

	vec2 currVel = texelFetch(VelocityTexture, T).xy;

	vec2 forceBuoy = vec2(0.0, buoyMag);

	NewVelocity = currVel + forceBuoy * dt;
}