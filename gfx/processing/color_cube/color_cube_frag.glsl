precision mediump float;
precision mediump int;

uniform mat4 colorMatrix;
uniform int colorSpace;

varying vec4 vertColor;

vec4 RGBtoCMYK(vec3 rgb) {
    float r = rgb.r;
    float g = rgb.g;
    float b = rgb.b;
    float k = min(1.0 - r, min(1.0 - g, 1.0 - b));
    vec3 cmy = vec3(0.0);
    float invK = 1.0 - k;
    if (invK != 0.0) {
        cmy.x = (1.0 - r - k) / invK;
        cmy.y = (1.0 - g - k) / invK;
        cmy.z = (1.0 - b - k) / invK;
    }
    return clamp(vec4(cmy, k), 0.0, 1.0);
}

vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 rgb2hsl(vec3 color) {
 	vec3 hsl; // init to 0 to avoid warnings ? (and reverse if + remove first part)

 	float fmin = min(min(color.r, color.g), color.b); //Min. value of RGB
 	float fmax = max(max(color.r, color.g), color.b); //Max. value of RGB
 	float delta = fmax - fmin; //Delta RGB value

 	hsl.z = (fmax + fmin) / 2.0; // Luminance

 	if (delta == 0.0) //This is a gray, no chroma...
 	{
 		hsl.x = 0.0; // Hue
 		hsl.y = 0.0; // Saturation
 	}
	else //Chromatic data...
 	{
		if (hsl.z < 0.5)
 			hsl.y = delta / (fmax + fmin); // Saturation
 		else
 			hsl.y = delta / (2.0 - fmax - fmin); // Saturation

 		float deltaR = (((fmax - color.r) / 6.0) + (delta / 2.0)) / delta;
 		float deltaG = (((fmax - color.g) / 6.0) + (delta / 2.0)) / delta;
 		float deltaB = (((fmax - color.b) / 6.0) + (delta / 2.0)) / delta;

 		if (color.r == fmax)
 			hsl.x = deltaB - deltaG; // Hue
 		else if (color.g == fmax)
 			hsl.x = (1.0 / 3.0) + deltaR - deltaB; // Hue
 		else if (color.b == fmax)
 			hsl.x = (2.0 / 3.0) + deltaG - deltaR; // Hue

 		if (hsl.x < 0.0)
 			hsl.x += 1.0; // Hue
 		else if (hsl.x > 1.0)
 			hsl.x -= 1.0; // Hue
 	}

 	return hsl;
}

void main() {
    vec4 color = colorMatrix * vertColor;
    if (colorSpace == 1)
        color = vec4(rgb2hsv(color.rgb), 1);
    else if (colorSpace == 2)
        color = vec4(rgb2hsl(color.rgb), 1);
    else if (colorSpace == 3)
        color = RGBtoCMYK(color.rgb);
    gl_FragColor = color;
}

