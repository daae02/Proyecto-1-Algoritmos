#include <fstream>
#include <string>

using namespace std;
void readFile(string filename, double values[],int size){
    ifstream archivo(filename);
	char linea[128];
    int index = 0;
    while(index !=size){
        archivo.getline(linea, sizeof(linea));
        values[index] = stod(linea);
        index++;
    }
    archivo.close();
    
}

/*int main(){
	int values[3];
    string filename = "values.txt";
    readFile(filename, values);
	int index = 0;    
    while(index != 3){
		cout << "value: " << values[index] << endl;
		index++;
	}
    
	return 0;
}*/
