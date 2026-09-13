#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 groundAmbient;
uniform vec3 skyAmbient;

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);

    vec3 base = texelColor.rgb * colDiffuse.rgb * fragColor.rgb;
    float baseAlpha = texelColor.a * colDiffuse.a * fragColor.a;

    vec3 N = normalize(fragNormal);
    float hemi = N.y * 0.5 + 0.5;
    vec3 ambient = mix(groundAmbient, skyAmbient, hemi);

    finalColor = vec4(base * ambient, baseAlpha);
}