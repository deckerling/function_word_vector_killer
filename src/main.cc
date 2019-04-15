// main.cc

// Copyright 2019 E. Decker
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fstream>
#include <iostream>
#include <regex>

#include "function_word_vector_killer.h"

std::string SetToLowerCase(std::string string) {
// Sets every character of a string to lower case and returns the string as a
// whole.
  for (auto& character : string)
    character = tolower(character);
  return string;
}

bool FileIsValid(const std::string& file) {
// Checks if "file" is readable and returns "false" if not and "true" otherwise.
  const std::ifstream file_stream(file);
  if (!file_stream.is_open()) {
    std::cerr << "ERROR: OPENING \"" << file << "\" FAILED! Make sure that the file exists and that the path is correct.\n";
    return false;
  } else if (file_stream.bad()) {
    std::cerr << "ERROR: OPENING \"" << file << "\" FAILED!\n";
    return false;
  }
  return true;
}

int main(int argc, char* argv[]) {
  if (argc == 2 || argc == 3) {
    if (!FileIsValid(argv[1])) { // checks if the input file is readable
      std::cout << "Program terminated.";
      return -1;
    }
    const std::string output_file = (argc == 3)? argv[2] : "default_output.txt";
    std::cout << "Input file: \"" << argv[1] << "\"\n";
    std::cout << "Output file: \"" << output_file << "\"\n";
    std::string language;
    std::cout << "\nDo you want to remove English or German function words?\n(Enter \"english\" or \"german\".) ";
    std::cin >> language;
    language = SetToLowerCase(language);
    int language_index;
    if (std::regex_match(language, (std::regex) "eng(lish)?"))
      language_index = 0;
    else if (std::regex_match(language, (std::regex) "ger(man)?"))
      language_index = 1;
    else {
      std::cout << "This program works only on English and German function words!\nProgram terminated.";
      return -1;
    }
    const std::vector<std::string> questions = {" 1/10 - Remove adpositions? ", " 2/10 - Remove articles etc.? ", " 3/10 - Remove conjunctions and subjunctions? ", " 4/10 - Remove interjections? ", " 5/10 - Remove interrogative words? ", " 6/10 - Remove numerals? ", " 7/10 - Remove particles? ", " 8/10 - Remove personal pronouns and possessives? ", " 9/10 - Remove temporal words? ", "10/10 - Remove miscellaneous functions words? "};
    std::vector<bool> words_to_remove(10);
    std::string answer;
    std::cout << "What words do you want to remove from your word vector file?\n(Answer by entering 'y' for \"yes\", 'a' to skip the other questions and remove all function words or enter every other character for \"no\")\n";
    for (unsigned i = 0; i < words_to_remove.size(); ++i) {
      std::cout << questions[i];
      std::cin >> answer;
      if (std::regex_match(SetToLowerCase(answer), (std::regex) "a(ll)?")) {
        std::fill(words_to_remove.begin(), words_to_remove.end(), true);
        break;
      }
      words_to_remove[i] = (std::regex_match(SetToLowerCase(answer), (std::regex) "y(es)?"))? true : false;
    }
    if (std::find(words_to_remove.begin(), words_to_remove.end(), true) == words_to_remove.end()) // terminates program if no "words_to_remove" were selected
      std::cout << "You didn't select words to remove - so there is nothing to do!\n";
    else
      Killer function_word_killer(argv[1], output_file, words_to_remove, language_index); // starts the actual "function word killer"
    std::cout << "\nProgram terminated.";
    return 0;
  }
  std::cerr << ((argc == 1)? "ERROR: MISSING ARGUMENT - No input file given!\n" : "ERROR: TOO MANY ARGUMENTS - Only one input file needed, an output file is optional!\n");
  std::cout << "Style of usage:\n\t.\\function_word_killer [input_file_with_word_vectors] [output_file (optional; default = default_output.txt)]\n";
  std::cout << "Example usage:\n\t.\\function_word_killer my_word_vectors.txt my_important_word_vectors.txt\n";
  std::cout << "Program terminated.";
  return -1;
}
