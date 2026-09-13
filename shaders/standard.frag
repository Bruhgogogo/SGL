#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 ambientColor;

out vec4 finalColor;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 base = texelColor.rgb * colDiffuse.rgb * fragColor.rgb;

    finalColor = vec4(base + ambientColor, texelColor.a * colDiffuse.a * fragColor.a);
}