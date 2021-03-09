#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() {root = nullptr;}
    virtual ~StudentSpellCheck();
    bool load(std::string dict_file);
    bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
    void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
    struct trieNode
    {
        trieNode *children[27];
        bool isEndOfWord;
    };
    trieNode *root;
    void trieDestructorHelper(trieNode* node);
    trieNode* getNode()
    {
        trieNode *pNode = new trieNode;
        pNode->isEndOfWord = false;
        for (int i = 0; i < 27; i++)
            pNode->children[i] = nullptr;
        return pNode;
    }
    void insert(trieNode *root, std::string word); // Inserting into root
    bool search(trieNode *root, std::string word); // Finding word in dictionary
    std::string lineDivider(std::string line, int &startPos, int &endPos);
};

#endif  // STUDENTSPELLCHECK_H_
