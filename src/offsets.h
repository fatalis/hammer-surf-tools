#ifndef OFFSETS_H
#define OFFSETS_H

// struct sizes
#define CSTRDLG_SIZE                       0x490
#define CMAPATOM_SIZE                      0x050
#define CMAPPOINT_SIZE                     0x010
#define CMAPCLASS_SIZE                     0x190
#define CMAPFACE_SIZE                      0x360
#define CMAPENTITY_SIZE                    0x280
#define CMAPSOLID_SIZE                     0x230
#define VEC3POINTS_SIZE                    0x03C

// struct offsets that are (hopefully) stable
#define CEDITGAMECLASS_OFFSET_CLASS        0x030
#define CLIPPER3D_OFFSET_PLANE_NORMAL      0x0FC
#define CHISTORYTRACK_OFFSET_NAME          0x02C
#define CMAINFRAME_OFFSET_OBJECTPROPERTIES 0x1F8
#define CMAPSOLID_OFFSET_FACES             0x1A0
#define CMAPCLASS_OFFSET_ORIGIN            0x050
#define CMAPCLASS_OFFSET_RENDER2DBOX       0x0A8
#define CMAPDOC_OFFSET_MPWORLD             0xB30
#define CMAPDOC_OFFSET_SELECTION           0xB88
#define CWND_OFFSET_HWND                   0x040
#define CSELECTION_OFFSET_SELECTIONLIST    0x018
#define CSTRDLG_OFFSET_STRING              0x140
#define SELECTION3D_OFFSET_SELECTION       0x1A8
#define CVISGROUP_OFFSET_NAME              0x029
#define CMAPFACE_OFFSET_PLANE              0x190
#define CMAPFACE_OFFSET_POINTS             0x1C8
#define CMAPENTITY_OFFSET_EDITGAMECLASS    0x190
#define CMAPENTITY_OFFSET_FLAGS            0x254 // used to set placeholder (flag 1 - non-brush entity flag)

// pattern scanned offsets
#ifdef USING_OFFSET_CMAINFRAME_FACEEDITSHEET
extern size_t CMainFrame_m_pFaceEditSheet_Offset;
#endif
#ifdef USING_OFFSET_CFACEEDITSHEET_FACES
extern size_t CFaceEditSheet_m_Faces_Offset;
#endif

void dump_offsets();

#endif // OFFSETS_H
