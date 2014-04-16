#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include "vtkStratifiedSampling.h"

int main(int argc, char *argv[])
{
  vtkSmartPointer<vtkSphereSource> sphereSource = 
    vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->Update();

  vtkSmartPointer<vtkStratifiedSampling> stratifiedSampling = 
    vtkSmartPointer<vtkStratifiedSampling>::New();
  stratifiedSampling->SetLevel(6);
  stratifiedSampling->SetLambda(10);
  stratifiedSampling->SetBad(0.5);
#if VTK_MAJOR_VERSION <= 5
  stratifiedSampling->SetInput(sphereSource->GetOutput());
#else
  stratifiedSampling->SetInputData(sphereSource->GetOutput());
#endif
  stratifiedSampling->Update();
  
//  vtkSmartPointer<vtkXMLPolyDataWriter> writer =
//      vtkSmartPointer<vtkXMLPolyDataWriter>::New();
//#if VTK_MAJOR_VERSION <= 5
//  writer->SetInput(stratifiedSampling->GetOutput());
//#else
//  writer->SetInputData(stratifiedSampling->GetOutput());
//#endif
//  writer->SetFileName("test.vtp");
//  writer->Write();

  // Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
  mapper->SetInput(stratifiedSampling->GetOutput());
#else
  mapper->SetInputData(stratifiedSampling->GetOutput());
#endif
  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);

  // Create a renderer, render window, and interactor
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  vtkInteractorStyleTrackballCamera* style =
      vtkInteractorStyleTrackballCamera::New();
  renderWindowInteractor->SetInteractorStyle(style);

  // Add the actor to the scene
  renderer->AddActor(actor);
  renderer->SetBackground(.2, .3, .4);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
