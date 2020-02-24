/* VESA structures from the VESA BIOS Specification, retrieved 15 Jan 2016
 * from http://flint.cs.yale.edu/cs422/readings/hardware/vbe3.pdf */

#ifndef PDCVESA_H
#define PDCVESA_H

#ifdef __GNUC__
#pragma pack(push, 1)
#endif
#ifdef __WATCOMC__
#pragma pack(__push, 1)
#endif
struct VbeInfoBlock {
    unsigned char  VbeSignature[4];   /* VBE Signature */
    unsigned short VbeVersion;        /* VBE Version */
    unsigned long  OemStringPtr;      /* VbeFarPtr to OEM String */
    unsigned long  Capabilities;      /* Capabilities of graphics controller */
    unsigned long  VideoModePtr;      /* VbeFarPtr to VideoModeList */
    unsigned short TotalMemory;       /* Number of 64kb memory blocks */
    /* Added for VBE 2.0+ */
    unsigned short OemSoftwareRev;    /* VBE implementation Software revision */
    unsigned long  OemVendorNamePtr;  /* VbeFarPtr to Vendor Name String */
    unsigned long  OemProductNamePtr; /* VbeFarPtr to Product Name String */
    unsigned long  OemProductRevPtr;  /* VbeFarPtr to Product Revision String */
    unsigned char  Reserved[222];     /* Reserved for VBE implementation scratch area */
    unsigned char  OemData[256];      /* Data Area for OEM Strings */
};

struct ModeInfoBlock {
    /* Mandatory information for all VBE revisions */
    unsigned short ModeAttributes;        /* mode attributes */
    unsigned char  WinAAttributes;        /* window A attributes */
    unsigned char  WinBAttributes;        /* window B attributes */
    unsigned short WinGranularity;        /* window granularity */
    unsigned short WinSize;               /* window size */
    unsigned short WinASegment;           /* window A start segment */
    unsigned short WinBSegment;           /* window B start segment */
    unsigned long  WinFuncPtr;            /* real mode pointer to window function */
    unsigned short BytesPerScanLine;      /* bytes per scan line */
    /* Mandatory information for VBE 1.2 and above */
    unsigned short XResolution;           /* horizontal resolution in pixels or characters */
    unsigned short YResolution;           /* vertical resolution in pixels or characters */
    unsigned char  XCharSize;             /* character cell width in pixels */
    unsigned char  YCharSize;             /* character cell height in pixels */
    unsigned char  NumberOfPlanes;        /* number of memory planes */
    unsigned char  BitsPerPixel;          /* bits per pixel */
    unsigned char  NumberOfBanks;         /* number of banks */
    unsigned char  MemoryModel;           /* memory model type */
    unsigned char  BankSize;              /* bank size in KB */
    unsigned char  NumberOfImagePages;    /* number of images */
    unsigned char  Reserved1;             /* reserved for page function */
    /* Direct Color fields (required for direct/6 and YUV/7 memory models) */
    unsigned char  RedMaskSize;           /* size of direct color red mask in bits */
    unsigned char  RedFieldPosition;      /* bit position of lsb of red mask */
    unsigned char  GreenMaskSize;         /* size of direct color green mask in bits */
    unsigned char  GreenFieldPosition;    /* bit position of lsb of green mask */
    unsigned char  BlueMaskSize;          /* size of direct color blue mask in bits */
    unsigned char  BlueFieldPosition;     /* bit position of lsb of blue mask */
    unsigned char  RsvdMaskSize;          /* size of direct color reserved mask in bits */
    unsigned char  RsvdFieldPosition;     /* bit position of lsb of reserved mask */
    unsigned char  DirectColorModeInfo;   /* direct color mode attributes */
    /* Mandatory information for VBE 2.0 and above */
    unsigned long  PhysBasePtr;           /* physical address for flat memory frame buffer */
    unsigned long  Reserved2;             /* Reserved - always set to 0 */
    unsigned short Reserved3;             /* Reserved - always set to 0 */
    /* Mandatory information for VBE 3.0 and above */
    unsigned short LinBytesPerScanLine;   /* bytes per scan line for linear modes */
    unsigned char  BnkNumberOfImagePages; /* number of images for banked modes */
    unsigned char  LinNumberOfImagePages; /* number of images for linear modes */
    unsigned char  LinRedMaskSize;        /* size of direct color red mask (linear modes) */
    unsigned char  LinRedFieldPosition;   /* bit position of lsb of red mask (linear modes) */
    unsigned char  LinGreenMaskSize;      /* size of direct color green mask  (linear modes) */
    unsigned char  LinGreenFieldPosition; /* bit position of lsb of green mask (linear modes) */
    unsigned char  LinBlueMaskSize;       /* size of direct color blue mask  (linear modes) */
    unsigned char  LinBlueFieldPosition;  /* bit position of lsb of blue mask (linear modes) */
    unsigned char  LinRsvdMaskSize;       /* size of direct color reserved mask (linear modes) */
    unsigned char  LinRsvdFieldPosition;  /* bit position of lsb of reserved mask (linear modes) */
    unsigned long  MaxPixelClock;         /* maximum pixel clock (in Hz) for graphics mode */
    unsigned char  Reserved4[190];        /* remainder of ModeInfoBlock */
};
#ifdef __GNUC__
#pragma pack(pop)
#endif
#ifdef __WATCOMC__
#pragma pack(__pop)
#endif

#endif
