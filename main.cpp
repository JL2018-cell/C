// Author: Lin Jianan
// U. No.: 3035577595
// Filename: main.cpp
// Assignment 2
// Description: This program calls functions from d2boh.h to convert decimal number to binary, octal, and hexadecimal number.
// Last Changed: November 7, 2020

#include <iostream>
#include "d2boh.h"
using namespace std;
int main() {
  int input; //input decimal number.
  int output[10]; //contain output in octal ot binary format.
  char outputHex[10]; //contain output in hexadecimal format.

  //The following convert input decimal number 
  //to binary number and display the result.
  cout<<"Enter a positive integer in the range [1, 100]:";
  cin>>input;
  cout<<"Binary of the given number = ";
  for (int i=decimal_to_binary(input,output);i>=0;i--) {
    cout<<output[i];
  }
  cout<<endl;

  //The following convert input decimal number 
  //to octal number and display the result.
  cout<<"Octal of the given number = ";
  for (int i=decimal_to_octal(input,output);i>=0;i--) {
    cout<<output[i];
  }
  cout<<endl;

  //The following convert input decimal number 
  //to hexadecimal number and display the result.
  cout<<"Hexadecimal of the given number = ";
  for (int i=decimal_to_hexadecimal(input,outputHex);i>=0;i--) {
    cout<<outputHex[i];
  }
  cout<<endl;
  return 0;
}

