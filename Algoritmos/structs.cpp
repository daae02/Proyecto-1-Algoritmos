#include <opencv2/opencv.hpp>
using namespace cv;
#define SWIDTH floor(192*SCALE) 
#define SHEIGHT floor(108*SCALE) 
#define SCALE 1
struct hashNode{
    Mat value;
    int index;
    hashNode* next;
    hashNode(Mat pValue, int pIndex){
        value = pValue;
        index = pIndex ;
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
    void insert(Mat table, int index){
        hashNode* newNode = new hashNode(table, index);
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
    void insert(Mat table){
        Vec3b pixelColor = table.at<cv::Vec3b>(0,0);
        buckets[pixelColor[0]].insert(table,buckets[pixelColor[0]].cant);
    }
    bool matXor(Mat matSrc, Mat matDest, int indexS, int indexD){
        String name1, name2;
        int matY = floor(matSrc.rows*0.30);
        int matX = floor(matSrc.cols*0.30);
        int coincidence = 30;
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
        
        name1 = "coincs/"+std::to_string(indexS)+std::to_string(indexD)+"n1.jpg";
        name2 = "coincs/"+std::to_string(indexS)+std::to_string(indexD)+"n2.jpg";
        imwrite(name1,matSrc);
        imwrite(name2,matDest);
        std::cout<<"Coincidencia encontrada"<<std::endl;
        return true;
        
    }
    int getCoincidences(hashMap pHash, int pBucket){
        int count = 0;
        /*std::cout<<"Pixeles en source: "<< this->buckets[pBucket].cant <<std::endl;
        std::cout<<"Pixeles en dest: "<< pHash.buckets[pBucket].cant <<std::endl;*/
        hashNode* tmpSrc = this->buckets[pBucket].first;
        hashNode* tmpDest = pHash.buckets[pBucket].first;
        hashNode* fnlSrc = this->buckets[pBucket].last;
        hashNode* fnlDest = pHash.buckets[pBucket].last;
        int contS = this->buckets[pBucket].cant; 
        int contD = pHash.buckets[pBucket].cant; 
        while(tmpSrc != NULL){
            while(tmpDest != NULL){
                if(matXor(tmpSrc->value,tmpDest->value,contS,contD)){
                    count++;
                    tmpSrc = tmpSrc->next;
                }
                tmpDest = tmpDest->next;
                contD++;
            }
            if(count != 0){
                return count;        
            }
            tmpSrc = tmpSrc->next;
            contS++;
        }
        return count;
    }
};