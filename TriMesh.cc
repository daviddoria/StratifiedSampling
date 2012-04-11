#include "TriMesh.h"

bool PolyDataToTriMesh(vtkSmartPointer<vtkPolyData> polydata, TriMesh *mesh)
{
  cout << "inside PolyDataToTriMesh" << endl;
  
  for(vtkIdType i = 0; i < polydata->GetNumberOfPoints(); i++)
    {
    double p[3];
    polydata->GetPoint(i, p);
    mesh->vertices.push_back(point(p[0],p[1],p[2]));
    }
    
  vtkSmartPointer<vtkCellArray> cells = polydata->GetPolys();
  /*  
  cout << polydata->GetNumberOfCells() << " cells." << endl;
  for(vtkIdType i = 0; i < polydata->GetNumberOfCells(); i++)
    {
    
    vtkIdType npts;
    vtkIdType *pts;

    cells->GetNextCell(npts, pts);
    if(npts != 3)
      {
      cout << "face " << i << " is not a triangle, it has " << npts << " points!" << endl;
      return false;
      }
    mesh->faces.push_back(TriMesh::Face(pts[0], pts[1], pts[2]));
    }
      */
  
    vtkIdType npts;
    vtkIdType *pts;
  cells->InitTraversal();
  while(cells->GetNextCell(npts, pts))
    {
    
    if(npts != 3)
      {
      cout << "not a triangle, it has " << npts << " points!" << endl;
      return false;
      }
      mesh->faces.push_back(TriMesh::Face(pts[0], pts[1], pts[2]));
    }
      
  return true;
}