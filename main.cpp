#include<QtGui>
#include<string>
#include<cmath>
#include<iostream>
#include"main.hh"

using namespace std;

_sceneArea::_sceneArea(QColor* pcs, int nofp){
  draggingMouse=false;
  dragPosition=pns::pos(0,0);
  pressedButton=0;
  withTransition=0;
  lastScene=0;
  newScene=0;
  timerId=0;
  
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
    //dotPainter.setPen(QPen(playerColors[i]));
    dotPainter.setPen(QPen(Qt::black));
    dotPainter.drawEllipse(2,2,13,13);
    dotPainter.end();
  }
}

_sceneArea::~_sceneArea(void){
  delete[] playerDots;
  delete lastScene;
}

// void _sceneArea::paintEvent(QPaintEvent* event){
//   if(withTransition){
//     QImage tmpImage(
//     withTransition=false;
//   }else{
//     QPainter painter(this);
//     painter.drawImage(QPoint(0,0), background);
//     for(auto posIt=logic.ants.begin(), endIt=logic.ants.end(); posIt!=endIt; posIt++){
//       painter.drawImage(posIt->first.col*16,posIt->first.row*16, playerDots[posIt->second]);
//     }
//   }
// }

void _sceneArea::paintEvent(QPaintEvent*){
  QPainter painter(this);
  QImage scene(this->width(), this->height(), QImage::Format_ARGB32);
  painter.drawImage(0, 0, background);
  
  if(newScene==0){
    newScene=new QImage(this->width(), this->height(), QImage::Format_ARGB32);
    QPainter newPainter(newScene);
    newPainter.setCompositionMode(QPainter::CompositionMode_Source);
    newPainter.fillRect(newScene->rect(), Qt::transparent);
    newPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    for(auto posIt=logic.ants.begin(), endIt=logic.ants.end(); posIt!=endIt; posIt++){
      newPainter.drawImage(posIt->first.col*16,posIt->first.row*16, playerDots[posIt->second]);
    }
    newPainter.end();
  }
  
  if(withTransition>0){
    QImage alphaScene(this->width(), this->height(), QImage::Format_Indexed8);
    alphaScene.fill(withTransition*255/8);
    lastScene->setAlphaChannel(alphaScene);
    painter.drawImage(0, 0, *lastScene);
    painter.drawImage(0, 0, *newScene);    
    painter.end();
    withTransition--;
  }else{
    painter.drawImage(0, 0, *newScene);
    painter.end();
    
    this->killTimer(timerId);
    if(lastScene) delete lastScene;
    lastScene=newScene;
    newScene=0;
    withTransition=0;
  }
}

void _sceneArea::timerEvent(QTimerEvent*){
  repaint();
}

void _sceneArea::mousePressEvent(QMouseEvent* mevent){
  int mx=mevent->x();
  int my=mevent->y();
  mx/=16;
  my/=16;
  
  draggingMouse=true;
  dragPosition=pns::pos(mx, my);
  pressedButton=mevent->button();
  if(pressedButton==1) addAnt(mx, my);
  if(pressedButton==2) removeAnt(mx, my);
  repaint();
}

void _sceneArea::mouseReleaseEvent(QMouseEvent*){
  draggingMouse=false;
  pressedButton=0;
}

void _sceneArea::mouseMoveEvent(QMouseEvent* mevent){
  if(!draggingMouse) return;
  
  int mx=mevent->x();
  int my=mevent->y();
  mx/=16;
  my/=16;
  if(! (pns::pos(mx, my)==dragPosition)){
    if(pressedButton==1) addAnt(mx, my);
    if(pressedButton==2) removeAnt(mx, my);
    dragPosition=pns::pos(mx, my);
    repaint();
  }
}

void _sceneArea::addAnt(int x, int y){
  logic.ants[pns::pos(y,x)]=activeColorId;
}

void _sceneArea::removeAnt(int x, int y){
  std::unordered_map<pns::pos, int>::iterator it=logic.ants.find(pns::pos(y,x));
  if(it!=logic.ants.end())
    logic.ants.erase(it);
}

void _sceneArea::selectPlayer(int id){
  activeColorId=id;
}

_scene::_scene(QWidget* parent):
QWidget(parent),
numOfPlayers(4){
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
    colorButtons[i]=new _numberedPushButton(this, QString("p")+str, i, sceneArea);
    colorButtons[i]->setMinimumWidth(buttonWidth);
    colorButtons[i]->setMaximumWidth(buttonWidth);
    colorButtons[i]->setPalette(QPalette(buttonColors[i]));
  }
  colorButtons[0]->setDisabled(true);
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

void _scene::enableButton(int id){
  colorButtons[id]->setEnabled(true);
}


int main(int argc, char** argv){
  QApplication app(argc, argv);
  
  _scene scene;
  scene.show();
  
  return app.exec();
}