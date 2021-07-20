// Author: Lin Jianan
// U. No.: 3035577595
// Filename: 2.cpp
// Assignment 2
// Description: Find Bounding Boxes for each shape and isolated bounding boxes.
// Last Changed: November 7, 2020

#include <iostream>
#include <limits>
#include <iomanip>
using namespace std;

const int X = 0;
const int Y = 1;

const int MAXGEOM = 20;

struct AABB {
  //The following lists bounding box information for each geometric shape.
  double min[2];  // min[0] = xmin, min[1] = ymin
  double max[2];  // max[0] = xmax, max[1] = ymax
  bool overlapped=false;
  // functions

  // Read a point set from user input and calculates AABB
  void ReadPointSet() {
    int n;
    double x, y;

    cin >> n;
    min[X] = min[Y] = numeric_limits<double>::max();
    max[X] = max[Y] = numeric_limits<double>::lowest();
    for (int i = 0; i < n; ++i) {
      cin >> x >> y;
      if (x < min[X]) min[X] = x;
      if (x > max[X]) max[X] = x;
      if (y < min[Y]) min[Y] = y;
      if (y > max[Y]) max[Y] = y;
    }
  }

  // Read a rectangle from user input and calculates AABB
  void ReadRectangle() {
    // --- YOUR CODE HERE (Task 1) ---
    double cx,cy,wx,ly;
    cin>>cx>>cy>>wx>>ly;
    min[X] = cx;
    min[Y] = cy;
    max[X] = cx;
    max[Y] = cy;
    if ((cx-(wx/2)) < min[X]) min[X] = cx-(wx/2);
    if ((cx+(wx/2)) > max[X]) max[X] = cx+(wx/2);
    if ((cy-(ly/2)) < min[Y]) min[Y] = cy-(ly/2);
    if ((cy+(ly/2)) > max[Y]) max[Y] = cy+(ly/2);
    
  }

  // Read a circle from user input and calculates AABB
  void ReadCircle() {
    // --- YOUR CODE HERE (Task 1) ---
    double cx,cy,r;
    cin>>cx>>cy>>r;
    min[X] = cx;
    min[Y] = cy;
    max[X] = cx;
    max[Y] = cy;
    if ((cx-r) < min[X]) min[X] = cx-r;
    if ((cx+r) > max[X]) max[X] = cx+r;
    if ((cy-r) < min[Y]) min[Y] = cy-r;
    if ((cy+r) > max[Y]) max[Y] = cy+r;
  }

};

//Calculate absolute value of (x-y).
double absolute_v(double x, double y) {
  if (x>y) return (x-y);
  else return (y-x);
}

// return true if two AABBs overlaps
//        false otherwise
bool IsOverlap(AABB p, AABB q) {
  // --- YOUR CODE HERE (Task 3) ---
  double cpx,cpy,cqx,cqy,wp,lp,wq,lq;
  cpx=(p.max[X]+p.min[X])/2; //x-coordinate of center of shape p
  cpy=(p.max[Y]+p.min[Y])/2; //y-coordinate of center of shape p
  cqx=(q.max[X]+q.min[X])/2;
  cqy=(q.max[Y]+q.min[Y])/2;
  wp=p.max[X]-p.min[X];  //width of shape p
  lp=p.max[Y]-p.min[Y];  //length/height of shape p
  wq=q.max[X]-q.min[X];
  lq=q.max[Y]-q.min[Y];
  return ((absolute_v(cpx,cqx)<=((wp+wq)/2)) && (absolute_v(cpy,cqy)<=((lp+lq)/2)));
}


int main()
{
  char type;

  AABB boxes[MAXGEOM];

  AABB res; // input AAAB and result AABB
  // initialize bounds of result AABB
  res.min[X] = res.min[Y] = numeric_limits<double>::max();
  res.max[X] = res.max[Y] = numeric_limits<double>::lowest();

  // read geometry type and dispatch to the corresponding
  // AABB read geomety functions to read from user input handle
  // and compute AABB
  // store all AABBs in the array "boxes"
  int numBoxes = 0;

  cin >> type;
  while (type != '#') {
    switch (type) {
      case 'R':
        boxes[numBoxes++].ReadRectangle();
        break;
      case 'C':
        boxes[numBoxes++].ReadCircle();
        break;
      case 'P':
        boxes[numBoxes++].ReadPointSet();
        break;
      default:
        break;
    }
    cin >> type;
  }

  // display AABBs for input geometries by going through the array "boxes"
  for (int i = 0; i < numBoxes; i++) {
    // --- YOUR CODE HERE (Task 2) ---
    cout<<"AABB"<<" ";
    cout<<setfill('0')<<setw(2);
    cout<<i;
    cout<<": "<<boxes[i].min[X]<<" "<<boxes[i].max[X]<<" "<<boxes[i].min[Y]<<" "<<boxes[i].max[Y]<<endl;
  }

  cout << "Isolated AABBs:\n";

  // for each AABB, if it does not overlap with any other AABBs, display its ID
  // --- YOUR CODE HERE (Task 4) ---
  for (int i=0;i<numBoxes;i++) {
    for (int j=0;j<numBoxes;j++) {
      if (i==j) continue;
      if(IsOverlap(boxes[i],boxes[j])) {
        boxes[i].overlapped=true;
        boxes[j].overlapped=true;
      }
    }
  }

  //Display isolated bounding box(es).
  for (int i=0;i<numBoxes;i++) {
    if (!boxes[i].overlapped) {
      cout<<setfill('0')<<setw(2);
      cout<<i<<endl;
    }
  }
  return 0;
}

