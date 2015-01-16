#include <string>

using namespace std;
class SDAT {
public:
	SDAT();
	string **getDynamicStringArray();

private:
	string **dynamicStringArray;
};

SDAT::SDAT(){
	dynamicStringArray = new string*[10];
	for(int i = 0; i < 10; i++){
		dynamicStringArray[i] = new string[2];
	}
	for (int i = 0; i < 10; i++){
		dynamicStringArray[i][0] = "dynamic";
		dynamicStringArray[i][1] = "string";
	}
}

string **SDAT::getDynamicStringArray(){
	return dynamicStringArray;
}