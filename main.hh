#include<iostream>
#include<unordered_map>
#include<algorithm>

#include<QtGui>
#include<QWidget>

#ifndef _POS_H
#define _POS_H
namespace pns{
struct pos{
  int row, col;
  pos(int r, int c):
  row(r), col(c){};
  pos():
  row(0), col(0){};
  bool operator==(pos const p)const{
    return p.row == row and p.col == col;
  };
  pos operator-(const pos p){
    return pos(row-p.row, col-p.col);
  };
  pos operator+(const pos p){
    return pos(row+p.row, col+p.col);
  };
  friend std::ostream& operator<<(std::ostream& o, const pos p){
    o << "(" << p.row << " " << p.col << ")";
    return o;
  };
};
}
#endif

#ifndef _POS_HASH_H
#define _POS_HASH_H
namespace std{
  template<>
  struct hash<pns::pos>{
    //static hash<long long> long_long_hasher;
    size_t operator()(pns::pos const& p)const{
      return (size_t) hash<unsigned long long>()
      (((unsigned long long)p.row) << 32 | ((unsigned long long)p.col));
      //     size_t operator()(const pos& p)const{
        //       return (size_t) long_long_hasher(((unsigned long long)p.row) << 32 | ((unsigned long long)p.col));
      
    }
  };
};
#endif

#ifndef _LOGIC_H
#define _LOGIC_H
using namespace pns;
class _logic{
public:
  std::unordered_map<pns::pos, int> ants;
  int euclideanDist2(pns::pos p1, pns::pos p2){
    return (p1.row-p2.row)*(p1.row-p2.row)+(p1.col-p2.col)*(p1.col-p2.col);
  };
  void battleResolution(void){
    int nOfAnts=ants.size();
    if(nOfAnts<2) return;
    int row[nOfAnts];
    int col[nOfAnts];
    int kind[nOfAnts];
    int score[nOfAnts];
    int dead[nOfAnts];

    int index=0;
    for(auto posIt=ants.begin(), endIt=ants.end(); posIt!=endIt; posIt++, index++){
      row[index]=posIt->first.row;
      col[index]=posIt->first.col;
      kind[index]=posIt->second;
      score[index]=0;
      dead[index]=true;
    }
    
    for(int index1 = 0; index1<nOfAnts; index1++){
      for(int index2 = index1 + 1; index2<nOfAnts; index2++){
        if(euclideanDist2(pos(row[index1], col[index1]), pos(row[index2], col[index2]))<=5){
          if(kind[index1] != kind[index2]){
            score[index1]++;
            score[index2]++;
          }
        }
      }
    }
    for(int index1 = 0; index1<nOfAnts; index1++){
      bool found = false;
      for(int index2 = 0; index2<nOfAnts; index2++){
        if(index1!=index2 and kind[index1]!=kind[index2] and
          euclideanDist2(pos(row[index1], col[index1]), pos(row[index2], col[index2]))<=5){
          if(score[index1] >= score[index2])
            found = true;
        }
      }
      dead[index1] = found;
    }
    
    ants.clear();
    for(index=0; index<nOfAnts; index++){
      if(!dead[index]){
        ants[pos(row[index], col[index])]=kind[index];
      }
    }
  };
};
#endif



#ifndef _SCENE_AREA_H
#define _SCENE_AREA_H
class _sceneArea:public QLabel{
  Q_OBJECT
public:
  _sceneArea(QColor*, int);
  ~_sceneArea(void);
  void selectPlayer(int);
  void drawAnt(int, int);
protected:
  void paintEvent(QPaintEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void timerEvent(QTimerEvent*);
private:
  void addAnt(int, int);
  void removeAnt(int, int);
  int numOfPlayers;
  QImage background;
  QColor* playerColors;
  QImage* playerDots;
  int activeColorId;
  _logic logic;
  bool draggingMouse;
  pns::pos dragPosition;
  int pressedButton;
  int withTransition;
  QImage* lastScene;
  QImage* newScene;
  int timerId;
public slots:
  void doResolve(){
    withTransition=5;
    logic.battleResolution();
    timerId=this->startTimer(150);
  }
};
#endif

#ifndef _PBUTTON_H
#define _PBUTTON_H
class _numberedPushButton:public QPushButton{
  Q_OBJECT
private:
  int id;
  _sceneArea* sceneAreaPtr;
public:
  _numberedPushButton(QString title, int _id, _sceneArea* scptr){
    sceneAreaPtr=scptr;
    id=_id;
    this->setText(title);
    connect(this, SIGNAL(clicked()), this, SLOT(clickedWithId()));
  }
public slots:
  void clickedWithId(){
    sceneAreaPtr->selectPlayer(id);
  }
};
#endif

#ifndef _SCENE_H
#define _SCENE_H
class _scene:public QWidget{
  Q_OBJECT
public:
  _scene(QWidget* parent = 0);
  ~_scene(void);
private:
  int numOfPlayers;
  _numberedPushButton** colorButtons;
  QPushButton* stepButton;
  QColor* buttonColors;
  _sceneArea* sceneArea;
};
#endif

