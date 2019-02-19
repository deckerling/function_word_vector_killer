// binary_tree.cc

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

#include <iostream>
#include <regex>

#include "function_word_vector_killer.h"

BinaryTree::BinaryTree() {
  root_ = NULL;
}

BinaryTree::~BinaryTree() {
  if (root_ != NULL)
    DestroyTree(root_);
}

void BinaryTree::DestroyTree(struct WordItem *current_word_item) {
  if (current_word_item->left != NULL)
    DestroyTree(current_word_item->left);
  if (current_word_item->right != NULL)
    DestroyTree(current_word_item->right);
  delete current_word_item;
}

void BinaryTree::AddWord(const std::string &word_to_add) {
  struct WordItem *new_word_item = new struct WordItem;
  new_word_item->word = word_to_add;
  if (root_ == NULL)
    root_ = new_word_item;
  else {
    struct WordItem *current_word_item = root_;
    while (true) {
      if (current_word_item->word == word_to_add) // avoids multiple additions of the same word (or rather: the same signifier)
        return;
      // If "word_to_add" is shorter than "current_word_item->word" but has the
      // same letters (e.g. "word_to_add" == "dein" and
      // "current_word_item->word" == "deiner") it is stored on the left of
      // "current_word_item".
      if (word_to_add.compare(current_word_item->word) < 0) {
        if (current_word_item->left == NULL) {
          current_word_item->left = new_word_item;
          return;
        } else
          current_word_item = current_word_item->left;
      } else {
        if (current_word_item->right == NULL) {
          current_word_item->right = new_word_item;
          return;
        } else
          current_word_item = current_word_item->right;
      }
    }
  }
}

bool BinaryTree::RemoveWord(const std::string &word_to_remove) {
// Searches for and removes the word (or rather the WordItem) that shall be
// removed from the "BinaryTree". Returns "false" if it can't find and
// therefore can't remove "word_to_remove" and returns "true" if it is found
// and removed.
  if (root_ == NULL)
    return false;
  if (root_->word == word_to_remove) { // if "root_" is the "WordItem" to remove
    if (root_->left != NULL) { // if "root_" has a child on its left side
      RemoveWordItemThatHasTwoChildren(root_);
      // This works because "RemoveWordItemThatHasTwoChildren()" always deletes
      // the "word_item_to_delete" by finding the rightest item on the left
      // below it and swapping their information in order to delete the
      // "word_item_to_delete" or rather its information (at the position of
      // its former rightest item on its left).
    } else if (root_->right != NULL) { // if "root_" has no child on its left side, but on its right side
      struct WordItem *previous_word_item = root_, *most_left_word_item_right_below_root = root_->right;
      int side;
      while (most_left_word_item_right_below_root->left != NULL || most_left_word_item_right_below_root->right != NULL) { // finds the most left item on the right below "root_"
        if (most_left_word_item_right_below_root->left != NULL) {
          side = -1;
          previous_word_item = most_left_word_item_right_below_root;
          most_left_word_item_right_below_root = most_left_word_item_right_below_root->left;
        } else if (most_left_word_item_right_below_root->right != NULL) {
          side = 1;
          previous_word_item = most_left_word_item_right_below_root;
          most_left_word_item_right_below_root = most_left_word_item_right_below_root->right;
        }
      }
      root_->word = most_left_word_item_right_below_root->word;
      (side == -1)? previous_word_item->left = NULL : previous_word_item->right = NULL;
      delete most_left_word_item_right_below_root;
    } else { // if "root_" has no children
      struct WordItem *current_word_item = root_;
      root_ = NULL;
      delete current_word_item;
    }
    return true;
  }
  // if "root_" is not the "word_item_to_delete"
  struct WordItem *current_word_item = root_;
  while (true) {
    if (word_to_remove.compare(current_word_item->word) < 0) {
      if (current_word_item->left == NULL) // if "word_to_remove" could'nt be found in the binary tree
        return false;
      else if (current_word_item->left->word == word_to_remove) {
        RemoveWordItem(current_word_item, current_word_item->left, -1);
        return true;
      } else
        current_word_item = current_word_item->left;
    } else {
      if (current_word_item->right == NULL) // if "word_to_remove" could'nt be found in the binary tree
        return false;
      else if (current_word_item->right->word == word_to_remove) {
        RemoveWordItem(current_word_item, current_word_item->right, 1);
        return true;
      } else
        current_word_item = current_word_item->right;
    }
  }
}

void BinaryTree::RemoveWordItem(struct WordItem *previous_word_item, struct WordItem *word_item_to_delete, const int side){
// If "side" == -1 -> "word_item_to_delete" is on the left of
// "previous_word_item"; if "side" == 1 -> "word_item_to_delete" is on the
// right of "previous_word_item".
  // If "word_item_to_delete" has no children: resets the pointer to it and
  // deletes it.
  if (word_item_to_delete->left == NULL && word_item_to_delete->right == NULL) {
    (side < 0)? previous_word_item->left = NULL : previous_word_item->right = NULL;
    delete word_item_to_delete;
  // If "word_item_to_delete" has one child: resets the pointer (which formerly
  // pointed to "word_item_to_delete") so that it is pointing to the child,
  // then deletes "word_item_to_delete".
  } else if (word_item_to_delete->left == NULL) { // if the child of "word_item_to_delete" is on its right
    (side < 0)? previous_word_item->left = word_item_to_delete->right : previous_word_item->right = word_item_to_delete->right;
    delete word_item_to_delete;
  } else if (word_item_to_delete->right == NULL) { // if the child of "word_item_to_delete" is on its left
    (side < 0)? previous_word_item->left = word_item_to_delete->left : previous_word_item->right = word_item_to_delete->left;
    delete word_item_to_delete;
  } else // if "word_item_to_delete" has two children
    RemoveWordItemThatHasTwoChildren(word_item_to_delete);
}

void BinaryTree::RemoveWordItemThatHasTwoChildren(struct WordItem *word_item_to_delete) {
// Finds the rightest item on the left below "word_item_to_delete" and swaps
// the information of both items, then deletes "word_item_to_delete" or rather
// its information (at the position of its former rightest item on its left)
// and sets the pointer to it "NULL".
  struct WordItem *previous_word_item = word_item_to_delete, *rightest_word_item_left_below = word_item_to_delete->left;
  while (rightest_word_item_left_below->right != NULL) {
    previous_word_item = rightest_word_item_left_below;
    rightest_word_item_left_below = rightest_word_item_left_below->right;
  }
  word_item_to_delete->word = rightest_word_item_left_below->word;
  if (word_item_to_delete != previous_word_item)
    previous_word_item->right = (rightest_word_item_left_below->left == NULL)? NULL : rightest_word_item_left_below->left;
  else
    word_item_to_delete->left = (rightest_word_item_left_below->left == NULL)? NULL : rightest_word_item_left_below->left;
  delete rightest_word_item_left_below;
}
