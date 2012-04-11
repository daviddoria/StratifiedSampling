#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataWriter.h>

#include "vtkStratifiedSampling.h"
    
int main(int argc, char *argv[])
{
  vtkSmartPointer<vtkSphereSource> sphereSource = 
      vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->Update();
  
  vtkSmartPointer<vtkStratifiedSampling> stratifiedSampling = 
      vtkSmartPointer<vtkStratifiedSampling>::New();
  stratifiedSampling->SetInputConnection(sphereSource->GetOutputPort());
  stratifiedSampling->Update();
  
  return EXIT_SUCCESS;
}
