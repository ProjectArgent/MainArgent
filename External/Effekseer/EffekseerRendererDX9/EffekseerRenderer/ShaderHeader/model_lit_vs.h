#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /Zpc /Tvs_3_0 /Emain /Fh ShaderHeader/model_lit_vs.h
//    Shader/model_lit_vs.fx
//
//
// Parameters:
//
//   float4 _32_fModelColor[10];
//   float4 _32_fUV[10];
//   float4x4 _32_mCameraProj;
//   float4x4 _32_mModel_Inst[10];
//   float4 _32_mUVInversed;
//
//
// Registers:
//
//   Name            Reg   Size
//   --------------- ----- ----
//   _32_mCameraProj c0       4
//   _32_mModel_Inst c4      40
//   _32_fUV         c44     10
//   _32_fModelColor c54     10
//   _32_mUVInversed c67      1
//

    vs_3_0
    def c64, 4, 0, 0, 0
    dcl_texcoord v0
    dcl_texcoord1 v1
    dcl_texcoord2 v2
    dcl_texcoord3 v3
    dcl_texcoord4 v4
    dcl_texcoord5 v5
    dcl_texcoord6 v6
    dcl_texcoord o0
    dcl_texcoord1 o1.xy
    dcl_texcoord2 o2.xyz
    dcl_texcoord3 o3.xyz
    dcl_texcoord4 o4.xyz
    dcl_texcoord5 o5
    dcl_position o6
    slt r0.x, v6.x, -v6.x
    frc r0.y, v6.x
    add r0.z, -r0.y, v6.x
    slt r0.y, -r0.y, r0.y
    mad r0.x, r0.x, r0.y, r0.z
    mova a0.x, r0.x
    mul r0.x, r0.x, c64.x
    mova a0.y, r0.x
    mul o0, v5, c54[a0.x]
    mad o1.x, v4.x, c44[a0.x].z, c44[a0.x].x
    mad r0.x, v4.y, c44[a0.x].w, c44[a0.x].y
    mad o1.y, c67.y, r0.x, c67.x
    mul r0, v1.y, c5[a0.y]
    mad r0, c4[a0.y], v1.x, r0
    mad r0, c6[a0.y], v1.z, r0
    dp4 r0.w, r0, r0
    rsq r0.w, r0.w
    mul o2.xyz, r0.w, r0
    mul r0, v2.y, c5[a0.y]
    mad r0, c4[a0.y], v2.x, r0
    mad r0, c6[a0.y], v2.z, r0
    dp4 r0.w, r0, r0
    rsq r0.w, r0.w
    mul o3.xyz, r0.w, r0
    mul r0, v3.y, c5[a0.y]
    mad r0, c4[a0.y], v3.x, r0
    mad r0, c6[a0.y], v3.z, r0
    dp4 r0.w, r0, r0
    rsq r0.w, r0.w
    mul o4.xyz, r0.w, r0
    mul r0, v0.y, c5[a0.y]
    mad r0, c4[a0.y], v0.x, r0
    mad r0, c6[a0.y], v0.z, r0
    add r0, r0, c7[a0.y]
    mul r1, r0.y, c1
    mad r1, c0, r0.x, r1
    mad r1, c2, r0.z, r1
    mad r0, c3, r0.w, r1
    mov o5, r0
    mov o6, r0

// approximately 40 instruction slots used
#endif

