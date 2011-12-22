/*
 * CudaMemoryAllocator.cu
 *
 * Privides functionality related to memory management and device
 * enumeration of CUDA capable hardware.
 *
 *  Created on: Nov 21, 2011
 *      Author: Devin Robison
 */
#include <spatialops/structured/CudaMemoryAllocator.h>

namespace ema {
namespace cuda {

void CudaSetDevice(unsigned int device) {
#ifdef DEBUG_EXT_ALLOC_CUDA_DEVICE_MNGR
  std::cout << "CudaSetdevice wrapper called, setting thread device as: " << device << std::endl;
#endif
  cudaError err;

  if (cudaSuccess != (err = cudaSetDevice(device))) {
    std::ostringstream msg;
    msg << "Failed to set thread device, at " << __FILE__ << " : " << __LINE__
        << std::endl;
    msg << "\t - " << cudaGetErrorString(err);
    throw(std::runtime_error(msg.str()));
  }
}

void CudaMalloc(void** src, size_t sz, unsigned int device) {
#ifdef  DEBUG_EXT_ALLOC_MEM
  std::cout << "CudaMalloc wrapper called (src,size,device)-> (";
  std::cout << src << "," << sz << "," << device << ")" << std::endl;
#endif
  cudaError err;

  CudaSetDevice(device);
  if (cudaSuccess != (err = cudaMalloc(src, sz))) {
    std::ostringstream msg;
    msg << "CudaMalloc failed, at " << __FILE__ << " : " << __LINE__
        << std::endl;
    msg << "\t - " << cudaGetErrorString(err);
    throw(std::runtime_error(msg.str()));
  }

#ifdef  DEBUG_EXT_ALLOC_MEM
  std::cout << "CudaMemcpy wrapper returning";
#endif
}

/*---------------------------------------------------------------------*/

void CudaFree(void* src, unsigned int device) {
#ifdef  DEBUG_EXT_ALLOC_MEM
  std::cout << "CudaFree wrapper called (void** src, int device) ->";
  std::cout << "(" << src << "," << device << ")\n";
#endif
  cudaError err;

  CudaSetDevice(device);
  if (cudaSuccess != (err = cudaFree(src))) {
    std::ostringstream msg;
    msg << "CudaMalloc failed, at " << __FILE__ << " : " << __LINE__
        << std::endl;
    msg << "\t - " << cudaGetErrorString(err);
    throw(std::runtime_error(msg.str()));
  }
}

/*---------------------------------------------------------------------*/

void CudaMemcpy(void* dest, void* src, size_t sz, unsigned int device,
    cudaMemcpyKind cmkk) {
#ifdef  DEBUG_EXT_ALLOC_MEM
  std::cout << "CudaMemcpy wrapper called (src,dest,size,device,type)-> (";
  std::cout << src << "," << dest << "," << sz << "," << device << "," << cmkk << std::endl;
#endif
  cudaError err;

  CudaSetDevice(device);
  if (cudaSuccess != (err = cudaMemcpy(dest, src, sz, cmkk))) {
    std::ostringstream msg;
    msg << "Memcopy failed, at" << __FILE__ << " : " << __LINE__ << std::endl;
    msg << "\t - " << cudaGetErrorString(err);
    throw(std::runtime_error(msg.str()));
  }

#ifdef  DEBUG_EXT_ALLOC_MEM
  std::cout << "CudaMemcpy wrapper exiting\n";
#endif
}

/*---------------------------------------------------------------------*/

void CudaMemset(void* dest, int value, size_t sz, unsigned int device) {
#ifdef  DEBUG_EXT_SET_MEM
  std::cout << "CudaMemset wrapper called (src, value, size, device)-> (";
  std::cout << src << "," << value << "," << sz << "," << device << "," << std::endl;
#endif
  cudaError err;

  CudaSetDevice(device);
  if (cudaSuccess != (err = cudaMemset(dest, value, sz))) {
    std::ostringstream msg;
    msg << "Memset failed, at" << __FILE__ << " : " << __LINE__ << std::endl;
    msg << "\t - " << cudaGetErrorString(err);
    throw(std::runtime_error(msg.str()));
  }

#ifdef  DEBUG_EXT_ALLOC_MEM
  std::cout << "CudaMemset wrapper exiting\n";
#endif
}

/******************* CUDADeviceManager Implementation ******************/

CUDADeviceManager& CUDADeviceManager::self() {
  static CUDADeviceManager CDM;

  return CDM;
}

/*---------------------------------------------------------------------*/

int CUDADeviceManager::get_device_count() const {
  return device_count;
}

/*---------------------------------------------------------------------*/

const CUDAMemStats& CUDADeviceManager::get_memory_statistics(int K) const {
  if (K < 0 || K >= device_count) {
    throw std::range_error("Specified device index out of bounds");
  }

  return (*device_stats[K]);
}

/*---------------------------------------------------------------------*/

void CUDADeviceManager::update_memory_statistics() {
  int device;

#ifdef  DEBUG_EXT_ALLOC_CUDA_DEVICE_MNGR
  std::cout << "CUDADeviceManager::update_memory_statistics -> called\n";
#endif

  for (device = 0; device < device_count; ++device) {
    CUDAMemStats& cms = *device_stats[device];
    cudaMemGetInfo(&cms.f, &cms.t);
    cms.inuse = cms.t - cms.f;
  }
}

/*---------------------------------------------------------------------*/

void CUDADeviceManager::print_device_info() const {
  int device = 0;

  for (std::vector<cudaDeviceProp*>::const_iterator cdpit =
      device_props.begin(); cdpit != device_props.end(); ++cdpit) {

    cudaDeviceProp& p = *device_props.back();

    std::cout << "\n\t[" << device << "] " << p.name << " ("
        << (p.integrated ? "Integrated" : "Discrete") << ")";
    std::cout << ", Compute Capability: " << p.major << "." << p.minor
        << std::endl;
    std::cout << "\n\t Multi-Processors: " << p.multiProcessorCount;
    std::cout << "\n\t  - Clock Rate: " << p.clockRate / 1000 << " MHz";
    std::cout << "\n\t  - Max Threads Per Dim: " << p.maxThreadsDim[0] << " x "
        << p.maxThreadsDim[1] << " x " << p.maxThreadsDim[2];
    std::cout << "\n\t  - Max Grid Size: " << p.maxGridSize[0] << " x "
        << p.maxGridSize[1] << " x " << p.maxGridSize[2];
    std::cout << "\n\t  - Warp Size: " << p.warpSize;
    std::cout << std::endl;
    std::cout << "\n\t Memory";
    std::cout << "\n\t  - Global: " << p.totalGlobalMem / 1000000 << " MB";
    std::cout << "\n\t  - Const: " << p.totalConstMem / 1000 << " KB";
    std::cout << "\n\t  - Shared ( Per Block ): " << p.sharedMemPerBlock / 1000
        << " KB";
    std::cout << "\n\t  - Registers Per Block (32 bit): " << p.regsPerBlock;
    std::cout << std::endl;

    device++;
  }
}

CUDADeviceManager::~CUDADeviceManager() {
#ifdef DEBUG_EXT_ALLOC_CUDA_DEVICE_MNGR
  std::cout << "CUDADeviceManager::~CUDADeviceManager() called" << std::endl;
#endif
  // Free device property structures
  for (std::vector<cudaDeviceProp*>::const_iterator cdpit =
      device_props.begin(); cdpit != device_props.end(); ++cdpit) {
    delete (*cdpit);
  }
  for (std::vector<CUDAMemStats*>::const_iterator cmsit = device_stats.begin();
      cmsit != device_stats.end(); ++cmsit) {
    delete (*cmsit);
  }
#ifdef DEBUG_EXT_ALLOC_CUDA_DEVICE_MNGR
  std::cout << "CUDADeviceManager::~CUDADeviceManager() finished" << std::endl;
#endif
}

/*---------------------------------------------------------------------*/

CUDADeviceManager::CUDADeviceManager() {
  //Gather system parameters
  cudaGetDeviceCount(&device_count);

#ifdef DEBUG_EXT_ALLOC_CUDA_DEVICE_MNGR
  std::cout << "Initializing CUDADeviceManager, found " << device_count << " devices." << std::endl;
#endif

  for (int device = 0; device < device_count; ++device) {
#ifdef DEBUG_EXT_ALLOC_CUDA_DEVICE_MNGR
    std::cout << "Processing device (" << device << ")\n";
#endif
    device_props.push_back(new cudaDeviceProp());
    device_stats.push_back(new CUDAMemStats());
    cudaGetDeviceProperties(device_props.back(), device);
  }

#ifdef DEBUG_EXT_ALLOC_CUDA_DEVICE_MNGR
  print_device_info();
#endif
}

/******************* CUDADeviceInterface Implementation *********************/
CUDADeviceInterface::~CUDADeviceInterface() {

}

CUDADeviceInterface::CUDADeviceInterface() {

}

CUDADeviceInterface& CUDADeviceInterface::self() {
  static CUDADeviceInterface CDI;

  return CDI;
}

/*---------------------------------------------------------------------*/

int CUDADeviceInterface::get_device_count() const {
  return CUDADeviceManager::self().get_device_count();
}

/*---------------------------------------------------------------------*/

const CUDAMemStats& CUDADeviceInterface::get_memory_statistics(int K) const {
  return CUDADeviceManager::self().get_memory_statistics(K);
}

/*---------------------------------------------------------------------*/

void CUDADeviceInterface::update_memory_statistics() {
  CUDADeviceManager::self().update_memory_statistics();
}

/*---------------------------------------------------------------------*/

void CUDADeviceInterface::print_device_info() const {
  CUDADeviceManager::self().print_device_info();
}

/*---------------------------------------------------------------------*/

CUDASharedPointer CUDADeviceInterface::get_shared_pointer(unsigned long int N,
    unsigned int K) {
  void* x;

#ifdef  DEBUG_EXT_ALLOC_MEM
  std::cout << "CUDADeviceInterface::get_shared_pointer -> Allocating new shared pointer, " << N << " bytes on device " << K << std::endl;
#endif
  if (K > CUDADeviceManager::self().device_count) {
    std::ostringstream msg;
    msg << "CudaMalloc failed, at " << __FILE__ << " : " << __LINE__
        << std::endl;
    msg << "\t - Invalid device index '" << K << "'";
    throw(std::range_error(msg.str()));
  }

  CudaMalloc(&x, N, K);

  return CUDASharedPointer(x, K);
}

/*---------------------------------------------------------------------*/

void* CUDADeviceInterface::get_raw_pointer(unsigned long int N,
    unsigned int K) {
  void* x;

#ifdef  DEBUG_EXT_ALLOC_MEM
  std::cout << "CUDADeviceInterface::get_raw_pointer -> Allocating new raw pointer, " << N << " bytes on device " << K << std::endl;
#endif
  if (K > CUDADeviceManager::self().device_count) {
    std::ostringstream msg;
    msg << "CudaMalloc failed, at " << __FILE__ << " : " << __LINE__
        << std::endl;
    msg << "\t - Invalid device index '" << K << "'";
    throw(std::range_error(msg.str()));
  }

  CudaMalloc(&x, N, K);

  return x;
}

/*---------------------------------------------------------------------*/

void CUDADeviceInterface::memcpy_to(CUDASharedPointer& dest, void* src,
    size_t sz) {
  CudaMemcpy(dest.ptr_, src, sz, (*dest.deviceID_), cudaMemcpyHostToDevice);
}

/*---------------------------------------------------------------------*/

void CUDADeviceInterface::memcpy_to(void* dest, void* src, size_t sz,
    unsigned int deviceID) {
  CudaMemcpy(dest, src, sz, deviceID, cudaMemcpyHostToDevice);
}

/*---------------------------------------------------------------------*/

void CUDADeviceInterface::memcpy_from(void* dest, CUDASharedPointer& src,
    size_t sz) {
  CudaMemcpy(dest, src.ptr_, sz, (*src.deviceID_), cudaMemcpyDeviceToHost);
}

/*---------------------------------------------------------------------*/

void CUDADeviceInterface::memcpy_from(void* dest, void* src, size_t sz,
    unsigned int deviceID) {
  CudaMemcpy(dest, src, sz, deviceID, cudaMemcpyDeviceToHost);
}

/*---------------------------------------------------------------------*/

void CUDADeviceInterface::memset(void* dest, int value, size_t sz,
    unsigned int deviceID) {
  CudaMemset(dest, value, sz, deviceID);
}

/*---------------------------------------------------------------------*/

void CUDADeviceInterface::memset(CUDASharedPointer& dest, int value,
    size_t sz) {
  CudaMemset(dest.ptr_, value, sz, (*dest.deviceID_));
}

/*---------------------------------------------------------------------*/

void CUDADeviceInterface::release(CUDASharedPointer& x) {
  x.detatch();
}

/*---------------------------------------------------------------------*/

void CUDADeviceInterface::release(void* x, unsigned int deviceID) {
  CudaFree(x, deviceID);
}

/******************* CUDASharedPointer Implementation *********************/
CUDASharedPointer::~CUDASharedPointer() {
#ifdef DEBUG_EXT_CUDA_SHARED_PTR
  std::cout << "CUDASharedPointer::~CUDASharedPointer called\n\n";
#endif
  detatch();
}

/*---------------------------------------------------------------------*/

CUDASharedPointer::CUDASharedPointer() :
    ptr_(NULL), refCount_(NULL), deviceID_(NULL) {
#ifdef DEBUG_EXT_CUDA_SHARED_PTR
  std::cout << "CUDASharedPointer::CUDASharedPointer called\n\n";
#endif
  /** Free floating pointer **/
}

/*---------------------------------------------------------------------*/

CUDASharedPointer::CUDASharedPointer(void* ptr, int K) :
    ptr_(ptr), refCount_(new int(1)), deviceID_(new int(K)) {
  /** Pointer initialized to wrap a normal void pointer **/
#ifdef DEBUG_EXT_CUDA_SHARED_PTR
  std::cout << "CUDASharedPointer::CUDASharedPointer(void* ptr, int K), called ->";
  std::cout << "(" << ptr << "," << K << ")\n\n";
#endif
}

CUDASharedPointer::CUDASharedPointer(const CUDASharedPointer& x) :
    ptr_(NULL), refCount_(NULL), deviceID_(NULL) {
  (*this) = x;
}

/*---------------------------------------------------------------------*/

CUDASharedPointer& CUDASharedPointer::operator=(void* x) {
#ifdef DEBUG_EXT_CUDA_SHARED_PTR
  std::cout << "CUDASharedPointer assigned to void*";
#endif
  if (refCount_ != NULL) {
#ifdef DEBUG_EXT_CUDA_SHARED_PTR
    std::cout << "\n\t - Non-zero ref count, decrementing";
#endif
    --(*refCount_);
    if ((*refCount_) == 0) {
#ifdef DEBUG_EXT_CUDA_SHARED_PTR
      std::cout << "\n\t - Pointer ref count is zero, freeing host and device memory";
#endif
      delete refCount_;
      refCount_ = NULL;

      CudaFree(ptr_, *deviceID_);
      ptr_ = NULL;

      delete deviceID_;
      deviceID_ = NULL;
    }
  }

  ptr_ = x;
  refCount_ = new int(1);
  deviceID_ = new int();
  cudaGetDevice(deviceID_);

#ifdef DEBUG_EXT_CUDA_SHARED_PTR
  std::cout << "\nCUDASharedPointer returning successfully\n\n";
#endif
  return *this;
}

/*---------------------------------------------------------------------*/

bool CUDASharedPointer::operator==(const CUDASharedPointer& x) const {
  if ((this->ptr_ == x.ptr_) && (this->deviceID_ == x.deviceID_)) {
    return true;
  }
  return false;
}

/*---------------------------------------------------------------------*/

CUDASharedPointer& CUDASharedPointer::operator=(const CUDASharedPointer& x) {
#ifdef DEBUG_EXT_CUDA_SHARED_PTR
  std::cout << "CUDASharedPointer assigned to CUDASharedPointer reference";
#endif

  /** Check to make sure we're not self assigning. **/
  if ((*this) == x) {
    return *this;
  }

  if (refCount_ != NULL) {
#ifdef DEBUG_EXT_CUDA_SHARED_PTR
    std::cout << "\n\t - Non-zero ref count, decrementing";
#endif
    --(*refCount_);
    if ((*refCount_) == 0) {
#ifdef DEBUG_EXT_CUDA_SHARED_PTR
      std::cout << "\n\t - Pointer ref count is zero, freeing host and device memory";
#endif
      delete refCount_;
      refCount_ = NULL;

      CudaFree(ptr_, *deviceID_);
      ptr_ = NULL;

      delete deviceID_;
      deviceID_ = NULL;
    }
  }

  ptr_ = x.ptr_;
  deviceID_ = x.deviceID_;
  refCount_ = x.refCount_;
  ++(*refCount_);
#ifdef DEBUG_EXT_CUDA_SHARED_PTR
  std::cout << "\nCUDASharedPointer returning successfully\n\n";
#endif
  return *this;
}

/*---------------------------------------------------------------------*/

void* CUDASharedPointer::operator->() {
  return ptr_;
}

/*---------------------------------------------------------------------*/

const void* CUDASharedPointer::operator->() const {
  return ptr_;
}

/*---------------------------------------------------------------------*/

bool CUDASharedPointer::isnull() const {
  return (ptr_ == NULL);
}

/*---------------------------------------------------------------------*/

int CUDASharedPointer::get_refcount() const {
  return *refCount_;
}

/*---------------------------------------------------------------------*/

int CUDASharedPointer::get_deviceID() const {
  return *deviceID_;
}
/*---------------------------------------------------------------------*/

CUDASharedPointer& CUDASharedPointer::detatch() {
  (*this) = ((void*) NULL);

  return *this;
}

} // End namespace - ema::cuda
} // End namespace - ema