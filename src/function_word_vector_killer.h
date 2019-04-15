// function_word_vector_killer.h

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

#include <string>
#include <vector>

#ifndef FUNCTIONS_WORD_VECTOR_KILLER_H_SRC_FUNCTIONS_WORD_VECTOR_KILLER_H_
#define FUNCTIONS_WORD_VECTOR_KILLER_H_SRC_FUNCTIONS_WORD_VECTOR_KILLER_H_

bool FileIsValid(const std::string& file);
std::string SetToLowerCase(std::string string);

class Killer {
// Class to collect the words that shall be removed and to store them in a
// binary tree (see the class "BinaryTree"). It will be checked whether the
// words to remove are contained in the "input_file" and if so they and their
// vectors won't be written to the "output_file".
 public:
  Killer(const std::string& input_file, const std::string& output_file, const std::vector<bool>& words_to_remove, const int language);
  ~Killer();

 private:
  const std::string input_file_, output_file_;
  const std::vector<bool> words_to_remove_;
  const int language_;
  void SelectAndRemoveWords();
  std::vector<std::string> LoadFunctionWords(const int index);
  std::vector<std::string> GetWordsFromALine(const std::string& line);
  bool IsNumber(const std::string& word);
};

class BinaryTree {
// Class to store the words that shall be removed in a binary tree.
 public:
  BinaryTree();
  ~BinaryTree();
  void AddWord(const std::string& word_to_add);
  bool RemoveWord(const std::string& word_to_remove);

 private:
  struct WordItem {
    std::string word = "DEFAULT";
    struct WordItem* left = NULL;
    struct WordItem* right = NULL;
  };
  struct WordItem* root_;
  void DestroyTree(struct WordItem* current_word_item);
  void RemoveWordItem(struct WordItem* previous_word_item, struct WordItem* word_item_to_delete, const int side);
  void RemoveWordItemThatHasTwoChildren(struct WordItem* word_item_to_delete);
};

#endif // FUNCTIONS_WORD_VECTOR_KILLER_H_SRC_FUNCTIONS_WORD_VECTOR_KILLER_H_
