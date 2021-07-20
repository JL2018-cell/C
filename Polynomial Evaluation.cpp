#include <iostream>
#include <string>
#include <math.h>
#include <iomanip>

// A copy is sotred in t9.cpp

using namespace std;
int main() {
  cout<<std::fixed;
  setprecision(6);
  string s;
  string sx="";
  string sdeg="";
  float a1,a2;
  int c=0;
  cin>>s;
  float x=stod(s);
  cin>>s;
  int deg=stoi(s);
  cin>>s;
  float result=stod(s);
  //float result=pow((stod(s)),deg);
  //cout<<"x="<<x<<"\n"<<"deg="<<deg<<"\n"<<endl;

  for (int i=0;i<deg;i++) {
    cin>>s;
    //cout<<"C: "<<s<<endl;
    a1=stod(s);

    result=(a1+result*x);
  }

  cout<<setprecision(6)<<result<<endl;
  /*
  while (!s.empty()) {
    cout<<"s="<<s<<endl;
    cin>>s;
  }

  for (int i=0;i<2;i++) {
    cin>>s;
  
  }
  */
  /*  
  for (int i=0;i<s.length();i++) {
    if (s[i]==' ') {
      c++;
    }
    else {
      if (c==0) sx+=s[i];
      else if (c==1) sdeg+=s[i];
      else break;
    }
  }

  cout<<"x="<<sx<<endl;
  cout<<"deg="<<sdeg<<endl;
  */
  /*
  cout<<setprecision(6);
  string s,sx,sdeg;
  int deg;
  float x;
  cin>>x;
  cin>>deg;
  float a;
  float result=0;
  for (int i=deg;i>=0;i--) {
    cin>>a;
    result+= a*pow(x,i);
  }
  */
  //cout<<setprecision(6)<< result<<endl;
  /*
  int j=0;
  sx=s;
  cin>>s;
  sdeg=s;
  cin>>s;
  */
  /*
  for(int i=0;i<s.length();i++) {
    if (j==0) {
      sx+=s[i];
    }
    if (j==1) {
      sdeg+=s[i];
    }
    if (s[i]==' ') j++;
    if (j==2) break;
  }
  */
  //cout<<"deg="<<sdeg<<", x="<<sx<<endl;
  return 0;
}
