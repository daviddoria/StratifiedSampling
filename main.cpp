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
#include "vtkArrowSource.h"
#include "vtkLineSource.h"
#include "vtkGlyph3D.h"
#include "vtkPointData.h"
#include "vtkTriangleFilter.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkProperty.h"
#include "vtkPolyDataNormals.h"

int main(int argc, char *argv[])
{
  // start with an arrow shape
  vtkSmartPointer<vtkArrowSource> arrowSource =
    vtkSmartPointer<vtkArrowSource>::New();
  arrowSource->Update();

  // triangulize before normal splitting and as
  // sampling needs triangulized points anyway
  vtkSmartPointer<vtkTriangleFilter> triFilter =
    vtkSmartPointer<vtkTriangleFilter>::New();
#if VTK_MAJOR_VERSION <= 5
  triFilter->SetInput(arrowSource->GetOutput());
#else
  triFilter->SetInputData(arrowSource->GetOutput());
#endif
  triFilter->Update();

  // if you don't want the sampling to interpolate
  // between normals of sharp edges, you have
  // have to split them like this
  vtkSmartPointer<vtkPolyDataNormals> normalFilter =
    vtkSmartPointer<vtkPolyDataNormals>::New();
  normalFilter->SplittingOn();
  normalFilter->SetFeatureAngle(1.0 / 180.0 * 3.1415);
  normalFilter->ComputePointNormalsOn();
#if VTK_MAJOR_VERSION <= 5
  normalFilter->SetInput(triFilter->GetOutput());
#else
  normalFilter->SetInputData(triFilter->GetOutput());
#endif
  normalFilter->Update();

  // now sample
  vtkSmartPointer<vtkStratifiedSampling> stratifiedSampling = 
    vtkSmartPointer<vtkStratifiedSampling>::New();
  stratifiedSampling->SetLevel(8);
  stratifiedSampling->SetLambda(10);
  stratifiedSampling->SetBad(0.5);

#if VTK_MAJOR_VERSION <= 5
  stratifiedSampling->SetInput(normalFilter->GetOutput());
#else
  stratifiedSampling->SetInputData(normalFilter->GetOutput());
#endif
  stratifiedSampling->Update();
  

  // Create a mapper and actor for the points
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

  // Create a mapper and actor for the normals
  vtkSmartPointer<vtkPolyDataMapper> mapperNormals =
    vtkSmartPointer<vtkPolyDataMapper>::New();

  vtkSmartPointer<vtkLineSource> line = vtkSmartPointer<vtkLineSource>::New();

  // default line goes from (-0.5, 0.0, 0.0) to (0.5, 0.0, 0.0)
  // change to go from (0,0,0) to (1,0,0)
  line->SetPoint1(0.0, 0.0, 0.0);
  line->SetPoint2(1.0, 0.0, 0.0);
  line->Update();

  double p[3];
  line->GetPoint1(p);
  std::cout << p[0] << ", " << p[1] << ", " << p[2] << std::endl;
  line->GetPoint2(p);
  std::cout << p[0] << ", " << p[1] << ", " << p[2] << std::endl;

  vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
  glyph->SetInput( stratifiedSampling->GetOutput());
  glyph->SetSourceConnection(line->GetOutputPort());
  glyph->OrientOn();
  glyph->SetVectorModeToUseNormal();
  glyph->SetScaleModeToScaleByVector();
  glyph->SetScaleFactor(0.02);
  glyph->Update();
  mapperNormals->SetInputConnection(glyph->GetOutputPort());

  vtkSmartPointer<vtkActor> actorNormals =
    vtkSmartPointer<vtkActor>::New();
  actorNormals->SetMapper(mapperNormals);

  // Create a mapper and actor for the mesh we used to sample
  vtkSmartPointer<vtkPolyDataMapper> mapperMesh =
    vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
  mapperMesh->SetInput(normalFilter->GetOutput());
#else
  mapperMesh->SetInputData(normalFilter->GetOutput());
#endif

  vtkSmartPointer<vtkActor> actorMesh =
    vtkSmartPointer<vtkActor>::New();
  actorMesh->SetMapper(mapperMesh);
  actorMesh->GetProperty()->SetRepresentationToWireframe();
  actorMesh->SetPosition(0.0, 1.0, 0.0);


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
  renderer->AddActor(actorNormals);
  renderer->AddActor(actorMesh);
  renderer->SetBackground(.2, .3, .4);

  // Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
