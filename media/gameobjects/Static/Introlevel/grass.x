xof 0303txt 0032
template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template VertexDuplicationIndices {
<b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}


Frame Scene_Root {


 FrameTransformMatrix {
  1.000000, 0.000000, 0.000000, 0.000000,
  0.000000, 1.000000, 0.000000, 0.000000,
  0.000000, 0.000000, 1.000000, 0.000000,
  0.000000, 0.000000, 0.000000, 1.000000;;
 }

  Frame Plane04 {

   FrameTransformMatrix {
    1.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 1.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 1.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 1.000000;;
   }

   Mesh {
    64;
    41.135071;0.000000;-36.686707;,
    44.135071;0.000000;-28.151615;,
    31.489334;0.250000;-27.997921;,
    31.489334;0.000000;-39.353371;,
    18.843605;0.250000;-27.997921;,
    18.176937;0.666667;-45.686707;,
    6.197867;0.250000;-28.151615;,
    5.531199;0.666667;-45.686707;,
    -6.447867;0.250000;-28.151615;,
    -6.781201;0.666666;-42.353371;,
    -19.093605;0.500000;-28.484951;,
    -19.426939;0.666666;-42.353371;,
    -31.739334;0.250000;-28.151615;,
    -31.739334;0.666665;-38.353371;,
    -48.385071;0.000000;-28.484947;,
    -43.051739;0.000000;-37.020042;,
    46.135071;0.000000;-17.616528;,
    31.489334;0.250000;-16.949860;,
    18.843605;0.250000;-16.949860;,
    6.197867;0.250000;-16.949860;,
    -6.447867;0.666666;-17.283195;,
    -19.093605;0.666666;-17.283195;,
    -31.739334;0.250000;-16.949860;,
    -48.385071;0.000000;-17.283192;,
    46.135071;0.000000;-6.414769;,
    31.489334;0.250000;-5.748102;,
    18.843605;0.250000;-5.748102;,
    6.197867;0.250000;-5.748102;,
    -6.447867;0.250000;-5.748102;,
    -19.093605;0.250000;-5.748102;,
    -31.739334;0.250000;-5.748102;,
    -44.385071;0.000000;-5.748102;,
    47.801735;0.000000;5.786990;,
    31.489334;0.250000;5.453657;,
    18.843605;0.250000;6.094389;,
    6.197867;0.250000;6.094389;,
    -6.447867;0.250000;5.453657;,
    -19.093605;0.250000;5.453657;,
    -31.739334;0.250000;5.453657;,
    -47.051735;0.000000;5.786990;,
    51.468403;0.000000;17.988749;,
    31.489334;0.250000;16.655415;,
    18.843605;0.250000;16.655415;,
    6.197867;0.250000;16.655415;,
    -6.447867;0.250000;16.655415;,
    -19.093605;0.250000;16.655415;,
    -31.739334;0.250000;16.655415;,
    -47.051735;0.000000;16.988749;,
    46.801735;0.000000;28.523836;,
    31.489334;0.250000;27.857170;,
    18.843605;0.250000;27.857170;,
    6.197867;0.250000;27.857170;,
    -6.447867;0.250000;27.857170;,
    -19.093605;0.250000;27.523834;,
    -31.739334;0.250000;27.857170;,
    -44.385071;0.000000;27.857170;,
    43.135071;0.000000;37.392269;,
    31.489334;0.000000;42.058937;,
    18.843605;0.000000;39.058937;,
    6.197867;0.000000;39.058937;,
    -6.447867;0.000000;43.058937;,
    -19.093605;0.000000;43.058937;,
    -31.739334;0.000000;39.058937;,
    -42.051739;0.000000;35.725601;;
    98;
    3;0,2,1;,
    3;0,3,2;,
    3;2,5,4;,
    3;2,3,5;,
    3;5,6,4;,
    3;5,7,6;,
    3;6,9,8;,
    3;6,7,9;,
    3;9,10,8;,
    3;9,11,10;,
    3;10,13,12;,
    3;10,11,13;,
    3;13,14,12;,
    3;13,15,14;,
    3;16,2,17;,
    3;16,1,2;,
    3;2,18,17;,
    3;2,4,18;,
    3;18,6,19;,
    3;18,4,6;,
    3;6,20,19;,
    3;6,8,20;,
    3;20,10,21;,
    3;20,8,10;,
    3;10,22,21;,
    3;10,12,22;,
    3;22,14,23;,
    3;22,12,14;,
    3;16,25,24;,
    3;16,17,25;,
    3;25,18,26;,
    3;25,17,18;,
    3;18,27,26;,
    3;18,19,27;,
    3;27,20,28;,
    3;27,19,20;,
    3;20,29,28;,
    3;20,21,29;,
    3;29,22,30;,
    3;29,21,22;,
    3;22,31,30;,
    3;22,23,31;,
    3;32,25,33;,
    3;32,24,25;,
    3;25,34,33;,
    3;25,26,34;,
    3;34,27,35;,
    3;34,26,27;,
    3;27,36,35;,
    3;27,28,36;,
    3;36,29,37;,
    3;36,28,29;,
    3;29,38,37;,
    3;29,30,38;,
    3;38,31,39;,
    3;38,30,31;,
    3;32,41,40;,
    3;32,33,41;,
    3;41,34,42;,
    3;41,33,34;,
    3;34,43,42;,
    3;34,35,43;,
    3;43,36,44;,
    3;43,35,36;,
    3;36,45,44;,
    3;36,37,45;,
    3;45,38,46;,
    3;45,37,38;,
    3;38,47,46;,
    3;38,39,47;,
    3;48,41,49;,
    3;48,40,41;,
    3;41,50,49;,
    3;41,42,50;,
    3;50,43,51;,
    3;50,42,43;,
    3;43,52,51;,
    3;43,44,52;,
    3;52,45,53;,
    3;52,44,45;,
    3;45,54,53;,
    3;45,46,54;,
    3;54,47,55;,
    3;54,46,47;,
    3;48,57,56;,
    3;48,49,57;,
    3;57,50,58;,
    3;57,49,50;,
    3;50,59,58;,
    3;50,51,59;,
    3;59,52,60;,
    3;59,51,52;,
    3;52,61,60;,
    3;52,53,61;,
    3;61,54,62;,
    3;61,53,54;,
    3;54,63,62;,
    3;54,55,63;;

    MeshNormals {
     64;
     0.012884;0.999812;-0.014465;,
     0.019701;0.999792;-0.005331;,
     0.015377;0.999874;-0.003888;,
     0.033268;0.999204;-0.021998;,
     -0.000072;0.999931;0.011778;,
     0.015043;0.999812;0.012230;,
     0.004878;0.999958;0.007733;,
     0.003183;0.999716;0.023634;,
     0.009945;0.999940;-0.004605;,
     0.006341;0.999705;0.023434;,
     -0.003518;0.999989;0.003198;,
     0.001937;0.999927;0.011970;,
     -0.017323;0.999642;0.020405;,
     -0.023535;0.999632;0.013483;,
     -0.027341;0.999626;0.000291;,
     -0.063439;0.997198;-0.039641;,
     0.017731;0.999842;-0.000936;,
     0.008534;0.999964;0.000000;,
     0.000000;1.000000;0.000000;,
     0.016468;0.999864;0.000000;,
     0.010834;0.999939;0.002102;,
     -0.016327;0.999810;0.010613;,
     -0.018748;0.999819;0.003307;,
     -0.015070;0.999883;0.002622;,
     0.017015;0.999855;-0.001158;,
     0.008303;0.999965;-0.000290;,
     0.000000;1.000000;0.000000;,
     0.004117;0.999981;0.004513;,
     0.000000;0.999837;0.018052;,
     -0.003996;0.999900;0.013537;,
     -0.009883;0.999951;0.000000;,
     -0.017765;0.999842;0.000363;,
     0.015094;0.999885;-0.001538;,
     0.007662;0.999971;0.000000;,
     0.000000;1.000000;0.000000;,
     0.000000;1.000000;0.000000;,
     0.000000;1.000000;0.000000;,
     0.000000;1.000000;0.000000;,
     -0.008603;0.999963;-0.000474;,
     -0.016366;0.999864;-0.001896;,
     0.012461;0.999922;0.000773;,
     0.007071;0.999975;0.000193;,
     0.000000;1.000000;0.000000;,
     0.000000;1.000000;0.000000;,
     0.000000;1.000000;0.000000;,
     0.000000;1.000000;0.000000;,
     -0.008163;0.999967;0.000000;,
     -0.017185;0.999852;0.001212;,
     0.011009;0.999923;0.005746;,
     0.007971;0.999929;0.008800;,
     -0.000662;0.999944;0.010568;,
     0.000000;0.999938;0.011157;,
     0.000597;0.999960;0.008912;,
     0.000000;0.999968;0.008046;,
     -0.008713;0.999922;0.008929;,
     -0.019765;0.999790;0.005355;,
     0.000000;1.000000;0.000000;,
     0.002566;0.999893;0.014379;,
     -0.002647;0.999748;0.022312;,
     0.001300;0.999782;0.020845;,
     0.002601;0.999861;0.016443;,
     -0.001764;0.999841;0.017734;,
     -0.007135;0.999726;0.022312;,
     -0.013489;0.999810;0.014087;;
     98;
     3;0,2,1;,
     3;0,3,2;,
     3;2,5,4;,
     3;2,3,5;,
     3;5,6,4;,
     3;5,7,6;,
     3;6,9,8;,
     3;6,7,9;,
     3;9,10,8;,
     3;9,11,10;,
     3;10,13,12;,
     3;10,11,13;,
     3;13,14,12;,
     3;13,15,14;,
     3;16,2,17;,
     3;16,1,2;,
     3;2,18,17;,
     3;2,4,18;,
     3;18,6,19;,
     3;18,4,6;,
     3;6,20,19;,
     3;6,8,20;,
     3;20,10,21;,
     3;20,8,10;,
     3;10,22,21;,
     3;10,12,22;,
     3;22,14,23;,
     3;22,12,14;,
     3;16,25,24;,
     3;16,17,25;,
     3;25,18,26;,
     3;25,17,18;,
     3;18,27,26;,
     3;18,19,27;,
     3;27,20,28;,
     3;27,19,20;,
     3;20,29,28;,
     3;20,21,29;,
     3;29,22,30;,
     3;29,21,22;,
     3;22,31,30;,
     3;22,23,31;,
     3;32,25,33;,
     3;32,24,25;,
     3;25,34,33;,
     3;25,26,34;,
     3;34,27,35;,
     3;34,26,27;,
     3;27,36,35;,
     3;27,28,36;,
     3;36,29,37;,
     3;36,28,29;,
     3;29,38,37;,
     3;29,30,38;,
     3;38,31,39;,
     3;38,30,31;,
     3;32,41,40;,
     3;32,33,41;,
     3;41,34,42;,
     3;41,33,34;,
     3;34,43,42;,
     3;34,35,43;,
     3;43,36,44;,
     3;43,35,36;,
     3;36,45,44;,
     3;36,37,45;,
     3;45,38,46;,
     3;45,37,38;,
     3;38,47,46;,
     3;38,39,47;,
     3;48,41,49;,
     3;48,40,41;,
     3;41,50,49;,
     3;41,42,50;,
     3;50,43,51;,
     3;50,42,43;,
     3;43,52,51;,
     3;43,44,52;,
     3;52,45,53;,
     3;52,44,45;,
     3;45,54,53;,
     3;45,46,54;,
     3;54,47,55;,
     3;54,46,47;,
     3;48,57,56;,
     3;48,49,57;,
     3;57,50,58;,
     3;57,49,50;,
     3;50,59,58;,
     3;50,51,59;,
     3;59,52,60;,
     3;59,51,52;,
     3;52,61,60;,
     3;52,53,61;,
     3;61,54,62;,
     3;61,53,54;,
     3;54,63,62;,
     3;54,55,63;;
    }

    MeshTextureCoords {
     64;
     0.000000;0.000000;,
     0.000000;0.142857;,
     0.142857;0.142857;,
     0.142857;0.000000;,
     0.285714;0.142857;,
     0.285714;0.000000;,
     0.428571;0.142857;,
     0.428571;0.000000;,
     0.571429;0.142857;,
     0.571429;0.000000;,
     0.714286;0.142857;,
     0.714286;0.000000;,
     0.857143;0.142857;,
     0.857143;0.000000;,
     1.000000;0.142857;,
     1.000000;0.000000;,
     0.000000;0.285714;,
     0.142857;0.285714;,
     0.285714;0.285714;,
     0.428571;0.285714;,
     0.571429;0.285714;,
     0.714286;0.285714;,
     0.857143;0.285714;,
     1.000000;0.285714;,
     0.000000;0.428571;,
     0.142857;0.428571;,
     0.285714;0.428571;,
     0.428571;0.428571;,
     0.571429;0.428571;,
     0.714286;0.428571;,
     0.857143;0.428571;,
     1.000000;0.428571;,
     0.000000;0.571429;,
     0.142857;0.571429;,
     0.285714;0.571429;,
     0.428571;0.571429;,
     0.571429;0.571429;,
     0.714286;0.571429;,
     0.857143;0.571429;,
     1.000000;0.571429;,
     0.000000;0.714286;,
     0.142857;0.714286;,
     0.285714;0.714286;,
     0.428571;0.714286;,
     0.571429;0.714286;,
     0.714286;0.714286;,
     0.857143;0.714286;,
     1.000000;0.714286;,
     0.000000;0.857143;,
     0.142857;0.857143;,
     0.285714;0.857143;,
     0.428571;0.857143;,
     0.571429;0.857143;,
     0.714286;0.857143;,
     0.857143;0.857143;,
     1.000000;0.857143;,
     0.000000;1.000000;,
     0.142857;1.000000;,
     0.285714;1.000000;,
     0.428571;1.000000;,
     0.571429;1.000000;,
     0.714286;1.000000;,
     0.857143;1.000000;,
     1.000000;1.000000;;
    }

    VertexDuplicationIndices {
     64;
     64;
     0,
     1,
     2,
     3,
     4,
     5,
     6,
     7,
     8,
     9,
     10,
     11,
     12,
     13,
     14,
     15,
     16,
     17,
     18,
     19,
     20,
     21,
     22,
     23,
     24,
     25,
     26,
     27,
     28,
     29,
     30,
     31,
     32,
     33,
     34,
     35,
     36,
     37,
     38,
     39,
     40,
     41,
     42,
     43,
     44,
     45,
     46,
     47,
     48,
     49,
     50,
     51,
     52,
     53,
     54,
     55,
     56,
     57,
     58,
     59,
     60,
     61,
     62,
     63;
    }

    MeshMaterialList {
     1;
     98;
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0;

     Material Material01 {
      0.800000;0.800000;0.800000;1.000000;;
      0.000000;
      0.000000;0.000000;0.000000;;
      0.000000;0.000000;0.000000;;
      TextureFilename {
       "grass_floor.jpg";
      }
     }
    }
 }
 }
}
