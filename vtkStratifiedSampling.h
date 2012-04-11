#ifndef __vtkStratifiedSampling_h
#define __vtkStratifiedSampling_h

#include "vtkPolyDataAlgorithm.h"

class vtkStratifiedSampling : public vtkPolyDataAlgorithm 
{
public:
  vtkTypeRevisionMacro(vtkStratifiedSampling,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkStratifiedSampling *New();
	  
  vtkSetMacro(Level, int);
  vtkGetMacro(Level, int);
  
  vtkSetMacro(Lambda, double);
  vtkGetMacro(Lambda, double);
  
  vtkSetMacro(Bad, double);
  vtkGetMacro(Bad, double);
  
protected:
  vtkStratifiedSampling();
  
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vtkStratifiedSampling(const vtkStratifiedSampling&);  // Not implemented.
  void operator=(const vtkStratifiedSampling&);  // Not implemented.
  
  int Level;
  double Lambda;
  double Bad;

  
};

#endif
