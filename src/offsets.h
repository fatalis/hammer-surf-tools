#ifndef OFFSETS_H
#define OFFSETS_H

// struct sizes
#define CSTRDLG_SIZE 320
#define CMAPFACE_SIZE                      0x360
#define CMAPENTITY_SIZE                    0x280
#define CMAPSOLID_SIZE                     0x230

// struct offsets that are (hopefully) stable
#define CLIPPER3D_OFFSET_PLANE_NORMAL      0x0FC
#define CMAINFRAME_OFFSET_OBJECTPROPERTIES 0x1F8
#define CMAPCLASS_OFFSET_FACES             0x1A0
#define CMAPCLASS_OFFSET_ORIGIN            0x050
#define CMAPCLASS_OFFSET_RENDER2DBOX       0x0A8
#define CMAPDOC_OFFSET_MPWORLD             0xB30
#define CMAPDOC_OFFSET_SELECTION           0xB88
#define COBJECTPROPERTIES_OFFSET_HWND      0x040
#define CSELECTION_OFFSET_SELECTIONLIST    0x018
#define SELECTION3D_OFFSET_SELECTION       0x1A8

// pattern scanned offsets
extern size_t CMainFrame_m_pFaceEditSheet_Offset;
extern size_t CFaceEditSheet_m_Faces_Offset;

#endif // OFFSETS_H
