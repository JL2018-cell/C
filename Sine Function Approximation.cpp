#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;
//Plm 1: deal with overflow.
//Plm 2: precision is not correct: 1 is shown instead of 1.000000000000000
/*
int f(int i) {
  if (i==0) return 1;
  else return i*f(i-1);
}
*/

double f(int i) {
  double r=1;
  for (int j=0;j<i;j++) {
    r*=(j+1);
  }
  return r;
}


int main() {
  double sum;
  cout<<std::fixed;
  setprecision(15);
  cout<<std::fixed;
  sum=0.0;
  double x,t;
  int n;
  cin>>x>>n;
  cout<<"sin(x) by cmath: "<<setprecision(15)<<(sin(x))<<endl;
  //for (int i=0;i<=n;i++) {
  int i=0;
  cout<<"Taylor series approximation:"<<endl;
  do {
    t=double((pow(-1,i)))/(f((2*i)+1));
    sum+=t*pow(x,(2*i)+1);
    cout<<i<<" "<<setprecision(15)<<sum<<endl;
    i++;
  }while(i<=n);

  return 0;
}
