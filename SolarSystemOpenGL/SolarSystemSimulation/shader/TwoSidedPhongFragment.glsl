#version 300 es
precision mediump float;

// Texture property
uniform sampler2D planetTexture;

// Material property
uniform vec3    MaterialAmbient;
uniform vec3    MaterialSpecular;
uniform vec3    MaterialDiffuse;

// Light property
uniform vec3    LightAmbient;
uniform vec3    LightSpecular;
uniform vec3    LightDiffuse;
uniform float   ShininessFactor;

uniform vec3    LightPosition;

in vec3    normalCoord;
in vec3    eyeCoord;
in vec2    textureCoord;

uniform lowp sampler2DShadow ShadowMap;

layout(location = 0) out vec4 FinalColor;

vec3 normalizeNormal, normalizeEyeCoord, normalizeLightVec, V, R, ambient, diffuse, specular;
float sIntensity, cosAngle;
uniform int isLightPrespectivePass;

vec3 PhongShading(vec3 matDiffuse,bool frontSide)
{
    normalizeNormal   = normalize( normalCoord );
    normalizeEyeCoord = normalize( eyeCoord );
    normalizeLightVec = normalize( LightPosition - eyeCoord );
    
    // Diffuse Intensity
    if ( frontSide){
        cosAngle = max( 0.0, dot( normalizeNormal, normalizeLightVec ));
    }
    else{
        cosAngle = max( 0.0, dot( -normalizeNormal, normalizeLightVec ));
    }

    // Specular Intensity
    V = -normalizeEyeCoord; // Viewer's vector
    R = reflect( -normalizeLightVec, normalizeNormal ); // Reflectivity
    sIntensity 	= pow( max( 0.0, dot( R, V ) ), ShininessFactor );

    // DS color as result of Material & Light interaction
    diffuse    = matDiffuse  * LightDiffuse;
    specular   = MaterialSpecular * LightSpecular;
    ambient    = MaterialAmbient  * LightAmbient;

    return ( cosAngle * diffuse ) + ( sIntensity * specular ) + ambient;
}

void main() {
    if(isLightPrespectivePass == 1){
        return;
    }
    
    vec3 texColor = texture(planetTexture,textureCoord).xyz;

    vec3 lightIntensity;
    
    if (gl_FrontFacing){
        lightIntensity = PhongShading(texColor,true);
    }
    else{
        lightIntensity = PhongShading(texColor,false);
    }

    FinalColor = vec4(lightIntensity , 1.0);
    
    return;
}


