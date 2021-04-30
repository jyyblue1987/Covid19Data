/*main.cpp*/

//
// header comment???
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <experimental/filesystem>
#include <locale>
#include <iomanip>
#include <vector>

using namespace std;
namespace fs = std::experimental::filesystem;


//
// getFilesWithinFolder
//
// Given the path to a folder, e.g. "./daily_reports/", returns 
// a vector containing the full pathnames of all regular files
// in this folder.  If the folder is empty, the vector is empty.
//
vector<string> getFilesWithinFolder(string folderPath)
{
  vector<string> files;

  for (const auto& entry : fs::directory_iterator(folderPath))
  {
    files.push_back(entry.path().string());
  }

  return files;
}


//
// main:
//
int main()
{
  cout << "** COVID-19 Data Analysis **" << endl;
  cout << endl;
  cout << "Based on data made available by John Hopkins University" << endl;
  cout << "https://github.com/CSSEGISandData/COVID-19" << endl;
  cout << endl;

  //
  // setup cout to use thousands separator, and 2 decimal places:
  //
  cout.imbue(std::locale(""));
  cout << std::fixed;
  cout << std::setprecision(2);

  //
  // get a vector of the daily reports, one filename for each:
  //
  vector<string> files = getFilesWithinFolder("./daily_reports/");
  
  
  //
  // TODO:
  //


  return 0;
}
