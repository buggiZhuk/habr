layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 vertexUV;

uniform vec4 param;
uniform mat4 ProjectionViewMatrix;
//0 width
//1 number of parts
//2 start position
//4 movement

//Projection * view * model

out vec3 Color;

void main()
{
    
    float width         = param[0];
	float numberOfParts = param[1];
	float startPosition = param[2];
	float movement      = param[3];
	
	//fragTexCoord = vertexUV;
	vec3 pos = Position;


	float posX = pos.x + startPosition;
	float angleIndent = 360 / numberOfParts;
	float lineNum =posX / (width / (numberOfParts + 1));
	float angle = lineNum * angleIndent + movement;
	

    angle =  ((angle) * 3.14159265358979323846 / 180.0);
	pos.y = pos.y + 0.2*cos(angle);
    Color = vec3(0.0, 0.5, 0.5);
    gl_Position = ProjectionViewMatrix * vec4(pos, 1.0);
}
