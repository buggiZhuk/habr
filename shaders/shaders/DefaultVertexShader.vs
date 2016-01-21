layout(location = 0) in vec3 Position;

uniform mat4 ProjectionViewMatrix;


void main()
{
    
    gl_Position = ProjectionViewMatrix * vec4(Position, 1.0);
}
