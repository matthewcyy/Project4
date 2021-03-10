#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>

TextEditor* createTextEditor(Undo* un)
{
    return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
     m_undoPtr = undo;
     m_curRow = 0; // Tracking the row
     m_curCol = 0; // Tracking the column
     m_lines.push_back(""); // Here so that the user may start writing on a blank, new file
     m_rowIt = m_lines.begin(); // Same as above
    // TODO
}

StudentTextEditor::~StudentTextEditor()
{
    // TODO
}

bool StudentTextEditor::load(std::string file) {
    std::ifstream infile(file);
    if (!infile)
        return false; // Invalid load
    reset(); // Clear what's currently on the screen
    std::string s;
    while (getline(infile,s)) // Importing each line in a file
    {
        if (s.size() != 0)
            if (s[s.size() - 1] == '\r')
                s.erase(s.size() - 1); // Removing \r chars that appear at the ends of some lines
        m_lines.push_back(s); // Adding to container
    }
    m_curRow = 0;
    m_curCol = 0;
    m_rowIt = m_lines.begin();
    return true;
}

bool StudentTextEditor::save(std::string file) {
    std::ofstream outfile(file);
    if (!outfile)
        return false;
    std::list<std::string>::iterator stringIt;
    stringIt = m_lines.begin();
    while (stringIt != m_lines.end())
    {
        outfile << *stringIt + '\n'; // Appending \n to all lines of current text
        stringIt++;
    }
    return true;
}

void StudentTextEditor::reset() {
    m_lines.clear(); // Clear container and reset position
    m_curRow = 0;
    m_curCol = 0;
    m_undoPtr->clear(); // Clear undo pointer's stack
}

void StudentTextEditor::move(Dir dir) {
    int maxRows = m_lines.size() - 1; // Number of rows or entries in list
    int maxCols = m_rowIt->size() - 1; // Col of last character in line
    switch (dir)
    {
        case UP:
            if (m_curRow != 0)
            {
                m_rowIt--;
                m_curRow--;
                int newCols = m_rowIt->size();
                if (m_curCol > newCols)
                    m_curCol = newCols; // If the new line is smaller length than the previous line, set the column to jsut after its last character
            }
            break;
        case DOWN:
            if (m_curRow != maxRows)
            {
                m_rowIt++;
                m_curRow++;
                int newCols = m_rowIt->size();
                if (m_curCol > newCols)
                    m_curCol = newCols; // If the new line is smaller length than the previous line, set the column to jsut after its last character
            }
            break;
        case LEFT:
            if (m_curCol == 0 && m_curRow != 0)
            {
                m_curRow--;
                m_rowIt--;
                maxCols = m_rowIt->size();
                m_curCol = maxCols; // Setting column to end of previous line
            }
            else if (m_curCol != 0)
                m_curCol--;
            break;
        case RIGHT:
            if (m_curCol == maxCols + 1 && m_curRow != maxRows) // If you're at the end of a line and there are more lines underneath...
            {
                m_curRow++;
                m_rowIt++;
                m_curCol = 0; // Adjust column to beginning of new line
            }
            else if (m_curCol != maxCols + 1)
                m_curCol++;
            break;
        case HOME:
            m_curCol = 0;
            break;
        case END:
            m_curCol = maxCols + 1;
            break;
    }
}

void StudentTextEditor::del() {
    if (m_curCol == m_rowIt->size() && m_curRow != m_lines.size() - 1) // If at the end of a line, and not at the first line
    {
        std::list<std::string>::iterator stringIt = m_rowIt;
        stringIt++;
        *m_rowIt += *stringIt; // Combine the row after to the current line
        m_lines.erase(stringIt);
        m_undoPtr->submit(Undo::JOIN, m_curRow, m_curCol);
    }
    else
    {
        if (m_rowIt->size() == 0)
            return;
        char c = m_rowIt->at(m_curCol);
        m_rowIt->erase(m_curCol, 1); // Just removing the examined character
        m_undoPtr->submit(Undo::DELETE, m_curRow, m_curCol, c);
    }
}

void StudentTextEditor::backspace() {
    if (m_curCol > 0) // If not at the beginning of a line
    {
        char c = m_rowIt->at(m_curCol - 1);
        m_rowIt->erase(m_curCol - 1, 1); // Just erase the given char
        m_curCol--;
        m_undoPtr->submit(Undo::DELETE, m_curRow, m_curCol, c);
    }
    else if (m_curCol == 0 && m_curRow != 0) // Otherwise when at the beginning of a line and not the first line
    {
        std::list<std::string>::iterator stringIt = m_rowIt;
        m_rowIt--;
        m_curRow--;
        m_curCol = m_rowIt->size();
        *m_rowIt += *stringIt; // Add row you were looking at to previous row
        m_lines.erase(stringIt); // Remove the previous row
        m_undoPtr->submit(Undo::JOIN, m_curRow, m_curCol);
    }
}

