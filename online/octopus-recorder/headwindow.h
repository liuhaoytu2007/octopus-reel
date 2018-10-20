/*
Octopus-ReEL - Realtime Encephalography Laboratory Network
   Copyright (C) 2007 Barkin Ilhan

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If no:t, see <https://www.gnu.org/licenses/>.

 Contact info:
 E-Mail:  barkin@unrlabs.org
 Website: http://icon.unrlabs.org/staff/barkin/
 Repo:    https://github.com/4e0n/
*/

#ifndef HEADWINDOW_H
#define HEADWINDOW_H

#include <QtGui>

#include "octopus_rec_master.h"
#include "octopus_head_glwidget.h"
#include "legendframe.h"

class HeadWindow : public QMainWindow {
 Q_OBJECT
 public:
  HeadWindow(QWidget *pnt,RecMaster* rm) : QMainWindow(pnt,Qt::Window) {
   parent=pnt; p=rm; setGeometry(p->hwX,p->hwY,p->hwWidth,p->hwHeight);
   setFixedSize(p->hwWidth,p->hwHeight);

   p->regRepaintHeadWindow(this);

   // *** STATUSBAR ***

   p->hwStatusBar=new QStatusBar(this);
   p->hwStatusBar->setGeometry(0,height()-20,width(),20);
   p->hwStatusBar->show(); setStatusBar(p->hwStatusBar);

   // *** MENUBAR ***

   menuBar=new QMenuBar(this); menuBar->setFixedWidth(width());
   setMenuBar(menuBar);
   QMenu *modelMenu=new QMenu("&Model",menuBar);
   QMenu *viewMenu=new QMenu("&View",menuBar);
   menuBar->addMenu(modelMenu);
   menuBar->addMenu(viewMenu);

   // Model Menu
   loadRealAction=new QAction("&Load Real...",this);
   saveRealAction=new QAction("&Save Real...",this);
   saveAvgsAction=new QAction("&Save All Averages",this);

   loadRealAction->setStatusTip(
    "Load previously saved electrode coordinates..");
   saveRealAction->setStatusTip(
    "Save measured/real electrode coordinates..");
   saveAvgsAction->setStatusTip(
    "Save current averages to separate files per event..");

   connect(loadRealAction,SIGNAL(triggered()),this,SLOT(slotLoadReal()));
   connect(saveRealAction,SIGNAL(triggered()),this,SLOT(slotSaveReal()));
   connect(saveAvgsAction,SIGNAL(triggered()),
           (QObject *)p,SLOT(slotExportAvgs()));

   modelMenu->addAction(loadRealAction);
   modelMenu->addAction(saveRealAction);
   modelMenu->addSeparator();
   modelMenu->addAction(saveAvgsAction);

   // View Menu
   toggleFrameAction=new QAction("Cartesian Frame",this);
   toggleGridAction=new QAction("Cartesian Grid",this);
   toggleDigAction=new QAction("Digitizer Coords/Frame",this);
   toggleParamAction=new QAction("Parametric Coords",this);
   toggleRealAction=new QAction("Measured Coords",this);
   toggleGizmoAction=new QAction("Gizmos",this);
   toggleAvgsAction=new QAction("Averages",this);
   toggleScalpAction=new QAction("MRI/Real Scalp Model",this);
   toggleSkullAction=new QAction("MRI/Real Skull Model",this);
   toggleBrainAction=new QAction("MRI/Real Brain Model",this);
   toggleSourceAction=new QAction("Source Model",this);

   toggleFrameAction->setStatusTip("Show/hide cartesian XYZ frame.");
   toggleGridAction->setStatusTip("Show/hide cartesian grid/rulers.");
   toggleDigAction->setStatusTip("Show/hide digitizer output.");
   toggleParamAction->setStatusTip(
    "Show/hide parametric/spherical coords/template.");
   toggleRealAction->setStatusTip(
    "Show/hide measured/real coords.");
   toggleGizmoAction->setStatusTip(
    "Show/hide loaded gizmo/hint list.");
   toggleAvgsAction->setStatusTip(
    "Show/hide Event Related Potentials on electrodes.");
   toggleScalpAction->setStatusTip(
    "Show/hide realistic scalp segmented from MRI data.");
   toggleSkullAction->setStatusTip(
    "Show/hide realistic skull segmented from MRI data.");
   toggleBrainAction->setStatusTip(
    "Show/hide realistic brain segmented from MRI data.");
   toggleBrainAction->setStatusTip(
    "Show/hide source model.");

   connect(toggleFrameAction,SIGNAL(triggered()),this,SLOT(slotToggleFrame()));
   connect(toggleGridAction,SIGNAL(triggered()),this,SLOT(slotToggleGrid()));
   connect(toggleDigAction,SIGNAL(triggered()),this,SLOT(slotToggleDig()));
   connect(toggleParamAction,SIGNAL(triggered()),this,SLOT(slotToggleParam()));
   connect(toggleRealAction,SIGNAL(triggered()),this,SLOT(slotToggleReal()));
   connect(toggleGizmoAction,SIGNAL(triggered()),this,SLOT(slotToggleGizmo()));
   connect(toggleAvgsAction,SIGNAL(triggered()),this,SLOT(slotToggleAvgs()));
   connect(toggleScalpAction,SIGNAL(triggered()),this,SLOT(slotToggleScalp()));
   connect(toggleSkullAction,SIGNAL(triggered()),this,SLOT(slotToggleSkull()));
   connect(toggleBrainAction,SIGNAL(triggered()),this,SLOT(slotToggleBrain()));
   connect(toggleSourceAction,SIGNAL(triggered()),
           this,SLOT(slotToggleSource()));

   viewMenu->addAction(toggleFrameAction);
   viewMenu->addAction(toggleGridAction);
   viewMenu->addAction(toggleDigAction);
   viewMenu->addSeparator();
   viewMenu->addAction(toggleParamAction);
   viewMenu->addAction(toggleRealAction);
   viewMenu->addSeparator();
   viewMenu->addAction(toggleGizmoAction);
   viewMenu->addAction(toggleAvgsAction);
   viewMenu->addSeparator();
   viewMenu->addAction(toggleScalpAction);
   viewMenu->addAction(toggleSkullAction);
   viewMenu->addAction(toggleBrainAction);
   viewMenu->addSeparator();
   viewMenu->addAction(toggleSourceAction);

   paramRLabel=new QLabel("Parametric Radius ("+
                          dummyString.setNum(p->scalpParamR)+" cm):",this);
   paramRLabel->setGeometry(p->hwHeight-20,40,p->hwWidth-p->hwHeight-10,20);

   QSlider *paramRSlider=new QSlider(Qt::Horizontal,this);
   paramRSlider->setGeometry(p->hwHeight-20,60,p->hwWidth-p->hwHeight-10,20);
   paramRSlider->setRange(70,500); // in mm because of int - divide by ten
   paramRSlider->setSingleStep(1);
   paramRSlider->setPageStep(10); // step in cms..
   paramRSlider->setSliderPosition(p->scalpParamR*10);
   paramRSlider->setEnabled(true);
   connect(paramRSlider,SIGNAL(valueChanged(int)),
           this,SLOT(slotSetScalpParamR(int)));

   QLabel *gizmoLabel=new QLabel("Gizmo",this);
   gizmoLabel->setGeometry(p->hwHeight+4,80,
                           (p->hwWidth-p->hwHeight)/2-10,20);
   QLabel *electrodeLabel=new QLabel("Electrode",this);
   electrodeLabel->setGeometry(p->hwHeight+(p->hwWidth-p->hwHeight)/2+14,80,
                               (p->hwWidth-p->hwHeight)/2-10,20);

   gizmoList=new QListWidget(this);
   gizmoList->setGeometry(p->hwHeight-20,100,
                          (p->hwWidth-p->hwHeight)/2-10,p->hwHeight-430);
   electrodeList=new QListWidget(this);
   electrodeList->setGeometry(p->hwHeight+(p->hwWidth-p->hwHeight)/2-20,100,
                              (p->hwWidth-p->hwHeight)/2-10,p->hwHeight-430);

   notchLabel=new QLabel("Notch Level (RMS) ("+
                    dummyString.setNum(p->notchThreshold)+" uV):",this);
   notchLabel->setGeometry(p->hwHeight-20,p->hwHeight-326,
                           p->hwWidth-p->hwHeight-10,20);

   QSlider *notchSlider=new QSlider(Qt::Horizontal,this);
   notchSlider->setGeometry(p->hwHeight-20,p->hwHeight-306,
                            p->hwWidth-p->hwHeight-10,20);
   notchSlider->setRange(1,500); // in mm because of int - divide by ten
   notchSlider->setSingleStep(1);
   notchSlider->setPageStep(10); // step in uVs..
   notchSlider->setSliderPosition((int)(p->notchThreshold*10.));
   notchSlider->setEnabled(true);
   connect(notchSlider,SIGNAL(valueChanged(int)),
           this,SLOT(slotSetNotchThr(int)));

   gizmoRealCB=new QCheckBox("Gizmo on Real Model",this);
   gizmoRealCB->setGeometry(p->hwHeight+4,p->hwHeight-286,
                            p->hwWidth-p->hwHeight-20,20);
   connect(gizmoRealCB,SIGNAL(clicked()),this,SLOT(slotGizmoRealCB()));

   elecRealCB=new QCheckBox("Electrodes on Real Model",this);
   elecRealCB->setGeometry(p->hwHeight+4,p->hwHeight-266,
                            p->hwWidth-p->hwHeight-20,20);
   connect(elecRealCB,SIGNAL(clicked()),this,SLOT(slotElecRealCB()));

   connect(gizmoList,SIGNAL(currentRowChanged(int)),
           this,SLOT(slotSelectGizmo(int)));
   connect(electrodeList,SIGNAL(currentRowChanged(int)),
           this,SLOT(slotSelectElectrode(int)));
   connect(electrodeList,SIGNAL(itemDoubleClicked(QListWidgetItem *)),
           this,SLOT(slotViewElectrode(QListWidgetItem *)));

   for (int i=0;i<p->gizmo.size();i++) gizmoList->addItem(p->gizmo[i]->name);

   QLabel *legendLabel=new QLabel("Event Counts/Legend:",this);
   legendLabel->setGeometry(p->hwHeight-20,p->hwHeight-240,170,20);
   legendFrame=new LegendFrame(this,p);
   legendFrame->setGeometry(p->hwHeight-20,p->hwHeight-220,
                            p->hwWidth-p->hwHeight+10,120);

   timePtLabel=new QLabel("Localization Time Point ("+dummyString.setNum(
                          p->cp.avgBwd+p->slTimePt*1000/p->sampleRate
                           )+" ms):",this);
   timePtLabel->setGeometry(p->hwHeight-20,p->hwHeight-80,
                            p->hwWidth-p->hwHeight-10,20);

   QSlider *timePtSlider=new QSlider(Qt::Horizontal,this);
   timePtSlider->setGeometry(p->hwHeight-20,p->hwHeight-60,
                             p->hwWidth-p->hwHeight-10,20);
   timePtSlider->setRange(0,p->cp.avgCount-1);
   timePtSlider->setSingleStep(1);
   timePtSlider->setPageStep(1);
   timePtSlider->setValue(p->slTimePt);
   timePtSlider->setEnabled(true);
   connect(timePtSlider,SIGNAL(valueChanged(int)),
           this,SLOT(slotSetTimePt(int)));

   QPushButton *dummyButton;
   avgAmpBG=new QButtonGroup();
   avgAmpBG->setExclusive(true);
   for (int i=0;i<6;i++) { // ERP AMP
    dummyButton=new QPushButton(this); dummyButton->setCheckable(true);
    dummyButton->setGeometry(20+i*60,height()-24,60,20);
    avgAmpBG->addButton(dummyButton,i);
   }
   avgAmpBG->button(0)->setText("2uV");
   avgAmpBG->button(1)->setText("5uV");
   avgAmpBG->button(2)->setText("10uV");
   avgAmpBG->button(3)->setText("20uV");
   avgAmpBG->button(4)->setText("50uV");
   avgAmpBG->button(5)->setText("100uV");
   avgAmpBG->button(5)->setDown(true);
   p->avgAmpX=100;
   connect(avgAmpBG,SIGNAL(buttonClicked(int)),this,SLOT(slotAvgAmp(int)));


   clrAvgsButton=new QPushButton("CLR",this);
   clrAvgsButton->setGeometry(550,height()-24,60,20);
   connect(clrAvgsButton,SIGNAL(clicked()),(QObject *)p,SLOT(slotClrAvgs()));

   headGLWidget=new HeadGLWidget(this,p);

   gizmoList->setCurrentRow(0); slotSelectGizmo(0);
   setWindowTitle("Octopus-GUI - Head & Configuration Window");
  }

