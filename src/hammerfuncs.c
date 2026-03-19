#include "hammerfuncs.h"

#ifdef USING_AFXMESSAGEBOX
AfxMessageBox_t AfxMessageBox;
#endif

#ifdef USING_CDIALOG_DOMODAL
CDialog_DoModal_t DoModal;
#endif

#ifdef USING_CHISTORY_KEEPNEW
CHistory_KeepNew_t CHistory_KeepNew;
#endif

#ifdef USING_CHISTORY_KEEP
CHistory_Keep_t CHistory_Keep;
#endif

#ifdef USING_CHISTORY_UNDO
CHistory_Undo_t CHistory_Undo;
#endif

#ifdef USING_CMAPFACE_GETORIENTATION
static const char *FaceOrientation_str[] = {
    "FLOOR",
    "CEILING",
    "NORTH_WALL",
    "SOUTH_WALL",
    "EAST_WALL",
    "WEST_WALL",
    "INVALID"
};
const char *GetFaceOrientationStr(FaceOrientation orientation) {
    return FaceOrientation_str[orientation];
}
#endif

#ifdef USING_CRENDER_DRAWTEXT
CRender_DrawText_t CRender_DrawText;
#endif

#ifdef USING_CSOLIDFACES_MAKEFACE
CSolidFaces_MakeFace_t CSolidFaces_MakeFace;
#endif

#ifdef USING_CSTRDLG_CSTRDLG
CStrDlg_CStrDlg_t CStrDlg;
#endif

#ifdef USING_GETHISTORY
GetHistory_t GetHistory;
#endif

#ifdef USING_GETMAINWND
GetMainWnd_t GetMainWnd;
#endif

#ifdef USING_MSG
Msg_t Msg;
#endif

#ifdef USING_VALVEALLOC
ValveAlloc_t ValveAlloc;
#endif

#ifdef USING_WCKEYVALUES_GETVALUE
WCKeyValues_GetValue_t WCKeyValues_GetValue;
#endif

#ifdef USING_CSELECTION_SELECTOBJECTLIST
CSelection_SelectObjectList_t CSelection_SelectObjectList;
#endif

#ifdef USING_CVISGROUP_ISAUTOVISGROUP
CVisGroup_IsAutoVisGroup_t CVisGroup_IsAutoVisGroup;
#endif

#ifdef USING_S_CLASSES
MCMSTRUCTVector *S_Classes;
#endif

CMapClassMethods_t  CMapClassMethods;
CMapDocMethods_t    CMapDocMethods;
CMapSolidMethods_t  CMapSolidMethods;
CMapFaceMethods_t   CMapFaceMethods;
CMapEntityMethods_t CMapEntityMethods;
