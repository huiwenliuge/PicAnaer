#include "CNNet.hpp"
#include <ctime>

CNNet::CNNet(const char* pathroot,int n){//,int* dimn,int* dimm,int depth){

    Mnum=n;
    int i,j;
    sque = NULL;
    ccore = (mamat*)malloc(sizeof(mamat)*Mnum);
    dccore = (mamat*)malloc(sizeof(mamat)*Mnum);
    for(i=0;i<Mnum;++i){
        ccore[i] = allocmamat(coredim);
        dccore[i] = allocmamat(coredim);
    }
    
    m2fc = (mamat**)malloc(sizeof(mamat*)*Mnum);
    dm2fc = (mamat**)malloc(sizeof(mamat*)*Mnum);
    for(i=0;i<Mnum;++i){
        m2fc[i] = (mamat*)malloc(sizeof(mamat)*fcnum);
        dm2fc[i] = (mamat*)malloc(sizeof(mamat)*fcnum);
        for(j=0;j<fcnum;++j){
            m2fc[i][j] = allocmamat(ldim);
            dm2fc[i][j] = allocmamat(ldim);
        }
    }

    const char* baseparm="./Dictionary";
    const char* parm = "PARM";
    const char* basemate="./Material";
    const char* hz = "A";
    char* parmpath = (char*)calloc(sizeof(char),101);
    char* dictpath = (char*)calloc(sizeof(char),101);
    strcpy(parmpath,baseparm);
    strcpy(parmpath+12,pathroot);
    strcpy(parmpath+12+strlen(pathroot),parm);
    strcpy(dictpath,basemate);
    strcpy(dictpath+10,pathroot);
    strcpy(dictpath+10+strlen(pathroot),hz);


    std::cout<<parmpath<<"\n";
    std::cout<<dictpath<<"\n";

    FetchParm(parmpath);
    FetchSque(dictpath);
}

void CNNet::crtParmbyRandom(){
    srand((unsigned int)time(0));
    int n,l,j,k;
    for(n=0;n<Mnum;++n){
        for(j=0;j<coredim;++j){
            for(k=0;k<coredim;++k){
                ccore[n][j][k]=rand()%101-50;
                ccore[n][j][k]/=25.0;
            }
        }
    }
    for(n=0;n<Mnum;++n){
        for(l=0;l<fcnum;++l){
            for(j=0;j<ldim;++j){
                for(k=0;k<ldim;++k){
                    m2fc[n][l][j][k]=rand()%101-50;
                    m2fc[n][l][j][k]/=25.0;
                }
            }
        }
    }
}

void CNNet::prtParm2txt(const char* path){

    FILE* fp = fopen(path,"w");
    fprintf(fp,"MLayer:%d;LLayer:%d\n",Mnum,Mnum);
    int n,l,j,k;
    for(n=0;n<Mnum;++n){
        fprintf(fp,"MLayer_%d\n",n);
        for(j=0;j<coredim;++j){
            for(k=0;k<coredim;++k){
                fprintf(fp,"#%lf#",ccore[n][j][k]);
            }
            fputc('\n',fp);
        }
    }
    for(n=0;n<Mnum;++n){
        for(l=0;l<fcnum;++l){
            fprintf(fp,"LLayer_%d_To_%d\n",n,l);
            for(j=0;j<ldim;++j){
                for(k=0;k<ldim;++k){
                    fprintf(fp,"#%lf#",m2fc[n][l][j][k]);
                }
                fputc('\n',fp);
            }
        }
    }
    fclose(fp);
}

void CNNet::FetchParm(const char* filepath){

    FILE* fp = fopen(filepath,"r");
    int n,m,j,k;
    fscanf(fp,"MLayer:%d;LLayer:%d\n",&n,&m);
    if(n!=Mnum || m!=Mnum)
        printf("Inconsistent Internal Dimensions\n");
    
    char* line = (char*)calloc(sizeof(char),101);
    for(n=0;n<Mnum;++n){
        fscanf(fp,"%s\n",line);
        for(j=0;j<coredim;++j){
            for(k=0;k<coredim;++k){
                fscanf(fp,"#%lf#",ccore[n][j]+k);
            }
            fgetc(fp);
        }
    }

    for(n=0;n<Mnum;++n){
        for(m=0;m<fcnum;++m){
            fscanf(fp,"%s\n",line);
            for(j=0;j<ldim;++j){
                for(k=0;k<ldim;++k){
                    fscanf(fp,"#%lf#",m2fc[n][m][j]+k);
                }
                fgetc(fp);
            }
        }
    }
    fclose(fp);
}

