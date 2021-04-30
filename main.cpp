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
#include <map>

using namespace std;
namespace fs = std::experimental::filesystem;

struct Report {
  int confirmed;
  int deaths;
  int recovered;
};
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

void readOneDailyReport(string filename, map<string, map<string, Report>> & data)
{
  ifstream infile(filename);

  if( infile.is_open() )
  {
    string line;
    getline(infile, line); // skip first line (header row):
    while (getline(infile, line))
    {
      if (line[0] == '"') // => province is "city, state"
      {
        //
        // we want to delete the " on either end of the value, and
        // delete the ',' embedded within => will get city state:
        //
        line.erase(0, 1); // delete the leading "
        size_t pos = line.find(','); // find embedded ','
        line.erase(pos, 1); // delete ','
        pos = line.find('"'); // find closing "
        line.erase(pos, 1); // delete closing "
      }

      stringstream s(line);
      string province, country, last_update;
      string confirmed, deaths, recovered;
      getline(s, province, ',');
      getline(s, country, ',');
      getline(s, last_update, ',');
      getline(s, confirmed, ',');

      getline(s, deaths, ',');
      getline(s, recovered, ',');
      if (confirmed == "")
        confirmed = "0";
      if (deaths == "")
        deaths = "0";
      if (recovered == "")
        recovered = "0";
      if (country == "Mainland China") // map to name in the earlier reports:
        country = "China";

      int confirmed_n = stoi(confirmed);
      int deaths_n = stoi(deaths);
      int recovered_n = stoi(recovered);
      
      // change date to standard
      int pos = last_update.find('/');
      string year, month, day;
      if( pos >= 0 )
      {
        day = last_update.substr(0,pos);
        if( day.length() < 2 )
          day = "0" + day;
        int pos1 = last_update.find('/', pos + 1);
        month = last_update.substr(pos + 1, pos1 - pos - 1);
        if( month.length() < 2 )
          month = "0" + month;
        int pos2 = last_update.find(' ', pos1 + 1);
        year = last_update.substr(pos1 + 1, pos2 - pos1 - 1);
        year = "20" + year;
      }
      
      pos = last_update.find('-');

      if( pos >= 0 )
      {
        year = last_update.substr(0,pos);
        int pos1 = last_update.find('-', pos + 1);
        month = last_update.substr(pos + 1, pos1 - pos - 1);
        int pos2 = last_update.find('T', pos1 + 1);
        day = last_update.substr(pos1 + 1, pos2 - pos1 - 1);
      }

      string date = day + "-" + month + "-" + year;

      // check if country key exists
      auto iter_country = data.find(country);
      if( iter_country == data.end() ) // not found
      {
        map<string, Report> report_map;

        Report report;
        report.confirmed = confirmed_n;
        report.recovered = recovered_n;
        report.deaths = deaths_n;
        report_map[date] = report; 

        data[country] = report_map;
      }
      else
      {
        map<string, Report> date_map = data[country];
        auto iter_date = date_map.find(date);
        if( iter_date == date_map.end() )
        {
          Report report;
          report.confirmed = confirmed_n;
          report.recovered = recovered_n;
          report.deaths = deaths_n;
          
          date_map[date] = report;
        }
        else
        {
          date_map[date].confirmed += confirmed_n;
          date_map[date].recovered += recovered_n;
          date_map[date].deaths += deaths_n;
        }
      }
    }

    infile.close();
  }
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
  vector<string> worldfacts_files = getFilesWithinFolder("./worldfacts/");
  
  
  //
  // TODO:
  //

  cout << ">> Processed " << files.size() << " daily reports" << endl;
  cout << ">> Processed " << worldfacts_files.size() << " files of world facts" << endl;

  // iterate the daily report files
  map<string, map<string, Report>> data;
  for (string filename : files)
  {
    readOneDailyReport(filename, data);
  }

  cout << ">> Current data on " << data.size() << " countries" << endl;


  return 0;
}
