#ifndef INCLUDE_Board_h_
#define INCLUDE_Board_h_

#include <vector>
#include "Reversi.h"
const int BOARD_SIZE = 8;
const int MAX_TURNS = 60;
//メソッドの最後にconstがついてるのはメンバ変数を変更しないようにするため
class Board
{
public:
  Board();

  void init();
  bool move(const Point& point);
  bool pass();
  bool undo();
  bool isGameOver() const;

  unsigned countDisc(Color color) const{
    /*colorの石の数を返す*/
    return Discs[color];
  }
  Color getColor(const Point& p) const{
    return RawBoard[p.x][p.y];
  }
  const std::vector<Point>& getMovablePos() const{
    /*打てる位置のvectorを返す*/
    return MovablePos[Turns];
  }
  std::vector<Disc> getUpdate() const{
    if(!UpdateLog.empty())return UpdateLog.back();
    else return std::vector<Disc>();
  }
  Color getCorrentColor() const{
    return CurrentColor;
  }
  unsigned getTurns() const{
    return Turns;
  }

private:
  enum Direction{
    //ビットマスク
    NONE = 0,         //石を打てない
    UPPER = 1,        //上方向に裏返せる
    UPPER_LEFT = 2,   //左上
    LEFT = 4,         //左
    LOWER_LEFT = 8,   //左下
    LOWER = 16,       //下
    LOWER_RIGHT = 32, //右下
    RIGHT = 64,       //右
    UPPER_RIGHT = 128,//右上
  };

  Color RawBoard[BOARD_SIZE+2][BOARD_SIZE+2];
  unsigned Turns; //手数
  Color CurrentColor; //現在のプレイヤー

  std::vector<std::vector<Disc> > UpdateLog;

  std::vector<Point> MovablePos[MAX_TURNS+1];
  unsigned MovableDir[MAX_TURNS+1][BOARD_SIZE+2][BOARD_SIZE+2];

  ColorStorage<unsigned> Discs; //各色の石の数

  void flipDiscs(const Point& point);
  unsigned checkMobility(const Disc& disc) const;
  void initMovable();

};
#endif
