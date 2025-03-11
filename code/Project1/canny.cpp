#include "canny.h"
//19 refs to N and M
//unsigned char filt[N][M], gradient[N][M], grad2[N][M], edgeDir[N][M]; //5 3 0 2
vector<vector<int>> f, g, eD;
unsigned char gaussianMask[5][5];
signed char GxMask[3][3], GyMask[3][3];
int width, height = 0;
float fulltimeG = 0;
float fulltimeS = 0;

// use of high res clock for accurate timing
typedef std::chrono::high_resolution_clock timer;

int main() {

	int i, j;
	const char* inList[] = { "input/100x100.pgm", "input/256x256.pgm", "input/512x512.pgm", "input/1024x1024.pgm", "input/2048x2048.pgm", "input/4096x4096.pgm"};
	const char* outListG[] = { "output/outG.pgm", "output/out2G.pgm", "output/out3G.pgm", "output/out4G.pgm", "output/out5G.pgm", "output/out6G.pgm"};
	const char* outListS[] = { "output/outS.pgm", "output/out2S.pgm", "output/out3S.pgm", "output/out4S.pgm", "output/out5S.pgm", "output/out6S.pgm"};

	image_detection(inList, outListG, outListS);

	long long int pixelS = imgNo * ((width - 2) * (height - 2));

	long float ppsS = pixelS / fulltimeS;
	float GppsS = ppsS / 1000000000;

	cout << "\nTotal time to process sobel mask: " << fulltimeS << endl;

	cout << "\nGpps of sobel mask: " << GppsS << endl;

	system("pause");
	return 0;
}



void GaussianBlur() {

	int i, j;
	unsigned int    row, col;
	int rowOffset;
	int colOffset;
	int newPixel;

	unsigned char temp;


	/* Declare Gaussian mask */
	gaussianMask[0][0] = 2;

	gaussianMask[0][1] = 4;
	gaussianMask[0][2] = 5;
	gaussianMask[0][3] = 4;
	gaussianMask[0][4] = 2;

	gaussianMask[1][0] = 4;
	gaussianMask[1][1] = 9;
	gaussianMask[1][2] = 12;
	gaussianMask[1][3] = 9;
	gaussianMask[1][4] = 4;

	gaussianMask[2][0] = 5;
	gaussianMask[2][1] = 12;
	gaussianMask[2][2] = 15;
	gaussianMask[2][3] = 12;
	gaussianMask[2][4] = 5;

	gaussianMask[3][0] = 4;
	gaussianMask[3][1] = 9;
	gaussianMask[3][2] = 12;
	gaussianMask[3][3] = 9;
	gaussianMask[3][4] = 4;

	gaussianMask[4][0] = 2;
	gaussianMask[4][1] = 4;
	gaussianMask[4][2] = 5;
	gaussianMask[4][3] = 4;
	gaussianMask[4][4] = 2;

	/*---------------------- Gaussian Blur ---------------------------------*/
	for (row = 2; row < width - 2; row++) {
		for (col = 2; col < height - 2; col++) {
			newPixel = 0;
			for (rowOffset = -2; rowOffset <= 2; rowOffset++) {
				for (colOffset = -2; colOffset <= 2; colOffset++) {
					newPixel += frame1[row + rowOffset][col + colOffset] * gaussianMask[2 + rowOffset][2 + colOffset];  // 2 ops
				}
			}
			f[row][col] = (unsigned char)(newPixel / 159);  // 1 ops
		}
	}


}


void Sobel() {


	int i, j;
	unsigned int    row, col;
	int rowOffset;
	int colOffset;
	int Gx;
	int Gy;
	float thisAngle;
	int newAngle = 0;
	int newPixel;

	unsigned char temp;




	/* Declare Sobel masks */
	GxMask[0][0] = -1; GxMask[0][1] = 0; GxMask[0][2] = 1;
	GxMask[1][0] = -2; GxMask[1][1] = 0; GxMask[1][2] = 2;
	GxMask[2][0] = -1; GxMask[2][1] = 0; GxMask[2][2] = 1;

	GyMask[0][0] = -1; GyMask[0][1] = -2; GyMask[0][2] = -1;
	GyMask[1][0] = 0; GyMask[1][1] = 0; GyMask[1][2] = 0;
	GyMask[2][0] = 1; GyMask[2][1] = 2; GyMask[2][2] = 1;

	/*---------------------------- Determine edge directions and gradient strengths -------------------------------------------*/
	for (row = 1; row < width - 1; row++) {
		for (col = 1; col < height - 1; col++) {

			Gx = 0;
			Gy = 0;

			/* Calculate the sum of the Sobel mask times the nine surrounding pixels in the x and y direction */
			for (rowOffset = -1; rowOffset <= 1; rowOffset++) {
				for (colOffset = -1; colOffset <= 1; colOffset++) {

					Gx += f[row + rowOffset][col + colOffset] * GxMask[rowOffset + 1][colOffset + 1]; // 2 ops
					Gy += f[row + rowOffset][col + colOffset] * GyMask[rowOffset + 1][colOffset + 1]; // 2 ops
				}
			}

			g[row][col] = (unsigned char)(sqrt(Gx * Gx + Gy * Gy));  // 3 ops

			thisAngle = (((atan2(Gx, Gy)) / 3.14159) * 180.0);

			/* Convert actual edge direction to approximate value */
			if (((thisAngle >= -22.5) && (thisAngle <= 22.5)) || (thisAngle >= 157.5) || (thisAngle <= -157.5))
				newAngle = 0;
			else if (((thisAngle > 22.5) && (thisAngle < 67.5)) || ((thisAngle > -157.5) && (thisAngle < -112.5)))
				newAngle = 45;
			else if (((thisAngle >= 67.5) && (thisAngle <= 112.5)) || ((thisAngle >= -112.5) && (thisAngle <= -67.5)))
				newAngle = 90;
			else if (((thisAngle > 112.5) && (thisAngle < 157.5)) || ((thisAngle > -67.5) && (thisAngle < -22.5)))
				newAngle = 135;


			eD[row][col] = newAngle;
		}
	}

}


