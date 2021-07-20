// Author: Lin Jianan
// U. No.: 3035577595
// Filename: d2boh.cpp
// Assignment 2
// Description: This program contains 3 functions that convert decimal number to binary, octal, and hexadecimal number.
// Last Changed: November 7, 2020

#include <iostream>
#include "d2boh.h"
using namespace std;

int decimal_to_binary(int input, int output[10]) {
  //Convert deciaml number to binary number.
  int count=0;
  while (input!=0) {
    output[count]=input%2;
    input=input/2;
    count++;
  }
  return (count-1);
  
}

int decimal_to_octal(int input, int output[10]) {
  //Convert deciaml number to octal number.
  int count=0;
  while (input!=0) {
    output[count]=input%8;
    input=input/8;
    count++;
  }
  return (count-1);

}

int decimal_to_hexadecimal(int input, char output[10]) {
  //Convert deciaml number to binary number.
  int count=0;
  while (input!=0) {
    if (input%16 <10)
      output[count]='0'+(input%16);
    else if (input%16==10)
      output[count]='A';
    else if (input%16==11)
      output[count]='B';
    else if (input%16==12)
      output[count]='C';
    else if (input%16==13)
      output[count]='D';
    else if (input%16==14)
      output[count]='E';
    else if (input%16==15)
      output[count]='F';
   
    input=input/16;
    count++;
  }
  return (count-1);

}
