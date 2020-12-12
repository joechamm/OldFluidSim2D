
layout (location = 0) out float NewDivergence;

uniform sampler2DRect VelocityTexture;

uniform int GridWidth;
uniform int GridHeight;

void main(void)
{
	ivec2 T = ivec2(gl_FragCoord.xy);

	float xScale = float(GridWidth);
	float yScale = float(GridHeight);

	vec2 leftVel = texelFetchOffset(VelocityTexture, T, ivec2(-1, 0)).xy;
	vec2 rightVel = texelFetchOffset(VelocityTexture, T, ivec2(1, 0)).xy;
	vec2 upVel = texelFetchOffset(VelocityTexture, T, ivec2(0, 1)).xy;
	vec2 downVel = texelFetchOffset(VelocityTexture, T, ivec2(0, -1)).xy;

	float delVelX = (rightVel - leftVel) * xScale;
	float delVelY = (upVel - downVel) * yScale;

	NewDivergence = (delVelX + delVelY) * 0.5;
}