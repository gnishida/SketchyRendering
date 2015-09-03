#version 330

// varying variables
in vec3 fColor;
in vec3 fTexCoord;
in vec3 fNormal;
in vec3 fPosition;

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

void main() {
	if (pass == 1) {
		outputF = vec4((fNormal + 1) * 0.5, 1);
	} else {
		// for color mode
		outputF = vec4(fColor, 1.0);

		// difference in normal between this pixel and the neighbor pixels
		vec3 n = texture(normalMap, vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight)).xyz;
		float d = texture(depthMap, vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight)).x;

		float diff = 0;
		int range = 1;
		for (int xx = -range; xx <= range; ++xx) {
			for (int yy = -range; yy <= range; ++yy) {
				if (xx == 0 && yy == 0) continue;

				vec3 nn = texture(normalMap, vec2((gl_FragCoord.x+xx) / screenWidth, (gl_FragCoord.y+yy) / screenHeight)).xyz;
				if (nn.x == 0 && nn.y == 0 && nn.z == 0) {
					diff = 1.0;
				} else {
					diff = max(diff, length(nn - n));
				}

				float dd = texture(depthMap, vec2((gl_FragCoord.x+xx) / screenWidth, (gl_FragCoord.y+yy) / screenHeight)).x;
				diff = max(diff, abs(dd - d) * 100);
			}
		}
		diff = min(1, diff);
		if (diff < 0.1) {
			diff = 0;
		} else {
			diff = 1;
		}
		outputF = vec4(1 - diff, 1 - diff, 1 - diff, 1);
		
		//outputF = texture(normalMap, vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight));
		return;

	

		if (textureEnabled == 1) { // for texture mode
			outputF = texture(tex0, fTexCoord.rg);
		}

		// lighting
		vec4 ambient = vec4(0.3, 0.3, 0.3, 1.0);
		vec4 diffuse = vec4(0.7, 0.7, 0.7, 1.0) * max(0.0, dot(-lightDir, fNormal));

		outputF = (ambient + diffuse) * outputF;
	}
}

