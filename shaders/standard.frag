#version 330

#define MAX_DIR_LIGHTS 2
#define MAX_POINT_LIGHTS 8
#define PI 3.14159265358979323846

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragWorldPos;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 groundAmbient;
uniform vec3 skyAmbient;

uniform int   dirLightCount;
uniform vec3  dirLightDirections[MAX_DIR_LIGHTS];
uniform vec3  dirLightColors[MAX_DIR_LIGHTS];
uniform float dirLightIntensities[MAX_DIR_LIGHTS];

uniform int   pointLightCount;
uniform vec3  pointLightPositions[MAX_POINT_LIGHTS];
uniform vec3  pointLightColors[MAX_POINT_LIGHTS];
uniform float pointLightIntensities[MAX_POINT_LIGHTS];
uniform float pointLightRanges[MAX_POINT_LIGHTS];

out vec4 finalColor;

vec3 BRDF(vec3 N, vec3 L, vec3 lightColor, float intensity)
{
    float ndotl = max(dot(N, L), 0.0);
    return lightColor * intensity * ndotl;
}

vec3 ComputeLighting(vec3 N, vec3 albedo)
{
    float hemi = N.y * 0.5 + 0.5;
    vec3 ambient = mix(groundAmbient, skyAmbient, hemi);

    vec3 diffuse = vec3(0.0);

    for (int i = 0; i < dirLightCount; i++)
    {
        vec3 L = normalize(-dirLightDirections[i]);
        diffuse += BRDF(N, L, dirLightColors[i], dirLightIntensities[i]);
    }

    for (int i = 0; i < pointLightCount; i++)
    {
        vec3 toLight = pointLightPositions[i] - fragWorldPos;
        float dist = length(toLight);

        if (dist > pointLightRanges[i]) continue;

        vec3 L = toLight / dist;

        float norm = dist / pointLightRanges[i];
        float atten = 1.0 - norm * norm;
        atten = atten * atten;

        diffuse += BRDF(N, L, pointLightColors[i], pointLightIntensities[i]) * atten;
    }

    return albedo * (ambient + diffuse / PI);
}

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);

    vec3 albedo = texelColor.rgb * colDiffuse.rgb * fragColor.rgb;
    float baseAlpha = texelColor.a * colDiffuse.a * fragColor.a;

    vec3 N = normalize(fragNormal);

    vec3 lit = ComputeLighting(N, albedo);

    finalColor = vec4(lit, baseAlpha);
}