

/*
 * The 32-bit mach header appears at the very beginning of the object file for
 * 32-bit architectures.
 */
struct mach_header {
	uint32_t	magic;		/* mach magic number identifier */
	uint32_t	cputype;	/* cpu specifier */
	uint32_t	cpusubtype;	/* machine specifier */
	uint32_t	filetype;	/* type of file */
	uint32_t	ncmds;		/* number of load commands */
	uint32_t	sizeofcmds;	/* the size of all the load commands */
	uint32_t	flags;		/* flags */
};
/* Constant for the magic field of the mach_header (32-bit architectures) */
#define	MH_MAGIC	0xfeedface	/* the mach magic number */
#define MH_CIGAM	0xcefaedfe	/* NXSwapInt(MH_MAGIC) */
/*
 *	Machine types known by all.
 */
#define	CPU_ARCH_MASK	0xff000000		/* mask for architecture bits */
#define CPU_ARCH_ABI64	0x01000000		/* 64 bit ABI */

#define CPU_TYPE_ANY		((int32_t) -1)

#define CPU_TYPE_VAX		((uint32_t) 1)
/* skip				((cpu_type_t) 2)	*/
/* skip				((cpu_type_t) 3)	*/
/* skip				((cpu_type_t) 4)	*/
/* skip				((cpu_type_t) 5)	*/
#define	CPU_TYPE_MC680x0	((uint32_t) 6)
#define CPU_TYPE_X86		((uint32_t) 7)
#define CPU_TYPE_I386		CPU_TYPE_X86		/* compatibility */
#define	CPU_TYPE_X86_64		(CPU_TYPE_X86 | CPU_ARCH_ABI64)

/* skip CPU_TYPE_MIPS		((cpu_type_t) 8)	*/
/* skip 			((cpu_type_t) 9)	*/
#define CPU_TYPE_MC98000	((uint32_t) 10)
#define CPU_TYPE_HPPA           ((uint32_t) 11)
#define CPU_TYPE_ARM		((uint32_t) 12)
#define CPU_TYPE_MC88000	((uint32_t) 13)
#define CPU_TYPE_SPARC		((uint32_t) 14)
#define CPU_TYPE_I860		((uint32_t) 15)
/* skip	CPU_TYPE_ALPHA		((cpu_type_t) 16)	*/
/* skip				((cpu_type_t) 17)	*/
#define CPU_TYPE_POWERPC		((uint32_t) 18)
#define CPU_TYPE_POWERPC64		(CPU_TYPE_POWERPC | CPU_ARCH_ABI64)
