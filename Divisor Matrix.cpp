#include <iostream>
using namespace std;
int main() {
  int a,b;
  int c,d;
  cout<<"a: ";
  cin>>a;
  cout<<"b: ";
  cin>>b;
  cout<<"Divisor 1: ";
  cin>>c;
  cout<<"Divisor 2: ";
  cin>>d;
  cout<<"M "<<c<<" "<<d<<endl;
  for (int i=a;i<b;i++) {
    cout<<i<<" "<<((i%c)==0)<<" "<<((i%d)==0)<<endl;
  }
  return 0;
}
