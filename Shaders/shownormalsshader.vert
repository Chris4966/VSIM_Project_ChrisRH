#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

void main()
{
    gl_Position = pMatrix * vMatrix * mMatrix * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(vMatrix * mMatrix)));
    vs_out.normal = normalize(vec3(pMatrix * vec4(normalMatrix * aNormal, 0.0)));
}
