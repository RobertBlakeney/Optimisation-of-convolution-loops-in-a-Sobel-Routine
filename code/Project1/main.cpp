
#include "canny.h"

int main() {

	int out, i, j;

	const char* inList[] = {"rec.pgm", "rec2.jpg"};
	const char* outListG[] = {"outG.pgm", "out2G.jpg"};
	const char* outListS[] = {"outS.pgm", "out2S.jpg"};

	int imgNo = 2;

	image_detection(inList, outListG, outListS, imgNo);

	system("pause");
	return 0;
}