 private slots:
  void slotRepaint() {
   electrodeList->setCurrentRow(p->curElecInSeq); legendFrame->repaint();
  }

  void slotLoadReal() {
   QString fileName=QFileDialog::getOpenFileName(this,"Load Real Coordset File",
                                 ".","Object Files (*.orc)");
   if (!fileName.isEmpty()) { p->loadReal(fileName); }
   else { qDebug("An error has been occured while loading measured coords!"); }
  }

  void slotSaveReal() {
   QString fileName=QFileDialog::getSaveFileName(this,"Save Real Coordset File",
                                 ".","Object Files (*.orc)");
   if (!fileName.isEmpty()) { p->saveReal(fileName+".orc"); }
   else { qDebug("An error has been occured while saving measured coords!"); }
  }

  void slotToggleFrame()     { p->hwFrameV   =(p->hwFrameV)   ? false:true; }
  void slotToggleGrid()      { p->hwGridV    =(p->hwGridV)    ? false:true; }
  void slotToggleDig()       { p->hwDigV     =(p->hwDigV)     ? false:true; }
  void slotToggleParam()     { p->hwParamV   =(p->hwParamV)   ? false:true; }
  void slotToggleReal()      { p->hwRealV    =(p->hwRealV)    ? false:true; }
  void slotToggleGizmo()     { p->hwGizmoV   =(p->hwGizmoV)   ? false:true; }
  void slotToggleAvgs()      { p->hwAvgsV    =(p->hwAvgsV)    ? false:true; }
  void slotToggleScalp()     { p->hwScalpV   =(p->hwScalpV)   ? false:true; }
  void slotToggleSkull()     { p->hwSkullV   =(p->hwSkullV)   ? false:true; }
  void slotToggleBrain()     { p->hwBrainV   =(p->hwBrainV)   ? false:true; }
  void slotToggleSource()    { p->hwSourceV  =(p->hwSourceV)  ? false:true; }

