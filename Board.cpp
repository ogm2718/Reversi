#include "Board.h"
#include <iostream>
using namespace std;
int dx[]={ -1,  0,  1, -1,  1, -1,  0,  1};
int dy[]={ -1, -1, -1,  0,  0,  1,  1,  1};//LU->U->...->LC->...RD
int dr[]={  2,  1,128,  4, 64,  8, 16, 32};
//{UPPER_LEFT,UPPER,UPPER_RIGHT,LEFT,RIGHT,LOWER_LEFT,LOWER,LOWER_RIGHT};

Board::Board(){
  init();
}

void Board::init(){
  /*すべてのスペースを空白にする*/
  for(unsigned x=1;x<=BOARD_SIZE;x++){
    for(unsigned y=1;y<=BOARD_SIZE;y++){
      RawBoard[x][y]=EMPTY;
    }
  }
  /*壁を設定*/
  for(unsigned x=0;x<=BOARD_SIZE+1;x++){
    RawBoard[x][0]=WALL;
    RawBoard[x][BOARD_SIZE+1]=WALL;
  }
  for(unsigned y=0;y<=BOARD_SIZE+1;y++){
    RawBoard[0][y]=WALL;
    RawBoard[BOARD_SIZE+1][y]=WALL;
  }

  /*初期配置*/
  RawBoard[4][4]=WHITE;
  RawBoard[5][5]=WHITE;
  RawBoard[4][5]=BLACK;
  RawBoard[5][4]=BLACK;

  Discs[BLACK]=2;
  Discs[WHITE]=2;
  Discs[EMPTY]=BOARD_SIZE*BOARD_SIZE-4;

  Turns = 0;
  CurrentColor = BLACK;
  UpdateLog.clear();

  initMovable();
}

unsigned Board::checkMobility(const Disc &disc)const {
  /*
  discで指定された座標に,disc.colorの石が置けるかどうか,
  また,どの方向に石を裏返すことが出来るかを判定する
  */
  if(RawBoard[disc.x][disc.y] != EMPTY) return NONE;

  int x,y;
  unsigned dir = NONE;

  for(int i=0;i<8;i++){
    if(RawBoard[disc.x+dx[i]][disc.y+dy[i]] == -disc.color){
      x = disc.x+2*dx[i];
      y = disc.y+2*dy[i];
      while(RawBoard[x][y]==-disc.color){x+=dx[i];y+=dy[i];}
      if(RawBoard[x][y]==disc.color)dir |= dr[i];
    }
  }
  return dir;
}

bool Board::undo(){
  /*1ターン戻せるかどうかを判定し,戻せるなら状況を戻す*/
  if(Turns == 0)return false;
  CurrentColor *=-1;

  const std::vector<Disc>& update = UpdateLog.back();

  if(update.empty()){
    /*前のターンにパスされているとき*/
    MovablePos[Turns].clear();
    for(int x=1;x<=BOARD_SIZE;x++){
      for(int y=1;y<=BOARD_SIZE;y++){
        MovableDir[Turns][x][y]=NONE;
      }
    }
  }else{
    /*前のターンがパスでない*/
    --Turns;
    RawBoard[update[0].x][update[0].y]=EMPTY;
    for(int i=1;i<(int)update.size();i++){
      RawBoard[update[i].x][update[i].y]=-CurrentColor;
    }
    unsigned discdiff = (unsigned)update.size();
    Discs[CurrentColor] -= discdiff;
    Discs[-CurrentColor] += discdiff -1;
    Discs[EMPTY]++;
  }
  UpdateLog.pop_back();

  return true;
}

bool Board::pass(){
  if(MovablePos[Turns].size()!=0)return false;
  if(isGameOver())return false;
  CurrentColor *= -1;
  UpdateLog.push_back(std::vector<Disc>());

  initMovable();
  return true;
}

bool Board::move(const Point& point){
  /*そもそも石が置けるかどうかの判定*/
  if(point.x < 0 || point.x > BOARD_SIZE) return false;
  if(point.y < 0 || point.y > BOARD_SIZE) return false;
  if(MovableDir[Turns][point.x][point.y] == NONE) return false;
  /*石を置き,挟んだ石を裏返す操作*/
  flipDiscs(point);
  /*手数と手番の色を更新*/
  ++Turns;
  CurrentColor *= -1;
  /*置ける位置とその点における方向を調べなおす*/
  initMovable();
  return true;
}

bool Board::isGameOver()const{
  /*
  ゲームの終了を判定する
  */
  if(Turns == MAX_TURNS)return true;
  if(!MovablePos[Turns].empty()) return false;
  Disc disc;
  disc.color = -CurrentColor;
  for(int x=1;x<=BOARD_SIZE;x++){
    disc.x = x;
    for(int y=1;y<=BOARD_SIZE;y++){
      disc.y = y;
      if(checkMobility(disc)!=NONE)return false;
    }
  }
  return true;
}


void Board::initMovable(){
  /*
  CurrentColorプレイヤが置ける石の位置と返る方向を調べる
  */
  Disc disc(0,0, CurrentColor);

  int dir;

  MovablePos[Turns].clear();
  for(int x=1;x<=BOARD_SIZE;x++){
    disc.x = x;
    for(int y=1;y<=BOARD_SIZE;y++){
      disc.y = y;
      dir = checkMobility(disc);
      if(dir != NONE)MovablePos[Turns].push_back(disc);
      MovableDir[Turns][x][y] = dir;
    }
  }
}

void Board::flipDiscs(const Point& point){
  /*
  pointに石を打ち,全て裏返す
  打った石と裏返した石の情報をUpdateLogに残す
  */
  int x,y;
  Disc operation(point.x,point.y,CurrentColor);//操作が行われた石(最初は打った石)

  int dir = MovableDir[Turns][point.x][point.y];
  std::vector<Disc> update;
  RawBoard[point.x][point.y] = CurrentColor;
  update.push_back(operation);
  /*
  各方向に行けるか,また,行けるときに裏返す石の情報を裏返しながら得る.
  */
  for(int i=0;i<8;i++){
    if(dir & dr[i]){
      x = point.x;
      y = point.y;
      //std::cout << dr[i] << endl;
      while(RawBoard[x+dx[i]][y+dy[i]]!=CurrentColor){
        RawBoard[x+dx[i]][y+dy[i]] = CurrentColor;
        operation.x = x+dx[i];
        operation.y = y+dy[i];
        x+=dx[i];
        y+=dy[i];
        update.push_back(operation);
      }
    }
  }

  /*
  それぞれの石の数を更新する
  */
  int discdiff = (int)update.size();
  Discs[CurrentColor] += discdiff;
  Discs[-CurrentColor] -= discdiff-1;
  Discs[EMPTY]--;

  UpdateLog.push_back(update);
}
