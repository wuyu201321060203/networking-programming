#ifndef SUDOKU_H
#define SUDOKU_H

#include <muduo/base/Types.h>
#include <muduo/base/StringPiece.h>

muduo::string solveSudoku(muduo::StringPiece const& puzzle);
int const kCells = 81;

#endif
