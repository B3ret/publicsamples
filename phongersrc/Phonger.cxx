// hopfer.benjamin@gmail.com
// http://benjaminhopfer.com
// 
// Phonger example application for Qt / vtk
// 32bit/64bit cross compile

#include "Phonger.h"

#include "ui_Phonger.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QStandardPaths>

#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>


static double const GRADIENT_BACKGROUND_TOP[3] = { 0.4, 0.4, 0.4 };
static double const GRADIENT_BACKGROUND_BOT[3] = { 0.6, 0.6, 0.6 };

 
Phonger::Phonger()
: _stlReader(vtkSmartPointer<vtkSTLReader>::New()),
  _normals(vtkSmartPointer<vtkPolyDataNormals>::New()),
  _subdiv(vtkSmartPointer<vtkLinearSubdivisionFilter>::New()),
  _mapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
  _actor(vtkSmartPointer<vtkActor>::New()),
  _renderer(vtkSmartPointer<vtkRenderer>::New())
{
  this->ui = new Ui_Phonger;
  this->ui->setupUi(this);
 
  connect(this->ui->pbOpenSTL, SIGNAL(clicked()), this, SLOT(slotOpenStl()));

  connect(this->ui->pbColAmbient, SIGNAL(clicked()), this, SLOT(slotSetAmbientColor()));
  connect(this->ui->pbColDiffuse, SIGNAL(clicked()), this, SLOT(slotSetDiffuseColor()));
  connect(this->ui->pbColSpecular, SIGNAL(clicked()), this, SLOT(slotSetSpecularColor()));

  connect(this->ui->dsbAmbCoeff, SIGNAL(valueChanged(double)), this, SLOT(slotDsbChanged(double)));
  connect(this->ui->dsbDiffCoeff, SIGNAL(valueChanged(double)), this, SLOT(slotDsbChanged(double)));
  connect(this->ui->dsbSpecCoeff, SIGNAL(valueChanged(double)), this, SLOT(slotDsbChanged(double)));
  connect(this->ui->dsbSpecPower, SIGNAL(valueChanged(double)), this, SLOT(slotDsbChanged(double)));

  connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));

  prop2Gui();

  update3d();
}
 

void Phonger::loadFile(QString const &filePath)
{
  _stlReader->SetFileName(filePath.toLocal8Bit());
  _stlReader->SetMerging(1);

  _normals->SetInputConnection(_stlReader->GetOutputPort());
  _normals->ConsistencyOn();
  _normals->SplittingOn();
  _normals->SetFeatureAngle(30);

  _normals->Update();

  _mapper->SetInputConnection(_normals->GetOutputPort());

  _actor->SetMapper(_mapper);
  _actor->GetProperty()->SetRepresentationToSurface();
  _actor->GetProperty()->SetInterpolationToPhong();

  // VTK Renderer
  _renderer = vtkSmartPointer<vtkRenderer>::New();
  _renderer->GradientBackgroundOn();
  _renderer->SetBackground(GRADIENT_BACKGROUND_TOP[0], GRADIENT_BACKGROUND_TOP[1], GRADIENT_BACKGROUND_TOP[2]);
  _renderer->SetBackground2(GRADIENT_BACKGROUND_BOT[0], GRADIENT_BACKGROUND_BOT[1], GRADIENT_BACKGROUND_BOT[2]);
  _renderer->GetActiveCamera()->ParallelProjectionOn();

  // Transparency support
  vtkRenderWindow *renderWindow = this->ui->qvtkWidget->GetRenderWindow();
  renderWindow->SetAlphaBitPlanes(1);
  renderWindow->SetMultiSamples(0);

  _renderer->SetUseDepthPeeling(1);
  _renderer->SetMaximumNumberOfPeels(10);
  _renderer->SetOcclusionRatio(0.05);

  // VTK/Qt wedded
  this->ui->qvtkWidget->GetRenderWindow()->RemoveRenderer(_renderer);
  this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(_renderer);

  update3d();
}


void Phonger::prop2Gui()
{
  ui->dsbAmbCoeff->setValue(_actor->GetProperty()->GetAmbient());
  ui->dsbDiffCoeff->setValue(_actor->GetProperty()->GetDiffuse());
  ui->dsbSpecCoeff->setValue(_actor->GetProperty()->GetSpecular());
  ui->dsbSpecPower->setValue(_actor->GetProperty()->GetSpecularPower());
}


void Phonger::gui2Prop()
{
  _actor->GetProperty()->SetAmbient(ui->dsbAmbCoeff->value());
  _actor->GetProperty()->SetDiffuse(ui->dsbDiffCoeff->value());
  _actor->GetProperty()->SetSpecular(ui->dsbSpecCoeff->value());
  _actor->GetProperty()->SetSpecularPower(ui->dsbSpecPower->value());

  update3d();
}


void Phonger::update3d()
{
  _renderer->RemoveActor(_actor);
  _renderer->AddActor(_actor);

  ui->qvtkWidget->update();
}


void Phonger::slotOpenStl()
{
  QStringList folders;
  folders.push_back(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)[0]);
  folders.push_back(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]);
  folders.push_back(QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0]);

  QFileDialog fileDialog(this, tr("Choose Stl file"));
  fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
  fileDialog.setFileMode(QFileDialog::ExistingFiles);
  fileDialog.setDirectory(_lastOpenedPath);
  fileDialog.setHistory(folders);
  fileDialog.setNameFilters({ tr("Stl files (*.stl)") });
  fileDialog.setViewMode(QFileDialog::Detail);

  if (fileDialog.exec())
  {
    QString firstPath = fileDialog.selectedFiles()[0];
    _lastOpenedPath = firstPath;
    loadFile(firstPath);
  }

  _renderer->RemoveActor(_actor);
  _renderer->AddActor(_actor);

  _renderer->ResetCameraClippingRange();
  _renderer->ResetCamera();
}


void Phonger::slotSetAmbientColor()
{
  double curColor[3], newColor[3];

  _actor->GetProperty()->GetAmbientColor(curColor);

  pickColor(curColor, tr("ambient"), newColor);

  _actor->GetProperty()->SetAmbientColor(newColor);

  update3d();
}


void Phonger::slotSetDiffuseColor()
{
  double curColor[3], newColor[3];

  _actor->GetProperty()->GetDiffuseColor(curColor);

  pickColor(curColor, tr("diffuse"), newColor);

  _actor->GetProperty()->SetDiffuseColor(newColor);

  update3d();
}


void Phonger::slotSetSpecularColor()
{
  double curColor[3], newColor[3];

  _actor->GetProperty()->GetSpecularColor(curColor);

  pickColor(curColor, tr("specular"), newColor);

  _actor->GetProperty()->SetSpecularColor(newColor);

  update3d();
}


void Phonger::pickColor(double const *curColor, QString const &descr, double *newColor)
{
  QColor curQColor(curColor[0] * 255, curColor[1] * 255, curColor[2] * 255);

  QColor pickedColor = QColorDialog::getColor(curQColor, this, tr("Pick %1 color").arg(descr));

  newColor[0] = pickedColor.redF();
  newColor[1] = pickedColor.greenF();
  newColor[2] = pickedColor.blueF();
}


void Phonger::slotDsbChanged(double newPower)
{
  gui2Prop();
}


void Phonger::slotExit()
{
  qApp->exit();
}