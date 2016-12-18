#include "CNNet.hpp"

int main(void){
    //int M[1]={1};
    //int N[1]={1};
    CNNet cnn("./",1);
    //cnn.crtParmbyRandom();
    //cnn.prtParm2txt("debug2");
    cnn.ForwardAlgorithm_Nthread(3);
    cnn.prtSque("debug5");
    for(int n=0;n<10;++n){
        cnn.ForwardAlgorithm_Nthread(3);
        cnn.BackwardAlgorithm(0.001);
        //std::cout<<"1"<<std::endl;
    }
    cnn.ForwardAlgorithm_Nthread(3);
    cnn.prtSque("debug4");
}