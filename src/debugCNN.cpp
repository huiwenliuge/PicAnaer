#include "CNNet.hpp"

int main(void){
    //int M[1]={1};
    //int N[1]={1};
    CNNet cnn("/",10);
    //cnn.crtParmbyRandom();
    cnn.prtParm2txt("./Dictionary/PARM2");
    // cnn.ForwardAlgorithm_Nthread(3);
    // cnn.prtSque("src/debug5");
    // cnn.ForwardAlgorithm();
    // cnn.prtSque("src/debug8");
    for(int n=0;n<1;++n){
        cnn.ForwardAlgorithm_Nthread(3);
        cnn.BackwardAlgorithm_Nthread(0.001,3);
        //std::cout<<"1"<<std::endl;
    }
    cnn.ForwardAlgorithm_Nthread(3);
    cnn.prtSque("src/debug4");

    // char* areapath = (char*)calloc(sizeof(char),101);

    // for(int i=0;i<10;++i){
    //     sprintf(areapath,"%s%d%c","/AREA",i,'\0');
    //     CNNet *Cnn = new CNNet(areapath,10);


    //     delete Cnn;
    // }

}