int image_detection(const char* inn[], const char* out1[], const char* out2[]) {


	int i, j;
	unsigned int row, col;
	int rowOffset;
	int colOffset;
	int Gx;
	int Gy;
	float thisAngle;
	int newAngle;
	int newPixel;


	unsigned char temp;

	//We run this part of the code to get the dimensions of the image to resize arrays for the program to work
	FILE* finput;
	auto data = openfile(inn[selImg], &finput);
	height = data.first;
	width = data.second;

	f = vector<vector<int>>(width, vector<int>(height));
	g = vector<vector<int>>(width, vector<int>(height));
	eD = vector<vector<int>>(width, vector<int>(height));


	//consider using realloc for better dynamic us of memory
	frame1 = (unsigned char**)malloc(width * sizeof(unsigned char*));
	if (frame1 == NULL) { printf("\nerror with malloc fr"); return -1; }
	for (i = 0; i < width; i++) {
		frame1[i] = (unsigned char*)malloc(height * sizeof(unsigned char));
		if (frame1[i] == NULL) { printf("\nerror with malloc fr"); return -1; }
	}


	//create the image
	print = (unsigned char**)malloc(width * sizeof(unsigned char*));
	if (print == NULL) { printf("\nerror with malloc fr"); return -1; }
	for (i = 0; i < width; i++) {
		print[i] = (unsigned char*)malloc(height * sizeof(unsigned char));
		if (print[i] == NULL) { printf("\nerror with malloc fr"); return -1; }
	}

	//initialize the image
	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
			print[i][j] = 0;

	read_image(inn[selImg], frame1);

	GaussianBlur();


	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
			print[i][j] = f[i][j];

	write_image(out1[selImg], print);

	auto ss = timer::now();

	for (int i = 0; i < imgNo; ++i)
		//Sobel();
		//SobelUnroll();
		//SobelUnroll_2Factor_RegBlocking();
		//SobelTiling_32();
		//SobelAvx();
		//SobelParallel();
		//SobelParallelAvx();
		SobelParallelAvxRegblocking();

	auto es = timer::now();

	/* write gradient to image*/

	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
			print[i][j] = g[i][j];

	write_image(out2[selImg], print);

	auto timeS = duration_cast<microseconds>(es - ss);
	float fTimeS = (float)timeS.count() / 1000000;
	fulltimeS += fTimeS;
	//cout << endl << "Time: " << fTimeG or fTimeS << "\n" << endl; //To check time of individual image

	for (i = 0; i < width; i++)
		free(frame1[i]);
	free(frame1);



	for (i = 0; i < width; i++)
		free(print[i]);
	free(print);


	return 0;

}


#pragma region fileMan




void read_image(const char filename[], unsigned char** image) // 0 ops
{
	int inint = -1;
	int c;
	FILE* finput;
	int i, j;

	printf("  Reading image from disk (%s)...\n", filename);
	//finput = NULL;
	openfile(filename, &finput);


	for (j = 0; j < width; j++)
		for (i = 0; i < height; i++) {
			c = getc(finput);


			image[j][i] = (unsigned char)c;
		}



	/* for (j=0; j<N; ++j)
	   for (i=0; i<M; ++i) {
		 if (fscanf(finput, "%i", &inint)==EOF) {
		   fprintf(stderr,"Premature EOF\n");
		   exit(-1);
		 } else {
		   image[j][i]= (unsigned char) inint; //printf("\n%d",inint);
		 }
	   }*/



	fclose(finput);
}





void write_image(const char* filename, unsigned char** image) //0 ops
{
	FILE* foutput;
	errno_t err;
	int i, j;


	printf("  Writing result to disk (%s)...\n", filename);
	if ((err = fopen_s(&foutput, filename, "wb")) != NULL) {
		printf("Unable to open file %s for writing\n", filename);
		exit(-1);
	}

	fprintf(foutput, "P2\n");
	fprintf(foutput, "%d %d\n", height, width);
	fprintf(foutput, "%d\n", 255);

	for (j = 0; j < width; ++j) {
		for (i = 0; i < height; ++i) {
			fprintf(foutput, "%3d ", image[j][i]);
			if (i % 32 == 31) fprintf(foutput, "\n");
		}
		if (height % 32 != 0) fprintf(foutput, "\n");
	}
	fclose(foutput);


}


