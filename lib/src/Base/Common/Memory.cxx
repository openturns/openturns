//                                               -*- C++ -*-
/**
 *  @brief This file declares memory debugging functions
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cstdio>
#include <algorithm>
#include <cassert>
#include "openturns/Memory.hxx"

#ifdef DEBUG_MEMORY


pthread_mutex_t LockNewDelete::NewMutex_ = PTHREAD_MUTEX_INITIALIZER;

LockNewDelete::LockNewDelete()
{
  //std::fprintf( stderr, "Lock\n" );
  Lock();
}

LockNewDelete::~LockNewDelete()
{
  Unlock();
  //std::fprintf( stderr, "Unlock\n" );
}

void LockNewDelete::Lock()
{
  pthread_mutex_lock( & NewMutex_ );
}

void LockNewDelete::Unlock()
{
  pthread_mutex_unlock( & NewMutex_ );
}


MemoryImplementation::MemoryImplementation()
  : memList_(),
    memSpectrum_(),
    memoryUsage_(0),
    fullMemoryUsage_(0),
    maxMemoryUsage_(0),
    maxFullMemoryUsage_(0),
    flag_(false)
{
  std::fprintf( stderr, "Memory debugging enabled. Library will run slowlier.\n" );
}

MemoryImplementation::~MemoryImplementation()
{
  std::fprintf( stderr, "Memory used : std=%16zu full=%16zu maxstd=%16zu maxfull=%16zu\n", memoryUsage_, fullMemoryUsage_, maxMemoryUsage_, maxFullMemoryUsage_ );
}


MemoryImplementation * Memory::p_impl_ = 0;

MemoryImplementation & Memory::GetInstance()
{
  if ( ! p_impl_ )
  {
    void * buf = calloc( 1, sizeof(MemoryImplementation) );
    p_impl_ = new(buf) MemoryImplementation;
    atexit( Memory::Release );
  }
  assert( p_impl_ );
  return *p_impl_;
}

void Memory::Release()
{
  assert( p_impl_ );
  p_impl_->~MemoryImplementation();
  free( p_impl_ );
  p_impl_ = 0;
}

static void printSpectrumHeader(const char * pfx)
{
  std::fprintf( stderr, "%s%16s : %16s\n", pfx, "size_", "number" );
}


class printSpectrumChunk
{
  const char * pfx_;
public:
  printSpectrumChunk(const char * pfx) : pfx_(pfx) {}
  void operator() (const MemSpectrum::value_type & ref) const
  {
    std::fprintf( stderr, "%s%16zu : %16zu\n", pfx_, ref.first, ref.second );
  }
};

void _printMemorySpectrum(const char * pfx, const MemSpectrum & spectrum)
{
  printSpectrumHeader( pfx );
  std::for_each( spectrum.begin(), spectrum.end(), printSpectrumChunk(pfx) );
}




static void printMemListHeader(const char * pfx)
{
  std::fprintf( stderr, "%s%16s %16s (%16s)\n", pfx, "addr", "size_", "ptr_");
}

static void printChunk(const char * pfx, const _OpenTURNS_memory * p_chunk)
{
  if (p_chunk)
  {
    std::fprintf( stderr, "%s%16p %16zu (%16p)\n", pfx, p_chunk, p_chunk->size_, p_chunk->ptr_);
  }
  else
  {
    std::fprintf( stderr, "%s%16s %16s (%16s)\n", pfx, "*", "*", "*");
  }
}

class printMemListChunk
{
  const char * pfx_;
public:
  printMemListChunk(const char * pfx) : pfx_(pfx) {}
  void operator() (const MemContainer::value_type & ref) const
  {
    const _OpenTURNS_memory * p_chunk = ref.second;
    printChunk( pfx_, p_chunk );
  }
};

class makeSpectrum
{
  MemSpectrum & spectrum_;
public:
  makeSpectrum( MemSpectrum & spectrum) : spectrum_(spectrum) {}
  void operator() (const MemContainer::value_type & ref) const
  {
    const _OpenTURNS_memory * p_chunk = ref.second;
    ++(spectrum_[ p_chunk->size_ ]);
  }
};



void * operator new (size_t sz)
{
  //std::fprintf( stderr, "Entering new (sz = %zu)\n", sz );

  void * buf = std::calloc( 1, sz );  // Allocates storage and set it to zero
  if (buf == 0) return buf;

  // Keep trace of allocated storage
  LockNewDelete lock;
  MemoryImplementation & inst = Memory::GetInstance();
  _OpenTURNS_memory * p_chunk = static_cast<_OpenTURNS_memory *>( std::calloc( 1, sizeof(_OpenTURNS_memory) ) );
  if (p_chunk)
  {
    p_chunk->ptr_  = buf;
    p_chunk->size_ = sz;
    inst.memList_[ buf ] = p_chunk;
    ++(inst.memSpectrum_[ sz ]);

    inst.memoryUsage_       += sz;
    inst.fullMemoryUsage_   += sz + sizeof(_OpenTURNS_memory);
    inst.maxMemoryUsage_     = std::max( inst.maxMemoryUsage_, inst.memoryUsage_ );
    inst.maxFullMemoryUsage_ = std::max( inst.maxFullMemoryUsage_, inst.fullMemoryUsage_ );
  }
  //printChunk( "new      : ", p_chunk );

  //std::fprintf( stderr, "Exiting  new\n" );
  if (inst.flag_) std::fprintf( stderr, "operator new ( %d ) -> %p\n", sz, buf );
  return buf;
}

void operator delete (void * ptr)
{
  //std::fprintf( stderr, "Entering delete (ptr = %p)\n", ptr );

  // Check storage and free trace
  LockNewDelete lock;
  MemoryImplementation & inst = Memory::GetInstance();
  MemContainer::iterator it = inst.memList_.find( ptr );
  if (it != inst.memList_.end())
  {
    _OpenTURNS_memory * p_chunk = it->second;
    inst.memoryUsage_     -= p_chunk->size_;
    inst.fullMemoryUsage_ -= p_chunk->size_ + sizeof(_OpenTURNS_memory);
    if (inst.flag_) std::fprintf( stderr, "operator delete ( %d ) <- %p\n", p_chunk->size_, ptr );
    inst.memList_.erase( it );

    //printChunk( "delete   : ", p_chunk );
    std::free( p_chunk );
  }


  std::free( ptr ); // Deallocates storage
  //std::fprintf( stderr, "Exiting  delete\n" );
}

void * operator new [] (size_t sz)
{
  //std::fprintf( stderr, "Entering new [] (sz = %zu)\n", sz );
  void * ptr = operator new ( sz );
  //std::fprintf( stderr, "Exiting  new []\n" );
  return ptr;
}

void operator delete [] (void * ptr)
{
  //std::fprintf( stderr, "Entering delete [] (ptr = %p)\n", ptr );
  operator delete ( ptr );
  //std::fprintf( stderr, "Exiting  delete []\n" );
}


#endif /* DEBUG_MEMORY */





