
layout (location = 0) vec2 NewVelocity;

uniform sampler2DRect VelocityTexture;
uniform sampler2DRect PressureTexture;

uniform int GridWidth;
uniform int GridHeight;

void main(void)
{
	ivec2 T = ivec2(gl_FragCoord.xy);

	float xScale = float(GridWidth);
	float yScale = float(GridHeight);

	float leftP = texelFetchOffset(PressureTexture, T, ivec2(-1, 0)).x;
	float rightP = texelFetchOffset(PressureTexture, T, ivec2(1, 0)).x;
	float upP = texelFetchOffset(PressureTexture, T, ivec2(0, 1)).x;
	float downP = texelFetchOffset(PressureTexture, T, ivec2(0, -1)).x;

	vec2 currVel = texelFetch(VelocityTexture, T).xy;

	float dPX = (rightP - leftP) * xScale * 0.5;
	float dPY = (upP - downP) * yScale * 0.5;

	NewVelocity = currVel - vec2(dPX, dPY);
}