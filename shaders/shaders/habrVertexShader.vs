layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 vertexUV;

uniform mat4 ProjectionViewMatrix;

out vec2 fragTexCoord;

void main()
{
    fragTexCoord = vertexUV;
    gl_Position = ProjectionViewMatrix * vec4(Position, 1.0);
}