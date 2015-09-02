#version 330

// varying variables
in vec3 fColor;
in vec3 fTexCoord;
in vec3 fNormal;
in vec3 fPosition;
noperspective in vec3 dist;

// output color
out vec4 outputF;

uniform int pass;	// 1 -- 1st pass / 2 -- 2nd pass

// uniform variables
uniform int textureEnabled;	// 0 -- color / 1 -- texture
uniform sampler2D tex0;
uniform int wireframeEnalbed; // 0 -- no wireframe / 1 -- add wireframe

uniform vec3 lightDir;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform int screenWidth;
uniform int screenHeight;

void main()
{
	if (pass == 1) {
		outputF = vec4(fNormal, 1);
	} else {
		// for color mode
		outputF = vec4(fColor, 1.0);

		/////// debug
		
		vec3 n = texture(normalMap, vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight)).xyz;
		float diff = 0;
		for (int xx = -3; xx <= 3; ++xx) {
			for (int yy = -3; yy <= 3; ++yy) {
				vec3 nn = texture(normalMap, vec2((gl_FragCoord.x+xx) / screenWidth, (gl_FragCoord.y+yy) / screenHeight)).xyz;
				if (nn.x == 0 && nn.y == 0 && nn.z == 0) {
					diff = 1.0;
				} else {
					diff = max(diff, length(nn - n));
				}
			}
		}
		diff = min(1, diff);
		outputF = vec4(1 - diff, 1 - diff, 1 - diff, 1);
		
		//outputF = texture(normalMap, vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight));
		return;

		/////// debug



		
		// determine frag distance to closest edge
		float nearD = min(min(dist[0],dist[1]),dist[2]);
		float edgeIntensity = exp2(-1.0*nearD*nearD);

		if (textureEnabled == 1) { // for texture mode
			outputF = texture(tex0, fTexCoord.rg);
		}

		// check the normal in the neighborhood


		// lighting
		vec4 ambient = vec4(0.3, 0.3, 0.3, 1.0);
		vec4 diffuse = vec4(0.7, 0.7, 0.7, 1.0) * max(0.0, dot(-lightDir, fNormal));

		outputF = (ambient + diffuse) * outputF;

		if (wireframeEnalbed == 1) {
			outputF = edgeIntensity * vec4(0.05, 0.05, 0.05, 1.0) + (1.0 - edgeIntensity) * outputF;
		}
	}
}

