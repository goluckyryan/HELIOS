//################################################
//                for h057 
//################################################
//Arrays for mapping things...
//With new mapping...1.15..starts with 1010 now...
const Int_t idConst = 1010; //Temp value to get idDet
                      // 0    1    2    3    4    5    6    7    8    9
Int_t idDetMap[160] = {300, 301, 400,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//RF TIMING STUFF
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//Empty
                       101, 102, 103, 104, 105, 106, 107, 108,  -1,  -1,//Recoils
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//Empty
                         1,   0,   5,   4,   3,   2,   1,   0,  -1,  -1,/*1*/
                         3,   2,   1,   0,   5,   4,   3,   2,  -1,  -1,/*2*/
                        11,  10,   9,   8,   7,   6,   5,   4,  -1,  -1,/*3*/
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//Empty
                         7,   6,  11,  10,   9,   8,   7,   6,  -1,  -1,/*4*/
                        15,  14,  13,  12,  11,  10,   9,   8,  -1,  -1,/*5*/
                        17,  16,  15,  14,  13,  12,  17,  16,  -1,  -1,/*6*/
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,//Empty
                        19,  18,  17,  16,  15,  14,  13,  12,  -1,  -1,/*7*/
                        21,  20,  19,  18,  23,  22,  21,  20,  -1,  -1,/*8*/
                        23,  22,  21,  20,  19,  18,  23,  22,  -1,  -1,/*9*/	  
                        -1,  -2,  -3,  -4,  -5,  -6,  -7,  -8,  -9, -10};///

                      // 0    1    2    3    4    5    6    7    8    9
Int_t idKindMap[160] = {-1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                         1,   1,   0,   0,   0,   0,   0,   0,  -1,  -1,//1
                         2,   2,   2,   2,   1,   1,   1,   1,  -1,  -1,//2
                         0,   0,   0,   0,   0,   0,   2,   2,  -1,  -1,//3
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                         2,   2,   1,   1,   1,   1,   1,   1,  -1,  -1,//4
                         0,   0,   0,   0,   2,   2,   2,   2,  -1,  -1,//5
                         1,   1,   1,   1,   1,   1,   0,   0,  -1,  -1,//6
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
                         0,   0,   2,   2,   2,   2,   2,   2,  -1,  -1,//7
                         1,   1,   1,   1,   0,   0,   0,   0,  -1,  -1,//8
                         2,   2,   2,   2,   2,   2,   1,   1,  -1,  -1,//9
                        -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1};
