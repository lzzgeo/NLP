/*
  Darts -- Double-ARray Trie System

  $Id: mkdarts.cpp 1674 2008-03-22 11:21:34Z taku $;

  Copyright(C) 2001-2007 Taku Kudo <taku@chasen.org>
  All rights reserved.
*/
#include <darts.h>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <set>

using namespace std;

int progress_bar(size_t current, size_t total) {
  static char bar[] = "*******************************************";
  static int scale = sizeof(bar) - 1;
  static int prev = 0;

  int cur_percentage  = static_cast<int>(100.0 * current/total);
  int bar_len         = static_cast<int>(1.0   * current*scale/total);

  if (prev != cur_percentage) {
    printf("Making Double Array: %3d%% |%.*s%*s| ",
           cur_percentage, bar_len, bar, scale - bar_len, "");
    if (cur_percentage == 100)  printf("\n");
    else                        printf("\r");
    fflush(stdout);
  }

  prev = cur_percentage;

  return 1;
};

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " File Index" << std::endl;
    return -1;
  }

  std::string file  = argv[argc-2];
  std::string index = argv[argc-1];
  

  Darts::DoubleArray da;
  //Darts::DoubleArrayUint16 da2;

  std::vector<const char *> key;
  std::istream *is;

  if (file == "-") {
    is = &std::cin;
  } else {
    is = new std::ifstream(file.c_str());
  }

  if (!*is) {
    std::cerr << "Cannot Open: " << file << std::endl;
    return -1;
  }

  set<string>   keyset;
  std::string line;
  while (std::getline(*is, line)) {
    keyset.insert(line);
  }
  if (file != "-") delete is;
  
  printf("vector size: %zu\n", keyset.size());


  set<string>::iterator itr = keyset.begin();
  set<string>::iterator itrEnd= keyset.end();
  while (itr!=itrEnd) {
    char *tmp = new char[itr->size()+1];
    std::strcpy(tmp, itr->c_str());
    key.push_back(tmp);

    printf("%s\n", itr->c_str());
    itr++;
  }


  if (da.build(key.size(), &key[0], 0, 0, &progress_bar) != 0
      || da.save(index.c_str()) != 0) {
    std::cerr << "Error: cannot build double array  " << file << std::endl;
    return -1;
  };

  for (unsigned int i = 0; i < key.size(); i++)
    delete [] key[i];

  std::cout << "Done!, Compression Ratio: " <<
    100.0 * da.nonzero_size() / da.size() << " %" << std::endl;

  return 0;
}
