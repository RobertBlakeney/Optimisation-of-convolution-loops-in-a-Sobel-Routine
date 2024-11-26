/*
#include "canny.h"

int main() {

	int i, j;
	const char* inList[] = {"input/rec.pgm", "input/rec2.pgm", "input/rec3.pgm"};
	const char* outListG[] = {"output/outG.pgm", "output/out2G.pgm", "output/out3G.pgm"};
	const char* outListS[] = {"output/outS.pgm", "output/out2S.pgm", "output/out3S.pgm"};
	int imgNo = 1;
	
	typedef std::chrono::high_resolution_clock timer;

	auto s = timer::now();

	image_detection(inList, outListG, outListS, imgNo);

	auto e = timer::now();

	

	auto time = duration_cast<microseconds>(e - s);
	float fTime = (float)time.count() / 1000000;

	cout << "\nTime: " << fTime << "s" << endl;

	system("pause");
	return 0;
}*/


