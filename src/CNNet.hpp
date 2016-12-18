#ifndef CNN_INC
#define CNN_INC

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <mutex>
#include <thread>

#define indim 24 //*24
#define coredim 5 //*5
#define mdim 20 //...
#define pooldim 2
#define ldim 10
#define fcnum 10

typedef double** mamat;

// struct hidlay{
//     mamat* MLayer;//n*12*12
//     mamat* Mask;  //n*12*12
//     mamat* LLayer;//m*3*3
//     mamat* DLDL; //m*3*3
//     mamat* DLDM; //n*12*12
// };
// typedef struct hidlay* HidLay;

// struct parm{
//     mamat* ccore;//n*5*5
//     mamat* m2fc; //m*3*3
// };
// typedef struct parm* Parm;

struct Squence{

    long num;
    mamat InLayer;//24*24
    mamat* MLayer;//n*20*20
    mamat* Mask;  //n*20*20
    mamat* LLayer;//m*10*10
    double* FullConn;//10
    double* NorLayer;//10
    double* stdAns;
    double loss;
    //HidLay* neuron;
    double* DLDf;//10
    mamat* DLDL; //m*10*10
    mamat* DLDM; //n*20*20
    struct Squence* next;
};
typedef struct Squence* squence;

// inline double triroot(double x){
//     return pow(x,1.0/3.0);
// }

// inline double dtriroot(double x){
//     return pow(x,-2.0/3.0)/3.0;
// }

inline double ReLU(double x){
    return 0.0<x ? x:0.0;
}

inline double dReLU(double x){
    return 0.0<x ? 1.0:0.0;
}

inline mamat allocmamat(int dim){
    mamat ret = (mamat)malloc(sizeof(double*)*dim);
    for(int i=0;i<dim;++i){
        ret[i]=(double*)calloc(sizeof(double),dim);
    }
    return ret;
}

inline void freemamat(mamat mat,int dim){
    for(int i=0;i<dim;++i){
        free(mat[i]);
    }
    free(mat);
}

inline mamat Conv(mamat convcore,mamat inmat,int convdim,int matdim){
    int mldim = matdim - convdim + 1;
    mamat mlayer = allocmamat(mldim);

    int i,j;
    int m,n;
    for(i=0;i<mldim;++i){
        for(j=0;j<mldim;++j){
            //conv M[i][j]
            for(m=0;m<convdim;++m){
                for(n=0;n<convdim;++n){
                    mlayer[i][j]+=inmat[i+m][j+n]*convcore[m][n];
                }
            }
        }
    }
    return mlayer;
}

inline mamat* Mask_LL(mamat mlayer,int mldim,int poolingdim){
    mamat* ret = (mamat*)malloc(sizeof(mamat*)*2);
    //ret[0] Mask
    //ret[1] LLayer
    ret[0] = allocmamat(mldim);
    int lldim = mldim/poolingdim;
    ret[1] = allocmamat(lldim);
    int i,j;
    int m,n;
    double tmp;
    int mp,np;
    for(i=0;i<lldim;++i){
        for(j=0;j<lldim;++j){
            //LLayer[i][j]
            //MaxPooling
            mp = poolingdim*i;
            np = poolingdim*j;
            tmp = mlayer[mp][np];
            m = mp;
            for(;m<poolingdim*i+poolingdim;++m){
                for(n=poolingdim*j;n<poolingdim*j+poolingdim;++n){
                    if(mlayer[m][n]>tmp){
                        tmp=mlayer[m][n];
                        mp=m;
                        np=n;
                    }
                }
            }
            //active by ReLU
            ret[1][i][j]=ReLU(tmp);
            ret[0][mp][np]=1.0;
        }
    }
    return ret;
}

class CNNet{

    public:
        CNNet(const char* pathroot,int m);//,int* dimn,int* dimm,int depth);
        ~CNNet(){
        //    FreeAll();
        }

        void crtParmbyRandom();
        void prtParm2txt(const char* path);
        void prtSque(const char* path);
        void ForwardAlgorithm();
        void ForwardAlgorithm_Nthread(int n);
        void BackwardAlgorithm(double lnrt);

    private:
        mamat* ccore;//n*5*5
        mamat** m2fc; //m*fcnum*3*3

        mamat* dccore;
        mamat** dm2fc;
        //Parm paramter;
        long totnum;
        long flag;
        int Mnum;
        squence sque;
        void FetchParm(const char* filepath);
        void FetchSque(const char* pathroot);
        void FreeAll();
        squence crtSque(long num);
        double learningrate;
        void _ForwardAlgorithm(squence squeunit);
        void _BackwardAlgorithm(squence squeunit);

        static void _ForwardAlgorithm_SingleTask(CNNet* cnn,int n);
};

#endif