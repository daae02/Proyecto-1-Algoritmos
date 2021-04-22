#include <opencv2/opencv.hpp>
using namespace cv;
struct hashNode{
    Mat value;
    hashNode* next;
    hashNode(Mat pValue){
        value = pValue;
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
    void insert(Mat table){
        hashNode* newNode = new hashNode(table);
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
        Vec3b pixelColor = table.at<cv::Vec3b>(1,1);
        buckets[pixelColor[0]].insert(table);
    }
    bool matXor(Mat matSrc, Mat matDest){
        int res;
        int matY = floor(matSrc.rows/2);
        int matX = matSrc.cols;
        for (int y = 0; y < matY; y++)
        {
            for (int x = 0; x < matX; x++)
            {
                if ((matSrc.at<cv::Vec3b>(y,x)[0]^matDest.at<cv::Vec3b>(y,x)[0]) != 0 ||
                    (matSrc.at<cv::Vec3b>(y,x)[1]^matDest.at<cv::Vec3b>(y,x)[1]) != 0 ||
                    (matSrc.at<cv::Vec3b>(y,x)[2]^matDest.at<cv::Vec3b>(y,x)[2]) != 0 )
                    {
                        return false;
                    }
            }
        }
        return true;
        
    }
    int getCoincidences(hashMap pHash, int pBucket){
        int count = 0;
        std::cout<<"Pixeles en source: "<< this->buckets[pBucket].cant <<std::endl;
        std::cout<<"Pixeles en dest: "<< pHash.buckets[pBucket].cant <<std::endl;
        hashNode* tmpSrc = this->buckets[pBucket].first;
        hashNode* tmpDest = pHash.buckets[pBucket].first;
        hashNode* fnlSrc = this->buckets[pBucket].last;
        hashNode* fnlDest = pHash.buckets[pBucket].last;
        while(tmpSrc != fnlSrc){
            while(tmpDest != fnlDest){
                if(matXor(tmpSrc->value,tmpDest->value))
                    count++;
                tmpDest = tmpDest->next;
            }
            if(count != 0){
                return count;
            }
            tmpSrc = tmpSrc->next;
        }
        return 0;
    }
};