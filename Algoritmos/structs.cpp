#include <opencv2/opencv.hpp>
using namespace cv;
#define SCALE 0.5
#define SWIDTH floor(192*SCALE) 
#define SHEIGHT floor(108*SCALE) 

struct hashNode{
    Mat value;
    int index;
    hashNode* next;
    int x;
    int y;
    hashNode(Mat pValue, int px, int py){
        value = pValue;
        x = px;
        y = py;
        next = NULL;
    }
};
struct hashBucket{
    hashNode* first;
    hashNode* last;
    int cant;
    hashBucket(){
        first = last = NULL;
        cant = 0;
    }
    void insert(Mat table, int x, int y){
        hashNode* newNode = new hashNode(table,x, y);
        if (first == NULL){
            first = last = newNode;
        }
        else{
            last->next = newNode;
            last = newNode;
        }
        cant++;
    }
};
struct hashMap{
    hashBucket buckets[256];
    hashMap(){
        for(int i=0; i < 256; i++){
            buckets[i] = hashBucket();
        }
    }
    void insert(Mat table, int x, int y){
        Vec3b pixelColor = table.at<cv::Vec3b>(0,0);
        buckets[pixelColor[0]].insert(table,x,y);
    }
    bool matXor(hashNode * fuente,hashNode * destino, int count){
        Mat matSrc = fuente->value;
        Mat matDest = destino->value;
        String name1, name2;
        int matY = floor(matSrc.rows*0.80);
        int matX = floor(matSrc.cols*0.80);
        int coincidence = 50;
        //int tolerace = floor((matY*matX)*0.60);
        for (int y = 0; y < matY; y++)
        {
            for (int x = 0; x < matX; x++)
            {
                if (abs(matSrc.at<cv::Vec3b>(y,x)[0] - matDest.at<cv::Vec3b>(y,x)[0]) > coincidence ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[1] - matDest.at<cv::Vec3b>(y,x)[1]) > coincidence ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[2] - matDest.at<cv::Vec3b>(y,x)[2]) > coincidence )
                    {
                        //tolerace--;
                        //if(tolerace == 0){
                            return false;
   
                        //}
                    }
                }
            }
        
        name1 = "coincs/"+std::to_string(count)+"_"+std::to_string(fuente->x)+"_"+std::to_string(fuente->y)+"n1.jpg";
        name2 = "coincs/"+std::to_string(count)+"_"+std::to_string(destino->x)+"_"+std::to_string(destino->y)+"n2.jpg";
        imwrite(name1,matSrc);
        imwrite(name2,matDest);
        std::cout<<"Coincidencia encontrada"<<std::endl;
        return true;
        
    }
    int getCoincidences(hashMap pHash, int pBucket,int count){
        /*std::cout<<"Pixeles en source: "<< this->buckets[pBucket].cant <<std::endl;
        std::cout<<"Pixeles en dest: "<< pHash.buckets[pBucket].cant <<std::endl;*/
        hashNode* tmpSrc = this->buckets[pBucket].first;
        hashNode* tmpDest;
        while(tmpSrc != NULL){
            tmpDest = pHash.buckets[pBucket].first;
            while(tmpDest != NULL){
                if(matXor(tmpSrc,tmpDest, count)){
                    count++;
                    break;
                }
                tmpDest = tmpDest->next;
            }
            /*if(count != 0){
                return count;        
            }*/
            tmpSrc = tmpSrc->next;
        }
        return count;
    }
};