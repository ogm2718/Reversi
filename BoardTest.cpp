#include <iostream>
#include <stdexcept>
#include "Board.h"

using namespace std;

class ConsoleBoard: public Board{
public:
  void print(){
    cout << " a b c d e f g h " << endl;
    for(int y=1;y<=8;y++){
      cout << y;
      for(int x = 1;x<=8;x++){
        switch (getColor(Point(x,y))) {
          case BLACK:
            cout << "B ";
          break;
          case WHITE:
            cout << "W ";
          break;
          default:
           cout << "  ";
        }
      }
      cout << endl;
    }
  }
};

int main(){
  ConsoleBoard Board;

  while (true) {
    Board.print();
    cout << "BLACK " << Board.countDisc(BLACK) << " ";
    cout << "WHITE " << Board.countDisc(WHITE) << " ";
    cout << "SPACE " << Board.countDisc(EMPTY) << endl << endl;

    cout << "Please input your move: ";
    Point p;
    string in;
    cin >> in;
    if(in=="p"){
      if(!Board.pass()){
        cout << "You cannot pass!" << endl;
        continue;
      }
    }
    if(in=="u"){
      Board.undo();
      continue;
    }
    try{
      Point parse(in);
      p = parse;
    }
    catch(invalid_argument e){
      cerr << "Please input Reversi type move!" << endl;
      continue;
    }

    if(Board.move(p) == false){
      cerr << "You cannot move this point!" << endl;
      continue;
    }

    if(Board.isGameOver()){
      cout << "----------- Finish this Game! ------------" << endl;
      return 0;
    }
  }
  return 0;
}