std::pair<int, int> openfile(const char* filename, FILE** finput) //3 ops
{
	int x0, y0;
	errno_t err;
	char header[255];
	int aa;

	if ((err = fopen_s(finput, filename, "rb")) != NULL) {
		printf("Unable to open file %s for reading\n", filename);
		exit(-1);
	}

	aa = fscanf_s(*finput, "%s", header, 20);

	/*if (strcmp(header,"P2")!=0) {
	   fprintf(stderr,"\nFile %s is not a valid ascii .pgm file (type P2)\n",
			   filename);
	   exit(-1);
	 }*/

	x0 = getint(*finput);
	y0 = getint(*finput);

	//if ((x0 != M) || (y0 != N)) {
	//	printf("Image dimensions do not match: %ix%i expected\n", N, M);
	//	exit(-1);
	//}

	//x0 = getint(*finput); /* read and throw away the range info */
	return { x0, y0 };
}


int getint(FILE* fp) /* adapted from "xv" source code */
{
	int c, i, firstchar, garbage;

	/* note:  if it sees a '#' character, all characters from there to end of
	   line are appended to the comment string */

	   /* skip forward to start of next number */
	c = getc(fp);
	while (1) {
		/* eat comments */
		if (c == '#') {
			/* if we're at a comment, read to end of line */
			char cmt[256], * sp;

			sp = cmt;  firstchar = 1;
			while (1) {
				c = getc(fp);
				if (firstchar && c == ' ') firstchar = 0;  /* lop off 1 sp after # */
				else {
					if (c == '\n' || c == EOF) break;
					if ((sp - cmt) < 250) *sp++ = c;
				}
			}
			*sp++ = '\n';
			*sp = '\0';
		}

		if (c == EOF) return 0;
		if (c >= '0' && c <= '9') break;   /* we've found what we were looking for */

		/* see if we are getting garbage (non-whitespace) */
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n' && c != ',') garbage = 1;

		c = getc(fp);
	}

	/* we're at the start of a number, continue until we hit a non-number */
	i = 0;
	while (1) {
		i = (i * 10) + (c - '0');
		c = getc(fp);
		if (c == EOF) return i;
		if (c < '0' || c>'9') break;
	}
	return i;
}
#pragma endregion

#pragma region OptMethods

void SobelUnroll() { //A simple unroll of the innermost loops


	int i, j;
	unsigned int    row, col;
	int rowOffset;
	int colOffset;
	int Gx;
	int Gy;
	float thisAngle;
	int newAngle = 0;
	int newPixel;

	unsigned char temp;




	/* Declare Sobel masks */
	GxMask[0][0] = -1; GxMask[0][1] = 0; GxMask[0][2] = 1;
	GxMask[1][0] = -2; GxMask[1][1] = 0; GxMask[1][2] = 2;
	GxMask[2][0] = -1; GxMask[2][1] = 0; GxMask[2][2] = 1;

	GyMask[0][0] = -1; GyMask[0][1] = -2; GyMask[0][2] = -1;
	GyMask[1][0] = 0; GyMask[1][1] = 0; GyMask[1][2] = 0;
	GyMask[2][0] = 1; GyMask[2][1] = 2; GyMask[2][2] = 1;

	/*---------------------------- Determine edge directions and gradient strengths -------------------------------------------*/
	for (row = 1; row < width - 1; row++) {
		for (col = 1; col < height - 1; col++) {

			Gx = 0;
			Gy = 0;

			Gx += f[row - 1][col - 1] * GxMask[0][0];
			Gx += f[row - 1][col] * GxMask[0][1];
			Gx += f[row - 1][col + 1] * GxMask[0][2];

			Gx += f[row][col - 1] * GxMask[1][0];
			Gx += f[row][col] * GxMask[1][1];
			Gx += f[row][col + 1] * GxMask[1][2];

			Gx += f[row + 1][col - 1] * GxMask[2][0];
			Gx += f[row + 1][col] * GxMask[2][1];
			Gx += f[row + 1][col + 1] * GxMask[2][2];


			Gy += f[row - 1][col - 1] * GyMask[0][0];
			Gy += f[row - 1][col] * GyMask[0][1];
			Gy += f[row - 1][col + 1] * GyMask[0][2];

			Gy += f[row][col - 1] * GyMask[1][0];
			Gy += f[row][col] * GyMask[1][1];
			Gy += f[row][col + 1] * GyMask[1][2];

			Gy += f[row + 1][col - 1] * GyMask[2][0];
			Gy += f[row + 1][col] * GyMask[2][1];
			Gy += f[row + 1][col + 1] * GyMask[2][2];


			g[row][col] = (unsigned char)(sqrt(Gx * Gx + Gy * Gy));  // 3 ops

			thisAngle = (((atan2(Gx, Gy)) / 3.14159) * 180.0);

			/* Convert actual edge direction to approximate value */
			if (((thisAngle >= -22.5) && (thisAngle <= 22.5)) || (thisAngle >= 157.5) || (thisAngle <= -157.5))
				newAngle = 0;
			else if (((thisAngle > 22.5) && (thisAngle < 67.5)) || ((thisAngle > -157.5) && (thisAngle < -112.5)))
				newAngle = 45;
			else if (((thisAngle >= 67.5) && (thisAngle <= 112.5)) || ((thisAngle >= -112.5) && (thisAngle <= -67.5)))
				newAngle = 90;
			else if (((thisAngle > 112.5) && (thisAngle < 157.5)) || ((thisAngle > -67.5) && (thisAngle < -22.5)))
				newAngle = 135;


			eD[row][col] = newAngle;
		}
	}

}

