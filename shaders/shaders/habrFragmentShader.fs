uniform sampler2D mTexel;

in vec2 fragTexCoord;
out vec4 FragColor;

void main()
{
    FragColor = texture(mTexel, fragTexCoord);
}
