// Author: Lin Jianan
// U. No.: 3035577595
// Filename: 3.cpp
// Assignment 2
// Description: Searches a word from multiple files, and output the number of lines that the word appears and the total number of occurrences of the word in each file.
// Last Changed: November 7, 2020

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

using namespace std;

// Search for a word w in a file named fn
// word: word to search for
// fn: name of file to search
// nLines: (output) number of lines w appear in fn
// total: (output) total number of occurrences of w in fn
// return 0 if file operation is successful
// return 1 otherwise

//convert every letter in a string into small-case letter.
string convert_to_small(string line){
  for (int i=0;i<line.length();i++) {
    line[i]=tolower(line[i]);
  }
  return line;
}

//Search word in every line that is read from file.
int SearchInLine (string word, string line) {
  word.insert(0," ");
  line.insert(0," ");
  word.insert(word.length()," ");
  line.insert(line.length()," ");
  int i,j=0;
  int j2=0;
  int track=0;
  int number=0;
  int pos=0;
  line=convert_to_small(line);
  for (int i=0;i<line.length()/word.length()+1;i++) {
    if ((j=line.find(word,pos))<line.length()) {
      number++;
      pos=j+1;
    }
    else if (j<=0) break;
    else break;
  }
  return number;
}


int SearchWord(string word, string fn, int &nLines, int &total) {
  int track=0;
  string line;
  ifstream fin;
  total=nLines=0;
  word=convert_to_small(word);
  fin.open(fn);
  if (fin.fail()) {
    return 1;
  }
  else {
    while (getline(fin, line)){
      if (line!="") {
        track=SearchInLine(word,line);
        if (track>0) {
          nLines+=1;
          total+=track;
        }
      }
    }
  }
  return 0;
}


// argc is the number of arguments from the command line
// argv[0] == program name
// argv[1..argc-1] are the arguments
// e.g., if this program's name is "prog" and the following command is run
// at the command prompt:
// > ./prog word file1 file2
// then in the main function, argc is 4, and argv[0] is "./prog",
// argv[1] is "word", argv[2] = "file1", argv[3] = "file2"
//
// usage: ./wordsearch word_to_search file1 file2 files3 ...
//
int main(int argc, char* argv[])
{
  if (argc < 3) {
    cout << "usage: " << argv[0] << " [word_to_search] [file1] [file2] [file3]...\n";
    exit(1);
  }

  string word = argv[1];  // word to search for

  // loop through all the filenames in the command line argument
  for (int i = 2; i < argc; ++i) {
    string filename = argv[i];
    int lineCount=0, totalCount=0;
    // search word in file
    if (SearchWord(word, filename, lineCount, totalCount))
      cout << filename << ": error opening file" << endl;
    else
      cout << filename << ": " << lineCount << " " << totalCount << endl;
  }


  return 0;

}


