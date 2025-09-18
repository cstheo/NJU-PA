#include <proc.h>
#include <elf.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Addr Elf64_Addr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Addr Elf32_Addr
#endif

#if defined(__ISA_AM_NATIVE__) || defined(__ISA_X86__)
#define EXPECT_TYPE EM_X86_64
#elif defined(ISA_X86__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_MIPS32__)
#define ELF_MACHINE_TYPE EM_MIPS
#elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__) || defined(__ISA_RISCV32E__)
#define EXPECT_TYPE EM_RISCV
#else 
#error Unsupported ISA
#endif

#define HAS_MAG_(e_, i_) (e_[EI_MAG##i_] == ELFMAG##i_)

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elf_header;
  ramdisk_read(&elf_header, 0, sizeof(Elf_Ehdr));
  assert(HAS_MAG_(elf_header.e_ident, 0) && HAS_MAG_(elf_header.e_ident, 1) &&
         HAS_MAG_(elf_header.e_ident, 2) && HAS_MAG_(elf_header.e_ident, 3));
  assert(elf_header.e_machine == EXPECT_TYPE);
  
  size_t phdr_offset = elf_header.e_phoff;
  Elf_Phdr phdr;
  for (int i = 0; i < elf_header.e_phnum; i++) {
    ramdisk_read(&phdr, phdr_offset + i * sizeof(phdr), sizeof(phdr));
    if (phdr.p_type != PT_LOAD) continue;

    void *segment = malloc(phdr.p_memsz);
    ramdisk_read(segment, phdr.p_offset, phdr.p_filesz);

    memcpy((Elf_Addr *)phdr.p_vaddr, segment, phdr.p_filesz);
    memset((Elf_Addr *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);    
  }

  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