void SobelUnroll_2Factor_RegBlocking() { //Preforms small factor register blocking


	int i, j;
	unsigned int    row, col;
	int rowOffset;
	int colOffset;
	int Gx, Gx2;
	int Gy, Gy2;
	float thisAngle, thisAngle2;
	int newAngle = 0;
	int newPixel;

	unsigned char temp;




	/* Declare Sobel masks */
	GxMask[0][0] = -1; GxMask[0][1] = 0; GxMask[0][2] = 1;
	GxMask[1][0] = -2; GxMask[1][1] = 0; GxMask[1][2] = 2;
	GxMask[2][0] = -1; GxMask[2][1] = 0; GxMask[2][2] = 1;

	GyMask[0][0] = -1; GyMask[0][1] = -2; GyMask[0][2] = -1;
	GyMask[1][0] = 0; GyMask[1][1] = 0; GyMask[1][2] = 0;
	GyMask[2][0] = 1; GyMask[2][1] = 2; GyMask[2][2] = 1;

	/*---------------------------- Determine edge directions and gradient strengths -------------------------------------------*/
	for (row = 1; row < width - 1; row++) {
		Gx = 0, Gx2 = 0, Gy = 0, Gy2 = 0;
		for (col = 1; col < height - 1; col += 2) {


			Gx += f[row - 1][col - 1] * GxMask[0][0];
			Gx += f[row - 1][col] * GxMask[0][1];
			Gx += f[row - 1][col + 1] * GxMask[0][2];

			Gx += f[row][col - 1] * GxMask[1][0];
			Gx += f[row][col] * GxMask[1][1];
			Gx += f[row][col + 1] * GxMask[1][2];

			Gx += f[row + 1][col - 1] * GxMask[2][0];
			Gx += f[row + 1][col] * GxMask[2][1];
			Gx += f[row + 1][col + 1] * GxMask[2][2];

			Gx2 += f[row - 1][col] * GxMask[0][0];
			Gx2 += f[row - 1][col + 1] * GxMask[0][1];
			Gx2 += f[row - 1][col + 2] * GxMask[0][2];

			Gx2 += f[row][col] * GxMask[1][0];
			Gx2 += f[row][col + 1] * GxMask[1][1];
			Gx2 += f[row][col + 2] * GxMask[1][2];

			Gx2 += f[row + 1][col] * GxMask[2][0];
			Gx2 += f[row + 1][col + 1] * GxMask[2][1];
			Gx2 += f[row + 1][col + 2] * GxMask[2][2];


			Gy += f[row - 1][col - 1] * GyMask[0][0];
			Gy += f[row - 1][col] * GyMask[0][1];
			Gy += f[row - 1][col + 1] * GyMask[0][2];

			Gy += f[row][col - 1] * GyMask[1][0];
			Gy += f[row][col] * GyMask[1][1];
			Gy += f[row][col + 1] * GyMask[1][2];

			Gy += f[row + 1][col - 1] * GyMask[2][0];
			Gy += f[row + 1][col] * GyMask[2][1];
			Gy += f[row + 1][col + 1] * GyMask[2][2];

			Gy2 += f[row - 1][col] * GyMask[0][0];
			Gy2 += f[row - 1][col + 1] * GyMask[0][1];
			Gy2 += f[row - 1][col + 2] * GyMask[0][2];

			Gy2 += f[row][col] * GyMask[1][0];
			Gy2 += f[row][col + 1] * GyMask[1][1];
			Gy2 += f[row][col + 2] * GyMask[1][2];

			Gy2 += f[row + 1][col] * GyMask[2][0];
			Gy2 += f[row + 1][col + 1] * GyMask[2][1];
			Gy2 += f[row + 1][col + 2] * GyMask[2][2];


			g[row][col] = (unsigned char)(sqrt(Gx * Gx + Gy * Gy));  // 3 ops
			g[row][col+1] = (unsigned char)(sqrt(Gx2 * Gx2 + Gy2 * Gy2));  // 3 ops

			thisAngle = (((atan2(Gx, Gy)) / 3.14159) * 180.0);
			thisAngle2 = (((atan2(Gx2, Gy2)) / 3.14159) * 180.0);

			/* Convert actual edge direction to approximate value */
			if (((thisAngle >= -22.5) && (thisAngle <= 22.5)) || (thisAngle >= 157.5) || (thisAngle <= -157.5))
				newAngle = 0;
			else if (((thisAngle > 22.5) && (thisAngle < 67.5)) || ((thisAngle > -157.5) && (thisAngle < -112.5)))
				newAngle = 45;
			else if (((thisAngle >= 67.5) && (thisAngle <= 112.5)) || ((thisAngle >= -112.5) && (thisAngle <= -67.5)))
				newAngle = 90;
			else if (((thisAngle > 112.5) && (thisAngle < 157.5)) || ((thisAngle > -67.5) && (thisAngle < -22.5)))
				newAngle = 135;


			eD[row][col] = newAngle;
		}
	}

}