  void slotSetScalpParamR(int x) {
   p->scalpParamR=(float)(x)/10.;
   paramRLabel->setText("Parametric Radius ("+
                        dummyString.setNum(p->scalpParamR)+" cm):");
   headGLWidget->slotRepaintGL(2+8+16); // update real+gizmo+avgs
  }

  void slotSetNotchThr(int x) {
   p->notchThreshold=(float)(x)/10.; // .1 uV Resolution
   notchLabel->setText("Notch Level (RMS) ("+
                        dummyString.setNum(p->notchThreshold)+" uV):");
  }

  void slotSetTimePt(int x) { p->slTimePt=(float)(x);
   timePtLabel->setText("Localization Time Point ("+dummyString.setNum(
                          p->cp.avgBwd+x*1000/p->sampleRate
                           )+" ms):");
   headGLWidget->slotRepaintGL(16); // update avgs
  }

  void slotSelectGizmo(int k) { int idx; Gizmo *g=p->gizmo[k];
   p->currentGizmo=k; electrodeList->clear();
   for (int i=0;i<g->seq.size();i++) {
    for (int j=0;j<p->acqChannels.size();j++)
     if (p->acqChannels[j]->physChn==g->seq[i]-1) { idx=j; break; }
    dummyString.setNum(p->acqChannels[idx]->physChn+1);
    electrodeList->addItem(dummyString+" "+p->acqChannels[idx]->name);
   }
   p->curElecInSeq=0;
   for (int j=0;j<p->acqChannels.size();j++)
    if (p->acqChannels[j]->physChn==g->seq[0]-1) { idx=j; break; }
   p->currentElectrode=idx;
   electrodeList->setCurrentRow(p->curElecInSeq);
   headGLWidget->slotRepaintGL(8);
  }

