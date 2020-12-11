
layout (location = 0) out float NewPressure;

uniform sampler2DRect PressureTexture;
uniform sampler2DRect DivergenceTexture;

uniform int GridWidth;
uniform int GridHeight;

void main(void)
{
	ivec2 T = ivec2(gl_FragCoord.xy);

	float dx = 1.0 / float(GridWidth);
	float alpha = - 1.0 * (dx * dx);
	float beta = 1.0 / 4.0;

	float div = texelFetch(DivergenceTexture, T).x;

	float leftPressure = texelFetchOffset(PressureTexture, T, ivec2(-1, 0)).x;
	float rightPressure = texelFetchOffset(PressureTexture, T, ivec2(1, 0)).x;
	float upPressure = texelFetchOffset(PressureTexture, T, ivec2(0, 1)).x;
	float downPressure = texelFetchOffset(PressureTexture, T, ivec2(0, -1)).x;

	NewPressure = (leftPressure + rightPressure + upPressure + downPressure + alpha * div) * beta;
}