void SobelTiling_32() { //Preforms tiling on a block size of 32


	int i, j;
	unsigned int    row, col;
	int rowOffset;
	int colOffset;
	int Gx;
	int Gy;
	float thisAngle;
	int newAngle = 0;
	int newPixel;

	unsigned char temp;




	/* Declare Sobel masks */
	GxMask[0][0] = -1; GxMask[0][1] = 0; GxMask[0][2] = 1;
	GxMask[1][0] = -2; GxMask[1][1] = 0; GxMask[1][2] = 2;
	GxMask[2][0] = -1; GxMask[2][1] = 0; GxMask[2][2] = 1;

	GyMask[0][0] = -1; GyMask[0][1] = -2; GyMask[0][2] = -1;
	GyMask[1][0] = 0; GyMask[1][1] = 0; GyMask[1][2] = 0;
	GyMask[2][0] = 1; GyMask[2][1] = 2; GyMask[2][2] = 1;

	/*---------------------------- Determine edge directions and gradient strengths -------------------------------------------*/
	const int tileSize = 32;

	for (int tileRow = 1; tileRow < width - 1; tileRow += tileSize)
		for (int tileCol = 1; tileCol < height - 1; tileCol += tileSize) {

			//ensure no out of bounds
			int rowEnd = (tileRow + tileSize < width - 1) ? tileRow + tileSize : width - 1;
			int colEnd = (tileCol + tileSize < height - 1) ? tileCol + tileSize : height - 1;

			for (row = tileRow; row < rowEnd; row++) {
				for (col = tileCol; col < colEnd; col++) {

					Gx = 0;
					Gy = 0;

					for (rowOffset = -1; rowOffset <= 1; rowOffset++) {
						for (colOffset = -1; colOffset <= 1; colOffset++) {
							Gx += f[row + rowOffset][col + colOffset] * GxMask[rowOffset + 1][colOffset + 1]; // 2 ops
							Gy += f[row + rowOffset][col + colOffset] * GyMask[rowOffset + 1][colOffset + 1]; // 2 ops
						}
					}


					g[row][col] = (unsigned char)(sqrt(Gx * Gx + Gy * Gy));  // 3 ops

					thisAngle = (((atan2(Gx, Gy)) / 3.14159) * 180.0);

					/* Convert actual edge direction to approximate value */
					if (((thisAngle >= -22.5) && (thisAngle <= 22.5)) || (thisAngle >= 157.5) || (thisAngle <= -157.5))
						newAngle = 0;
					else if (((thisAngle > 22.5) && (thisAngle < 67.5)) || ((thisAngle > -157.5) && (thisAngle < -112.5)))
						newAngle = 45;
					else if (((thisAngle >= 67.5) && (thisAngle <= 112.5)) || ((thisAngle >= -112.5) && (thisAngle <= -67.5)))
						newAngle = 90;
					else if (((thisAngle > 112.5) && (thisAngle < 157.5)) || ((thisAngle > -67.5) && (thisAngle < -22.5)))
						newAngle = 135;


					eD[row][col] = newAngle;
				}
			}

		}

}

void SobelAvx() { // USes avx registers to compute 8 values at a time


	int i, j;
	int row, col;
	int rowOffset;
	int colOffset;
	int Gx;
	int Gy;
	float thisAngle;
	int newAngle;
	int newPixel;

	unsigned char temp;

	/* Declare Sobel masks */

	alignas(32) int gxMask[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	};

	alignas(32) int gyMask[3][3] = {
	{-1, -2, -1},
	{0, 0, 0},
	{1, 2, 1}
	};

	/*---------------------------- Determine edge directions and gradient strengths -------------------------------------------*/

	for (row = 1; row < width - 1; ++row) {
		for (col = 1; col < height - 1; col += 8) {
			__m256i Gx = _mm256_setzero_si256();
			__m256i Gy = _mm256_setzero_si256();

			for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
				for (int colOffset = -1; colOffset <= 1; ++colOffset) {
					alignas(32) int tempPixels[8];
					for (int i = 0; i < 8; ++i)
						if (col + colOffset + i < width)
							tempPixels[i] = f[row + rowOffset][col + colOffset + i];

					__m256i pixels = _mm256_load_si256((__m256i*)tempPixels);

					__m256i GxMask = _mm256_set1_epi32(gxMask[rowOffset + 1][colOffset + 1]);
					__m256i GyMask = _mm256_set1_epi32(gyMask[rowOffset + 1][colOffset + 1]);

					Gx = _mm256_add_epi32(Gx, _mm256_mullo_epi32(pixels, GxMask));
					Gy = _mm256_add_epi32(Gy, _mm256_mullo_epi32(pixels, GyMask));
				}
			}

			__m256i gradient = _mm256_add_epi32(_mm256_abs_epi32(Gx), _mm256_abs_epi32(Gy));

			alignas(32) int result[8];
			alignas(32) int gx[8];
			alignas(32) int gy[8];

			_mm256_store_si256((__m256i*)result, gradient);
			_mm256_store_si256((__m256i*)gx, Gx);
			_mm256_store_si256((__m256i*)gy, Gy);

			for (int i = 0; i < 8; ++i) {
				if (col + i < width)
					g[row][col + i] = result[i];

				float thisAngle = (atan2(gx[i], gy[i]) / 3.14159f) * 180.0f;

				if (((thisAngle >= -22.5) && (thisAngle <= 22.5)) || (thisAngle >= 157.5) || (thisAngle <= -157.5))
					newAngle = 0;
				else if (((thisAngle > 22.5) && (thisAngle < 67.5)) || ((thisAngle > -157.5) && (thisAngle < -112.5)))
					newAngle = 45;
				else if (((thisAngle >= 67.5) && (thisAngle <= 112.5)) || ((thisAngle >= -112.5) && (thisAngle <= -67.5)))
					newAngle = 90;
				else if (((thisAngle > 112.5) && (thisAngle < 157.5)) || ((thisAngle > -67.5) && (thisAngle < -22.5)))
					newAngle = 135;
				if (col + i < width)
					eD[row][col + i] = newAngle;
			}
		}
	}

}

