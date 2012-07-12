/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef KERNEL_H
#define KERNEL_H

#include "MooseObject.h"
#include "SetupInterface.h"
#include "Coupleable.h"
#include "MooseVariableInterface.h"
#include "FunctionInterface.h"
#include "UserObjectInterface.h"
#include "TransientInterface.h"
#include "MaterialPropertyInterface.h"
#include "PostprocessorInterface.h"
#include "GeometricSearchInterface.h"
#include "Assembly.h"
#include "MooseVariable.h"
#include "SubProblem.h"
#include "MooseTypes.h"

// libMesh
#include "fe.h"
#include "quadrature.h"

class MooseMesh;
class Problem;
class SubProblem;
class Kernel;

template<>
InputParameters validParams<Kernel>();

class Kernel :
  public MooseObject,
  public SetupInterface,
  public Coupleable,
  public ScalarCoupleable,
  public MooseVariableInterface,
  public FunctionInterface,
  public UserObjectInterface,
  public TransientInterface,
  public PostprocessorInterface,
  public MaterialPropertyInterface,
  protected GeometricSearchInterface
{
public:
  Kernel(const std::string & name, InputParameters parameters);

  virtual void computeResidual();
  virtual void computeJacobian();
  /**
   * Computes d-residual / d-jvar... storing the result in Ke.
   */
  virtual void computeOffDiagJacobian(unsigned int jvar);

  /**
   * The variable number that this kernel operates on.
   */
  MooseVariable & variable() { return _var; }

  SubProblem & subProblem() { return _subproblem; }

  // materials
  template<typename T>
  MaterialProperty<T> & getMaterialProperty(const std::string & name);


protected:
  Problem & _problem;
  SubProblem & _subproblem;
  SystemBase & _sys;

  THREAD_ID _tid;

  Assembly & _assembly;
  MooseVariable & _var;
  MooseMesh & _mesh;
  unsigned int _dim;

  const Elem * & _current_elem;
  /// Volume of the current element
  const Real & _current_elem_volume;

  unsigned int _qp;
  const MooseArray< Point > & _q_point;
  QBase * & _qrule;
  const MooseArray<Real> & _JxW;
  const MooseArray<Real> & _coord;

  unsigned int _i, _j;
  // shape functions
  const VariablePhiValue & _phi;
  const VariablePhiGradient & _grad_phi;

// test functions
  const VariableTestValue & _test;
  const VariableTestGradient & _grad_test;
//  const std::vector<std::vector<RealTensor> > & _second_test;

  /// Holds the solution at current quadrature points
  VariableValue & _u;
  /// Holds the solution gradient at the current quadrature points
  VariableGradient & _grad_u;

  /// Time derivative of u
  VariableValue & _u_dot;
  /// Derivative of u_dot wrt u
  VariableValue & _du_dot_du;

  // Single Instance Variables
  Real & _real_zero;
  MooseArray<Real> & _zero;
  MooseArray<RealGradient> & _grad_zero;
  MooseArray<RealTensor> & _second_zero;

  /// Holds residual entries as their accumulated by this Kernel
  DenseVector<Number> _local_re;

  /// The aux variables to save the residual contributions to
  bool _has_save_in;
  std::vector<MooseVariable*> _save_in;
  std::vector<AuxVariableName> _save_in_strings;

  virtual Real computeQpResidual() = 0;
  virtual Real computeQpJacobian();

  /**
   * This is the virtual that derived classes should override for computing an off-diagonal jacobian component.
   */
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  virtual void precalculateResidual();
};


template<typename T>
MaterialProperty<T> &
Kernel::getMaterialProperty(const std::string & name)
{
  if (parameters().isParamValid("block"))
  {
    // check blocks where the kernel is defined
    std::vector<SubdomainName> blocks = parameters().get<std::vector<SubdomainName> >("block");
    for (std::vector<SubdomainName>::iterator it = blocks.begin(); it != blocks.end(); ++it)
      _subproblem.checkMatProp(_subproblem.mesh().getSubdomainID(*it), name);
  }
  else
  {
    // no kernel blocks specified, check all blocks that are in the mesh
    const std::set<SubdomainID> & blocks = _mesh.meshSubdomains();
    for (std::set<SubdomainID>::const_iterator it = blocks.begin(); it != blocks.end(); ++it)
      _subproblem.checkMatProp(*it, name);
  }

  return MaterialPropertyInterface::getMaterialProperty<T>(name);
}

#endif /* KERNEL_H */
