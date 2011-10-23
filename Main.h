#pragma once



using namespace std;

const int LONGSTRLEN = 1000;
typedef char LONGSTR[LONGSTRLEN];
const int STRLEN = 200;
typedef char STR[STRLEN];

const int STROKE_SCALE = 25; /* Scaling between clip runtime and curve length. */
const float STROKE_LINE_WIDTH = 8.0f;

const int TAB = 9;
#ifdef WIN32
  #define M_PI 3.1415926
#endif
const float RAD_TO_DEG = (float)(180.0/M_PI);



const int INVALID = 1;

const float PREVIEW_SCALE_FACTOR = 15.0f;

const float PLAYBACK_SLOWDOWN = 1.0f;//0.1f;  //only play 30% the dt per frame read from the bvh files

const float MINI_MODEL_SCALE = 4.0f;
const float MINI_MODEL_DY = 10.0f;
const float STROKE_SIZE = 5.0f;
const float ENDPOINT_SIZE = 15.0f;

const float STROKE_CONNECT_THRESHOLD = 10.0f;