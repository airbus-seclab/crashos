// This file is part of CrashOS and is released under GPLv2 (see crashos/LICENSE.md)
// Copyright Airbus Group
#include <core/xen.h>
#include <core/cpu.h>
#include <core/string.h>
#include <core/page.h>
#include <core/print.h>

static uint32_t xen_cpuid_base()
{
   char     *xen = "XenVMMXenVMM";
   uint32_t base, eax, sig[3];

   for (base = 0x40000000; base < 0x40010000; base += 0x100)
   {
      cpuid(base, &eax, &sig[0], &sig[1], &sig[2]);

      if (strcmp(xen, sig, 12) && ((eax - base) >= 2))
         return base;
   }

   return 0;
}

int xen_setup()
{
   uint16_t major, minor;
   uint32_t eax, ebx, ecx, edx, pages, msr, base;

   base = xen_cpuid_base();
   printf_log("Xen CPUID base = 0x%x\n", base);
   cpuid(base + 1, &eax, &ebx, &ecx, &edx);

   major = eax >> 16;
   minor = eax & 0xffff;
   printf_log("Xen version (0x%x) %d.%d.\n", eax, major, minor);

   cpuid(base + 2, &pages, &msr, &ecx, &edx);
   wrmsr64(msr, 0, hypercall_page);

   printf_log("Xen hypercall page ready !\n");
   return 0;
}
