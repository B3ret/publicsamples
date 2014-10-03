// Example code by Benjamin Hopfer for a blog entry hosted at
// http://www.toptal.com/data-science/3d-data-visualization-with-open-source-tools-an-example-using-vtk
//
// You can find me at http://benjaminhopfer.com

#include <vtkActor.h>
#include <vtkArrayCalculator.h>
#include <vtkCamera.h>
#include <vtkClipDataSet.h>
#include <vtkCutter.h>
#include <vtkDataSetMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkNew.h>
#include <vtkPlane.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRibbonFilter.h>
#include <vtkStreamTracer.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>

int main(int argc, char** argv)
{
  // Setup the renderer
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(0.9, 0.9, 0.9);

  // Setup the render window
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer.Get());
  renWin->SetSize(500, 500);

  // Setup the render window interactor
  vtkNew<vtkRenderWindowInteractor> interact;
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  interact->SetRenderWindow(renWin.Get());
  interact->SetInteractorStyle(style.Get());

  // Read the file
  vtkSmartPointer<vtkXMLUnstructuredGridReader> pumpReader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
  pumpReader->SetFileName("rotor.vtu");

  // Get the pressure range
  pumpReader->Update();
  double pressureRange[2];
  pumpReader->GetOutput()->GetPointData()->GetArray("Pressure")->GetRange(pressureRange);

  // Clip the left part from the input
  vtkNew<vtkPlane> planeLeft;
  planeLeft->SetOrigin(0.0, 0.0, 0.0);
  planeLeft->SetNormal(-1.0, 0.0, 0.0);

  vtkNew<vtkClipDataSet> clipperLeft;
  clipperLeft->SetInputConnection(pumpReader->GetOutputPort());
  clipperLeft->SetClipFunction(planeLeft.Get());

  // Create the wireframe representation for the left part
  vtkNew<vtkDataSetMapper> leftWireMapper;
  leftWireMapper->SetInputConnection(clipperLeft->GetOutputPort());
  leftWireMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> leftWireActor;
  leftWireActor->SetMapper(leftWireMapper.Get());
  leftWireActor->GetProperty()->SetRepresentationToWireframe();
  leftWireActor->GetProperty()->SetColor(0.8, 0.8, 0.8);
  leftWireActor->GetProperty()->SetLineWidth(0.5);
  leftWireActor->GetProperty()->SetOpacity(0.8);
  renderer->AddActor(leftWireActor.Get());

  // Clip the right part from the input
  vtkNew<vtkPlane> planeRight;
  planeRight->SetOrigin(0.0, 0.0, 0.0);
  planeRight->SetNormal(1.0, 0.0, 0.0);

  vtkNew<vtkClipDataSet> clipperRight;
  clipperRight->SetInputConnection(pumpReader->GetOutputPort());
  clipperRight->SetClipFunction(planeRight.Get());

  // Create the wireframe representation for the right part
  vtkNew<vtkDataSetMapper> rightWireMapper;
  rightWireMapper->SetInputConnection(clipperRight->GetOutputPort());
  rightWireMapper->ScalarVisibilityOff();

  vtkNew<vtkActor> rightWireActor;
  rightWireActor->SetMapper(rightWireMapper.Get());
  rightWireActor->GetProperty()->SetRepresentationToWireframe();
  rightWireActor->GetProperty()->SetColor(0.2, 0.2, 0.2);
  rightWireActor->GetProperty()->SetLineWidth(0.5);
  rightWireActor->GetProperty()->SetOpacity(0.1);
  renderer->AddActor(rightWireActor.Get());

  // Create the pressure representation for the left part
  vtkNew<vtkDataSetMapper> pressureColorMapper;
  pressureColorMapper->SetInputConnection(clipperLeft->GetOutputPort());
  pressureColorMapper->SelectColorArray("Pressure");
  pressureColorMapper->SetScalarRange(pressureRange);

  vtkNew<vtkActor> pressureColorActor;
  pressureColorActor->SetMapper(pressureColorMapper.Get());
  pressureColorActor->GetProperty()->SetOpacity(0.5);

  renderer->AddActor(pressureColorActor.Get());

  // Create the source points for the streamlines
  vtkNew<vtkPointSource> pointSource;
  pointSource->SetCenter(0.0, 0.0, 0.015);
  pointSource->SetRadius(0.2);
  pointSource->SetDistributionToUniform();
  pointSource->SetNumberOfPoints(1500);

  // Trace the streamlines
  vtkNew<vtkStreamTracer> tracer;
  tracer->SetInputConnection(clipperRight->GetOutputPort());
  tracer->SetSourceConnection(pointSource->GetOutputPort());
  tracer->SetIntegrationDirectionToBoth();
  tracer->SetIntegratorTypeToRungeKutta45();

  // Compute the velocity magnitues and create the ribbons
  vtkNew<vtkArrayCalculator> magCalc;
  magCalc->SetInputConnection(tracer->GetOutputPort());
  magCalc->AddVectorArrayName("Velocity");
  magCalc->SetResultArrayName("MagVelocity");
  magCalc->SetFunction("mag(Velocity)");

  magCalc->Update();
  double magVelocityRange[2];
  magCalc->GetOutput()->GetPointData()->GetArray("MagVelocity")->GetRange(magVelocityRange);

  // Create and render the ribbons
  vtkNew<vtkRibbonFilter> ribbonFilter;
  ribbonFilter->SetInputConnection(magCalc->GetOutputPort());
  ribbonFilter->SetWidth(0.0005);

  vtkNew<vtkPolyDataMapper> streamlineMapper;
  streamlineMapper->SetInputConnection(ribbonFilter->GetOutputPort());
  streamlineMapper->SelectColorArray("MagVelocity");
  streamlineMapper->SetScalarRange(magVelocityRange);

  vtkNew<vtkActor> streamlineActor;
  streamlineActor->SetMapper(streamlineMapper.Get());
  renderer->AddActor(streamlineActor.Get());

  // Render and show interactive window
  renWin->Render();
  interact->Initialize();
  interact->Start();
  return 0;
}