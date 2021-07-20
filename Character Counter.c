// Author: Lin Jianan
// U. No.: 3035577595
// Filename: 4.c
// Assignment 2
// Description: count the number of occurrences of each character is an input string.
// Last Changed: November 7, 2020

#include <stdio.h>

const int MAX_TEXT_LEN = 50;

//Initialize int array: fill 0 into every entry of array.
void init2(int a[]) {
  for (int i=0;i<MAX_TEXT_LEN;i++) {
    a[i]=0;
  }
}

//convert every character to small letter.
char convert_small(char c) {
  if (c>=65 && c<=90) {
    c+=32;
  }
  return c;
}

//Initialize char array: fill '\0' into every entry of array.
void init(char a[]) {
  for (int i=0;i<MAX_TEXT_LEN;i++) {
    a[i]='\0';
  }
}

//Judge whether a character is present in the array.
int prsnt(char elm, char a[]) {
  int i=0;
  while (a[i]!='\0') {
    if (elm>=65 && elm<=90 && ((elm==a[i]) || (elm+32==a[i]))) {
      return i;
    }
    else if (elm>=97 && elm<=122 && ((elm==a[i]) || (elm-32==a[i]))) {
      return i;
    }
    else if (elm==a[i]) // desired element is present in string.
      return i;
    i++;
  }
  return 100; //no desired element is present in string.
}

//Display characters appeared in input string and corresponding frequency.
void show(int Freq[], char Syn[]) {
  int i=0;
  while (Syn[i]!='\0') {
    printf("%c %d\n",convert_small(Syn[i]),Freq[i]);
    i++;
  }
}

int main()
{
  char text[MAX_TEXT_LEN+1]; //Record input string.
  char Syn[MAX_TEXT_LEN+1]; //Record what character(s) appear(s) in input string
  int Freq[MAX_TEXT_LEN+1]; //Record frequency that character appears
  init(text); //initialize array
  init(Syn);
  init2(Freq);
  scanf("%s", text);
  // printf("%s\n", text);
  int i=0;
  int j=0;
  int k;
  while (text[i]!='\0') { 
  //Scan every character of input string and record characters appear 
  //and corresponding frequency.
    if (text[i]=='M') 
      printf("Detect M\n");
    k=prsnt(text[i],Syn);
    if (k!=100) {  // desired element is present in string.
      Freq[k]+=1;
    }
    else { //no desired element is present in string.
      Syn[j]=text[i];
      Freq[j]=1;
      j++;
    }
    i++;
  }

  show(Freq, Syn); //Display result.
  return 0;
}