size_t GetMemoryUsage()
{
#ifdef DEBUG_MEMORY
  LockNewDelete lock;
  return Memory::GetInstance().memoryUsage_;
#else
  return 0;
#endif /* DEBUG_MEMORY */
}


size_t GetFullMemoryUsage()
{
#ifdef DEBUG_MEMORY
  LockNewDelete lock;
  return Memory::GetInstance().fullMemoryUsage_;
#else
  return 0;
#endif /* DEBUG_MEMORY */
}


size_t GetMaxMemoryUsage()
{
#ifdef DEBUG_MEMORY
  LockNewDelete lock;
  return Memory::GetInstance().maxMemoryUsage_;
#else
  return 0;
#endif /* DEBUG_MEMORY */
}


size_t GetMaxFullMemoryUsage()
{
#ifdef DEBUG_MEMORY
  LockNewDelete lock;
  return Memory::GetInstance().maxFullMemoryUsage_;
#else
  return 0;
#endif /* DEBUG_MEMORY */
}


void printMemoryUsage()
{
#ifdef DEBUG_MEMORY
  LockNewDelete lock;
  MemoryImplementation & inst = Memory::GetInstance();
  const char pfx1[] = "usage    : ";
  printMemListHeader( pfx1 );
  std::for_each( inst.memList_.begin(), inst.memList_.end(), printMemListChunk( pfx1 ) );

  const char pfx2[] = "cur. sp. : ";
  MemSpectrum spectrum;
  std::for_each( inst.memList_.begin(), inst.memList_.end(), makeSpectrum( spectrum ) );
  _printMemorySpectrum( pfx2, spectrum );

  std::fprintf( stderr, "summary  : std=%16zu full=%16zu maxstd=%16zu maxfull=%16zu\n",
                inst.memoryUsage_,
                inst.fullMemoryUsage_,
                inst.maxMemoryUsage_,
                inst.maxFullMemoryUsage_ );
#else
  fprintf( stderr, "No memory debugging available. Compile with -DDEBUG_MEMORY flag or --enable-debug=memory option.\n" );
#endif /* DEBUG_MEMORY */
}

void printMemorySpectrum()
{
#ifdef DEBUG_MEMORY
  LockNewDelete lock;
  const char pfx[] = "cum. sp. : ";
  _printMemorySpectrum( pfx, Memory::GetInstance().memSpectrum_ );
#else
  fprintf( stderr, "No memory debugging available. Compile with -DDEBUG_MEMORY flag or --enable-debug=memory option.\n" );
#endif /* DEBUG_MEMORY */
}


void SetMemoryFlag(bool flag)
{
#ifdef DEBUG_MEMORY
  LockNewDelete lock;
  Memory::GetInstance().flag_ = flag;
#else
  (void) flag;
  fprintf( stderr, "No memory debugging available. Compile with -DDEBUG_MEMORY flag or --enable-debug=memory option.\n" );
#endif /* DEBUG_MEMORY */
}


