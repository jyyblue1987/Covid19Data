/*main.cpp*/

//
// header comment???
// Your Name: xxx
// Program Overview: 
//  use a C++ map data structure (and other data structures if needed) to input and analyze daily reports surrounding the Covid-19 virus
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
#include <algorithm>    // std::sort

using namespace std;
namespace fs = std::experimental::filesystem;

struct Report {
  string country;
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

  std::sort(files.begin(), files.end());
  return files;
}

// Purpose: read covid report data from file and save it on map data based on country key
// Params:
//     [IN] filename: input file name
//     [OUT] data: covid 19 report data
// Return Type:
//    None
void readOneDailyReport(string filename, map<string, map<string, Report>> & data)
{
  ifstream infile(filename);

  if( infile.is_open() == false )
    return;
  
  string line;
  getline(infile, line); // skip first line (header row):
  while (getline(infile, line))
  {
    if (line[0] == '"') // => province is "city, state"
    {     
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
    
    int pos3 = filename.rfind(".");
    string date = filename.substr(pos3 - 10, 10);

    // check if country key exists
    auto iter_country = data.find(country);
    
    if( iter_country == data.end() ) // not found
    {
      map<string, Report> report_map;

      Report report;
      report.confirmed = confirmed_n;
      report.recovered = recovered_n;
      report.deaths = deaths_n;
      report.country = country;

      report_map[date] = report; 

      data[country] = report_map;

      continue;
    }
    
    map<string, Report> date_map = data[country];
    auto iter_date = date_map.find(date);
    if( iter_date == date_map.end() )
    {
      Report report;
      report.confirmed = confirmed_n;
      report.recovered = recovered_n;
      report.deaths = deaths_n;
      report.country = country;
      
      data[country][date] = report;
    }
    else
    {
      data[country][date].confirmed += confirmed_n;
      data[country][date].recovered += recovered_n;
      data[country][date].deaths += deaths_n;
    }
    
  }

  infile.close();  
}

// Purpose: read life expectation data from file and save it on map data based on country key
// Params:
//     [IN] filename: input file name
//     [OUT] life_map: life expectation data
// Return Type:
//    None
void readLifeExpect(string filename, map<string, float> &life_map)
{
  ifstream infile(filename);

  if( infile.is_open() == false )
    return;
  
  string line;
  getline(infile, line); // skip first line (header row):
  while (getline(infile, line))
  {
    string num, country, life;

    stringstream s(line);

    getline(s, num, ',');
    getline(s, country, ',');
    getline(s, life, ',');

    life_map[country] = stof(life);
  }

  infile.close();
}

// Purpose: read population data from file and save it on map data based on country key
// Params:
//     [IN] filename: input file name
//     [OUT] life_map: population data
// Return Type:
//    None
void readPopuplate(string filename, map<string, int> &popu_map)
{
  ifstream infile(filename);

  if( infile.is_open() == false )
    return;
  
  string line;
  getline(infile, line); // skip first line (header row):
  while (getline(infile, line))
  {
    string num, country, count;

    stringstream s(line);

    getline(s, num, ',');
    getline(s, country, ',');
    getline(s, count, ',');

    popu_map[country] = stoi(count);
  }

  infile.close();
}

// Purpose: get the maximum date from covid 19 report map data
// Params:
//     [IN] data: report map data
// Return Type:
//      String: maximum report date
string getMaxDate(map<string, map<string, Report>> & data)
{
  string max_date = "";

  // get max date
  for(auto const &x : data) {
    auto const &date_map = x.second;
    auto iter = date_map.rbegin();

    // cout << x.first << ", " << iter->first << endl;
    string date = iter->first;
    if( date > max_date )
      max_date = date;
  }

  return max_date;
}

// Purpose: calculate and display Total statistics data
// Params:
//     [IN] data: report map data
// Return Type:
//      None
void displayTotal(map<string, map<string, Report>> & data)
{
  int confirmed = 0, recovered = 0, deaths = 0;
  string max_date = getMaxDate(data);

  for(auto const &x : data) {
    auto const &date_map = x.second;
    auto iter = date_map.rbegin();

    string date = iter->first;
    if( date != max_date )
    {
      // cout << x.first << "----" << date << endl;
      continue; 
    }

    confirmed += iter->second.confirmed;
    deaths += iter->second.deaths;
    recovered += iter->second.recovered;   
  }

  float deaths_percent = ((float)deaths * 100) / confirmed;
  float recovered_percent = ((float)recovered * 100) / confirmed;

  cout << "As of " << max_date << ", the world-wide totals are:" << endl;
  cout << " confirmed: " << confirmed << endl;
  cout << " deaths: " << deaths << " (" << deaths_percent << "%)" << endl;
  cout << " recovered: " << recovered << " (" << recovered_percent << "%)" << endl;
}

// Purpose: calculate and display Total statistics data group by Country
// Params:
//     [IN] data: report map data
// Return Type:
//      None
void displayCountryData(map<string, map<string, Report>> & data)
{
  string max_date = getMaxDate(data);
  for(auto const &x : data) {
    auto const &date_map = x.second;
    auto iter = date_map.rbegin();
    string date = iter->first;
    if( date != max_date )
    {
      // cout << x.first << "----" << date << endl;
      continue; 
    }

    cout << x.first << ": " << iter->second.confirmed << ", " << iter->second.deaths << ", " << iter->second.recovered << endl;     
  }
}

// Purpose: Comparator function for sorting report data
// Params:
//     [IN] a: compare1
//     [IN] b: compare2
// Return Type:
//      bool: true/false
bool confirmComparator(Report &a, Report &b)
{
  return a.confirmed > b.confirmed;
}

// Purpose: calculate and display top 10 countrie's statistics data
// Params:
//     [IN] data: report map data
// Return Type:
//      None
void displayTop10(map<string, map<string, Report>> & data)
{
  string max_date = getMaxDate(data);
  vector<Report> list;
  for(auto const &x : data) {
    auto const &date_map = x.second;
    auto iter = date_map.rbegin();
    string date = iter->first;
    if( date != max_date )
    {
      continue; 
    }

    list.push_back(iter->second);
  }

  std::sort(list.begin(), list.end(), confirmComparator);

  int i = 1;
  for (Report & report : list)
  {
    if( i > 10 )
      break;
    cout << i << ". " << report.country << ": " << report.confirmed << endl;
    i++;
  }
}

// Purpose: display report data for individual country
// Params:
//     [IN] country: country name
//     [IN] data: report map data
//     [IN] life: life expectation data
//     [IN] popu: population data
// Return Type:
//      None
void displayIndividualCountry(string country, map<string, map<string, Report>> & data, map<string, float> life, map<string, int> popu)
{
  int popu_val = 0;
  if( popu.find(country) != popu.end() )
    popu_val = popu[country];

  float life_val = 0.0f;
  if( life.find(country) != life.end() )
    life_val = life[country];

  cout << "Population: " << popu_val << endl;
  cout << "Life Expectancy: " << life_val << " years" << endl;

  if( data.find(country) == data.end() )
    return;
  
  auto last = data[country].rbegin();

  cout << "Latest data:" << endl;
  cout << " confirmed: " << last->second.confirmed << endl;
  cout << " deaths: " << last->second.deaths << endl;
  cout << " recovered: " << last->second.recovered << endl;

  map<string, Report> country_map = data[country];
  int i = 0;

  string first_confirm = "none";

  for(auto iter = country_map.begin(); iter != country_map.end(); iter++)
  {
    if( iter->second.confirmed > 0 )
    {
      first_confirm = iter->first;
      break;
    }
  }

  string first_deaths = "none";

  for(auto iter = country_map.begin(); iter != country_map.end(); iter++)
  {
    if( iter->second.deaths > 0 )
    {
      first_deaths = iter->first;
      break;
    }
  }
  
  cout << "First confirmed case: " << first_confirm << endl;
  cout << "First recorded death: " << first_deaths << endl;

  cout << "Do you want to see a timeline? Enter c/d/r/n> ";
  string command;
  cin >> command;


  if( command == "c" )
    cout << "Confirmed:";
  if( command == "d" )
    cout << "Deaths:";
  if( command == "r" )
    cout << "Recovered:";

  cout << endl;

  for(auto iter = country_map.begin(); iter != country_map.end(); iter++)
  {      
    int cnt = 0;
    if( command == "c" )
      cnt = iter->second.confirmed;
    if( command == "d" )
      cnt = iter->second.deaths;
    if( command == "r" )
      cnt = iter->second.recovered;

    if( cnt > 0)
      cout << iter->first << " (day " << (i + 1) << "): " << cnt << endl; 

    i++;        
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

  // read life expectation
  map<string, float> life;
  readLifeExpect(worldfacts_files[0], life);

  // read population
  map<string, int> popu;
  readPopuplate(worldfacts_files[1], popu);

  cout << ">> Current data on " << data.size() << " countries" << endl;

  while(true) 
  {
    cout << endl << "Enter command (help for list, # to quit)> ";
    string command;
    cin >> command;
    if( command == "help" )
    {
      cout << "Available commands:" << endl;
      cout << " <name>: enter a country name such as US or China" << endl;
      cout << " countries: list all countries and most recent report" << endl;
      cout << " top10: list of top 10 countries based on most recent # of confired cases" << endl;
      cout << " totals: world-wide totals of confirmed, deaths, recoverd" << endl;
    }
    if( command == "#" )
      break;
    else if( command == "totals" )
    {
      displayTotal(data);
    }
    else if( command == "countries" )
    {
      displayCountryData(data);
    }
    else if( command == "top10" )
    {
      displayTop10(data);
    }
    else {
      auto iter = data.find(command);
      if( iter == data.end() )
      {
        cout << "country or command not found..." << endl;
        continue;
      }

      displayIndividualCountry(command, data, life, popu);
    }

  }
  return 0;
}
