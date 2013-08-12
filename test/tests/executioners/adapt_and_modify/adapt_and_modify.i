[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 15
  ny = 15
  # This example uses Adaptivity Indicators, which are written out as
  # CONSTANT MONOMIAL variables, which don't currently work correctly
  # with ParallelMesh in parallel.  See #2122 for more information.
  distribution = serial
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
  [./td]
    type = TimeDerivative
    variable = u
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = u
    boundary = left
    value = 0
  [../]
  [./right]
    type = DirichletBC
    variable = u
    boundary = right
    value = 1
  [../]
[]

[UserObjects]
  [./rh_uo]
    type = RandomHitUserObject
    execute_on = timestep_begin
    num_hits = 1
  [../]
  [./rhsm]
    type = RandomHitSolutionModifier
    execute_on = custom
    modify = u
    random_hits = rh_uo
    amount = 1000
  [../]
[]

[Executioner]
  type = AdaptAndModify
  num_steps = 4
  dt = 1e-3
  petsc_options = -snes_mf_operator
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  adapt_cycles = 2
[]

[Adaptivity]
  marker = rhm # Switch to combo to get the effect of both
  [./Indicators]
    [./gji]
      type = GradientJumpIndicator
      variable = u
    [../]
  [../]
  [./Markers]
    [./rhm]
      type = RandomHitMarker
      random_hits = rh_uo
    [../]
    [./efm]
      type = ErrorFractionMarker
      coarsen = 0.001
      indicator = gji
      refine = 0.8
    [../]
    [./combo]
      type = ComboMarker
      markers = 'efm rhm'
    [../]
  [../]
[]

[Output]
  output_initial = true
  exodus = true
  perf_log = true
[]

