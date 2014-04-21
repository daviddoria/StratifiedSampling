#include "vtkStratifiedSampling.h"
#include "TriMesh.h"
#include "cloud.h"
#include "stratify.h"

#include <vtkCellArray.h>
#include <vtkDataObject.h>
#include <vtkFloatArray.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkTriangle.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>

vtkStandardNewMacro(vtkStratifiedSampling);

vtkStratifiedSampling::vtkStratifiedSampling()
{
  this->Level = 6;
  this->Lambda = 10.0;
  this->Bad = 0.5;
  this->RandomSeed = time(NULL);
}

int vtkStratifiedSampling::RequestData(vtkInformation *vtkNotUsed(request),
                                             vtkInformationVector **inputVector,
                                             vtkInformationVector *outputVector)
{
  // Control the randomness
  srand(this->RandomSeed);
  srand48(this->RandomSeed);

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
    
  // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  cout << "There are " << input->GetNumberOfPoints() << " points." << endl;
  cout << "There are " << input->GetNumberOfCells() << " cells." << endl;
  
  vtkPolyData *output = vtkPolyData::SafeDownCast(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  TriMesh *mesh = new TriMesh;
  
  PolyDataToTriMesh(input, mesh);
  cout << "vertices: " <<  mesh->vertices.size() << " faces: " << mesh->faces.size() << endl;
  
  Cloud cloud;
  Array<char> mask;

  SamplerOctree *tree = StratifiedSample(mesh, 
                        this->Level, this->Lambda, this->Bad,
                        &cloud, mask);

  output->Initialize();
  vtkSmartPointer<vtkPoints> points = 
      vtkSmartPointer<vtkPoints>::New();

  vtkSmartPointer<vtkFloatArray> pointNormalsArray =
    vtkSmartPointer<vtkFloatArray>::New();
  pointNormalsArray->SetNumberOfComponents(3); //3d normals (ie x,y,z)
  pointNormalsArray->SetNumberOfTuples(cloud.vertex.size());

  for(unsigned int i = 0; i < cloud.vertex.size(); i++)
    {
    points->InsertNextPoint(cloud.vertex[i][0], cloud.vertex[i][1], cloud.vertex[i][2]);

    float pN[3] = {cloud.normal[i][0], cloud.normal[i][1], cloud.normal[i][2]};
    pointNormalsArray->SetTuple(i, pN);
    }

  vtkSmartPointer<vtkPolyData> outputPoints = 
      vtkSmartPointer<vtkPolyData>::New();

  outputPoints->SetPoints(points);

  // Add the normals to the points in the polydata
  outputPoints->GetPointData()->SetNormals(pointNormalsArray);

  vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter = 
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
#if VTK_MAJOR_VERSION <= 5
  glyphFilter->SetInput(outputPoints);
#else
  glyphFilter->SetInputData(outputPoints);
#endif
  glyphFilter->Update();

  output->ShallowCopy(glyphFilter->GetOutput());

  return 1;
}

//----------------------------------------------------------------------------
void vtkStratifiedSampling::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
