#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
    struct undoInfo
    {
        Action type;
        int row;
        int col;
        int countFromStart;
        std::string chars;
    };
    std::stack<undoInfo> undoStack;

};

#endif // STUDENTUNDO_H_
