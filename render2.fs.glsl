
layout (location = 0) out vec4 FragColor;

subroutine vec4 RenderTypeRoutine( vec2 coord );

subroutine uniform RenderTypeRoutine RenderType;

uniform sampler2D VelocityTexture;
uniform sampler2D TemperatureTexture;

uniform int GridWidth;
uniform int GridHeight;

subroutine( RenderTypeRoutine )
vec4 RenderTemp( vec2 coord )
{
	float t = texture2D(TemperatureTexture, coord).x;

	vec4 color;
	if(t > 0.0)
	{
		color = vec4(t, 0.0, 0.0, 1.0);
	}
	else
	{
		color = vec4(0.0, 0.0, - t, 1.0);
	}
	return color;
}

subroutine( RenderTypeRoutine )
vec4 RenderVel( vec2 coord )
{
	vec2 localVel = texture2D(VelocityTexture, coord).xy;

	vec2 normalVel = normalize(localVel);

	vec4 color;
	if(normalVel.x > 0.0)
	{
		if(normalVel.y > 0.0)
		{
			color = vec4(normalVel.x, normalVel.y, 0.0, 1.0);
		}
		else
		{
			color = vec4(normalVel.x, 0.0, -normalVel.y, 1.0);
		}
	}
	else
	{
		if(normalVel.y > 0.0)
		{
			color = vec4(0.0, -normalVel.x, normalVel.y, 1.0);
		}
		else
		{
			color = vec4(0.0, -normalVel.x, -normalVel.y, 1.0);
		}
	}
	return color;
}

void main(void)
{
	float dx = 1.0 / float(GridWidth);
	float dy = 1.0 / float(GridHeight);
	vec2 uv = gl_FragCoord.xy * vec2(dx, dy);

	FragColor = RenderType( uv );	
}