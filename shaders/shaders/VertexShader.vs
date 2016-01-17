#if __VERSION__ > 130

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 offset;

#else

 in vec3 Position;

#endif

uniform mat4 ProjectionViewMatrix;

//Projection * view * model

void main()
{
    vec3 newPos = Position;
    newPos.x *= offset[2];
    newPos.y *= offset[2];
    newPos.x += offset[0];
    newPos.y += offset[1];
    newPos.z += 300;
    
    gl_Position = ProjectionViewMatrix * vec4(newPos, 1.0);
}
