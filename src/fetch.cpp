#include <iostream>
#include <fstream>

int main(void)
{
    FILE* fphzk = NULL;
    int i, j, k, offset;
    int flag;
    unsigned char buffer[32];
    //unsigned char word[3] = "我";
    //unsigned char key[8] = {
    //    0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01
    //};

    fphzk = fopen("HZK16", "rb");
    if(fphzk == NULL){
        fprintf(stderr, "error hzk16\n");
        return 1;
    }
    //offset = (94*(unsigned int)(word[0]-0xa0-1)+(word[1]-0xa0-1))*32;
    fseek(fphzk, 256, SEEK_SET);
    fread(buffer, 1, 32, fphzk);
    for(k=0; k<32; k++){
        printf("%02X \n", buffer[k]);
    }
    for(k=0; k<16; k++){
        for(j=0; j<2; j++){
            for(i=0; i<8; i++){
                flag = buffer[k*2+j];
                printf("%s", flag?"●":"○");
            }
        }
        printf("\n");
    }
    fclose(fphzk);
    fphzk = NULL;
    return 0;
}