#include <opencv2/opencv.hpp>
#include "files.cpp"
#include <chrono>
#include <random>
using namespace cv;
#define SCALE 0.25
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
    hashNode* verticalFollowingMat;
    hashBucket * container;
    bool paired;
    hashNode(){}
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
        verticalFollowingMat = NULL;
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
    hashNode* insert(Mat pTable, int pX, int pY){
        cant++;
        hashNode* newNode = new hashNode(pTable,pX, pY, this);
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
    int probPiece;
    double probPercentage;
    hashMap(double pPercX, double pPercY, int pTolerance, double pProbPercentage){
        percX = pPercX;
        percY = pPercY;
        tolerance = pTolerance;
        probPercentage = pProbPercentage;
        for(int bucketNum=0; bucketNum < 256; bucketNum++){
            buckets[bucketNum] = hashBucket();
        }
    }
    hashNode* insert(Mat pTable, int pX, int pY){
        Vec3b pixelColor = pTable.at<cv::Vec3b>(0,0);
        return buckets[pixelColor[0]].insert(pTable,pX,pY);
         
    }
    // --------------------  BACKTRACKING / DINAMICA  -------------------------
    int findBiggestBucket(){
        hashBucket * biggest = NULL;
        int biggestNumber;
        for (int bucketNum = 0; bucketNum < 256; bucketNum++)
        {
            if((biggest == NULL || biggest->cant<buckets[bucketNum].cant) && !buckets[bucketNum].checked){
                biggest = &buckets[bucketNum];
                biggestNumber = bucketNum;
            }
        }
        biggest->checked=true;
        //cout << "Biggest Bucket: " << biggestNumber << endl;
        return biggestNumber;
    } 

    void evaluateMats(hashNode * pHash, int pX, int pY, int pShifting){
        if(pHash != NULL && pShifting < SWIDTH &&(pHash->y < pY || pHash->x < pX)){
            if (!pHash->paired){
                pHash->container->cant--;
                pHash->paired= true;
            }
            pShifting++;
            if (pHash->x < pX)
                evaluateMats(pHash->followingMat, pX, pY,pShifting);
            if (pHash->y < pY)
                evaluateMats(pHash->verticalFollowingMat, pX, pY,pShifting);
        }        
    }

    bool matDifferenceBT(hashNode * pFuente,hashNode * pDestino){
        Mat matSrc = pFuente->value;
        Mat matDest = pDestino->value;
        int matY = floor(matSrc.rows*percX);
        int matX = floor(matSrc.cols*percY);
        int total = matSrc.rows * matSrc.cols;
        int cont = 0;
        for (int y = 0; y < matY; y++)
        {
            for (int x = 0; x < matX; x++)
            {
                if (abs(matSrc.at<cv::Vec3b>(y,x)[0] - matDest.at<cv::Vec3b>(y,x)[0]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[1] - matDest.at<cv::Vec3b>(y,x)[1]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[2] - matDest.at<cv::Vec3b>(y,x)[2]) > tolerance )
                    {
                        if(x > matX*0.5 && y > matY*0.5)
                            evaluateMats(pFuente,pFuente->x+x,pFuente->y+y, 0);
                        return false;
                    }
                }
            }
        //cout << "Coincidencia encontrada" << endl;
        return true;
    
        
    }
    int getCoincidencesBT(hashMap pHash, int pBucket){
        int count = 0;
        hashNode* tmpSrc = this->buckets[pBucket].first;
        hashNode* tmpDest;
        while(tmpSrc != NULL){
            tmpDest = pHash.buckets[pBucket].first;
            while(tmpDest!= NULL){
                if(abs(tmpDest->g -tmpSrc->g)< tolerance
                && !tmpDest->paired
                && matDifferenceBT(tmpSrc,tmpDest)){
                    count++;
                    tmpDest->paired = true;
                    break;
                }
                tmpDest = tmpDest->next;
            }
            tmpSrc = tmpSrc->next;
        }

        return count;
    }

    // --------------------  DIVIDE AND CONQUER  -------------------------

    hashBucket * getHalf(hashBucket * pBucket, bool pFirst){ 
        int mitad = floor(pBucket->cant/2);
        hashBucket * bucketHalf = new hashBucket();
        hashNode * temp = pBucket->first;
        for (int bucketNum = 0; bucketNum < mitad; bucketNum++)
        {  
            temp = temp->next;
        }
        if(pFirst){
            bucketHalf ->first = pBucket->first;
            bucketHalf->last=temp->before;
            bucketHalf->cant = mitad;
        }
        else{
            bucketHalf->first=temp;
            bucketHalf->last = pBucket->last;
            bucketHalf->cant = mitad;
        }
        return bucketHalf;

    }

        bool matDifferenceDivideAndConquer(hashNode * pFuente,hashNode * pDestino){
        Mat matSrc = pFuente->value;
        Mat matDest = pDestino->value;
        int matY = floor(matSrc.rows*percX);
        int matX = floor(matSrc.cols*percY);
        int total = matSrc.rows * matSrc.cols;
        int cont = 0;
        for (int y = 0; y < matY; y++)
        {
            for (int x = 0; x < matX; x++)
            {
                if (abs(matSrc.at<cv::Vec3b>(y,x)[0] - matDest.at<cv::Vec3b>(y,x)[0]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[1] - matDest.at<cv::Vec3b>(y,x)[1]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[2] - matDest.at<cv::Vec3b>(y,x)[2]) > tolerance )
                    {
                        return false;
                    }
                }
            }
        return true;
        
    }

    int divideLists(hashBucket * pBucket, hashNode * pNodo){
        if(pBucket->cant == 1){
            return matDifferenceDivideAndConquer(pBucket->first,pNodo);
        }
        else if(pBucket->first->g <= pNodo->g ){
            hashBucket * bucketHalf1 = getHalf(pBucket,true);
            hashBucket * bucketHalf2 = getHalf(pBucket,false);
            return divideLists(bucketHalf1, pNodo) + divideLists(bucketHalf2, pNodo);
        }
        else if(pBucket->first->g > pNodo->g){
            return 0;
        }
        return 0;
    }
    int getCoincidencesDivideAndConquer(hashMap pHash, int pBucket){
        int count = 0;
        hashNode* tmpSrc = this->buckets[pBucket].first;
        hashBucket * cBucket = pHash.buckets+pBucket;
        if(cBucket->first == NULL){
            return count;
        } 
        while(tmpSrc != NULL){
            count += divideLists(cBucket, tmpSrc);
            tmpSrc = tmpSrc->next;
        }
        return count;
        
    }

    // --------------------  PROBABILISTICO  -------------------------

    int probabilisticPerNode(hashNode * pNodo, hashBucket * pBucket){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_real_distribution<float> distributionInteger(0, 100);
        int revision;
        double count = 0;
        hashNode* tmpDest = pBucket->first;
        double percentage = 0;
        while (tmpDest != NULL)
        {
            if (percentage == 0){
                revision = probPiece;
                while (tmpDest->next != NULL && revision > 0)
                {
                    if(!tmpDest->paired && tolerance > abs(pNodo->g - tmpDest->g) && matDifferenceBT(pNodo, tmpDest)){
                        count++;
                        tmpDest->paired = true;
                    }
                    revision--;
                    tmpDest = tmpDest->next;
                }
                if(count != 0){
                    percentage = (count/probPiece)*100;
                }
            }
            else{
                if( distributionInteger(generator) < percentage && !tmpDest->paired){
                    count++;
                    tmpDest->paired = true;
                    percentage = (count/probPiece)*100;
                }
            }
            tmpDest = tmpDest->next;
        }
        return count; 
    }


    int getCoincidencesProbabilistic(hashMap pHash, int pBucket){
        int count = 0;
        hashNode* tmpSrc = this->buckets[pBucket].first;
        hashBucket * cBucket = (pHash.buckets+pBucket);
        probPiece =  floor(cBucket->cant * probPercentage);
        if(pHash.buckets[pBucket].cant == 0){
            return count;
        } 
        while(tmpSrc != NULL){
            count += probabilisticPerNode(tmpSrc, cBucket);
            tmpSrc = tmpSrc->next;
        }
        return count;
    }
};
