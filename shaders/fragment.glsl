#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 lightPosition; // in screen space

// Adjustable attenuation parameters for circle of light
const float LINEAR_TERM    = 0.008;   // Reduced for larger light area
const float QUADRATIC_TERM = 0.00007; // Reduced for larger light area
const float MIN_BRIGHTNESS = 0.0;     // Complete darkness outside light circle
const float LIGHT_RADIUS = 600.0;      // Radius of visible circle in pixels

float attenuate(float distance, float linearTerm, float quadraticTerm)
{
    // Create a sharp falloff - small circle of light, complete darkness outside
    float attenuation = 1.0 / (1.0 + linearTerm * distance + quadraticTerm * distance * distance);
    
    // Make it fall off more sharply and completely dark beyond radius
    if (distance > LIGHT_RADIUS) {
        return 0.0;
    }
    
    // Smooth falloff within radius
    float normalizedDist = distance / LIGHT_RADIUS;
    attenuation *= (1.0 - normalizedDist * normalizedDist);
    
    return max(attenuation, MIN_BRIGHTNESS);
}

void main()
{
    // Use screen-space fragment coord for distance to the light
    float dist = distance(lightPosition, gl_FragCoord.xy);
    float brightness = attenuate(dist, LINEAR_TERM, QUADRATIC_TERM);

    vec4 texColor = texture(texture0, fragTexCoord) * fragColor;
    finalColor = vec4(texColor.rgb * brightness, texColor.a);
}