void StudentTextEditor::insert(char ch) {
    if (ch == '\t')
    {
        m_rowIt->insert(m_curCol, 4, ' ');
        m_undoPtr->submit(Undo::INSERT, m_curRow, m_curCol + 1, ' '); // Assigning before so that
        m_curCol += 4; // If a tab, insert 4 spaces and increase the columns by 4
    }
    else
    {
        m_rowIt->insert(m_curCol, 1, ch); // Otherwise just insert the character and increase column position
        m_curCol++;
        m_undoPtr->submit(Undo::INSERT, m_curRow, m_curCol, ch);
    }
}

void StudentTextEditor::enter() {
    m_undoPtr->submit(Undo::SPLIT, m_curRow, m_curCol); // Assigning location before split happens in undo
    std::list<std::string>::iterator insertIt;
    insertIt = m_rowIt;
    insertIt++;
    std::string addThis = m_rowIt->substr(m_curCol, m_rowIt->size() - m_curCol); // Creating the string from the previous line to add to the new line
    *m_rowIt = m_rowIt->substr(0, m_curCol); // Assigning the current line to only have up to where the enter was placed
    m_lines.insert(insertIt, addThis); // Inserting the portion of the line into a new line in the container
    m_curRow++;
    m_rowIt++;
    m_curCol = 0; // Adjusting position
}

void StudentTextEditor::getPos(int& row, int& col) const {
    row = m_curRow;
    col = m_curCol;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
    if (startRow < 0 || numRows < 0 || startRow > m_lines.size())
        return -1;
    lines.clear();
    std::list<std::string>::const_iterator stringIt;
    stringIt = m_rowIt; // Current row iterator
    int diff = abs(m_curRow - startRow); // Calculating difference between current row and desired start row
    if (m_curRow < startRow)
    {
        for (int i = 0; i < diff; i++) // Moving to start line when m_curRow is less than startRow
            stringIt++;
    }
    else if (m_curRow > startRow)
    {
        for (int i = 0; i < diff; i++) // Moving to start line when m_curRow is more than startRow
            stringIt--;
    }
    int count = 0;
    while (count < numRows && count < m_lines.size()) // Add onto lines, up to either the desired amount or the max number of lines remaining in the container
    {
        lines.push_back(*stringIt);
        stringIt++;
        count++;
    }
    return lines.size();
}

void StudentTextEditor::undo() {
    int row, col, count;
    std::string text;
    Undo::Action look = m_undoPtr->get(row, col, count, text);
    if (look == Undo::ERROR)
    {
        row = m_curRow; // Nothing should happen to row and col if error
        col = m_curCol;
    }
    int diff = abs(m_curRow - row); // Calculating difference between current row and desired start row
    if (m_curRow < row)
    {
        for (int i = 0; i < diff; i++) // Moving to start line when m_curRow is less than startRow
            m_rowIt++;
    }
    else if (m_curRow > row)
    {
        for (int i = 0; i < diff; i++) // Moving to start line when m_curRow is more than startRow
            m_rowIt--;
    }
    m_curRow = row;
    m_curCol = col;
    std::list<std::string>::iterator stringIt = m_rowIt;
    switch (look)
    {
        case Undo::ERROR:
            return;
            break;
        case Undo::INSERT:
            m_rowIt->insert(m_curCol, text); // Inserting returned text to col and row returned
            break;
        case Undo::DELETE:
            m_rowIt->erase(m_curCol, count); // Deleting text to where col and row returned
            break;
        case Undo::SPLIT: {
            stringIt++;
            std::string addThis = m_rowIt->substr(m_curCol, m_rowIt->size() - m_curCol); // Creating the string from the previous line to add to the new line
            *m_rowIt = m_rowIt->substr(0, m_curCol); // Assigning the current line to only have up to where the enter was placed
            m_lines.insert(stringIt, addThis); // Inserting the portion of the line into a new line in the container
            break;
        }
        case Undo::JOIN:
            stringIt++;
            *m_rowIt += *stringIt;
            m_lines.erase(stringIt); // Joining the two lines
            break;
    }
    // TODO
}
