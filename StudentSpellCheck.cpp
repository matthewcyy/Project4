#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

SpellCheck* createSpellCheck()
{
    return new StudentSpellCheck;
}

void StudentSpellCheck::trieDestructorHelper(trieNode* node)
{
    if (node == nullptr)
        return;
    for (int i = 0; i < 27; i++)
        trieDestructorHelper(node->children[i]); // Recursively destroying all trie nodes
    delete node;
}

StudentSpellCheck::~StudentSpellCheck() {
    trieDestructorHelper(root);
}

void StudentSpellCheck::insert(trieNode *root, std::string word) // Inserting into root
{
    trieNode *pCrawl = root;
    for (int i = 0; i < word.length(); i++)
    {
        char c = word[i];
        word[i] = tolower(c);
        if ((97 <= c && c <= 122) || c == '\'')
        {
            int index = word[i] - 'a' + 1;
            if (c == '\'')
                index = 0;
            if (!pCrawl->children[index]) // Searching to see if a child/letter is not contained
                pCrawl->children[index] = getNode(); // If not contained, make a new node with it
            pCrawl = pCrawl->children[index]; // Have current node move to the child just created or just looked at
        }
    }
    pCrawl->isEndOfWord = true;
}

bool StudentSpellCheck::search(trieNode *root, std::string word) // Finding word in dictionary, O(L)
{
    trieNode *pCrawl = root;
    for (int i = 0; i < word.length(); i++)
    {
        char c = word[i];
        word[i] = tolower(c);
        int index = word[i] - 'a' + 1;
        if (word[i] == '\'')
            index = 0;
        if (!pCrawl->children[index]) // Searching to see if a child/letter is not contained
            return false; // If doesn't contain appropriate child for next letter, return false
        pCrawl = pCrawl->children[index]; // Move to the node, if contained
    }
    return (pCrawl != nullptr && pCrawl->isEndOfWord); // If reach end of word and pCrawl was not a nullpointer, return true
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
    std::ifstream infile(dictionaryFile);
    if (!infile)
        return false;
    root = getNode();
    std::string s;
    while (getline(infile, s))
        insert(root, s); // Inserting all words into trie
    return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
    for (int i = 0; i < word.size(); i++)
    {
        char c = word[i];
        word[i] = tolower(c); // Making word all lower-case, O(L)
    }
    if (search(root, word)) // O(L) time
        return true;
    else
    {
        suggestions.clear();
        int count = 0;
        for (int i = 0; i < word.size(); i++) // L. For each letter, we will replace the letter with all possible letters and apostrophe and search the dictionary for it. All other letters remain the same
        {
            std::string replacementLook = word;
            for (int j = 0; j < 27; j++) // L * 27
            {
                std::string charLook = "";
                if (j == 0)
                    charLook += '\'';
                else
                    charLook += j + 96;
                replacementLook = replacementLook.replace(i, 1, charLook); // L * 27 * L. Replacing char with new char in alphabet
                if (search(root, replacementLook) && count < max_suggestions) //L * 27 * L
                {
                    suggestions.push_back(replacementLook); // Runs up to max_suggestions times, and is O(1), so L*L*27 + max_suggestions*1. Adds words which are one letter different from word to
                    count++;
                }
            }
        }
        return false;
    }
}

std::string StudentSpellCheck::lineDivider(std::string line, int &startPos, int &endPos)
{
    std::string newWord = "";
    int i = startPos;
    char c = line.at(i);
    bool wasChar = false;
    while (i < line.size()) // Loop through all possible non-letters until the first real letter
    {
        c = line.at(i);
        if (isalpha(c) || c == '\'')
            break;
        i++;
    }
    startPos = i; // Starting location for word you're checking
    while (i < line.size()) // Loop through and add all letters or apostrophes starting at the first letter in the string
    {
        c = line.at(i);
        i++;
        if (!(isalpha(c) || c == '\''))
        {   wasChar = true;
            break;
        }
        newWord += c; // Adding all letters to word
    }
    endPos = i - 2; // Ending location for word found
    if (i == line.size() && !wasChar)
        endPos = i - 1; // Here since when i == line.size(), i won't be incremented so have to properly adjust endPos
    return newWord;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
    int startPos = 0, endPos = 0;
    int lineSize = line.size();
    std::string lineParts = line;
    while (startPos < lineSize) // Maybe chance back to lineSize - 1
    {
        std::string checkWord = lineDivider(lineParts, startPos, endPos); // Getting word
        if (checkWord == "")
            break; // If no words on line, just break
        if(!search(root, checkWord)) // Searching for parsed word in dictionary
        {
            Position wordPosition = {startPos, endPos};
            problems.push_back(wordPosition); // If not found in dictionary, add to problems with proper startPos and endPos
        }
        startPos = endPos + 1;
    }
    // TODO
}
