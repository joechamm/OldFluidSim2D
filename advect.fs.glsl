
layout (location = 0) out vec2 NewVelocity;
layout (location = 1) out float NewTemperature;
layout (location = 2) out float NewDensity;

uniform sampler2DRect VelocityTexture;
uniform sampler2DRect TemperatureTexture;
uniform sampler2DRect DensityTexture;

uniform int GridWidth;
uniform int GridHeight;
uniform float dt;

void main(void)
{
	ivec2 T = ivec2(gl_FragCoord.xy);

	vec2 localVel = texelFetch(VelocityTexture, T).xy;

	float xScale = float(GridWidth);
	float yScale = float(GridHeight);
	localVel.x *= xScale;
	localVel.y *= yScale;
	localVel *= dt;
	
	ivec2 Offset = T - ivec2(localVel);
	if(Offset.x > (GridWidth - 2))
	{
		Offset.x = GridWidth - 2;
	}
	if(Offset.x < 1)
	{
		Offset.x = 1;
	}
	if(Offset.y > (GridHeight - 2))
	{
		Offset.y = GridHeight - 2;
	}
	if(Offset.y < 1)
	{
		Offset.y = 1;
	}

	NewVelocity = texelFetch(VelocityTexture, Offset).xy;
	
	NewTemperature = texelFetch(TemperatureTexture, Offset).x;

	NewDensity = texelFetch(DensityTexture, Offset).x;
}