#ifndef x86_INTRINSICS
#include <stdint.h>



#if !defined(__x86_64__) && !defined(_M_X64)
#error "Only use this header on x86_64!
#endif


uint32_t x86_cpuid_max_leaves(void);
void x86_cpuid_read_tsc_freq(uint32_t* eax, uint32_t* ebx, uint32_t* ecx);
int x86_can_use_rdtsc(uint64_t* tsc_freq);
uint64_t x86_rdtsc(void);



#define x86_INTRINSICS
#endif //x86_INTRINSICS


#if defined(x86_INTRINSICS_IMPLEMENTATION)


int x86_can_use_rdtsc(uint64_t* tsc_freq)
{
  uint32_t maxCpuIdLeaves = 123, eax = 69, ebx = 69, ecx = 69;
  maxCpuIdLeaves = x86_cpuid_max_leaves();
  if (maxCpuIdLeaves < 0x15) return 0;

  x86_cpuid_read_tsc_freq(&eax, &ebx, &ecx);
  if (ebx == 0 || ecx == 0) return 0;
  *tsc_freq = ((uint64_t)ebx/(uint64_t)eax) * (uint64_t)ecx;
  return 1;
}

uint32_t x86_cpuid_max_leaves(void)
{
  uint32_t maxCpuIdLeaves = 123;
  __asm__ __volatile__(
    "movl $0, %%eax\n"
    "cpuid\n"
    "movl %%eax, %[output]\n"
    : [output] "=r" (maxCpuIdLeaves)
    :
    : "%eax", "memory"
  );
  return maxCpuIdLeaves;
}

void x86_cpuid_read_tsc_freq(uint32_t* eax, uint32_t* ebx, uint32_t* ecx)
{
  uint32_t resA, resB, resC;
  __asm__ __volatile__(
    "movl $0x15, %0\n"
    "cpuid\n"
    : "=a" (resA), "=b" (resB), "=c" (resC)
    :
    : "memory"
  );
  *eax = resA;
  *ebx = resB;
  *ecx = resC;
}

uint64_t x86_rdtsc(void)
{
  uint64_t result;
  __asm__ __volatile__ (
    "lfence\n"
    "rdtsc\n"
    "lfence\n"
    "shl $32, %%rdx\n"
    "or %%rdx, %0"
    : "=a" (result)
    :
    : "%rdx", "memory"
  );
  return result;
}

#endif