void CNNet::FetchSque(const char* pathroot){

    char* path = (char*)malloc(sizeof(char)*101);
    FILE* fp;
    int i,m,j,k,l;
    int pathrootlen = strlen(pathroot);
    char* line = (char*)malloc(sizeof(char)*101);
    char tmp;
    totnum = 0;
    sque=NULL;
    squence squetmp;
    for(i=0;i<fcnum;++i){
        sprintf(path,"%s%d%c",pathroot,i,'\0');
        printf("%s\n",path);
        fp = fopen(path,"r");
        fscanf(fp,"total_include:%d\n",&m);
        for(j=0;j<m;++j){
            fscanf(fp,"%s\n",line);
            squetmp = crtSque(totnum);
            squetmp->stdAns[i]=1.0;
            for(k=0;k<indim;++k){
                for(l=0;l<indim;++l){
                    fscanf(fp,"%c",&tmp);
                    squetmp->InLayer[k][l]=(double)(tmp-48);
                }
                fgetc(fp);
            }
            squetmp->next=sque;
            sque=squetmp;
            ++totnum;
        }
        fclose(fp);
    }
}

squence CNNet::crtSque(long num){
    squence squeunit = (squence)malloc(sizeof(struct Squence));
    squeunit->num = num;
    squeunit->InLayer = allocmamat(indim);
    squeunit->MLayer = (mamat*)calloc(sizeof(mamat),Mnum);
    squeunit->Mask  = (mamat*)calloc(sizeof(mamat),Mnum);
    squeunit->LLayer = (mamat*)calloc(sizeof(mamat),Mnum);
    squeunit->FullConn = (double*)calloc(sizeof(double),fcnum);
    squeunit->NorLayer = (double*)calloc(sizeof(double),fcnum);
    squeunit->DLDf = (double*)calloc(sizeof(double),fcnum);
    squeunit->stdAns = (double*)calloc(sizeof(double),fcnum);
    squeunit->DLDL = (mamat*)malloc(sizeof(mamat)*Mnum);
    squeunit->DLDM = (mamat*)malloc(sizeof(mamat)*Mnum);


    for(int i=0;i<Mnum;++i){
        squeunit->DLDL[i]=allocmamat(ldim);
        //squeunit->LLayer[i]=NULL;
        //squeunit->Mask[i]=NULL;
        //squeunit->MLayer[i]=NULL;
        squeunit->DLDM[i]=allocmamat(mdim);
    }

    return squeunit;
}

void CNNet::prtSque(const char* path){

    FILE* fp = fopen(path,"w");

    squence squetmp = sque;

    int i,j,k;
    while(squetmp){
        fprintf(fp,"No:%ld__InVec:{\n",squetmp->num);
        for(i=0;i<indim;++i){
            for(j=0;j<indim;++j){
                fprintf(fp,"%d",(int)squetmp->InLayer[i][j]);
            }
            fputc('\n',fp);
        }
        fprintf(fp,"}\nMLayer_%d_total:{\n",Mnum);
        for(i=0;i<Mnum;++i){
            fprintf(fp,"MLayer__%d[\n",i);
            for(j=0;j<mdim;++j){
                for(k=0;k<mdim;++k){
                    fprintf(fp,"%lf,",squetmp->MLayer[i][j][k]);
                }
                fputc('\n',fp);
            }
            fprintf(fp,"]\n");
        }
        fprintf(fp,"}\nMask_%d_total:{\n",Mnum);
        for(i=0;i<Mnum;++i){
            fprintf(fp,"Mask__%d[\n",i);
            for(j=0;j<mdim;++j){
                for(k=0;k<mdim;++k){
                    fprintf(fp,"%lf,",squetmp->Mask[i][j][k]);
                }
                fputc('\n',fp);
            }
            fprintf(fp,"]\n");
        }
        fprintf(fp,"}\nLLayer_%d_total:{\n",Mnum);
        for(i=0;i<Mnum;++i){
            fprintf(fp,"LLayer__%d[\n",i);
            for(j=0;j<ldim;++j){
                for(k=0;k<ldim;++k){
                    fprintf(fp,"%lf,",squetmp->LLayer[i][j][k]);
                }
                fputc('\n',fp);
            }
            fprintf(fp,"]\n");
        }
        fprintf(fp,"}\nFullConn:{\n");
        for(i=0;i<fcnum;++i){
            fprintf(fp,"%lf,",squetmp->FullConn[i]);
        }
        fprintf(fp,"}\nNorLayer:{\n");
        for(i=0;i<fcnum;++i){
            fprintf(fp,"%lf,",squetmp->NorLayer[i]);
        }
        fprintf(fp,"}\nstdAns:{\n");
        for(i=0;i<fcnum;++i){
            fprintf(fp,"%lf,",squetmp->stdAns[i]);
        }
        fprintf(fp,"}\nloss:%lf\n",squetmp->loss);
        squetmp = squetmp->next;
    }
    fclose(fp);
}

