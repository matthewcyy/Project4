#include "StudentUndo.h"
#include <stack>

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
    int colDiff = 0;
    if (!undoStack.empty())
        colDiff = undoStack.top().col - col;
    bool wasAdded = false;
    if (!undoStack.empty() && undoStack.top().row == row && undoStack.top().type == action) // If we find operations at similar location
    {
        if (action == Action::DELETE && (colDiff == 1 || colDiff == 0)) // Batching for delete, where deletions result in column decreasing and difference in deletes = 1 greater than the previous (backspace) or the same (delete)
        {
            if (colDiff == 1) // Delete option was backspacing...
            {
                std::string addThis = "";
                addThis += ch;
                addThis += undoStack.top().chars; // These 3 lines make sure that the words erased are not backwards after undo'ing, for deletions
                undoStack.top().chars = addThis;
                undoStack.top().col--;
                undoStack.top().countFromStart++;
            }
            else
                undoStack.top().chars += ch;
            wasAdded = true;
        }
        if (action == Action::INSERT && (colDiff == -1)) // Batching for inserts, where inserts result in column increasing and difference in inserts = -1 between top and new insert.
        {
            undoStack.top().chars += ch;
            undoStack.top().countFromStart++; // Used to move curosr back to original location for inserts
            undoStack.top().col++;
            wasAdded = true;
        }
    }
    if (!wasAdded)
    {
        undoInfo addThis = {action, row, col, 1, ""};
        addThis.chars += ch;
        undoStack.push(addThis); // If not batched above, then adding on its ownto stack
    }
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
    if (undoStack.empty())
        return Action::ERROR;
    Action lastAction = undoStack.top().type;
    int lastActionRow = undoStack.top().row;
    int lastActionCol = undoStack.top().col;
    int countStart = undoStack.top().countFromStart;
    Action toReturn = Action::ERROR;
    switch (lastAction)
    {
        case Undo::INSERT:
            toReturn = Action::DELETE;
            row = lastActionRow;
            col = lastActionCol - countStart; // Returning cursor to starting location
            count = undoStack.top().chars.size(); // Number of chars to delete
            text = "";
            break;
        case Undo::DELETE:
            toReturn = Action::INSERT;
            row = lastActionRow;
            col = lastActionCol;
            count = 1;
            text = undoStack.top().chars; // Inserting backin word which was deleted
            break;
        case Undo::JOIN:
            toReturn = Action::SPLIT;
            row = lastActionRow;
            col = lastActionCol;
            count = 1;
            text = "";
            break;
        case Undo::SPLIT:
            toReturn = Action::JOIN;
            row = lastActionRow;
            col = lastActionCol;
            count = 1;
            text = "";
            break;
        case Undo::ERROR:
            break;
    }
    undoStack.pop();
    return toReturn;  // TODO
}

void StudentUndo::clear() {
    while (!undoStack.empty())
        undoStack.pop();
}
