typedef int Color;
const Color EMPTY = 0;
const Color BLACK = 1;
const Color WHITE = -1;
const Color WALL = 2;

struct Point
{
  int x;
  int y;

  Point(){
    Point(0,0);
  }

  Point(int x,int y){
    this->x = x;
    this->y = y;
  }
};

struct Disc : public Point{
  Color color;

  Disc() : Point(0,0){
    color = EMPTY;
  }

  Disc(int x,int y, Color color) : Point(x,y){
    this->color = color;
  }
};

template<typename T> class ColorStorage{
private:
  T data[3];
public:
  T& operator[](Color color){
    return data[color+1];
  }

  T& operator[](Color color) const {
    return data[color+1];
  }

};
