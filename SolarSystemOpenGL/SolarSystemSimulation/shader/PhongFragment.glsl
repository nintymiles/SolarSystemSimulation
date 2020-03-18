#version 300 es
precision highp float;

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
uniform float   PickingAlpha;

uniform vec3    LightPosition;

in vec3    normalCoord;
in vec3    eyeCoord;
in vec2    textureCoord;

layout(location = 0) out vec4 FinalColor;

vec3 PhongShading(vec3 matDiffuse)
{
    vec3 normalizeNormal   = normalize( normalCoord );
    vec3 normalizeEyeCoord = normalize( -eyeCoord );
    vec3 normalizeLightVec = normalize( LightPosition - eyeCoord );
    
    // Diffuse Intensity
    float cosAngle = max( 0.0, dot( normalizeNormal, normalizeLightVec ));

    // Specular Intensity
    vec3 V = normalizeEyeCoord; // Viewer's vector
    vec3 R = reflect( -normalizeLightVec, normalizeNormal ); // Reflectivity
    float sIntensity 	= pow( max( 0.0, dot( R, V ) ), ShininessFactor );

    // DS color as result of Material & Light interaction
    vec3 diffuse    = matDiffuse  * LightDiffuse;
    vec3 specular   = MaterialSpecular * LightSpecular;
    vec3 ambient    = MaterialAmbient  * LightAmbient;

    return (cosAngle * diffuse + sIntensity * specular + ambient) ;
}

void main() {
    
    vec3 texColor = texture(planetTexture,textureCoord).xyz;

    vec3 lightIntensity = PhongShading(texColor);

    float pAlpha = PickingAlpha;
//    if(PickingAlpha==0.0)
//        pAlpha=1.0;

    //test code
    if(pAlpha==0.5){
        FinalColor = vec4(lightIntensity,pAlpha)+vec4(0.5,0,0,0);
    }else{
        FinalColor = vec4(lightIntensity,pAlpha);
    }
    
//    FinalColor = vec4(lightIntensity,pAlpha);

}


