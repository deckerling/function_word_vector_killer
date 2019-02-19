// killer.cc

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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include "function_word_vector_killer.h"

Killer::Killer(const std::string &input_file, const std::string &output_file, const std::vector<bool> &words_to_remove, const int language)
    : input_file_(input_file),
      output_file_(output_file),
      words_to_remove_(words_to_remove),
      language_(language) {
  SelectAndRemoveWords();
}

Killer::~Killer() {}

void Killer::SelectAndRemoveWords() {
  BinaryTree function_words_to_remove;
  auto rng = std::default_random_engine {};
  std::vector<std::string> list_of_words_to_remove;
  const std::vector<std::string> labels_of_words_to_remove = {"Adpositions:\n\t", "Articles etc. (various pronouns, demonstratives, ...):\n\t", "Conjunctions and subjunctions:\n\t", "Interjections:\n\t", "Interrogative words:\n\t", "Numerals:\n\t", "Particles:\n\t", "Personal pronouns and possessives:\n\t", "Temporal words:\n\t", "Miscellaneous words:\n\t"};
  std::cout << "\nThe following words will be removed from your word vector file (\"" << input_file_ << "\"):\n";
  for (unsigned i = 0; i < words_to_remove_.size(); ++i) {
    if (words_to_remove_[i]) {
      std::cout << labels_of_words_to_remove[i];
      list_of_words_to_remove = LoadFunctionWords(i);
      if (!list_of_words_to_remove.empty()) {
        // Prints the "words_to_remove_" in the same order as they can be found
        // in their txt-file.
        for (unsigned j = 0; j < list_of_words_to_remove.size(); ++j) {
          std::cout << list_of_words_to_remove[j];
          if (j != (list_of_words_to_remove.size()-1))
            std::cout << ", ";
        }
        std::cout << '\n';
      }
      // Shuffles the vector "list_of_words_to_remove" in order to improve the
      // binary tree that is about to be built (the lists of function words in
      // the directory "data" are sorted in alphabetical order (except for the
      // numerals) to make it easier for the user to find a certain word in
      // those files).
      std::shuffle(std::begin(list_of_words_to_remove), std::end(list_of_words_to_remove), rng);
      for (unsigned j = 0; j < list_of_words_to_remove.size(); ++j)
        function_words_to_remove.AddWord(list_of_words_to_remove[j]); // adds the selected functions words to the binary tree
    }
  }
  std::cout << "\n\tCreating new file (\"" << output_file_ << "\") without those words and their vectors..." << std::endl;
  int num_of_removed_vectors = 0, num_of_checked_vectors = 0, flush_interval_factor = 1;
  std::string line;
  std::string word;
  std::ifstream input_file_stream(input_file_);
  std::ofstream output_file_stream(output_file_);
  // Reads "input_file_", checks for every line if the word vector should be
  // removed and writes those that shouldn't be removed in "output_file_".
  while (std::getline(input_file_stream, line)) {
    word = SetToLowerCase(line.substr(0, line.find_first_of(' '))); // note that the word comparison is not case sensitive!
    if (words_to_remove_[5] && IsNumber(word)) // if numerals are selected to be removed every "word vector" representing a numeric string will be removed
      num_of_removed_vectors++;
    // "RemoveWord()" returns "true" if "word" was both found and removed from
    // the binary tree, and "false" if not (removing the word makes the binary
    // tree smaller and therefore upcoming searches in many cases faster).
    else if (!function_words_to_remove.RemoveWord(word))
      output_file_stream << line << '\n';
    else
      num_of_removed_vectors++;
    // Flushes the "output_file_stream" in an interval of 1000 (with respect to
    // the number of word vectors that have already been checked).
    num_of_checked_vectors++;
    if (num_of_checked_vectors == (1000*flush_interval_factor)) {
      output_file_stream.flush();
      flush_interval_factor++;
    }
  }
  std::cout << "\t---Done.\n";
  std::cout << "\nNumber of removed word vectors = " << num_of_removed_vectors << '\n';
}

std::vector<std::string> Killer::LoadFunctionWords(const int index) {
// Loads the function words via index (the function words should be saved in
// txt-files (check the paths!)).
  const std::string directory_to_open = (language_ == 0)? "data/english" : "data/german";
  std::string file_to_open;
  std::string line;
  switch (index) {
    case 0:
      file_to_open = "adpositions.txt";
      break;
    case 1:
      file_to_open = "articles_and_the_like.txt";
      break;
    case 2:
      file_to_open = "conjunctions_and_subjunctions.txt";
      break;
    case 3:
      file_to_open = "interjections.txt";
      break;
    case 4:
      file_to_open = "interrogative_words.txt";
      break;
    case 5:
      file_to_open = "numerals.txt";
      break;
    case 6:
      file_to_open = "particles.txt";
      break;
    case 7:
      file_to_open = "personal_pronouns_and_possessives.txt";
      break;
    case 8:
      file_to_open = "temporal_words.txt";
      break;
    case 9:
      file_to_open = "miscellaneous_words.txt";
      break;
  }
  if (!CheckFile(directory_to_open+"/"+file_to_open)) {
    std::cerr << "\tThose function words couldn't be added to the words to remove.\n";
    return {}; // returns an empty vector if the file is bad or couldn't be found
  } else {
    std::ifstream file_stream(directory_to_open+"/"+file_to_open);
    std::vector<std::string> list_of_words_to_remove;
    while (std::getline(file_stream, line))
      list_of_words_to_remove = GetWordsFromALine(line);
    return list_of_words_to_remove;
  }
}

std::vector<std::string> Killer::GetWordsFromALine(const std::string &line) {
// Returns a std::vector containing all words found in a line (every line will
// be equal to the content of a function word file).
  std::stringstream stream(line);
  std::string word;
  std::vector<std::string> words;
  while (getline(stream, word, ' '))
    words.push_back(word);
  return words;
}

bool Killer::IsNumber(const std::string &word) {
// Returns "true" if "word" (i.e. a string) is a number and "false" otherwise.
  int fractional_count = 0;
  for (unsigned i = 0; i < word.length(); ++i) {
    if (std::isdigit(word[i]) == 0) {
      if (i > 0 && fractional_count == 0 && (word[i] == '.' && word[i] == ','))
        fractional_count = 1;
      else if (i != 0 || (word[0] != '+' && word[0] != '-'))
        return false;
    }
  }
  return true;
}