  void slotSelectElectrode(int k) { p->curElecInSeq=k;
   for (int i=0;i<p->acqChannels.size();i++)
    if (p->acqChannels[i]->physChn==p->gizmo[p->currentGizmo]->seq[k]-1)
     { p->currentElectrode=i; break; }
   headGLWidget->slotRepaintGL(2+16); // update real+avgs
  }

  void slotViewElectrode(QListWidgetItem *item) { int idx,pChn;
   QString s=item->text(); QStringList l=s.split(" "); pChn=l[0].toInt()-1;
   for (int i=0;i<p->acqChannels.size();i++)
    if (p->acqChannels[i]->physChn==pChn) { idx=i; break; }
   float theta=p->acqChannels[idx]->param.y;
   float phi=p->acqChannels[idx]->param.z;
   headGLWidget->setView(theta,phi);
  }

  void slotGizmoRealCB() {
   p->gizmoOnReal=gizmoRealCB->isChecked();
   headGLWidget->slotRepaintGL(8); // update gizmo
  }

  void slotElecRealCB() {
   p->elecOnReal=elecRealCB->isChecked();
   headGLWidget->slotRepaintGL(16); // update averages
  }

  void slotAvgAmp(int x) {
   switch (x) {
    case 0: p->avgAmpX=  2.0; avgAmpBG->button(5)->setDown(false); break;
    case 1: p->avgAmpX=  5.0; avgAmpBG->button(5)->setDown(false); break;
    case 2: p->avgAmpX= 10.0; avgAmpBG->button(5)->setDown(false); break;
    case 3: p->avgAmpX= 20.0; avgAmpBG->button(5)->setDown(false); break;
    case 4: p->avgAmpX= 50.0; avgAmpBG->button(5)->setDown(false); break;
    case 5: p->avgAmpX=100.0; avgAmpBG->button(5)->setDown(false); break;
   }
  }

 private:
  RecMaster *p; QWidget *parent; HeadGLWidget *headGLWidget;

  QMenuBar *menuBar;
  QAction *loadRealAction,*saveRealAction,*saveAvgsAction,
          *toggleFrameAction,*toggleGridAction,*toggleDigAction,
          *toggleParamAction,*toggleRealAction,
          *toggleGizmoAction,*toggleAvgsAction,
          *toggleScalpAction,*toggleSkullAction,*toggleBrainAction,
          *toggleSourceAction;

  QLabel *paramRLabel,*notchLabel,*timePtLabel;
  QCheckBox *gizmoRealCB,*elecRealCB; QString dummyString;
  QListWidget *gizmoList,*electrodeList; LegendFrame *legendFrame;

  QButtonGroup *avgAmpBG; QVector<QPushButton*> avgAmpButtons;
  QPushButton *clrAvgsButton;
};

#endif
