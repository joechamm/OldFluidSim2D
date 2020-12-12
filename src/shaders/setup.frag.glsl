
layout (location = 0) out vec2 NewVelocity;
layout (location = 1) out float NewTemperature;
layout (location = 2) out float NewDensity;
layout (location = 3) out float NewPressure;

uniform samplerBuffer VelocityBuffer;
uniform samplerBuffer TemperatureBuffer;

uniform int GridWidth;
uniform int GridHeight;

void main(void)
{
	ivec2 T = ivec2(gl_FragCoord.xy);

	int idx = (T.y * GridWidth) + T.x;

	NewVelocity = texelFetch(VelocityBuffer, idx).xy;
	NewTemperature = texelFetch(TemperatureBuffer, idx).x;

	float tempMag = NewTemperature * NewTemperature;
	NewPressure = 0.0;
	NewDensity = tempMag;
}