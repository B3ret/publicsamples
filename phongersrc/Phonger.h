// hopfer.benjamin@gmail.com
// http://benjaminhopfer.com
// 
// Phonger example application for Qt / vtk
// 32bit/64bit cross compile

#ifndef Phonger_H
#define Phonger_H


#include <vtkActor.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>

#include <QMainWindow>

class Ui_Phonger;

class Phonger : public QMainWindow
{
  Q_OBJECT

public:
 
  Phonger();

  ~Phonger() { };

 
private slots:

  void slotOpenStl();

  void slotSetAmbientColor();
  
  void slotSetDiffuseColor();

  void slotSetSpecularColor();

  void slotDsbChanged(double newPower);
 
  void slotExit();


private:
 
  // Designer form
  Ui_Phonger *ui;

  QString _lastOpenedPath;
  vtkSmartPointer<vtkSTLReader> _stlReader;
  vtkSmartPointer<vtkPolyDataMapper> _mapper;
  vtkSmartPointer<vtkPolyDataNormals> _normals;
  vtkSmartPointer<vtkLinearSubdivisionFilter> _subdiv;
  vtkSmartPointer<vtkActor> _actor;
  vtkSmartPointer<vtkRenderer> _renderer;

  void loadFile(QString const &filePath);

  void pickColor(double const *curColor, QString const &descr, double *newColor);

  void prop2Gui();

  void gui2Prop();

  void update3d();

};
 

#endif