#include <Board.h>

int dx[]={-1,0,1,-1,1,-1,0,1};
int dy[]={-1,-1,-1,0,0,1,1,1};//LU->U->...->LC->...RD
int dr[]={UPPER_LEFT,UPPER,UPPER_RIGHT,LEFT,RIGHT,LOWER_LEFT,LOWER,LOWER_RIGHT};

unsigned Board::checkMobility(const Disc &disc){
  /*
  discで指定された座標に,disc.colorの石が置けるかどうか,
  また,どの方向に石を裏返すことが出来るかを判定する
  */
  if(RawBoard[disc.x][disc.y] != EMPTY) return NONE;

  int x,y;
  unsigned dir = NONE;

  for(int i=0;i<9;i++){
    if(RawBoard[disc.x+dx[i]][disc.y+dy[i]] == -disc.color){
      x = disc.x+2*dx[i];
      y = disc.y+2*dy[i];
      while(RawBoard[x][y]==-disc.color){x+=dx[i];y+=d[y];}
      if(RawBoard[x][y]==disc.color)dir |= dr[i];
    }
  }
  return dir;
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
  if(point.x < 0 || point.x >= BOARD_SIZE) return false;
  if(point.y < 0 || point.y >= BOARD_SIZE) return false;
  if(MovableDir[Turns][point.x][point.y] == NONE) return false;
  /*石を置き,挟んだ石を裏返す操作*/
  flipDiscs(Point);
  /*手数と手番の色を更新*/
  ++Turns;
  CurrentColor *= -1;
  /*置ける位置とその点における方向を調べなおす*/
  initMovable();

  return true;
}

bool Board::isGameOver(){
  /*
  ゲームの終了を判定する
  */
  if(Turns == MAX_TURNS) return true;
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

  getMovablePos[Turns].clear();
  for(int x=1;x<=BOARD_SIZE;x++){
    disc.x = x;
    for(int y=1;y<=BOARD_SIZE;y++){
      disc.y = y;
      dir = checkMobility(disc);
      if(dir != NONE)getMovablePos[Turns].push_back(disc);
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

  int dir = MovableDir[Turns][x][y];

  std::vector<Disc> update;

  RawBoard[point.x][point.y] = CurrentColor;
  update.push_back(operation);
  /*
  各方向に行けるか,また,行けるときに裏返す石の情報を裏返しながら得る.
  */
  for(int i=0;i<9;i++){
    if(dir & dr[i]){
      x = point.x;
      y = point.y;
      while(RawBoard[x+dx[i]][y+dy[i]]!=CurrentColor){
        RawBoard[x+dx[i]][y+dy[i]] = CurrentColor;
        operation.x = x;
        operation.y = y;
        update.push_back(operation);
      }
    }
  }

  /*
  それぞれの石の数を更新する
  */
  int discdiff = update.size();
  Discs[CurrentColor] += discdiff;
  Discs[-CurrentColor] -= discdiff;
  Discs[EMPTY]--;

  UpdateLog.push_back(update);
}
