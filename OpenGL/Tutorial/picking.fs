#version 410                                                                        
#extension GL_EXT_gpu_shader4 : enable

uniform uint gDrawIndex;                                                            
uniform uint gObjectIndex;                                                          



out uvec3 FragColor;

void main()                                                                         
{                                                                                   
   FragColor = uvec3(gObjectIndex, gDrawIndex,gl_PrimitiveID + 1);                 
}