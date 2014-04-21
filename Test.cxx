#include <vtkMath.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

#include "vtkStratifiedSampling.h"
    
int main(int argc, char *argv[])
{
  // Verify arguments
  if(argc < 2)
  {
      std::cerr << "Required arguments: groundTruthFileName" << std::endl;
      exit(-1);
  }

  // Parse arguments
  std::string groundTruthFilename = argv[1];

  // Generate an object
  vtkSmartPointer<vtkSphereSource> sphereSource = 
      vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->Update();
  
  // Sample the object
  vtkSmartPointer<vtkStratifiedSampling> stratifiedSampling = 
      vtkSmartPointer<vtkStratifiedSampling>::New();
  stratifiedSampling->SetInputConnection(sphereSource->GetOutputPort());
  stratifiedSampling->SetRandomSeed(0); // Use the same random numbers each time
  stratifiedSampling->Update();
  
  // Read the ground truth object
  vtkSmartPointer<vtkXMLPolyDataReader> groundTruthReader =
      vtkSmartPointer<vtkXMLPolyDataReader>::New();
  groundTruthReader->SetFileName(groundTruthFilename.c_str());
  groundTruthReader->Update();

  // Compare global properties
  if(groundTruthReader->GetOutput()->GetNumberOfPoints() !=
     stratifiedSampling->GetOutput()->GetNumberOfPoints())
  {
    std::cout << "The number of points of the sampled object is "
              << stratifiedSampling->GetOutput()->GetNumberOfPoints()
              << " but the number of points of the ground truth is "
              << groundTruthReader->GetOutput()->GetNumberOfPoints() << std::endl;
  }

  int returnStatus = EXIT_SUCCESS;

  // Compare point locations
  for(vtkIdType i = 0; i < stratifiedSampling->GetOutput()->GetNumberOfPoints(); ++i)
  {
    double sampledPoint[3];
    double groundTruthPoint[3];

    stratifiedSampling->GetOutput()->GetPoint(i, sampledPoint);
    groundTruthReader->GetOutput()->GetPoint(i, groundTruthPoint);

    double squaredDistance = vtkMath::Distance2BetweenPoints(sampledPoint, groundTruthPoint);

    double epsilon = 1e-6;
    if(fabs(squaredDistance) > epsilon)
    {
        std::cerr << "Position of point " << i << " does not match grount truth!" << std::endl;
        returnStatus = EXIT_FAILURE;
    }
  }

  return returnStatus;
}
