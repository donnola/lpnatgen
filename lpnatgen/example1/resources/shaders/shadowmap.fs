#version 330

// This shader is based on the basic lighting shader
// This only supports one light, which is directional, and it (of course) supports shadows

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
//in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

#define     MAX_LIGHTS              4
#define     LIGHT_DIRECTIONAL       0
#define     LIGHT_POINT             1

struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
};

// Input lighting values
uniform Light lights[MAX_LIGHTS];
uniform vec3 lightDir;
uniform vec4 lightColor;
uniform vec4 ambient;
uniform vec3 viewPos;
uniform float fogDensity;

// Input shadowmapping values
uniform mat4 lightVP; // Light source view-projection matrix
uniform sampler2D shadowMap;

uniform int shadowMapResolution;

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 lightDot = vec3(0.0);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPos - fragPosition);
    vec3 specular = vec3(0.0);

    vec3 l = -lightDir;

    float NdotL = max(dot(normal, l), 0.0);
    lightDot += lightColor.rgb*NdotL;

    float specCo = 0.0;
    if (NdotL > 0.0) specCo = pow(max(0.0, dot(viewD, reflect(-(l), normal))), 16.0); // 16 refers to shine
    specular += specCo;

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].enabled == 1)
        {
            vec3 light = vec3(0.0);

            if (lights[i].type == LIGHT_DIRECTIONAL) light = -normalize(lights[i].target - lights[i].position);
            if (lights[i].type == LIGHT_POINT) light = normalize(lights[i].position - fragPosition);

            float NdotL = max(dot(normal, light), 0.0);
            lightDot += lights[i].color.rgb*NdotL;
        }
    }

    finalColor = (texelColor*((colDiffuse + vec4(specular, 1.0))*vec4(lightDot, 1.0)));

    // Shadow calculations
    vec4 fragPosLightSpace = lightVP * vec4(fragPosition, 1);
    fragPosLightSpace.xyz /= fragPosLightSpace.w; // Perform the perspective division
    fragPosLightSpace.xyz = (fragPosLightSpace.xyz + 1.0f) / 2.0f; // Transform from [-1, 1] range to [0, 1] range
    vec2 sampleCoords = fragPosLightSpace.xy;
    float curDepth = fragPosLightSpace.z;
    // Slope-scale depth bias: depth biasing reduces "shadow acne" artifacts, where dark stripes appear all over the scene.
    // The solution is adding a small bias to the depth
    // In this case, the bias is proportional to the slope of the surface, relative to the light
    float bias = max(0.0002 * (1.0 - dot(normal, l)), 0.00002) + 0.00001;
    int shadowCounter = 0;
    const int numSamples = 9;
    // PCF (percentage-closer filtering) algorithm:
    // Instead of testing if just one point is closer to the current point,
    // we test the surrounding points as well.
    // This blurs shadow edges, hiding aliasing artifacts.
    vec2 texelSize = vec2(1.0f / float(shadowMapResolution));
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float sampleDepth = texture(shadowMap, sampleCoords + texelSize * vec2(x, y)).r;
            if (curDepth - bias > sampleDepth)
            {
                shadowCounter++;
            }
        }
    }
    finalColor = mix(finalColor, vec4(0, 0, 0, 1), float(shadowCounter) / float(numSamples));

    // Add ambient lighting whether in shadow or not
    finalColor += texelColor*(ambient/10.0)*colDiffuse;

    float dist = length(viewPos - fragPosition);

    const vec4 fogColor = vec4(0.5, 0.5, 0.5, 1.0);
    //const float fogDensity = 0.16;

    // Exponential fog
    float fogFactor = 1.0/exp((dist*fogDensity)*(dist*fogDensity));

    // Linear fog (less nice)
    //const float fogStart = 2.0;
    //const float fogEnd = 10.0;
    //float fogFactor = (fogEnd - dist)/(fogEnd - fogStart);

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Gamma correction
    finalColor = pow(finalColor, vec4(1.0/2.2));
    finalColor = mix(fogColor, finalColor, fogFactor);
}