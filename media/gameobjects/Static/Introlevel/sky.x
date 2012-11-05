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

  Frame Sphere02 {

   FrameTransformMatrix {
    1.000000, 0.000000, 0.000000, 0.000000,
    0.000000, 1.000000, 0.000000, 0.000000,
    0.000000, 0.000000, 1.000000, 0.000000,
    0.000000, 0.000000, 0.000000, 1.000000;;
   }

   Mesh {
    89;
    235.125870;-470.251923;-135.750000;,
    271.499969;-470.251923;-0.000006;,
    -0.000003;-543.000122;-0.000006;,
    271.500000;470.251678;-0.000006;,
    235.125885;470.251678;-135.750015;,
    -0.000003;542.999878;-0.000006;,
    135.750000;-470.251923;-235.125916;,
    -0.000003;-543.000122;-0.000006;,
    135.750015;470.251678;-235.125946;,
    -0.000003;542.999878;-0.000006;,
    -0.000003;-470.251923;-271.500031;,
    -0.000003;-543.000122;-0.000006;,
    -0.000003;470.251678;-271.500061;,
    -0.000003;542.999878;-0.000006;,
    -135.750000;-470.251923;-235.125916;,
    -0.000003;-543.000122;-0.000006;,
    -135.750015;470.251678;-235.125946;,
    -0.000003;542.999878;-0.000006;,
    -235.125916;-470.251923;-135.750000;,
    -0.000003;-543.000122;-0.000006;,
    -235.125946;470.251678;-135.750015;,
    -0.000003;542.999878;-0.000006;,
    -271.500031;-470.251923;-0.000006;,
    -0.000003;-543.000122;-0.000006;,
    -271.500061;470.251678;-0.000006;,
    -0.000003;542.999878;-0.000006;,
    -235.125916;-470.251923;135.750000;,
    -0.000003;-543.000122;-0.000006;,
    -235.125946;470.251678;135.750015;,
    -0.000003;542.999878;-0.000006;,
    -135.750000;-470.251923;235.125870;,
    -0.000003;-543.000122;-0.000006;,
    -135.750015;470.251678;235.125885;,
    -0.000003;542.999878;-0.000006;,
    -0.000003;-470.251923;271.499969;,
    -0.000003;-543.000122;-0.000006;,
    -0.000003;470.251678;271.500000;,
    -0.000003;542.999878;-0.000006;,
    135.750000;-470.251923;235.125870;,
    -0.000003;-543.000122;-0.000006;,
    135.750015;470.251678;235.125885;,
    -0.000003;542.999878;-0.000006;,
    235.125870;-470.251923;135.750000;,
    -0.000003;-543.000122;-0.000006;,
    235.125885;470.251678;135.750015;,
    -0.000003;542.999878;-0.000006;,
    271.499969;-470.251923;-0.000006;,
    -0.000003;-543.000122;-0.000006;,
    271.500000;470.251678;-0.000006;,
    -0.000003;542.999878;-0.000006;,
    407.250000;-271.500031;-235.125946;,
    470.251831;-271.500031;-0.000006;,
    235.125885;-271.500031;-407.250000;,
    -0.000003;-271.500031;-470.251831;,
    -235.125946;-271.500031;-407.250000;,
    -407.250000;-271.500031;-235.125946;,
    -470.251831;-271.500031;-0.000006;,
    -407.250000;-271.500031;235.125885;,
    -235.125946;-271.500031;407.250000;,
    -0.000003;-271.500031;470.251831;,
    235.125885;-271.500031;407.250000;,
    407.250000;-271.500031;235.125885;,
    470.251831;-271.500031;-0.000006;,
    543.000000;-0.000033;-0.000006;,
    470.251770;-0.000033;-271.500031;,
    271.499969;-0.000033;-470.251770;,
    -0.000003;-0.000033;-543.000000;,
    -271.500031;-0.000033;-470.251770;,
    -470.251770;-0.000033;-271.500031;,
    -543.000000;-0.000033;-0.000006;,
    -470.251770;-0.000033;271.499969;,
    -271.500031;-0.000033;470.251770;,
    -0.000003;-0.000033;543.000000;,
    271.499969;-0.000033;470.251770;,
    470.251770;-0.000033;271.499969;,
    543.000000;-0.000033;-0.000006;,
    407.250000;271.499969;-235.125916;,
    470.251770;271.499969;-0.000006;,
    235.125870;271.499969;-407.250000;,
    -0.000003;271.499969;-470.251770;,
    -235.125916;271.499969;-407.250000;,
    -407.250000;271.499969;-235.125916;,
    -470.251770;271.499969;-0.000006;,
    -407.250000;271.499969;235.125870;,
    -235.125916;271.499969;407.250000;,
    -0.000003;271.499969;470.251770;,
    235.125870;271.499969;407.250000;,
    407.250000;271.499969;235.125870;,
    470.251770;271.499969;-0.000006;;
    120;
    3;0,2,1;,
    3;3,5,4;,
    3;6,7,0;,
    3;4,9,8;,
    3;10,11,6;,
    3;8,13,12;,
    3;14,15,10;,
    3;12,17,16;,
    3;18,19,14;,
    3;16,21,20;,
    3;22,23,18;,
    3;20,25,24;,
    3;26,27,22;,
    3;24,29,28;,
    3;30,31,26;,
    3;28,33,32;,
    3;34,35,30;,
    3;32,37,36;,
    3;38,39,34;,
    3;36,41,40;,
    3;42,43,38;,
    3;40,45,44;,
    3;46,47,42;,
    3;44,49,48;,
    3;50,1,51;,
    3;0,1,50;,
    3;6,0,50;,
    3;6,50,52;,
    3;53,6,52;,
    3;10,6,53;,
    3;14,10,53;,
    3;14,53,54;,
    3;55,14,54;,
    3;18,14,55;,
    3;22,18,55;,
    3;22,55,56;,
    3;57,22,56;,
    3;26,22,57;,
    3;30,26,57;,
    3;30,57,58;,
    3;59,30,58;,
    3;34,30,59;,
    3;38,34,59;,
    3;38,59,60;,
    3;61,38,60;,
    3;42,38,61;,
    3;46,42,61;,
    3;46,61,62;,
    3;50,51,63;,
    3;50,63,64;,
    3;65,50,64;,
    3;52,50,65;,
    3;53,52,65;,
    3;53,65,66;,
    3;67,53,66;,
    3;54,53,67;,
    3;55,54,67;,
    3;55,67,68;,
    3;69,55,68;,
    3;56,55,69;,
    3;57,56,69;,
    3;57,69,70;,
    3;71,57,70;,
    3;58,57,71;,
    3;59,58,71;,
    3;59,71,72;,
    3;73,59,72;,
    3;60,59,73;,
    3;61,60,73;,
    3;61,73,74;,
    3;75,61,74;,
    3;62,61,75;,
    3;76,63,77;,
    3;64,63,76;,
    3;65,64,76;,
    3;65,76,78;,
    3;79,65,78;,
    3;66,65,79;,
    3;67,66,79;,
    3;67,79,80;,
    3;81,67,80;,
    3;68,67,81;,
    3;69,68,81;,
    3;69,81,82;,
    3;83,69,82;,
    3;70,69,83;,
    3;71,70,83;,
    3;71,83,84;,
    3;85,71,84;,
    3;72,71,85;,
    3;73,72,85;,
    3;73,85,86;,
    3;87,73,86;,
    3;74,73,87;,
    3;75,74,87;,
    3;75,87,88;,
    3;76,77,3;,
    3;76,3,4;,
    3;8,76,4;,
    3;78,76,8;,
    3;79,78,8;,
    3;79,8,12;,
    3;16,79,12;,
    3;80,79,16;,
    3;81,80,16;,
    3;81,16,20;,
    3;24,81,20;,
    3;82,81,24;,
    3;83,82,24;,
    3;83,24,28;,
    3;32,83,28;,
    3;84,83,32;,
    3;85,84,32;,
    3;85,32,36;,
    3;40,85,36;,
    3;86,85,40;,
    3;87,86,40;,
    3;87,40,44;,
    3;48,87,44;,
    3;88,87,48;;

    MeshNormals {
     89;
     -0.431481;0.867044;0.249116;,
     -0.573584;0.819147;-0.000000;,
     -0.000000;1.000000;-0.000000;,
     -0.573583;-0.819147;-0.000000;,
     -0.431481;-0.867044;0.249116;,
     -0.000000;-1.000000;-0.000000;,
     -0.286792;0.819147;0.496738;,
     -0.000000;1.000000;-0.000000;,
     -0.286792;-0.819147;0.496738;,
     -0.000000;-1.000000;-0.000000;,
     0.000000;0.867044;0.498231;,
     -0.000000;1.000000;-0.000000;,
     0.000000;-0.867044;0.498231;,
     -0.000000;-1.000000;-0.000000;,
     0.286792;0.819147;0.496738;,
     -0.000000;1.000000;-0.000000;,
     0.286792;-0.819147;0.496738;,
     -0.000000;-1.000000;-0.000000;,
     0.431481;0.867044;0.249116;,
     -0.000000;1.000000;-0.000000;,
     0.431481;-0.867044;0.249116;,
     -0.000000;-1.000000;-0.000000;,
     0.573584;0.819147;-0.000000;,
     -0.000000;1.000000;-0.000000;,
     0.573584;-0.819147;-0.000000;,
     -0.000000;-1.000000;-0.000000;,
     0.431481;0.867044;-0.249116;,
     -0.000000;1.000000;-0.000000;,
     0.431481;-0.867044;-0.249116;,
     -0.000000;-1.000000;-0.000000;,
     0.286792;0.819147;-0.496738;,
     -0.000000;1.000000;-0.000000;,
     0.286792;-0.819147;-0.496738;,
     -0.000000;-1.000000;-0.000000;,
     0.000000;0.867044;-0.498231;,
     -0.000000;1.000000;-0.000000;,
     0.000000;-0.867044;-0.498231;,
     -0.000000;-1.000000;-0.000000;,
     -0.286792;0.819147;-0.496738;,
     -0.000000;1.000000;-0.000000;,
     -0.286792;-0.819147;-0.496738;,
     -0.000000;-1.000000;-0.000000;,
     -0.431481;0.867044;-0.249116;,
     -0.000000;1.000000;-0.000000;,
     -0.431481;-0.867044;-0.249116;,
     -0.000000;-1.000000;-0.000000;,
     -0.573584;0.819147;-0.000000;,
     -0.000000;1.000000;-0.000000;,
     -0.573583;-0.819147;-0.000000;,
     -0.000000;-1.000000;-0.000000;,
     -0.749141;0.501714;0.432517;,
     -0.865033;0.501714;-0.000000;,
     -0.432517;0.501714;0.749141;,
     -0.000000;0.501714;0.865033;,
     0.432517;0.501714;0.749141;,
     0.749141;0.501714;0.432517;,
     0.865033;0.501714;-0.000000;,
     0.749141;0.501714;-0.432517;,
     0.432517;0.501714;-0.749141;,
     -0.000000;0.501714;-0.865033;,
     -0.432517;0.501714;-0.749141;,
     -0.749141;0.501714;-0.432517;,
     -0.865033;0.501714;-0.000000;,
     -1.000000;-0.000000;-0.000000;,
     -0.866025;-0.000000;0.500000;,
     -0.500000;-0.000000;0.866025;,
     -0.000000;-0.000000;1.000000;,
     0.500000;-0.000000;0.866025;,
     0.866025;-0.000000;0.500000;,
     1.000000;-0.000000;-0.000000;,
     0.866025;-0.000000;-0.500000;,
     0.500000;-0.000000;-0.866025;,
     -0.000000;-0.000000;-1.000000;,
     -0.500000;-0.000000;-0.866025;,
     -0.866025;0.000000;-0.500000;,
     -1.000000;-0.000000;-0.000000;,
     -0.749141;-0.501715;0.432517;,
     -0.865033;-0.501715;-0.000000;,
     -0.432517;-0.501714;0.749141;,
     -0.000000;-0.501714;0.865033;,
     0.432517;-0.501714;0.749141;,
     0.749141;-0.501714;0.432517;,
     0.865033;-0.501714;-0.000000;,
     0.749141;-0.501714;-0.432517;,
     0.432517;-0.501715;-0.749141;,
     -0.000000;-0.501715;-0.865033;,
     -0.432517;-0.501714;-0.749141;,
     -0.749141;-0.501714;-0.432517;,
     -0.865033;-0.501715;-0.000000;;
     120;
     3;0,2,1;,
     3;3,5,4;,
     3;6,7,0;,
     3;4,9,8;,
     3;10,11,6;,
     3;8,13,12;,
     3;14,15,10;,
     3;12,17,16;,
     3;18,19,14;,
     3;16,21,20;,
     3;22,23,18;,
     3;20,25,24;,
     3;26,27,22;,
     3;24,29,28;,
     3;30,31,26;,
     3;28,33,32;,
     3;34,35,30;,
     3;32,37,36;,
     3;38,39,34;,
     3;36,41,40;,
     3;42,43,38;,
     3;40,45,44;,
     3;46,47,42;,
     3;44,49,48;,
     3;50,1,51;,
     3;0,1,50;,
     3;6,0,50;,
     3;6,50,52;,
     3;53,6,52;,
     3;10,6,53;,
     3;14,10,53;,
     3;14,53,54;,
     3;55,14,54;,
     3;18,14,55;,
     3;22,18,55;,
     3;22,55,56;,
     3;57,22,56;,
     3;26,22,57;,
     3;30,26,57;,
     3;30,57,58;,
     3;59,30,58;,
     3;34,30,59;,
     3;38,34,59;,
     3;38,59,60;,
     3;61,38,60;,
     3;42,38,61;,
     3;46,42,61;,
     3;46,61,62;,
     3;50,51,63;,
     3;50,63,64;,
     3;65,50,64;,
     3;52,50,65;,
     3;53,52,65;,
     3;53,65,66;,
     3;67,53,66;,
     3;54,53,67;,
     3;55,54,67;,
     3;55,67,68;,
     3;69,55,68;,
     3;56,55,69;,
     3;57,56,69;,
     3;57,69,70;,
     3;71,57,70;,
     3;58,57,71;,
     3;59,58,71;,
     3;59,71,72;,
     3;73,59,72;,
     3;60,59,73;,
     3;61,60,73;,
     3;61,73,74;,
     3;75,61,74;,
     3;62,61,75;,
     3;76,63,77;,
     3;64,63,76;,
     3;65,64,76;,
     3;65,76,78;,
     3;79,65,78;,
     3;66,65,79;,
     3;67,66,79;,
     3;67,79,80;,
     3;81,67,80;,
     3;68,67,81;,
     3;69,68,81;,
     3;69,81,82;,
     3;83,69,82;,
     3;70,69,83;,
     3;71,70,83;,
     3;71,83,84;,
     3;85,71,84;,
     3;72,71,85;,
     3;73,72,85;,
     3;73,85,86;,
     3;87,73,86;,
     3;74,73,87;,
     3;75,74,87;,
     3;75,87,88;,
     3;76,77,3;,
     3;76,3,4;,
     3;8,76,4;,
     3;78,76,8;,
     3;79,78,8;,
     3;79,8,12;,
     3;16,79,12;,
     3;80,79,16;,
     3;81,80,16;,
     3;81,16,20;,
     3;24,81,20;,
     3;82,81,24;,
     3;83,82,24;,
     3;83,24,28;,
     3;32,83,28;,
     3;84,83,32;,
     3;85,84,32;,
     3;85,32,36;,
     3;40,85,36;,
     3;86,85,40;,
     3;87,86,40;,
     3;87,40,44;,
     3;48,87,44;,
     3;88,87,48;;
    }

    MeshTextureCoords {
     89;
     0.083333;0.833333;,
     0.000000;0.833333;,
     0.000000;1.000000;,
     0.000000;0.166667;,
     0.083333;0.166667;,
     0.000000;0.000000;,
     0.166667;0.833333;,
     0.083333;1.000000;,
     0.166667;0.166667;,
     0.083333;0.000000;,
     0.250000;0.833333;,
     0.166667;1.000000;,
     0.250000;0.166667;,
     0.166667;0.000000;,
     0.333333;0.833333;,
     0.250000;1.000000;,
     0.333333;0.166667;,
     0.250000;0.000000;,
     0.416667;0.833333;,
     0.333333;1.000000;,
     0.416667;0.166667;,
     0.333333;0.000000;,
     0.500000;0.833333;,
     0.416667;1.000000;,
     0.500000;0.166667;,
     0.416667;0.000000;,
     0.583333;0.833333;,
     0.500000;1.000000;,
     0.583333;0.166667;,
     0.500000;0.000000;,
     0.666667;0.833333;,
     0.583333;1.000000;,
     0.666667;0.166667;,
     0.583333;0.000000;,
     0.750000;0.833333;,
     0.666667;1.000000;,
     0.750000;0.166667;,
     0.666667;0.000000;,
     0.833333;0.833333;,
     0.750000;1.000000;,
     0.833333;0.166667;,
     0.750000;0.000000;,
     0.916667;0.833333;,
     0.833333;1.000000;,
     0.916667;0.166667;,
     0.833333;0.000000;,
     1.000000;0.833333;,
     0.916667;1.000000;,
     1.000000;0.166667;,
     0.916667;0.000000;,
     0.083333;0.666667;,
     0.000000;0.666667;,
     0.166667;0.666667;,
     0.250000;0.666667;,
     0.333333;0.666667;,
     0.416667;0.666667;,
     0.500000;0.666667;,
     0.583333;0.666667;,
     0.666667;0.666667;,
     0.750000;0.666667;,
     0.833333;0.666667;,
     0.916667;0.666667;,
     1.000000;0.666667;,
     0.000000;0.500000;,
     0.083333;0.500000;,
     0.166667;0.500000;,
     0.250000;0.500000;,
     0.333333;0.500000;,
     0.416667;0.500000;,
     0.500000;0.500000;,
     0.583333;0.500000;,
     0.666667;0.500000;,
     0.750000;0.500000;,
     0.833333;0.500000;,
     0.916667;0.500000;,
     1.000000;0.500000;,
     0.083333;0.333333;,
     0.000000;0.333333;,
     0.166667;0.333333;,
     0.250000;0.333333;,
     0.333333;0.333333;,
     0.416667;0.333333;,
     0.500000;0.333333;,
     0.583333;0.333333;,
     0.666667;0.333333;,
     0.750000;0.333333;,
     0.833333;0.333333;,
     0.916667;0.333333;,
     1.000000;0.333333;;
    }

    VertexDuplicationIndices {
     89;
     89;
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
     63,
     64,
     65,
     66,
     67,
     68,
     69,
     70,
     71,
     72,
     73,
     74,
     75,
     76,
     77,
     78,
     79,
     80,
     81,
     82,
     83,
     84,
     85,
     86,
     87,
     88;
    }

    MeshMaterialList {
     1;
     120;
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

     Material Material02 {
      0.800000;0.800000;0.800000;1.000000;;
      0.000000;
      0.000000;0.000000;0.000000;;
      0.000000;0.000000;0.000000;;
      TextureFilename {
       "nebo.jpg";
      }
     }
    }

 }
 }
}