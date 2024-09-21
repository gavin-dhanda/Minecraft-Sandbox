#version 330 core

// Task 16: Create a UV coordinate in variable
in vec2 fragUV; // Corresponding in variable


// Task 8: Add a sampler2D uniform
uniform sampler2D myTexture;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool postProcessing;
out vec4 fragColor;

void main()
{
    fragColor = vec4(1);
    // Task 17: Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(myTexture, vec2(fragUV));
    // Task 33: Invert fragColor's r, g, and b color channels if your bool is true
    if(postProcessing){
                fragColor.x = fragColor.x;
                fragColor.y = fragColor.y;
                fragColor.z = fragColor.z;
    }

}
