#include <iostream>
using namespace std;
void RectangleBB(double &xmin, double &xmax, double &ymin, double &ymax, string s, int c) {
  //cout<<xmin<<' '<<xmax<<' '<<ymin<<' '<<ymax<<endl;
  double x,y,w,l;
  cin>>s;
  x=stod(s);
  cin>>s;
  y=stod(s);
  cin>>s;
  w=stod(s);
  cin>>s;
  l=stod(s);

  if (c==1) {
    xmin=x-(w/2);
    ymin=y-(l/2);
    xmax=x+(w/2);
    ymax=y+(l/2);
  }
  else {
    if (xmin>(x-(w/2))) {
      xmin=x-(w/2);
    }
    if (ymin>(y-(l/2))) {
      ymin=y-(l/2);
    }
    if (xmax<(x+(w/2))) {
      xmax=x+(w/2);
    }
    if (ymax<(y+(l/2))) {
      ymax=y+(l/2);
    }
  }

}

void CircleBB(double &xmin, double &xmax, double &ymin, double &ymax, string s, int c) {
  double x,y,r;
  cin>>s;
  x=stod(s);
  cin>>s;
  y=stod(s);
  cin>>s;
  r=stod(s);

  if (c==1) {
    xmin=x-r;
    ymin=y-r;
    xmax=x+r;
    ymax=y+r;
  }
  else {
    if (xmin>(x-(r))) {
      xmin=x-(r);
    }
    if (ymin>(y-(r))) {
      ymin=y-(r);
    }
    if (xmax<(x+(r))) {
      xmax=x+(r);
    }
    if (ymax<(y+(r))) {
      ymax=y+(r);
    }
  }
}


void PointSetBB(double &xmin, double &xmax, double &ymin, double &ymax, string s, int c) {
  int n;
  double x,y;
  cin>>s;
  n=stoi(s);

  for (int i=0;i<n;i++) {
    cin>>s;
    x=stod(s);
    cin>>s;
    y=stod(s);

    if (c==1 && i==0) {
      xmin=x;
      ymin=y;
      xmax=x;
      ymax=y;
    }
    else {
      if (xmin>x) {
        xmin=x;
      }
      if (xmax<x) {
        xmax=x;
      }
      if (ymin>y) {
        ymin=y;
      }
      if (ymax<y) {
        ymax=y;
      }
    }
  }

} 

int main() {
  string s;
  int c=0;
  double xmin, xmax, ymin, ymax;
  while (1) {
    cin>>s;
    c++;
    if (s[0]=='R') {
      RectangleBB(xmin,xmax,ymin,ymax,s,c);
    }
    else if (s[0]=='C') {
      CircleBB(xmin,xmax,ymin,ymax,s,c);
    }
    else if (s[0]=='P') {
      PointSetBB(xmin,xmax,ymin,ymax,s,c);
    }
    else {
      break;
    }
  }  
  cout<<((xmin+xmax)/2)<<" "<<((ymin+ymax)/2)<<" "<<(xmax-xmin)<<" "<<(ymax-ymin)<<endl;

  return 0;
}

