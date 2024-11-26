
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <pmmintrin.h>
#include <process.h>
#include <chrono>
#include <iostream>
#include <immintrin.h>
#include <utility>
#include <vector>

using namespace std;
using namespace std::chrono;

static unsigned char **frame1;
static unsigned char **print;

static char IN[] = "rec.pgm";
static  char OUT[] = "out.pgm";
static char OUT_NAME1[] = "out1.pgm";
static char OUT_NAME2[] = "out2.pgm";
static char OUT_NAME3[] = "out3.pgm";
static char OUT_NAME4[] = "out4.pgm";

//#define N 1024
//#define M 1024
extern int width, height;


int image_detection(const char* input_files[], const char* output_files1[], const char* output_files2[], int num_images);


//void write_acc(char* filename, int angl);
//void read_frame(char filename[80], unsigned char image[N][M]);
void read_image(const char filename[], unsigned char **image);
void GaussianBlur();
void Sobel();
void write_image(const char *filename, unsigned char **image);
std::pair<int, int> openfile(const char *filename, FILE** finput);
int getint(FILE *fp);



//void printt(int tem, int i, int j, int max1, int max2);

//void trace_edges(int x0, int y0, int x, int y);
//int sub_frames(int dtheta);







