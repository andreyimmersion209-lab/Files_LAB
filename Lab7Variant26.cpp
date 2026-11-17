/******************************
 * Автор:   Хан Кирилл        *
 * Задание: Работа с файлами  *
 * Вариант: 26                *
 ******************************/

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Car {
  string manufacturer;
  string model;
  double engineVolume;
  double price;
};

bool validText(const string& text) {
  if (text.empty() || text.find_first_not_of(' ') == string::npos) {
    return false;
  }
  for (size_t symbolIndex = 0; symbolIndex < text.size(); ++symbolIndex) {
    if (text[symbolIndex] < ' ' || text[symbolIndex] > '~' || text[symbolIndex] == ';') {
      return false;
    }
  }
  return true;
}

vector<string> splitFields(const string& line) {
  vector<string> fields;
  size_t start = 0, separator;
  while (true) {
    separator = line.find(';', start);
    if (separator == string::npos) {
      fields.push_back(line.substr(start));
      break;
    }
    fields.push_back(line.substr(start, separator - start));
    start = separator + 1;
  }
  return fields;
}

bool readDouble(const string& text, double& value) {
  istringstream parser(text);
  char extra;
  if (!(parser >> value) || !isfinite(value)) {
    return false;
  }
  return !(parser >> extra);
}

bool parseCar(const string& line, Car& car) {
  const size_t fieldCount = 4;
  vector<string> fields;
  fields = splitFields(line);
  if (fields.size() != fieldCount || !validText(fields[0]) || !validText(fields[1])) {
    return false;
  }
  if (!readDouble(fields[2], car.engineVolume) || car.engineVolume <= 0.0 ||
      !readDouble(fields[3], car.price) || car.price < 0.0) {
    return false;
  }
  car.manufacturer = fields[0];
  car.model = fields[1];
  return true;
}

bool readCars(const string& path, vector<Car>& cars, string& error) {
  const int maxCount = 100;
  ifstream input(path);
  string line;
  Car current;
  int lineNumber = 0;
  cars.clear();
  if (!input) {
    error = "ERROR input";
    return false;
  }
  while (getline(input, line)) {
    ++lineNumber;
    if (!line.empty() && line[line.size() - 1] == '\r') {
      line.erase(line.size() - 1);
    }
    if (lineNumber > maxCount || !parseCar(line, current)) {
      error = "ERROR line " + to_string(lineNumber);
      return false;
    }
    cars.push_back(current);
  }
  if (input.bad()) {
    error = "ERROR read";
    return false;
  }
  return true;
}

vector<Car> selectCars(const vector<Car>& cars, const string& manufacturer) {
  vector<Car> selected;
  for (size_t carIndex = 0; carIndex < cars.size(); ++carIndex) {
    if (cars[carIndex].manufacturer == manufacturer) {
      selected.push_back(cars[carIndex]);
    }
  }
  return selected;
}

bool averagePrice(const vector<Car>& cars, double& average) {
  double total = 0.0;
  if (cars.empty()) {
    return false;
  }
  for (size_t carIndex = 0; carIndex < cars.size(); ++carIndex) {
    total += cars[carIndex].price;
    if (!isfinite(total)) {
      return false;
    }
  }
  average = total / cars.size();
  return true;
}

size_t largestEngineIndex(const vector<Car>& cars) {
  size_t largestIndex = 0;
  for (size_t carIndex = 1; carIndex < cars.size(); ++carIndex) {
    if (cars[carIndex].engineVolume > cars[largestIndex].engineVolume) {
      largestIndex = carIndex;
    }
  }
  return largestIndex;
}

void appendCar(ostringstream& report, const Car& car) {
  report << car.manufacturer << ";" << car.model << ";" << car.engineVolume << ";" << car.price << "\n";
}

string makeReport(const vector<Car>& cars, const vector<Car>& selected, const string& manufacturer,
                  double average, size_t largestIndex) {
  ostringstream report;
  report << "Manufacturer: " << manufacturer << "\nCars:\n";
  if (selected.empty()) {
    report << "EMPTY\n";
  } else {
    for (size_t carIndex = 0; carIndex < selected.size(); ++carIndex) {
      appendCar(report, selected[carIndex]);
    }
  }
  report << "Count: " << selected.size() << "\nAverage price: ";
  if (selected.empty()) {
    report << "NONE\n";
  } else {
    report << average << "\n";
  }
  report << "Largest engine (all cars):\n";
  if (cars.empty()) {
    report << "NONE\n";
  } else {
    appendCar(report, cars[largestIndex]);
  }
  return report.str();
}

bool saveReport(const string& path, const string& report, string& error) {
  ofstream output(path);
  if (!output) {
    error = "ERROR output";
    return false;
  }
  output << report;
  output.flush();
  output.close();
  if (!output) {
    error = "ERROR write";
    return false;
  }
  return true;
}

int main() {
  string inputPath, outputPath, manufacturer, error, report;
  vector<Car> cars, selected;
  double average = 0.0;
  size_t largestIndex = 0;

  if (!getline(cin, inputPath) || !getline(cin, outputPath) || !getline(cin, manufacturer) ||
      inputPath.empty() || outputPath.empty()) {
    cout << "ERROR parameters\n";
    return 1;
  }
  if (inputPath == outputPath) {
    cout << "ERROR paths\n";
    return 1;
  }
  if (!validText(manufacturer)) {
    cout << "ERROR query\n";
    return 1;
  }
  if (!readCars(inputPath, cars, error)) {
    cout << error << "\n";
    return 1;
  }

  selected = selectCars(cars, manufacturer);
  if (!selected.empty() && !averagePrice(selected, average)) {
    cout << "ERROR calculation\n";
    return 1;
  }
  if (!cars.empty()) {
    largestIndex = largestEngineIndex(cars);
  }
  report = makeReport(cars, selected, manufacturer, average, largestIndex);

  if (!saveReport(outputPath, report, error)) {
    cout << error << "\n";
    return 1;
  }
  cout << report;
  return 0;
}
