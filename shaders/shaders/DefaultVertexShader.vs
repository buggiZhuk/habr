layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 outColor;

uniform mat4 ProjectionViewMatrix;

//Projection * view * model

out vec3 Color;

void main()
{
    Color = vec3(outColor[0], outColor[1], outColor[2]);
    gl_Position = ProjectionViewMatrix * vec4(Position, 1.0);
}
