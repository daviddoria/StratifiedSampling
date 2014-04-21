#ifndef __vtkStratifiedSampling_h
#define __vtkStratifiedSampling_h

#include "vtkPolyDataAlgorithm.h"

class vtkStratifiedSampling : public vtkPolyDataAlgorithm 
{
public:
  // Prevent "error: invalid conversion from 'vtkPolyDataAlgorithm*'
  // to 'vtkStratifiedSampling*'" when building Paraview Plugin
  static vtkStratifiedSampling *New();
  vtkTypeMacro(vtkStratifiedSampling, vtkPolyDataAlgorithm);

  void PrintSelf(ostream& os, vtkIndent indent);
	  
  vtkSetMacro(Level, int);
  vtkGetMacro(Level, int);
  
  vtkSetMacro(Lambda, double);
  vtkGetMacro(Lambda, double);
  
  vtkSetMacro(Bad, double);
  vtkGetMacro(Bad, double);

  vtkSetMacro(RandomSeed, unsigned int);
  vtkGetMacro(RandomSeed, unsigned int);
  
protected:
  vtkStratifiedSampling();
  
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vtkStratifiedSampling(const vtkStratifiedSampling&);  // Not implemented.
  void operator=(const vtkStratifiedSampling&);  // Not implemented.
  
  int Level;
  double Lambda;
  double Bad;
  unsigned int RandomSeed;
};

#endif
