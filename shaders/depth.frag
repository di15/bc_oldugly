

uniform sampler2D texture0;

varying vec2 texCoordOut0;

void main(void)
{
	//gl_FragColor = color * texture2D(texture0, texCoordOut0);
	//gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	//gl_FragColor = vec4(fragCoordZ, fragCoordZ, fragCoordZ, 1.0);
	//gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);


	vec4 texel0 = texture2D(texture0, texCoordOut0);
	//gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, texel0.w);

	if(texel0.w < 0.8)
		discard;

	gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
}