void SobelParallel() { // uses openmp to parallelize the routine


	int i, j;
	int    row, col;
	int rowOffset;
	int colOffset;
	int Gx;
	int Gy;
	float thisAngle;
	int newAngle = 0;
	int newPixel;

	unsigned char temp;

	omp_set_num_threads(8);



	/* Declare Sobel masks */
	GxMask[0][0] = -1; GxMask[0][1] = 0; GxMask[0][2] = 1;
	GxMask[1][0] = -2; GxMask[1][1] = 0; GxMask[1][2] = 2;
	GxMask[2][0] = -1; GxMask[2][1] = 0; GxMask[2][2] = 1;

	GyMask[0][0] = -1; GyMask[0][1] = -2; GyMask[0][2] = -1;
	GyMask[1][0] = 0; GyMask[1][1] = 0; GyMask[1][2] = 0;
	GyMask[2][0] = 1; GyMask[2][1] = 2; GyMask[2][2] = 1;


	/*---------------------------- Determine edge directions and gradient strengths -------------------------------------------*/
	#pragma omp parallel for private(row, col, rowOffset, colOffset, Gx, Gy, thisAngle, newAngle) shared(f, g, eD, GxMask, GyMask)
	for (row = 1; row < width - 1; row++) {
		for (col = 1; col < height - 1; col++) {

			Gx = 0;
			Gy = 0;

			/* Calculate the sum of the Sobel mask times the nine surrounding pixels in the x and y direction */
			for (rowOffset = -1; rowOffset <= 1; rowOffset++) {
				for (colOffset = -1; colOffset <= 1; colOffset++) {

					Gx += f[row + rowOffset][col + colOffset] * GxMask[rowOffset + 1][colOffset + 1]; // 2 ops
					Gy += f[row + rowOffset][col + colOffset] * GyMask[rowOffset + 1][colOffset + 1]; // 2 ops
				}
			}

			g[row][col] = (unsigned char)(sqrt(Gx * Gx + Gy * Gy));  // 3 ops

			thisAngle = (((atan2(Gx, Gy)) / 3.14159) * 180.0);

			/* Convert actual edge direction to approximate value */
			if (((thisAngle >= -22.5) && (thisAngle <= 22.5)) || (thisAngle >= 157.5) || (thisAngle <= -157.5))
				newAngle = 0;
			else if (((thisAngle > 22.5) && (thisAngle < 67.5)) || ((thisAngle > -157.5) && (thisAngle < -112.5)))
				newAngle = 45;
			else if (((thisAngle >= 67.5) && (thisAngle <= 112.5)) || ((thisAngle >= -112.5) && (thisAngle <= -67.5)))
				newAngle = 90;
			else if (((thisAngle > 112.5) && (thisAngle < 157.5)) || ((thisAngle > -67.5) && (thisAngle < -22.5)))
				newAngle = 135;


			eD[row][col] = newAngle;
		}
	}

}


