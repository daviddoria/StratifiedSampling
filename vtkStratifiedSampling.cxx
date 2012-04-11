#include "vtkStratifiedSampling.h"
#include "TriMesh.h"
#include "cloud.h"
#include "stratify.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"
#include "vtkTriangle.h"
#include "vtkCellArray.h"
#include "vtkVertexGlyphFilter.h"

vtkCxxRevisionMacro(vtkStratifiedSampling, "$Revision: 1.70 $");
vtkStandardNewMacro(vtkStratifiedSampling);

vtkStratifiedSampling::vtkStratifiedSampling()
{
  this->Level = 6;
  this->Lambda = 10.0;
  this->Bad = 0.5;

}

int vtkStratifiedSampling::RequestData(vtkInformation *vtkNotUsed(request),
                                             vtkInformationVector **inputVector,
                                             vtkInformationVector *outputVector)
{
  
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

  for(unsigned int i = 0; i < cloud.vertex.size(); i++)
    {
    points->InsertNextPoint(cloud.vertex[i][0], cloud.vertex[i][1], cloud.vertex[i][2]);
    }

  vtkSmartPointer<vtkPolyData> outputPoints = 
      vtkSmartPointer<vtkPolyData>::New();

  outputPoints->SetPoints(points);

  vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter = 
    vtkSmartPointer<vtkVertexGlyphFilter>::New();
  glyphFilter->SetInputData(outputPoints);
  glyphFilter->Update();

  output->ShallowCopy(glyphFilter->GetOutput());

  return 1;
}


//----------------------------------------------------------------------------
void vtkStratifiedSampling::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

