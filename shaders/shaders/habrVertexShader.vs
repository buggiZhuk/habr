layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 vertexUV;
//layout(location = 2) in vec2 offset;

uniform mat4 ProjectionViewMatrix;

//0 width
//1 number of parts
//2 start position
//4 movement
uniform vec4 params;

out vec2 fragTexCoord;
//Projection * view * model

void main()
{
    float width         = params[0];
	float numberOfParts = params[1];
	float startPosition = params[2];
	float movement      = params[3];
	
	fragTexCoord = vertexUV;
	vec3 pos = Position;

	float posX = pos.x + startPosition;
	float angleIndent = 360 / numberOfParts;
	float lineNum =posX / (width / (numberOfParts + 1));
	float angle = lineNum * angleIndent + movement;
	

    angle =  ((angle) * 3.14159265358979323846 / 180.0);
	pos.y = pos.y + 0.1*cos(angle);
	
    gl_Position = ProjectionViewMatrix * vec4(pos, 1.0);
}