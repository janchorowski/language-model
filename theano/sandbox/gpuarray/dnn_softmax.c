#section support_code_struct

cudnnTensorDescriptor_t APPLY_SPECIFIC(input);
cudnnTensorDescriptor_t APPLY_SPECIFIC(output);

#section init_code_struct

APPLY_SPECIFIC(input) = NULL;
APPLY_SPECIFIC(output) = NULL;

{
  cudnnStatus_t err;
  err = cudnnCreateTensorDescriptor(&APPLY_SPECIFIC(input));
  if (err != CUDNN_STATUS_SUCCESS) {
    PyErr_Format(PyExc_MemoryError, "could not allocate tensor descriptor: %s",
                 cudnnGetErrorString(err));
    FAIL;
  }
  err = cudnnCreateTensorDescriptor(&APPLY_SPECIFIC(output));
  if (err != CUDNN_STATUS_SUCCESS) {
    PyErr_Format(PyExc_MemoryError, "could not allocate tensor descriptor: %s",
                 cudnnGetErrorString(err));
    FAIL;
  }
}

#section cleanup_code_struct

if (APPLY_SPECIFIC(input) != NULL)
  cudnnDestroyTensorDescriptor(APPLY_SPECIFIC(input));
if (APPLY_SPECIFIC(output) != NULL)
  cudnnDestroyTensorDescriptor(APPLY_SPECIFIC(output));

#section support_code_struct

int APPLY_SPECIFIC(softmax)(PyGpuArrayObject *x,
                            PyGpuArrayObject **out) {
  cudnnStatus_t err;
  PyGpuContextObject *c = pygpu_default_context();

  if (c_set_tensorNd(x, APPLY_SPECIFIC(input)) != 0)
    return 1;

  if (theano_prep_output(out, PyGpuArray_NDIM(x),
                         PyGpuArray_DIMS(x), x->ga.typecode,
                         GA_C_ORDER, c) != 0)
    return 1;

  if (c_set_tensorNd(*out, APPLY_SPECIFIC(output)) != 0)
    return 1;

  {
    const float alphaf = 1;
    const float betaf = 0;
    const double alphad = 1;
    const double betad = 0;
    void *alpha, *beta;

    switch (x->ga.typecode) {
    case GA_DOUBLE:
      alpha = (void *)&alphad;
      beta = (void *)&betad;
      break;
    case GA_FLOAT:
    case GA_HALF:
      alpha = (void *)&alphaf;
      beta = (void *)&betaf;
      break;
    default:
      PyErr_SetString(PyExc_TypeError, "Unsupported type in softmax");
      return 1;
    }

    cuda_enter(c->ctx);
    err = cudnnSoftmaxForward(
      APPLY_SPECIFIC(_handle),
      SOFTMAX_ALGO,
      SOFTMAX_MODE,
      alpha,
      APPLY_SPECIFIC(input),
      PyGpuArray_DEV_DATA(x),
      beta,
      APPLY_SPECIFIC(output),
      PyGpuArray_DEV_DATA(*out)
    );
    cuda_exit(c->ctx);
  }

  if (err != CUDNN_STATUS_SUCCESS) {
    PyErr_Format(PyExc_RuntimeError, "error during operation: %s",
                 cudnnGetErrorString(err));
    return 1;
  }
  return 0;
}
