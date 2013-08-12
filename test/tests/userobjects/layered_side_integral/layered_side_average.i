[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 6
  ny = 6
  nz = 6
  # This test currently diffs when run in parallel with ParallelMesh enabled,
  # most likely due to the fact that CONSTANT MONOMIALS are currently not written
  # out correctly in this case.  For more information, see #2122.
  distribution = serial
[]

[Variables]
  [./u]
  [../]
[]

[AuxVariables]
  [./layered_side_average]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Functions]
  [./y]
    type = ParsedFunction
    value = y
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[BCs]
  [./bottom]
    type = DirichletBC
    variable = u
    boundary = bottom
    value = 0
  [../]
  [./top]
    type = DirichletBC
    variable = u
    boundary = top
    value = 1
  [../]
[]

[AuxBCs]
  [./lsia]
    type = SpatialUserObjectAux
    variable = layered_side_average
    boundary = right
    user_object = layered_side_average
  [../]
[]

[UserObjects]
  [./layered_side_average]
    type = LayeredSideAverage
    direction = y
    num_layers = 3
    variable = u
    execute_on = residual
    boundary = right
  [../]
[]

[Executioner]
  type = Steady
[]

[Output]
  output_initial = true
  exodus = true
[]

