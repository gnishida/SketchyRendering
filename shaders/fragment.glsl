#version 330

// varying variables
in vec3 vColor;
in vec3 vTexCoord;
in vec3 vNormal;
in vec3 vPosition;

// output color
out vec4 outputF;

uniform int pass;	// 1 -- 1st pass / 2 -- 2nd pass

// uniform variables
uniform int textureEnabled;	// 0 -- color / 1 -- texture
uniform sampler2D tex0;

uniform vec3 lightDir;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform int screenWidth;
uniform int screenHeight;

// return random value in [-1, 1]
float random(vec3 position, float scale, int cycle_size) {
	return float(int((position.x + position.y + position.z) * scale) % (cycle_size * 2 + 1) - cycle_size) / float(cycle_size);
}

void main() {
	if (pass == 1) {
		outputF = vec4((vNormal + 1) * 0.5, 1);
	} else {
		// for color mode
		outputF = vec4(vColor, 1.0);

		float diff = 0;
		int range = 1;

		int num_iterations = 2;
		float scale_x[5] = float[](93.0, 317.0, 237.0, 133.0, 147.0);
		float scale_y[5] = float[](170.0, 317.0, 237.0, 133.0, 147.0);
		float scale_z[5] = float[](93.0, 317.0, 237.0, 133.0, 147.0);
		int cycle_size[5] = int[](73, 193, 311, 117, 213);
		float jitter_size = 3.0;
		for (int iter = 0; iter < num_iterations; ++iter) {
			float sx = gl_FragCoord.x + random(vPosition, scale_x[iter], cycle_size[iter]) * jitter_size;
			float sy = gl_FragCoord.y + random(vPosition, scale_y[iter], cycle_size[iter]) * jitter_size;
			float sz = gl_FragCoord.z + random(vPosition, scale_z[iter], cycle_size[iter]) * jitter_size;

			// difference in normal between this pixel and the neighbor pixels
			vec3 n = texture(normalMap, vec2(sx / screenWidth, sy / screenHeight)).xyz;
			float d = texture(depthMap, vec2(sx / screenWidth, sy / screenHeight)).x;

			for (int xx = -range; xx <= range; ++xx) {
				for (int yy = -range; yy <= range; ++yy) {
					if (xx == 0 && yy == 0) continue;

					vec3 nn = texture(normalMap, vec2((sx+xx) / screenWidth, (sy+yy) / screenHeight)).xyz;
					if (nn.x == 0 && nn.y == 0 && nn.z == 0) {
						diff = 1.0;
					} else {
						diff = max(diff, length(nn - n));
					}

					float dd = texture(depthMap, vec2((sx+xx) / screenWidth, (sy+yy) / screenHeight)).x;
					diff = max(diff, abs(dd - d) * 100);
				}
			}
		}

		diff = min(1, diff);
		if (diff < 0.1) {
			diff = 0;
		} else {
			diff = 1;
		}
		outputF = vec4(1 - diff, 1 - diff, 1 - diff, 1);
		
		return;

	

		if (textureEnabled == 1) { // for texture mode
			outputF = texture(tex0, vTexCoord.rg);
		}

		// lighting
		vec4 ambient = vec4(0.3, 0.3, 0.3, 1.0);
		vec4 diffuse = vec4(0.7, 0.7, 0.7, 1.0) * max(0.0, dot(-lightDir, vNormal));

		outputF = (ambient + diffuse) * outputF;
	}
}