void SobelParallelAvx() {
	
	int i, j;
	int row, col;
	int rowOffset;
	int colOffset;
	int Gx;
	int Gy;
	float thisAngle;
	int newAngle;
	int newPixel;

	unsigned char temp;

	/* Declare Sobel masks */

	alignas(32) int gxMask[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	};

	alignas(32) int gyMask[3][3] = {
	{-1, -2, -1},
	{0, 0, 0},
	{1, 2, 1}
	};

	omp_set_num_threads(8);

	/*---------------------------- Determine edge directions and gradient strengths -------------------------------------------*/
	#pragma omp parallel for private(row, col, rowOffset, colOffset, Gx, Gy, thisAngle, newAngle) shared(f, g, eD, GxMask, GyMask)
	for (row = 1; row < width - 1; ++row) {
		for (col = 1; col < height - 1; col += 8) {
			__m256i Gx = _mm256_setzero_si256();
			__m256i Gy = _mm256_setzero_si256();

			for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
				for (int colOffset = -1; colOffset <= 1; ++colOffset) {
					alignas(32) int tempPixels[8];
					for (int i = 0; i < 8; ++i)
						if (col + colOffset + i < width)
							tempPixels[i] = f[row + rowOffset][col + colOffset + i];

					__m256i pixels = _mm256_load_si256((__m256i*)tempPixels);

					__m256i GxMask = _mm256_set1_epi32(gxMask[rowOffset + 1][colOffset + 1]);
					__m256i GyMask = _mm256_set1_epi32(gyMask[rowOffset + 1][colOffset + 1]);

					Gx = _mm256_add_epi32(Gx, _mm256_mullo_epi32(pixels, GxMask));
					Gy = _mm256_add_epi32(Gy, _mm256_mullo_epi32(pixels, GyMask));
				}
			}

			__m256i gradient = _mm256_add_epi32(_mm256_abs_epi32(Gx), _mm256_abs_epi32(Gy));

			alignas(32) int result[8];
			alignas(32) int gx[8];
			alignas(32) int gy[8];

			_mm256_store_si256((__m256i*)result, gradient);
			_mm256_store_si256((__m256i*)gx, Gx);
			_mm256_store_si256((__m256i*)gy, Gy);

			for (int i = 0; i < 8; ++i) {
				if (col + i < width)
					g[row][col + i] = result[i];

				float thisAngle = (atan2(gx[i], gy[i]) / 3.14159f) * 180.0f;

				if (((thisAngle >= -22.5) && (thisAngle <= 22.5)) || (thisAngle >= 157.5) || (thisAngle <= -157.5))
					newAngle = 0;
				else if (((thisAngle > 22.5) && (thisAngle < 67.5)) || ((thisAngle > -157.5) && (thisAngle < -112.5)))
					newAngle = 45;
				else if (((thisAngle >= 67.5) && (thisAngle <= 112.5)) || ((thisAngle >= -112.5) && (thisAngle <= -67.5)))
					newAngle = 90;
				else if (((thisAngle > 112.5) && (thisAngle < 157.5)) || ((thisAngle > -67.5) && (thisAngle < -22.5)))
					newAngle = 135;
				if (col + i < width)
					eD[row][col + i] = newAngle;
			}
		}
	}
}

