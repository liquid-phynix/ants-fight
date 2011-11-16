#include<QtGui>
#include<string>
#include<cmath>
#include<iostream>
#include"main.hh"

using namespace std;

_sceneArea::_sceneArea(QColor* pcs, int nofp){
  numOfPlayers=nofp;
  playerColors=pcs;
  activeColorId=0;
  QVarLengthArray<QLine> gridLines;
  background.load(":/images/bck.jpg");
  gridLines=QVarLengthArray<QLine>();
  for(int x=0; x<=this->width(); x+=16){
    gridLines.append(QLine(x, 0, x, this->height()));
  }
  for(int y=0; y<=this->height(); y+=16){
    gridLines.append(QLine(0, y, this->width(), y));
  }
  QPainter painter(&background); 
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  painter.drawLines(gridLines.data(), gridLines.size());
  painter.end();
  playerDots=new QImage[numOfPlayers];
  for(int i=0; i<numOfPlayers; i++){
    playerDots[i]=QImage(16, 16, QImage::Format_ARGB32);
    playerDots[i].fill(0);
    QPainter dotPainter(&playerDots[i]);
    dotPainter.setRenderHint(QPainter::Antialiasing, true);
    dotPainter.setBrush(QBrush(playerColors[i]));
    dotPainter.setPen(QPen(playerColors[i]));
    dotPainter.drawEllipse(2,2,14,14);
    dotPainter.end();
  }
}

_sceneArea::~_sceneArea(void){
  delete[] playerDots;
}

void _sceneArea::paintEvent(QPaintEvent* event){
  QPainter painter(this);
  painter.drawImage(QPoint(0,0), background);
  
  for(auto posIt=logic.ants.begin(), endIt=logic.ants.end(); posIt!=endIt; posIt++){
    painter.drawImage(posIt->first.col*16,posIt->first.row*16, playerDots[posIt->second]);
  }
  //painter.setRenderHint(QPainter::Antialiasing, true);
  //painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  //painter.drawLines(gridLines.data(), gridLines.size());
}

void _sceneArea::mousePressEvent(QMouseEvent* mevent){
  int mx=mevent->x();
  int my=mevent->y();
  mx/=16;
  my/=16;
  
  if(mevent->button()==1) addAnt(mx, my);
  if(mevent->button()==2) removeAnt(mx, my);
  repaint();
}

void _sceneArea::addAnt(int x, int y){
  logic.ants[pns::pos(y,x)]=activeColorId;
}

void _sceneArea::removeAnt(int x, int y){
  auto it=logic.ants.find(pns::pos(y,x));
  if(it!=logic.ants.end())
    logic.ants.erase(it);
}

void _sceneArea::selectPlayer(int id){
  activeColorId=id;
}

_scene::_scene(QWidget* parent):
QWidget(parent),
numOfPlayers(8){
  int buttonWidth=70;
  QString str;
  buttonColors=new QColor[numOfPlayers];
  colorButtons=new _numberedPushButton*[numOfPlayers];
  for(int i=0; i<numOfPlayers; i++){
    buttonColors[i]=QColor::fromHsv(i*360/(1+numOfPlayers), 255, 255);
  }
  sceneArea=new _sceneArea(buttonColors, numOfPlayers);
  sceneArea->setMinimumSize(321, 321);
  sceneArea->setMaximumSize(321, 321);
  for(int i=0; i<numOfPlayers; i++){
    str.setNum(i+1);
    colorButtons[i]=new _numberedPushButton(QString("p")+str, i, sceneArea);
    colorButtons[i]->setMinimumWidth(buttonWidth);
    colorButtons[i]->setMaximumWidth(buttonWidth);
    colorButtons[i]->setPalette(QPalette(buttonColors[i]));
  }
  QVBoxLayout* colorButtonsLayout=new QVBoxLayout;
  colorButtonsLayout->addWidget(colorButtons[0], Qt::AlignTop);
  for(int i=1; i<numOfPlayers; i++)
    colorButtonsLayout->addWidget(colorButtons[i]);
  colorButtonsLayout->addStretch();
  stepButton=new QPushButton(QString("resolve!"));
  stepButton->setMinimumWidth(buttonWidth);
  stepButton->setMaximumWidth(buttonWidth);
  connect(stepButton, SIGNAL(clicked()), sceneArea, SLOT(doResolve()));
  colorButtonsLayout->addWidget(stepButton);
  
  QGridLayout* mainLayout=new QGridLayout;
  mainLayout->addWidget(sceneArea, 0, 1);
  mainLayout->addLayout(colorButtonsLayout, 0, 0);
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->sizeHint();
  setLayout(mainLayout);
  setWindowTitle("ABS: Ant Battle Sim");
}

_scene::~_scene(void){
  delete[] buttonColors;
  delete[] colorButtons;
}



int main(int argc, char** argv){
  QApplication app(argc, argv);
  
  _scene scene;
  scene.show();
  
  return app.exec();
}