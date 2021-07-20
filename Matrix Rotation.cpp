// Author: Lin Jianan
// U. No.: 3035577595
// Filename: 1.cpp
// Assignment 2
// Description: Program to rotate innter matrix clockwise by 90 degrees.
// Last Changed: November 7, 2020


#include <iostream>

const int MAXDIM = 10;

//----------------------DO NOT CHANGE ANYTHING ABOVE THIS LINE------------------
using namespace std;
#include <string>

void read_matrix(int matrix[10][10],int &dim) {
//input dimentsion and elements in matrix.
  cin>>dim;
  string s;
  for (int i=0;i<dim;i++) {
    cin>>s;
    for (int i2=0;i2<dim-1;i2++) {
      matrix[i][i2]=stoi(s);
      cin>>s;
    }
    matrix[i][dim-1]=stoi(s);
  }
}


void display_inner_cw_rotated(int matrix[10][10],int dim) {
//Display result: rotate innter matrix clockwise by 90 degrees.
  for (int j=1;j<dim-1;j++) {
    for (int i=dim-2;i>=1;i--) {
    //for (int j=0;j<dim;j++) {
      cout<<matrix[i][j]<<" ";
    }
    cout<<endl;
  }
}

//----------------------DO NOT CHANGE ANYTHING BELOW THIS LINE------------------



int main()
{
  int matrix[MAXDIM][MAXDIM]{};
  int dim = 0;
  read_matrix(matrix, dim);
  display_inner_cw_rotated(matrix, dim);

  return 0;
}

