
layout (location = 0) out vec2 NewVelocity;
layout (location = 1) out float NewTemperature;

uniform sampler2D VelocityTexture;
uniform sampler2D TemperatureTexture;

uniform int GridWidth;
uniform int GridHeight;

void main(void)
{
	float dx = 1.0 / float(GridWidth);
	float dy = 1.0 / float(GridHeight);
	vec2 InverseSize = vec2(dx, dy);
	vec2 uv = gl_FragCoord.xy * InverseSize;

	NewVelocity = texture2D(VelocityTexture, uv).xy;
	NewTemperature = texture2D(TemperatureTexture, uv).x;
}