const BYTE g_vs30_main[] =
{
      0,   3, 254, 255, 254, 255, 
     85,   0,  67,  84,  65,  66, 
     28,   0,   0,   0,  31,   1, 
      0,   0,   0,   3, 254, 255, 
      5,   0,   0,   0,  28,   0, 
      0,   0,  16,   1,   0,   0, 
     24,   1,   0,   0, 128,   0, 
      0,   0,   2,   0,  54,   0, 
     10,   0, 218,   0, 144,   0, 
      0,   0,   0,   0,   0,   0, 
    160,   0,   0,   0,   2,   0, 
     44,   0,  10,   0, 178,   0, 
    168,   0,   0,   0,   0,   0, 
      0,   0, 184,   0,   0,   0, 
      2,   0,   0,   0,   4,   0, 
      2,   0, 200,   0,   0,   0, 
      0,   0,   0,   0, 216,   0, 
      0,   0,   2,   0,   4,   0, 
     40,   0,  18,   0, 232,   0, 
      0,   0,   0,   0,   0,   0, 
    248,   0,   0,   0,   2,   0, 
     67,   0,   1,   0,  14,   1, 
      8,   1,   0,   0,   0,   0, 
      0,   0,  95,  51,  50,  95, 
    102,  77, 111, 100, 101, 108, 
     67, 111, 108, 111, 114,   0, 
      1,   0,   3,   0,   1,   0, 
      4,   0,  10,   0,   0,   0, 
      0,   0,   0,   0,  95,  51, 
     50,  95, 102,  85,  86,   0, 
      1,   0,   3,   0,   1,   0, 
      4,   0,  10,   0,   0,   0, 
      0,   0,   0,   0,  95,  51, 
     50,  95, 109,  67,  97, 109, 
    101, 114,  97,  80, 114, 111, 
    106,   0,   3,   0,   3,   0, 
      4,   0,   4,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     95,  51,  50,  95, 109,  77, 
    111, 100, 101, 108,  95,  73, 
    110, 115, 116,   0,   3,   0, 
      3,   0,   4,   0,   4,   0, 
     10,   0,   0,   0,   0,   0, 
      0,   0,  95,  51,  50,  95, 
    109,  85,  86,  73, 110, 118, 
    101, 114, 115, 101, 100,   0, 
      1,   0,   3,   0,   1,   0, 
      4,   0,   1,   0,   0,   0, 
      0,   0,   0,   0, 118, 115, 
     95,  51,  95,  48,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  57,  46, 
     50,  57,  46,  57,  53,  50, 
     46,  51,  49,  49,  49,   0, 
     81,   0,   0,   5,  64,   0, 
     15, 160,   0,   0, 128,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     31,   0,   0,   2,   5,   0, 
      0, 128,   0,   0,  15, 144, 
     31,   0,   0,   2,   5,   0, 
      1, 128,   1,   0,  15, 144, 
     31,   0,   0,   2,   5,   0, 
      2, 128,   2,   0,  15, 144, 
     31,   0,   0,   2,   5,   0, 
      3, 128,   3,   0,  15, 144, 
     31,   0,   0,   2,   5,   0, 
      4, 128,   4,   0,  15, 144, 
     31,   0,   0,   2,   5,   0, 
      5, 128,   5,   0,  15, 144, 
     31,   0,   0,   2,   5,   0, 
      6, 128,   6,   0,  15, 144, 
     31,   0,   0,   2,   5,   0, 
      0, 128,   0,   0,  15, 224, 
     31,   0,   0,   2,   5,   0, 
      1, 128,   1,   0,   3, 224, 
     31,   0,   0,   2,   5,   0, 
      2, 128,   2,   0,   7, 224, 
     31,   0,   0,   2,   5,   0, 
      3, 128,   3,   0,   7, 224, 
     31,   0,   0,   2,   5,   0, 
      4, 128,   4,   0,   7, 224, 
     31,   0,   0,   2,   5,   0, 
      5, 128,   5,   0,  15, 224, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   6,   0,  15, 224, 
     12,   0,   0,   3,   0,   0, 
      1, 128,   6,   0,   0, 144, 
      6,   0,   0, 145,  19,   0, 
      0,   2,   0,   0,   2, 128, 
      6,   0,   0, 144,   2,   0, 
      0,   3,   0,   0,   4, 128, 
      0,   0,  85, 129,   6,   0, 
      0, 144,  12,   0,   0,   3, 
      0,   0,   2, 128,   0,   0, 
     85, 129,   0,   0,  85, 128, 
      4,   0,   0,   4,   0,   0, 
      1, 128,   0,   0,   0, 128, 
      0,   0,  85, 128,   0,   0, 
    170, 128,  46,   0,   0,   2, 
      0,   0,   1, 176,   0,   0, 
      0, 128,   5,   0,   0,   3, 
      0,   0,   1, 128,   0,   0, 
      0, 128,  64,   0,   0, 160, 
     46,   0,   0,   2,   0,   0, 
      2, 176,   0,   0,   0, 128, 
      5,   0,   0,   4,   0,   0, 
     15, 224,   5,   0, 228, 144, 
     54,  32, 228, 160,   0,   0, 
      0, 176,   4,   0,   0,   6, 
      1,   0,   1, 224,   4,   0, 
      0, 144,  44,  32, 170, 160, 
      0,   0,   0, 176,  44,  32, 
      0, 160,   0,   0,   0, 176, 
      4,   0,   0,   6,   0,   0, 
      1, 128,   4,   0,  85, 144, 
     44,  32, 255, 160,   0,   0, 
      0, 176,  44,  32,  85, 160, 
      0,   0,   0, 176,   4,   0, 
      0,   4,   1,   0,   2, 224, 
     67,   0,  85, 160,   0,   0, 
      0, 128,  67,   0,   0, 160, 
      5,   0,   0,   4,   0,   0, 
     15, 128,   1,   0,  85, 144, 
      5,  32, 228, 160,   0,   0, 
     85, 176,   4,   0,   0,   5, 
      0,   0,  15, 128,   4,  32, 
    228, 160,   0,   0,  85, 176, 
      1,   0,   0, 144,   0,   0, 
    228, 128,   4,   0,   0,   5, 
      0,   0,  15, 128,   6,  32, 
    228, 160,   0,   0,  85, 176, 
      1,   0, 170, 144,   0,   0, 
    228, 128,   9,   0,   0,   3, 
      0,   0,   8, 128,   0,   0, 
    228, 128,   0,   0, 228, 128, 
      7,   0,   0,   2,   0,   0, 
      8, 128,   0,   0, 255, 128, 
      5,   0,   0,   3,   2,   0, 
      7, 224,   0,   0, 255, 128, 
      0,   0, 228, 128,   5,   0, 
      0,   4,   0,   0,  15, 128, 
      2,   0,  85, 144,   5,  32, 
    228, 160,   0,   0,  85, 176, 
      4,   0,   0,   5,   0,   0, 
     15, 128,   4,  32, 228, 160, 
      0,   0,  85, 176,   2,   0, 
      0, 144,   0,   0, 228, 128, 
      4,   0,   0,   5,   0,   0, 
     15, 128,   6,  32, 228, 160, 
      0,   0,  85, 176,   2,   0, 
    170, 144,   0,   0, 228, 128, 
      9,   0,   0,   3,   0,   0, 
      8, 128,   0,   0, 228, 128, 
      0,   0, 228, 128,   7,   0, 
      0,   2,   0,   0,   8, 128, 
      0,   0, 255, 128,   5,   0, 
      0,   3,   3,   0,   7, 224, 
      0,   0, 255, 128,   0,   0, 
    228, 128,   5,   0,   0,   4, 
      0,   0,  15, 128,   3,   0, 
     85, 144,   5,  32, 228, 160, 
      0,   0,  85, 176,   4,   0, 
      0,   5,   0,   0,  15, 128, 
      4,  32, 228, 160,   0,   0, 
     85, 176,   3,   0,   0, 144, 
      0,   0, 228, 128,   4,   0, 
      0,   5,   0,   0,  15, 128, 
      6,  32, 228, 160,   0,   0, 
     85, 176,   3,   0, 170, 144, 
      0,   0, 228, 128,   9,   0, 
      0,   3,   0,   0,   8, 128, 
      0,   0, 228, 128,   0,   0, 
    228, 128,   7,   0,   0,   2, 
      0,   0,   8, 128,   0,   0, 
    255, 128,   5,   0,   0,   3, 
      4,   0,   7, 224,   0,   0, 
    255, 128,   0,   0, 228, 128, 
      5,   0,   0,   4,   0,   0, 
     15, 128,   0,   0,  85, 144, 
      5,  32, 228, 160,   0,   0, 
     85, 176,   4,   0,   0,   5, 
      0,   0,  15, 128,   4,  32, 
    228, 160,   0,   0,  85, 176, 
      0,   0,   0, 144,   0,   0, 
    228, 128,   4,   0,   0,   5, 
      0,   0,  15, 128,   6,  32, 
    228, 160,   0,   0,  85, 176, 
      0,   0, 170, 144,   0,   0, 
    228, 128,   2,   0,   0,   4, 
      0,   0,  15, 128,   0,   0, 
    228, 128,   7,  32, 228, 160, 
      0,   0,  85, 176,   5,   0, 
      0,   3,   1,   0,  15, 128, 
      0,   0,  85, 128,   1,   0, 
    228, 160,   4,   0,   0,   4, 
      1,   0,  15, 128,   0,   0, 
    228, 160,   0,   0,   0, 128, 
      1,   0, 228, 128,   4,   0, 
      0,   4,   1,   0,  15, 128, 
      2,   0, 228, 160,   0,   0, 
    170, 128,   1,   0, 228, 128, 
      4,   0,   0,   4,   0,   0, 
     15, 128,   3,   0, 228, 160, 
      0,   0, 255, 128,   1,   0, 
    228, 128,   1,   0,   0,   2, 
      5,   0,  15, 224,   0,   0, 
    228, 128,   1,   0,   0,   2, 
      6,   0,  15, 224,   0,   0, 
    228, 128, 255, 255,   0,   0
};
