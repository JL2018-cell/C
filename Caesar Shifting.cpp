#include <iostream>
using namespace std;

char CaesarShift(char c, int k) {
  int val = c;
  char c2;
  int val2;
  if (val>=65 && val<=90) {
    if ((val+k)>90) {
      c2=97+k-(90-val)-1;
      return c2;
    }
    else if ((val+k)<65) {
      c2=122-(-k-(val-65))+1;
      return c2;
    }
    else {
      val2 = val + k;
      c2=(val2+32);
      return c2;
    }
  }
  else if (val>=97 && val<=122) {
    if ((val+k)>122) {
      c2=65+k-(122-val)-1;
      return c2;
    }
    else if ((val+k)<97) {
      c2=90-(-k-(val-97))+1;
      return c2;
    }
    else {
      val2 = val+k;
      c2=(val2-32);
      return c2;
    }
  }
  else {
    return c;
  }
}

int main() {
  string s;
  char state;
  char d='d';
  char D='D';
  char tmp;
  int k;
  string s2;
  s2="";
  cin>>state>>k>>s;
  //cout<<"s="<<s<<endl;
  if (state==d || state==D) {
    while (s!="!") {
      tmp=(CaesarShift(s[0],-(k%26)));
      //cout<<"s[0]="<<s[0]<<endl;
      cin>>s;
      s2+=tmp;
    }
  }
  else {
    while (s!="!") {
      tmp=(CaesarShift(s[0],k%26));
      //cout<<"s[0]="<<s[0]<<endl;
      cin>>s;
      s2+=tmp;
    }
  }
  //s2+=tmp;
  //s2.append(tmp);
  cout<<s2<<"!"<<endl;
  return 0;
}


