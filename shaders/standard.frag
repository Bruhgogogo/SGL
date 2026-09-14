#version 330

#define MAX_DIR_LIGHTS 4
#define PI 3.14159265358979323846

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec3 groundAmbient;
uniform vec3 skyAmbient;

uniform int   dirLightCount;
uniform vec3  dirLightDirections[MAX_DIR_LIGHTS];
uniform vec3  dirLightColors[MAX_DIR_LIGHTS];
uniform float dirLightIntensities[MAX_DIR_LIGHTS];

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);

    vec3 albedo = texelColor.rgb * colDiffuse.rgb * fragColor.rgb;
    float baseAlpha = texelColor.a * colDiffuse.a * fragColor.a;

    vec3 N = normalize(fragNormal);

    float hemi = N.y * 0.5 + 0.5;
    vec3 ambient = mix(groundAmbient, skyAmbient, hemi);

    vec3 diffuse = vec3(0.0);

    for (int i = 0; i < dirLightCount; i++)
    {
        vec3 L = normalize(-dirLightDirections[i]);
        float ndotl = max(dot(N, L), 0.0);
        diffuse += dirLightColors[i] * dirLightIntensities[i] * ndotl;
    }

    vec3 lit = albedo * (ambient + diffuse / PI);

    finalColor = vec4(lit, baseAlpha);
}