void CNNet::ForwardAlgorithm(){
    squence stmp = sque;
    while(stmp){
        _ForwardAlgorithm(stmp);
        stmp=stmp->next;
    }
}

void CNNet::ForwardAlgorithm_Nthread(int Nthread){
    squence stmp = sque;
    flag = totnum;
    std::thread th[Nthread];
    int n;
    for(n=0;n<Nthread;++n){
        th[n]=std::thread(_ForwardAlgorithm_SingleTask,\
            this,n);
    }
    for(n=0;n<Nthread;++n)
        th[n].join();

}

void CNNet::BackwardAlgorithm(double lnrt){
    learningrate = lnrt;
    int m,n,i,j;
    //clear
    for(m=0;m<Mnum;++m){
        for(i=0;i<coredim;++i){
            memset(dccore[m][i],'\0',sizeof(double)*coredim);
        }
        for(i=0;i<fcnum;++i){
            for(j=0;j<ldim;++j){
                memset(dm2fc[m][i][j],'\0',sizeof(double)*ldim);
            }
        }
    }

    squence stmp = sque;
    while(stmp){
        //std::cout<<stmp->num<<'\n';
        _BackwardAlgorithm(stmp);
        stmp=stmp->next;
    }

    for(m=0;m<Mnum;++m){
        for(i=0;i<coredim;++i){
            for(j=0;j<coredim;++j){
                ccore[m][i][j]-=dccore[m][i][j]*learningrate;
            }
        }
        for(n=0;n<fcnum;++n){
            for(i=0;i<ldim;++i){
                for(j=0;j<ldim;++j){
                    m2fc[m][n][i][j]-=dm2fc[m][n][i][j]*\
                    learningrate;
                }
            }
        }
    }

}

void CNNet::BackwardAlgorithm_Nthread(double lnrt,int Nthread){
    learningrate = lnrt;
    int m,n,i,j;
    for(m=0;m<Mnum;++m){
        for(i=0;i<coredim;++i){
            memset(dccore[m][i],'\0',sizeof(double)*coredim);
        }
        for(i=0;i<fcnum;++i){
            for(j=0;j<ldim;++j){
                memset(dm2fc[m][i][j],'\0',sizeof(double)*ldim);
            }
        }
    }

    flag = totnum;
    //std::cout<<"flag"<<flag<<'\n';
    std::thread th[2];

    for(n=0;n<Nthread;++n){
        th[n]=std::thread(_BackwardAlgorithm_SingleTask,\
            this,0);
    }

    for(n=0;n<Nthread;++n)
        th[n].join();

    for(m=0;m<Mnum;++m){
        for(i=0;i<coredim;++i){
            for(j=0;j<coredim;++j){
                ccore[m][i][j]-=dccore[m][i][j]*learningrate;
            }
        }
        for(n=0;n<fcnum;++n){
            for(i=0;i<ldim;++i){
                for(j=0;j<ldim;++j){
                    m2fc[m][n][i][j]-=dm2fc[m][n][i][j]*\
                    learningrate;
                }
            }
        }
    }
}

