#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform int selecionado;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
	if(selecionado == 7) {
		FragColor = FragColor * vec4(0.0, 0.0, 1.0, 1.0);
	}
}