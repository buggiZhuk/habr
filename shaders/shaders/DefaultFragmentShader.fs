in  vec3 Color;
out vec4 FragColor;

void main()
{
    FragColor = vec4(Color[0], Color[1], Color[2], 1);
}
