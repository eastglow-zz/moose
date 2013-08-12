[Mesh]
  type = GeneratedMesh
  dim = 2

  nx = 10
  ny = 100

  xmin = 0.0
  xmax = 1.0

  ymin = 0.0
  ymax = 10.0

  # The centroid partitioner orders elements based on
  # the position of their centroids
  partitioner = centroid

  # This will order the elements based on the y value of
  # their centroid.  Perfect for meshes predominantly in
  # one direction
  centroid_partitioner_direction = y

  # The CentroidPartitioner itself seems to work OK with ParallelMesh
  # enabled; this test doesn't work because there is something wrong
  # with ExodusII_IO::write_element_data() which causes the processor
  # ID auxiliary field to be written out completely wrong.
  distribution = serial
[]

[Variables]
  active = 'u'

  [./u]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxVariables]
  [./proc_id]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  active = 'diff'

  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[AuxKernels]
  [./proc_id]
    type = ProcessorIDAux
    variable = proc_id
  [../]
[]

[BCs]
  active = 'left right'

  [./left]
    type = DirichletBC
    variable = u
    boundary = 3
    value = 0
  [../]

  [./right]
    type = DirichletBC
    variable = u
    boundary = 1
    value = 1
  [../]
[]

[Executioner]
  type = Steady
  petsc_options = '-snes_mf_operator'
[]

[Output]
  file_base = out
  output_initial = true
  elemental_as_nodal = true
  interval = 1
  exodus = true
  print_linear_residuals = true
  perf_log = true
[]