void CNNet::_ForwardAlgorithm(squence squeunit){

    int m,i,j;
    int out;
    //conv_layer
    mamat *tmp;
    for(m=0;m<Mnum;++m){
        if(squeunit->MLayer[m]!=NULL){
            freemamat(squeunit->MLayer[m],mdim);
        }
        //conv
        squeunit->MLayer[m]=Conv(ccore[m],\
                            squeunit->InLayer,\
                            coredim,indim);
        //pooling
        tmp = Mask_LL(squeunit->MLayer[m],\
                    mdim,pooldim);
        if(squeunit->Mask[m]!=NULL)
            freemamat(squeunit->Mask[m],mdim);
        if(squeunit->LLayer[m]!=NULL)
            freemamat(squeunit->LLayer[m],ldim);
        squeunit->Mask[m] = tmp[0];
        squeunit->LLayer[m] = tmp[1];
    }
    free(tmp);
    //Full Connection
    for(out=0;out<fcnum;++out){
        squeunit->FullConn[out]=0.0;
        for(m=0;m<Mnum;++m){
            for(i=0;i<ldim;++i){
                for(j=0;j<ldim;++j){
                    squeunit->FullConn[out]+=\
                    m2fc[m][out][i][j]*squeunit->LLayer[m][i][j];
                }
            }
        }
    }

    //softmax
    double normalizor = 0.0;
    for(out=0;out<fcnum;++out){
        normalizor += exp(squeunit->FullConn[out]);
    }
    squeunit->loss = 0.0;
    //std::cout<<normalizor<<std::endl;
    for(out=0;out<fcnum;++out){
        squeunit->NorLayer[out]=\
        exp(squeunit->FullConn[out])/normalizor;
        //std::cout<<exp(squeunit->FullConn[out])/normalizor<<std::endl;
        //loss function
        squeunit->loss+=\
        -squeunit->stdAns[out]*log(squeunit->NorLayer[out]);
    }

}

void CNNet::_BackwardAlgorithm(squence squeunit){

    int i,j;
    int m,n;
    int out;
    //Dloss./Dy
    for(out=0;out<fcnum;++out){
        squeunit->DLDf[out]=-(squeunit->stdAns[out]\
                            -squeunit->NorLayer[out]);
    }

    //Dloss./Dllayer
    for(out=0;out<Mnum;++out){
        for(i=0;i<ldim;++i){
            for(j=0;j<ldim;++j){
                squeunit->DLDL[out][i][j]=0.0;
                for(m=0;m<fcnum;++m){
                    squeunit->DLDL[out][i][j]+=\
                    squeunit->DLDf[m]*m2fc[out][m][i][j];
                }
            }
        }
    }
    //int out2;
    int s1,s2;
    //Dloss./Dmlayer
    for(out=0;out<Mnum;++out){
        for(i=0;i<ldim;++i){
            for(j=0;j<ldim;++j){
                s1=i*pooldim;
                s2=j*pooldim;
                for(m=s1;m<s1+pooldim;++m){
                    for(n=s2;n<s2+pooldim;++n){
                        squeunit->DLDM[out][m][n]=\
                        squeunit->DLDL[out][i][j]*\
                        squeunit->Mask[out][m][n]*\
                        dReLU(squeunit->MLayer[out][m][n]);
                    }
                }
            }
        }
    }

    //
    // Update 
    static std::mutex gmutex;

    for(out=0;out<Mnum;++out){
        for(i=0;i<fcnum;++i){
            for(m=0;m<ldim;++m){
                for(n=0;n<ldim;++n){
                    gmutex.lock();
                    dm2fc[out][i][m][n]+=\
                    squeunit->DLDf[i]*\
                    squeunit->LLayer[out][m][n];
                    gmutex.unlock();
                }
            }
        }
    }

    for(out=0;out<Mnum;++out){ 
        for(i=0;i<mdim;++i){
            for(j=0;j<mdim;++j){
                //
                for(m=0;m<coredim;++m){
                    for(n=0;n<coredim;++n){
                        gmutex.lock();
                        dccore[out][m][n]+=\
                        squeunit->DLDM[out][i][j]*\
                        squeunit->InLayer[i+m][j+n];
                        gmutex.unlock();
                    }
                }

            }
        }
    }

}

squence gonext(squence squ,long num){
    squence ret = squ;
    if(num < 0) return NULL;
    while(ret->num > num){
        ret=ret->next;
    }
    return ret;
}

void CNNet::_ForwardAlgorithm_SingleTask(CNNet* cnn,int n){
    long num;
    squence tsqu = cnn->sque;
    num = tsqu->num;
    static std::mutex gmutex;
    while(cnn->flag > 0){
        gmutex.lock();
        cnn->flag--;
        num = cnn->flag;
        gmutex.unlock();
        tsqu = gonext(tsqu,num);
        if(tsqu!=NULL){
            cnn->_ForwardAlgorithm(tsqu);
        }
    }
}

void CNNet::_BackwardAlgorithm_SingleTask(CNNet* cnn,int n){

    long num;
    squence tsqu = cnn->sque;
    num = tsqu->num;
    static std::mutex gmutex;

    while(cnn->flag > 0){
        gmutex.lock();
        cnn->flag--;
        num = cnn->flag;
        //std::cout<<num<<'\n';
        gmutex.unlock();
        tsqu = gonext(tsqu,num);
        if(tsqu!=NULL){
            cnn->_BackwardAlgorithm(tsqu);
        }

    }

}
