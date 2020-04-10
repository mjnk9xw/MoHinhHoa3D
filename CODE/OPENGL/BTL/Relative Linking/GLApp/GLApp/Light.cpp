#include "Light.h"

const float SUNRISE_HOUR = 6.0f;
const float SUNSET_HOUR = 21.0f;
const float DAY_TIME_UPDATE_PER_SECOND = 0.1f;

vec3 Light::lightDirectionFromTime(float lightIntensity,float hours)
{
	vec3 direction(0);
	if (hours < SUNRISE_HOUR || hours > SUNSET_HOUR)
	{
		direction = vec3(0, 1, 0);
	}
	else
	{
		float highest = SUNRISE_HOUR + 0.5f * (SUNSET_HOUR - SUNRISE_HOUR);
		float percentage = lightIntensity * 0.5f;
		if (hours > highest) percentage = 0.5f + 0.5f * (1 - lightIntensity);

		// x,y,z in [0,1]
		float y = sin(PI * percentage);
		float x = 2.0f * percentage - 1.0f;
		float z = 0.0f;
		direction = vec3(x, y, z);
		direction = normalize(direction);
	}

	return direction;
}

float Light::lightIntensityFromTime(float hours)
{
	float intensity = 0;
	if (hours < SUNRISE_HOUR || hours > SUNSET_HOUR)
	{
		intensity = 0.0f;
	}
	else
	{
		float highest = SUNRISE_HOUR + 0.5 * (SUNSET_HOUR - SUNRISE_HOUR);
		if (hours < highest)
		{
			intensity = (1 / (highest - SUNRISE_HOUR)) * (hours - SUNRISE_HOUR);
		}
		else
		{
			intensity = (1 / (highest - SUNSET_HOUR)) * (hours - SUNSET_HOUR);
		}
	}

	return intensity;
}

void Light::updateLight(float *lightIntensity, vec3 *lightDirection, float *daytime, float passedTime)
{
	*daytime += DAY_TIME_UPDATE_PER_SECOND * passedTime;
	if (*daytime >= 24.0f) *daytime -= 24.0f;
	*lightIntensity = lightIntensityFromTime(*daytime);
	*lightDirection = lightDirectionFromTime(*lightIntensity, *daytime);
}