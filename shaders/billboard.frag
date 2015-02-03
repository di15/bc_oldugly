uniform vec4 color;

//varying vec3 normalOut;
varying vec2 texCoordOut0;
uniform sampler2D texture0;

void main(void)
{
	vec4 texel = texture2D(texture0, texCoordOut0);

	if(texel.w < 0.5)	
		discard;

	texel.w = 1;

	gl_FragColor = color * texture2D(texture0, texCoordOut0);
}