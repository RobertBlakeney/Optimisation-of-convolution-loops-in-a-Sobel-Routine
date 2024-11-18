
#include "canny.h"

int main() {

	int i, j;

	const char* inList[] = {"input/rec.pgm", "input/rec2.pgm", "input/rec3.pgm"};
	const char* outListG[] = {"output/outG.pgm", "output/out2G.pgm", "output/out3G.pgm"};
	const char* outListS[] = {"output/outS.pgm", "output/out2S.pgm", "output/out3S.pgm"};

	int imgNo = 3;

	image_detection(inList, outListG, outListS, imgNo);

	system("pause");
	return 0;
}


