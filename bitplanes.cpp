#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

char* progpath;

void usage() {
  cerr << "Usage: \n";
  cerr << "  " << progpath << " <image path> <output image path>\n";
}

void fatalerror(const char* error) {
  cerr << "ERROR: " << error << endl;
  usage();
  exit(1);
}

void info(const char* msg) {
  cerr << "[+] " << msg << endl;
}

int main( int argc, char** argv ) {
  progpath = argv[0];
  if ( argc < 2 ) {
    fatalerror("Input image not specified");
  }
  if ( argc < 3 ) {
    fatalerror("Output image not specified");
  }

  Mat orig = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  info("Loaded image");

  Mat result(orig.rows * 8, orig.cols * 4, CV_8UC3, Scalar::all(0));
  
  for ( int bitplane = 1, pos = 7 ; bitplane < 0x100 ; bitplane <<= 1, pos-- ) {
    Mat res;
    bitwise_and(orig, bitplane, res);
    divide(res, bitplane, res);
    multiply(res, 0xFF, res);

    Mat channels[3];
    split(res, channels);
    for ( int i = 0 ; i < 3 ; i++ ) {
      Mat zero = Mat(channels[i].size(), CV_8UC1, Scalar::all(0));
      Mat ch[3] = {zero, zero, zero};
      ch[i] = channels[i];
      merge(ch, 3, channels[i]);
    }

    res.copyTo(result(Rect(0, pos * orig.rows, orig.cols, orig.rows)));
    channels[0].copyTo(result(Rect(3 * orig.cols, pos * orig.rows, orig.cols, orig.rows)));
    channels[1].copyTo(result(Rect(2 * orig.cols, pos * orig.rows, orig.cols, orig.rows)));
    channels[2].copyTo(result(Rect(1 * orig.cols, pos * orig.rows, orig.cols, orig.rows)));
  }
  info("Extracted bitplanes");
  
  imwrite(argv[2], result);
  info("Outputted file");
  
  return 0;
}
