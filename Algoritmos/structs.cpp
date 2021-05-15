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
    int probPiece;
    double probPercentage;
    hashMap(double pPercX, double pPercY, int pTolerance, double pProbPercentage){
        percX = pPercX;
        percY = pPercY;
        tolerance = pTolerance;
        probPercentage = pProbPercentage;
        for(int i=0; i < 256; i++){
            buckets[i] = hashBucket();
        }
    }
    hashNode* insert(Mat table, int x, int y){
        Vec3b pixelColor = table.at<cv::Vec3b>(0,0);
        return buckets[pixelColor[0]].insert(table,x,y);
         
    }
    // --------------------  BACKTRACKING / DINAMICA  -------------------------
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

    void evaluateMats(hashNode * hash, int px, int py, int shifting){
        if(hash != NULL && shifting < SWIDTH &&(hash->y < py || hash->x < px)){
            if (!hash->paired){
                hash->container->cant--;
                hash->paired= true;
            }
            shifting++;
            if (hash->x < px)
                evaluateMats(hash->followingMat, px, py,shifting);
            if (hash->y < py)
                evaluateMats(hash->verticalFollowingMat, px, py,shifting);
        }        
    }

    bool matDifferenceBT(hashNode * fuente,hashNode * destino){
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
                if (abs(matSrc.at<cv::Vec3b>(y,x)[0] - matDest.at<cv::Vec3b>(y,x)[0]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[1] - matDest.at<cv::Vec3b>(y,x)[1]) > tolerance ||
                    abs(matSrc.at<cv::Vec3b>(y,x)[2] - matDest.at<cv::Vec3b>(y,x)[2]) > tolerance )
                    {
                        if(x > matX*0.5 && y > matY*0.5)
                            evaluateMats(fuente,fuente->x+x,fuente->y+y, 0);
                        return false;
                    }
                }
            }
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

    hashBucket * getHalf(hashBucket * bucket, bool first){ 
        int mitad = floor(bucket->cant/2);
        hashBucket * bucketHalf = new hashBucket();
        hashNode * temp = bucket->first;
        for (int i = 0; i < mitad; i++)
        {  
            temp = temp->next;
        }
        if(first){
            bucketHalf ->first = bucket->first;
            bucketHalf->last=temp->before;
            bucketHalf->cant = mitad;
        }
        else{
            bucketHalf->first=temp;
            bucketHalf->last = bucket->last;
            bucketHalf->cant = mitad;
        }
        
        return bucketHalf;

    }

        bool matDifferenceDivideAndConquer(hashNode * fuente,hashNode * destino){
        Mat matSrc = fuente->value;
        Mat matDest = destino->value;
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

    int divideLists(hashBucket * bucket, hashNode * nodo){
        if(bucket->cant == 1){
            return matDifferenceDivideAndConquer(bucket->first,nodo);
        }
        else if(bucket->first->g <= nodo->g ){
            hashBucket * bucketHalf1 = getHalf(bucket,true);
            hashBucket * bucketHalf2 = getHalf(bucket,false);
            return divideLists(bucketHalf1, nodo) + divideLists(bucketHalf2, nodo);
        }
        else if(bucket->first->g > nodo->g){
            return 0;
        }
        return 0;
    }
    int getCoincidencesDivideAndConquer(hashMap pHash, int pBucket){
        int count = 0;
        hashNode* tmpSrc = this->buckets[pBucket].first;
        hashBucket * cBucket = (pHash.buckets+pBucket);
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

    int probabilisticPerNode(hashNode * nodo, hashBucket * bucket){
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_int_distribution<int> distributionInteger(1, 100);
        int revision;
        double count = 0;
        hashNode* tmpDest = bucket->first;
        double percentage = 0;
        int random;
        while (tmpDest != NULL)
        {
            if (percentage == 0){
                revision = probPiece;
                while (tmpDest->next != NULL && revision > 0)
                {
                    if(!tmpDest->paired && tolerance > abs(nodo->g - tmpDest->g))
                        count += matDifferenceBT(nodo, tmpDest);
                    revision--;
                    tmpDest = tmpDest->next;
                }
                if(count != 0){
                    percentage = (count/probPiece)*100;
                }
            }
            else{
                random =  distributionInteger(generator);
                if( random < percentage && !tmpDest->paired){
                    count++;
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
