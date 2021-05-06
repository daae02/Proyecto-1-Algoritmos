#include <opencv2/opencv.hpp>
#include "files.cpp"
using namespace cv;
#define SCALE 0.5
#define SWIDTH floor(192*SCALE) 
#define SHEIGHT floor(108*SCALE) 

struct hashBucket;

struct hashNode{
    Mat value;
    int r;
    int g;
    int x;
    int y;
    hashNode* next;
    hashNode* before;
    hashNode* followingMat;
    hashBucket * container;
    bool paired;
    hashNode(Mat pValue, int px, int py,hashBucket * pContainer){
        value = pValue;
        r = value.at<cv::Vec3b>(0,0)[0];
        g = value.at<cv::Vec3b>(0,0)[1];
        x = px;
        y = py;
        next = NULL;
        before = NULL;
        paired = false;
        followingMat = NULL;
        container = pContainer;
    }
};
struct hashBucket{
    bool checked;
    hashNode* first;
    hashNode* last;
    int cant;
    hashBucket(){
        first = last = NULL;
        cant = 0;
        checked = false;
    }
    hashNode* insert(Mat table, int x, int y){
        cant++;
        hashNode* newNode = new hashNode(table,x, y, this);
        if (first == NULL){
            first = last = newNode;
        }
        else{
            if(newNode->g < first->g){
                newNode -> next = first;
                first -> before = newNode;
                first = newNode;
                
            }
            else{
                hashNode*tempNode = first;
                while(tempNode != NULL){
                    if(newNode->g < tempNode->g){
                        tempNode -> before -> next = newNode;
                        newNode ->next = tempNode;
                        newNode ->before = tempNode ->before;
                        tempNode -> before = newNode;
                        return newNode;
                    }
                    tempNode = tempNode ->next;
                }
                newNode ->before = last;
                last -> next = newNode;
                last = newNode;
            }
        }
        return newNode;
    }
};
struct hashMap{
    hashBucket buckets[256];
    double percX;
    double percY;
    int tolerance;
    hashMap(double pPercX, double pPercY, int pTolerance){
        percX = pPercX;
        std::cout << percX << endl; 
        percY = pPercY;
        std::cout << percY << endl;
        tolerance = pTolerance;
        std::cout << pTolerance << endl;
        for(int i=0; i < 256; i++){
            buckets[i] = hashBucket();
        }
    }
    hashNode* insert(Mat table, int x, int y){
        Vec3b pixelColor = table.at<cv::Vec3b>(0,0);
        return buckets[pixelColor[0]].insert(table,x,y);
         
    }
    
    /*void adjustPercentage(double totalX, double checkedX, double totalY , double checkedY){
        double percentageX = checkedX/totalX;
        double percentageY = checkedY/totalY;
        std::cout<<"viejo x: "<<percX<<" viejo y: "<<percY<<std::endl;
        std::cout<<"nuevo x: "<<percentageX<<" nuevo y: "<<percentageY<<std::endl;
        if(percentageX < percX && percX-percentageX < 0.1){
            percX = (percentageX+percX)/2;
            //std::cout<<"x: "<<percX<<" y: "<<percY<<std::endl;
        }
        if(percentageY < percY && (percX-percentageY) < 0.1){
            percY = (percentageY+percY)/2;
            //std::cout<<"x: "<<percX<<" y: "<<percY<<std::endl;
        }
        //std::cout<<"por x: "<<percentageX<<" por y: "<<percentageY<<std::endl;
    }*/

    int findBiggestBucket(){
        hashBucket * biggest = NULL;
        int biggestNumber;
        for (int i = 0; i < 256; i++)
        {
            if((biggest == NULL || biggest->cant<buckets[i].cant) && !buckets[i].checked){
                biggest = &buckets[i];
                biggestNumber = i;
            }
        }
        biggest->checked=true;
        return biggestNumber;
    } 

    void evaluateMats(hashNode * hash, Vec3b pixelColor){
        
        if(hash->value.at<cv::Vec3b>(0,0)[0] != pixelColor[0]
        || hash->value.at<cv::Vec3b>(0,0)[1] != pixelColor[1]
        || hash->value.at<cv::Vec3b>(0,0)[2] != pixelColor[2]){
            hash->paired= true;
            hash->container->cant--;
            if(hash->followingMat!=NULL){
                evaluateMats(hash->followingMat, pixelColor);
            } 
        }
        
    }

    bool matDifference(hashNode * fuente,hashNode * destino, int count){
        Mat matSrc = fuente->value;
        Mat matDest = destino->value;
        String name1, name2;
        int matY = floor(matSrc.rows*percX);
        int matX = floor(matSrc.cols*percY);
        int total = matSrc.rows * matSrc.cols;
        int cont = 0;
        for (int y = 0; y < matY; y++)
        {
            for (int x = 0; x < matX; x++)
            {
                //cont++;
                if (abs(matSrc.at<cv::Vec3b>(y,x)[0] - matDest.at<cv::Vec3b>(y,x)[0]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[1] - matDest.at<cv::Vec3b>(y,x)[1]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[2] - matDest.at<cv::Vec3b>(y,x)[2]) > tolerance )
                    {
                         /*if ((x + y*matX)> (matX*matY)*0.5)
                            adjustPercentage(total,(x + y*matX), matSrc.rows, y);*/
                        evaluateMats(fuente,matSrc.at<cv::Vec3b>(y,x));
                        return false;
                    }
                }
            }
        
        name1 = "coincs/"+std::to_string(count)+"_"+std::to_string(fuente->x)+"_"+std::to_string(fuente->y)+"n1.jpg";
        name2 = "coincs/"+std::to_string(count)+"_"+std::to_string(destino->x)+"_"+std::to_string(destino->y)+"n2.jpg";
        //imwrite(name1,matSrc);
        //imwrite(name2,matDest);
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
            while(tmpDest!= NULL){
                if(abs(tmpDest->g -tmpSrc->g)< tolerance
                && !tmpDest->paired
                && matDifference(tmpSrc,tmpDest, count)){
                    count++;
                    tmpDest->paired = true;
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