void SobelParallelAvxRegblocking() {

	int i, j;
	int row, col;
	int rowOffset;
	int colOffset;
	int Gx;
	int Gy;
	float thisAngle;
	int newAngle;
	int newPixel;
	int size = 8;

	unsigned char temp;

	/* Declare Sobel masks */

	alignas(32) int gxMask[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	};

	alignas(32) int gyMask[3][3] = {
	{-1, -2, -1},
	{0, 0, 0},
	{1, 2, 1}
	};

	omp_set_num_threads(8);

	/*---------------------------- Determine edge directions and gradient strengths -------------------------------------------*/
	#pragma omp parallel for private(row, col, rowOffset, colOffset, Gx, Gy, thisAngle, newAngle) shared(f, g, eD, GxMask, GyMask)
	for (row = 1; row < width - 1; ++row) {
		for (col = 1; col < height - 1; col += 16) {
			__m256i Gx = _mm256_setzero_si256();
			__m256i Gy = _mm256_setzero_si256();
			__m256i Gx2 = _mm256_setzero_si256();
			__m256i Gy2 = _mm256_setzero_si256();
			alignas(32) int tempPixels[8];
			alignas(32) int tempPixels2[8];

			for (int i = 0; i < 8; ++i)
				if (col - 1 + i + size < width) {
					tempPixels[i] = f[row - 1][col - 1 + i];
					tempPixels2[i] = f[row - 1][col - 1 + i + size];
				}
					

			__m256i pixels = _mm256_load_si256((__m256i*)tempPixels);
			__m256i pixels2 = _mm256_load_si256((__m256i*)tempPixels2);
			__m256i GxMask = _mm256_set1_epi32(gxMask[0][0]);
			__m256i GyMask = _mm256_set1_epi32(gyMask[0][0]);
			Gx = _mm256_add_epi32(Gx, _mm256_mullo_epi32(pixels, GxMask));
			Gy = _mm256_add_epi32(Gy, _mm256_mullo_epi32(pixels, GyMask));
			Gx2 = _mm256_add_epi32(Gx2, _mm256_mullo_epi32(pixels2, GxMask));
			Gy2 = _mm256_add_epi32(Gy2, _mm256_mullo_epi32(pixels2, GyMask));

			for (int i = 0; i < 8; ++i)
				if (col + i + size < width) {
					tempPixels[i] = f[row - 1][col + i];
					tempPixels2[i] = f[row - 1][col + i + size];
				}
					

			pixels = _mm256_load_si256((__m256i*)tempPixels);
			pixels2 = _mm256_load_si256((__m256i*)tempPixels2);
			GyMask = _mm256_set1_epi32(gyMask[0][1]);
			Gy2 = _mm256_add_epi32(Gy2, _mm256_mullo_epi32(pixels2, GyMask));

			for (int i = 0; i < 8; ++i)
				if (col + 1 + i + size < width) {
					tempPixels[i] = f[row - 1][col + 1 + i];
					tempPixels[i] = f[row - 1][col + 1 + i + size];
				}
					

			pixels = _mm256_load_si256((__m256i*)tempPixels);
			pixels2 = _mm256_load_si256((__m256i*)tempPixels2);
			GxMask = _mm256_set1_epi32(gxMask[0][2]);
			GyMask = _mm256_set1_epi32(gyMask[0][2]);
			Gx = _mm256_add_epi32(Gx, _mm256_mullo_epi32(pixels, GxMask));
			Gy = _mm256_add_epi32(Gy, _mm256_mullo_epi32(pixels, GyMask));
			Gx2 = _mm256_add_epi32(Gx2, _mm256_mullo_epi32(pixels2, GxMask));
			Gy2 = _mm256_add_epi32(Gy2, _mm256_mullo_epi32(pixels2, GyMask));

			for (int i = 0; i < 8; ++i)
				if (col - 1 + i + size < width) {
					tempPixels[i] = f[row][col - 1 + i];
					tempPixels2[i] = f[row][col - 1 + i + size];
				}
					

			pixels = _mm256_load_si256((__m256i*)tempPixels);
			pixels2 = _mm256_load_si256((__m256i*)tempPixels2);
			GxMask = _mm256_set1_epi32(gxMask[1][0]);
			Gx2 = _mm256_add_epi32(Gx2, _mm256_mullo_epi32(pixels2, GxMask));

			for (int i = 0; i < 8; ++i)
				if (col + 1 + i + size < width) {
					tempPixels[i] = f[row][col + 1 + i];
					tempPixels2[i] = f[row][col + 1 + i + size];
				}
					

			pixels = _mm256_load_si256((__m256i*)tempPixels);
			pixels2 = _mm256_load_si256((__m256i*)tempPixels2);
			GxMask = _mm256_set1_epi32(gxMask[1][2]);
			Gx = _mm256_add_epi32(Gx, _mm256_mullo_epi32(pixels, GxMask));
			Gx2 = _mm256_add_epi32(Gx2, _mm256_mullo_epi32(pixels2, GxMask));

			for (int i = 0; i < 8; ++i)
				if (col - 1 + i + size < width) {
					tempPixels[i] = f[row + 1][col - 1 + i];
					tempPixels2[i] = f[row + 1][col - 1 + i + size];
				}
					

			pixels = _mm256_load_si256((__m256i*)tempPixels);
			pixels2 = _mm256_load_si256((__m256i*)tempPixels2);
			GxMask = _mm256_set1_epi32(gxMask[2][0]);
			GyMask = _mm256_set1_epi32(gyMask[2][0]);
			Gx = _mm256_add_epi32(Gx, _mm256_mullo_epi32(pixels, GxMask));
			Gy = _mm256_add_epi32(Gy, _mm256_mullo_epi32(pixels, GyMask));
			Gx2 = _mm256_add_epi32(Gx2, _mm256_mullo_epi32(pixels2, GxMask));
			Gy2 = _mm256_add_epi32(Gy2, _mm256_mullo_epi32(pixels2, GyMask));
			
			for (int i = 0; i < 8; ++i)
				if (col + i + size < width) {
					tempPixels[i] = f[row + 1][col + i];
					tempPixels2[i] = f[row + 1][col + i + size];
				}
					

			pixels = _mm256_load_si256((__m256i*)tempPixels);
			pixels2 = _mm256_load_si256((__m256i*)tempPixels2);
			GyMask = _mm256_set1_epi32(gyMask[2][1]);
			Gy = _mm256_add_epi32(Gy, _mm256_mullo_epi32(pixels, GyMask));
			Gy2 = _mm256_add_epi32(Gy2, _mm256_mullo_epi32(pixels2, GyMask));
			
			for (int i = 0; i < 8; ++i)
				if (col + 1 + i + size < width) {
					tempPixels[i] = f[row + 1][col + 1 + i];
					tempPixels2[i] = f[row + 1][col + 1 + i + size];
				}
					

			pixels = _mm256_load_si256((__m256i*)tempPixels);
			pixels2 = _mm256_load_si256((__m256i*)tempPixels2);
			GxMask = _mm256_set1_epi32(gxMask[2][2]);
			GyMask = _mm256_set1_epi32(gyMask[2][2]);
			Gx = _mm256_add_epi32(Gx, _mm256_mullo_epi32(pixels, GxMask));
			Gy = _mm256_add_epi32(Gy, _mm256_mullo_epi32(pixels, GyMask));
			Gx2 = _mm256_add_epi32(Gx2, _mm256_mullo_epi32(pixels2, GxMask));
			Gy2 = _mm256_add_epi32(Gy2, _mm256_mullo_epi32(pixels2, GyMask));


			__m256i gradient = _mm256_add_epi32(_mm256_abs_epi32(Gx), _mm256_abs_epi32(Gy));
			__m256i gradient2 = _mm256_add_epi32(_mm256_abs_epi32(Gx2), _mm256_abs_epi32(Gy2));

			alignas(32) int result[8];
			alignas(32) int result2[8];

			_mm256_store_si256((__m256i*)result, gradient);
			_mm256_store_si256((__m256i*)result2, gradient2);


			for (int i = 0; i < 8; ++i) {
				if (col + i + size < width) {
					g[row][col + i] = result[i];
					g[row][col + i + size] = result2[i];
				}
					
			}
		}
	}
}


#pragma endregion 