//################################################
//                for h052 
//################################################
//Arrays for mapping things...
//With new mapping...1.15..starts with 1010 now...
const Int_t idConst = 1010; //Temp value to get idDet
                      // 0    1    2    3    4    5    6    7    8    9
Int_t idDetMap[160] = {  7,   6,  11,  10,   9,   8,   7,   6,  -1,  -1, /*4*/
                        15,  14,  13,  12,  11,  10,   9,   8,  -1,  -1, /*5*/
                        17,  16,  15,  14,  13,  12,  17,  16,  -1,  -1, /*6*/
                       101, 102, 103, 104, 105, 106, 107, 108,  -1,  -1, //Recoils
                         1,   0,   5,   4,   3,   2,   1,   0,  -1,  -1, /*1*/
                         3,   2,   1,   0,   5,   4,   3,   2,  -1,  -1, /*2*/
                        11,  10,   9,   8,   7,   6,   5,   4,  -1,  -1, /*3*/
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, //Empty
                       200, 201, 202, 203, 204, 205, 206, 207,  -1,  -1, //Empty
                       208, 209, 210, 211, 212, 213, 214, 215,  -1,  -1, //Empty
                       216, 217, 218, 219, 220, 221, 222, 223,  -1,  -1, //Empty
                       224, 225, 226, 227, 228, 229, 230, 231,  -1,  -1, //Empty
                        19,  18,  17,  16,  15,  14,  13,  12,  -1,  -1, /*7*/
                        21,  20,  19,  18,  23,  22,  21,  20,  -1,  -1, /*8*/
                        23,  22,  21,  20,  19,  18,  23,  22,  -1,  -1, /*9*/      
                        -1,  -1,  -1, 300, 301, 302,  -1, 303, 304, 305};///RF TIMING STUFF

                      // 0   1   2   3   4   5   6   7   8   9
Int_t idKindMap[160] = { 2,  2,  1,  1,  1,  1,  1,  1, -1, -1,//4
                         0,  0,  0,  0,  2,  2,  2,  2, -1, -1,//5
                         1,  1,  1,  1,  1,  1,  0,  0, -1, -1,//6
                         0,  1,  2,  3,  4,  5,  6,  7, -1, -1,//Recoils
                         1,  1,  0,  0,  0,  0,  0,  0, -1, -1,//1
                         2,  2,  2,  2,  1,  1,  1,  1, -1, -1,//2
                         0,  0,  0,  0,  0,  0,  2,  2, -1, -1,//3
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                         0,  0,  2,  2,  2,  2,  2,  2, -1, -1,//7
                         1,  1,  1,  1,  0,  0,  0,  0, -1, -1,//8
                         2,  2,  2,  2,  2,  2,  1,  1, -1, -1,//9
                        -1, -1, -1,  0,  1,  2, -1,  3,  4,  5};//RF TIMING STUFF
                         //-3RT1,-4RT2,-5MT,-6MT,-8TAC,-9MT