#include "patterns.h"
#include "hooks.h"
#include "scriptfuncs.h"
#include "hammerfuncs.h"

static void *resolve_rel(void *call_addr, const uint8_t *insn) {
    if (insn[0] == 0xE8) { // call
        int32_t rel = *(int32_t *)(insn + 1);
        return call_addr + 5 + rel;
    } else if (insn[0] == 0x48) { // lea
        int32_t rel = *(int32_t *)(insn + 3);
        return call_addr + 7 + rel;
    }
    ASSERT(false);
    return nullptr;
}

static Pattern_t g_patterns[] = {
#ifdef USING_GETMAINWND
    {
        // find "You are about to export over your"
        "GetMainWnd",
        (const uint8_t[]){
            0x48, 0x8B, 0x0A,
            0x48, 0x8B, 0x01,
            0xFF, 0x50, 0x08,
            0x85, 0xFF,
            0x75, 0x2A,
            0xE8, 0x8F, 0x55, 0xFE, 0xFF,
            0x41, 0xB9, 0x34, 0x00, 0x00, 0x00
        },
        "xxxxxxxxxxxxxx????xxxxxx",
        (void **)&GetMainWnd,
        nullptr,
        13,
        PATTERN_REL
    },
#endif
#ifdef USING_CSTRDLG_CSTRDLG
    {
        "CStrDlg::CStrDlg",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x18,
            0x89, 0x54, 0x24, 0x10,
            0x48, 0x89, 0x4C, 0x24, 0x08,
            0x55,
            0x56,
            0x57,
            0x41, 0x54,
            0x41, 0x55,
            0x41, 0x56,
            0x41, 0x57,
            0x48, 0x83, 0xEC, 0x40
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CStrDlg
    },
#endif
#ifdef USING_CDIALOG_DOMODAL
    {
        "CDialog::DoModal",
        (const uint8_t[]){
            0x48, 0x89, 0x4C, 0x24, 0x08,
            0x53,
            0x56,
            0x57,
            0x41, 0x54,
            0x41, 0x55,
            0x41, 0x56,
            0x41, 0x57,
            0x48, 0x83, 0xEC, 0x30,
            0x48, 0x8B, 0xD9
        },
        "xxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&DoModal
    },
#endif
#ifdef USING_CMAPCLASS_ENUMCHILDREN
    {
        "CMapClass::EnumChildren",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x48, 0x89, 0x6C, 0x24, 0x10,
            0x48, 0x89, 0x74, 0x24, 0x18,
            0x57,
            0x41, 0x56,
            0x41, 0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x33, 0xDB,
            0x49, 0x8B, 0xE9,
            0x4D, 0x8B, 0xF0,
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMapClassMethods.EnumChildren
    },
#endif
#ifdef USING_CMAPDOC_UPDATEALLVIEWS
    {
        "CMapDoc::UpdateAllViews",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x18,
            0x57,
            0x48, 0x83, 0xEC, 0x30,
            0x48, 0x8B, 0xD9,
            0x8B, 0xFA
        },
        "xxxxxxxxxxxxxxx",
        (void **)&CMapDocMethods.UpdateAllViews
    },
#endif
#ifdef USING_CRENDER_DRAWTEXT
    {
        "CRender::DrawText",
        (const uint8_t[]){
            0x48, 0x89, 0x5c, 0x24, 0x10,
            0x48, 0x89, 0x6c, 0x24, 0x18,
            0x56,
            0x57,
            0x41, 0x56,
            0x48, 0x81, 0xec, 0x30, 0x01, 0x00, 0x00,
            0x45, 0x8b, 0xf0
        },
        "xxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CRender_DrawText
    },
#endif
#ifdef USING_CMAPDOC_SETMODIFIEDFLAG
    {
        "CMapDoc::SetModifiedFlag",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x8B, 0xFA,
            0x48, 0x8B, 0xD9,
            0x85, 0xD2,
            0x74, 0x6B
        },
        "xxxxxxxxxxxxxxxxxxx",
        (void **)&CMapDocMethods.SetModifiedFlag
    },
#endif
#ifdef USING_CMAPDOC_DELETEOBJECT
    // 48 89 5c 24 10  48 89 74 24 18  57  48 83 ec 30  48 8b da  48 8b f1  e8 ?? ?? ?? ??  48 8b d3  48 8b c8
    {
        "CMapDoc::DeleteObject",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x10,
            0x48, 0x89, 0x74, 0x24, 0x18,
            0x57,
            0x48, 0x83, 0xEC, 0x30,
            0x48, 0x8B, 0xDA,
            0x48, 0x8B, 0xF1,
            0xE8, 0x46, 0xEE, 0xFC, 0xFF,
            0x48, 0x8B, 0xD3,
            0x48, 0x8B, 0xC8
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMapDocMethods.DeleteObject
    },
#endif
#ifdef USING_CMAPFACE_SETTEXTURE
    {
        // 48 89 5c 24 18     56     57      41 56    48 81 ec 40 01 00 00        48 8b c2
        "CMapFace::SetTexture",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x41, 0x0F, 0xB6, 0xD8,
            0x48, 0x8B, 0xF9
        },
        "xxxxxxxxxxxxxxxxx",
        (void **)&CMapFaceMethods.SetTexture
    },
#endif
#ifdef USING_SCRIPTDATAPTR
    {
        // 48 89 5c 24 18     56     57      41 56    48 81 ec 40 01 00 00        48 8b c2
        "ScriptDataPtr",
        (const uint8_t[]){
            0x48, 0x89, 0x48, 0x48,
            0x48, 0x8D, 0x0D, 0x47, 0x1F, 0xAB, 0x00,
            0xC7, 0x40, 0x18, 0x1F, 0x00, 0x00, 0x00
        },
        "xxxxxxx????xxxxxxx",
        (void **)&ScriptDataPtr,
        nullptr,
        4,
        PATTERN_REL,
    },
#endif
#ifdef USING_GETHISTORY
    {
        // 48 89 5c 24 30  e8 ?? ?? ?? ??  48 8b 8f 88 0b 00 00
        "GetHistory",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x30,
            0xE8, 0x59, 0x7A, 0xFC, 0xFF,
            0x48, 0x8B, 0x8F, 0x88, 0x0B, 0x00, 0x00
        },
        "xxxxxx????xxxxxxx",
        (void **)&GetHistory,
        nullptr,
        5,
        PATTERN_REL,
    },
#endif
#ifdef USING_CMAPENTITY_CMAPENTITY
    {
        // 48 89 5c 24 10    48 89 4c 24 08    57    48 83 ec 20    48 8b f9    e8 ?? ?? ?? ??    90    48 8d 8f 90 01 00 00
        "CMapEntity::CMapEntity",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x10,
            0x48, 0x89, 0x4C, 0x24, 0x08,
            0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x48, 0x8B, 0xF9,
            0xE8, 0x19, 0xB5, 0xFE, 0xFF,
            0x90,
            0x48, 0x8D, 0x8F, 0x90, 0x01, 0x00, 0x00
        },
        "xxxxxxxxxxxxxxxxxxx????xxxxxxxx",
        (void **)&CMapEntityMethods.CMapEntity,
    },
#endif
#ifdef USING_AFXMESSAGEBOX
    {
        // 48 89 5c 24 08  48 89 74 24 10  57  48 83 ec 30  41 8b d8  8b fa  48 8b f1 e8 ?? ?? ?? ?? 44 8b cb
        "AfxMessageBox",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x48, 0x89, 0x74, 0x24, 0x10,
            0x57,
            0x48, 0x83, 0xEC, 0x30,
            0x41, 0x8B, 0xD8,
            0x8B, 0xFA,
            0x48, 0x8B, 0xF1,
            0xE8, 0xEC, 0xFD, 0xFF, 0xFF,
            0x44, 0x8B, 0xCB
        },
        "xxxxxxxxxxxxxxxxxxxxxxxx????xxx",
        (void **)&AfxMessageBox,
    },
#endif
#ifdef USING_MSG
    {
        // 48 8b c4  48 89 50 10  4c 89 40 18  4c 89 48 20  53  55  48 81 ec 48 02 00 00
        "Msg",
        (const uint8_t[]){
            0x48, 0x8B, 0xC4,
            0x48, 0x89, 0x50, 0x10,
            0x4C, 0x89, 0x40, 0x18,
            0x4C, 0x89, 0x48, 0x20,
            0x53,
            0x55,
            0x48, 0x81, 0xEC, 0x48, 0x02, 0x00, 0x00
        },
        "xxxxxxxxxxxxxxxxxxxx",
        (void **)&Msg,
    },
#endif
#ifdef USING_VALVEALLOC
    {
        // 48 89 5c 24 08  57  48 83 ec 20  48 8b f9  e8 ?? ?? ?? ??  48 8b d8  48 85 c0  75 2b 66 0f 1f 44 00 00
        "ValveAlloc",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x48, 0x8B, 0xF9,
            0xE8, 0x6E, 0x66, 0x0C, 0x00,
            0x48, 0x8B, 0xD8,
            0x48, 0x85, 0xC0,
            0x75, 0x2B,
            0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00
        },
        "xxxxxxxxxxxxxx????xxxxxxxxxxxxxx",
        (void **)&ValveAlloc,
    },
#endif
#ifdef USING_CMAPSOLID_CLIPBYFACE
    {
        // 48 89 5c 24 08  57  48 83 ec 20  48 8b f9  e8 ?? ?? ?? ??  48 8b d8  48 85 c0  75 2b 66 0f 1f 44 00 00
        "CMapSolid::ClipByFace",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x48, 0x89, 0x6C, 0x24, 0x10,
            0x48, 0x89, 0x74, 0x24, 0x20,
            0x57,
            0x41, 0x56,
            0x41, 0x57,
            0x48, 0x81, 0xEC, 0x90, 0x03, 0x00, 0x00
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMapSolidMethods.ClipByFace,
    },
#endif
#ifdef USING_CMAPSOLID_SPLIT
    {
        // 48 8b c4  48 89 58 10  48 89 68 18  48 89 70 20  57  41 54  41 55  41 56  41 57  48 81 ec b0 03 00 00
        "CMapSolid::ClipByFace",
        (const uint8_t[]){
            0x48, 0x8B, 0xC4,
            0x48, 0x89, 0x58, 0x10,
            0x48, 0x89, 0x68, 0x18,
            0x48, 0x89, 0x70, 0x20,
            0x57,
            0x41, 0x54,
            0x41, 0x55,
            0x41, 0x56,
            0x41, 0x57,
            0x48, 0x81, 0xEC, 0xB0, 0x03, 0x00, 0x00
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMapSolidMethods.Split
    },
#endif
#ifdef USING_CMAPSOLID_CMAPSOLID
    {
        // 48 89 5c 24 18  48 89 74 24 20  48 89 4c 24 08  57  48 83 ec 20  48 8b da  48 8b f9  e8 ?? ?? ?? ??  90  48 8d 05
        "CMapSolid::CMapSolid",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x18,
            0x48, 0x89, 0x74, 0x24, 0x20,
            0x48, 0x89, 0x4C, 0x24, 0x08,
            0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x48, 0x8B, 0xDA,
            0x48, 0x8B, 0xF9,
            0xE8, 0x61, 0x7F, 0xFB, 0xFF,
            0x90,
            0x48, 0x8D, 0x05
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxx",
        (void **)&CMapSolidMethods.CMapSolid,
    },
#endif
#ifdef USING_CMAPSOLID_ADDPLANE
    {
        // 48 89 5c 24 08  48 89 6c 24 10  48 89 74 24 18  57  41 56  41 57  48 81 ec 80 03 00 00  48 8b da
        "CMapSolid:AddPlane",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x48, 0x89, 0x6C, 0x24, 0x10,
            0x48, 0x89, 0x74, 0x24, 0x18,
            0x57,
            0x41, 0x56,
            0x41, 0x57,
            0x48, 0x81, 0xEC, 0x80, 0x03, 0x00, 0x00,
            0x48, 0x8B, 0xDA
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMapSolidMethods.AddPlane,
    },
#endif
#ifdef USING_CSOLIDFACES_MAKEFACE
    {
        // 48 89 5c 24 08  48 89 74 24 10  57  48 83 ec 20  8b 71 10  ba 01 00 00 00  48 8b d9  e8 ?? ?? ?? ??  8b 43 10 8b ce  2b c6  ff c8  48 69 f9 60 03 00 00
        "CSolidFaces::MakeFace",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x48, 0x89, 0x74, 0x24, 0x10,
            0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x8B, 0x71, 0x10,
            0xBA, 0x01, 0x00, 0x00, 0x00,
            0x48, 0x8B, 0xD9,
            0xE8, 0xB1, 0x67, 0xF2, 0xFF,
            0x8B, 0x43, 0x10,
            0x8B, 0xCE,
            0x2B, 0xC6,
            0xFF, 0xC8,
            0x48, 0x69, 0xF9, 0x60, 0x03, 0x00, 0x00
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxxx",
        (void **)&CSolidFaces_MakeFace,
    },
#endif
#ifdef USING_CMAPFACE_COPYFROM
    {
        // 48 89 5c 24 08  48 89 74 24 10  57  48 83 ec 20  8b 71 10  ba 01 00 00 00  48 8b d9  e8 ?? ?? ?? ??  8b 43 10 8b ce  2b c6  ff c8  48 69 f9 60 03 00 00
        "CMapFace::CopyFrom",
        (const uint8_t[]){
            0x48, 0x89, 0x6C, 0x24, 0x18,
            0x56,
            0x57,
            0x41, 0x56,
            0x48, 0x83, 0xEC, 0x20,
            0x45, 0x0F, 0xB6, 0xF1,
            0x41, 0x8B, 0xE8,
            0x48, 0x8B, 0xF2,
            0x48, 0x8B, 0xF9,
            0x48, 0x85, 0xD2
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMapFaceMethods.CopyFrom,
    },
#endif
#ifdef USING_CMAPSOLID_CREATEFROMPLANES
    {
        // 48 8b c4  4c 89 40 18  89 50 10  55  53  56  57  41 54  41 55  41 56  41 57  48 8d a8 a8 fe ff ff
        "CMapSolid::CreateFromPlanes",
        (const uint8_t[]){
            0x48, 0x8B, 0xC4,
            0x4C, 0x89, 0x40, 0x18,
            0x89, 0x50, 0x10,
            0x55,
            0x53,
            0x56,
            0x57,
            0x41, 0x54,
            0x41, 0x55,
            0x41, 0x56,
            0x41, 0x57,
            0x48, 0x8D, 0xA8, 0xA8, 0xFE, 0xFF, 0xFF
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMapSolidMethods.CreateFromPlanes,
    },
#endif
#ifdef USING_CMAPFACE_CREATEFACE
    {
        // 48 89 5c 24 10  48 89 6c 24 18  48 89 74 24 20  57  41 54  41 57  48 83 ec 20  48 8d b9 c8 01 00 00
        "CMapFace::CreateFace",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x10,
            0x48, 0x89, 0x6C, 0x24, 0x18,
            0x48, 0x89, 0x74, 0x24, 0x20,
            0x57,
            0x41, 0x54,
            0x41, 0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x48, 0x8D, 0xB9, 0xC8, 0x01, 0x00, 0x00
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMapFaceMethods.CreateFace,
    },
#endif
#ifdef USING_CMAPFACE_INITIALIZETEXTUREAXES
    {
        // 48 89 5c 24 10  48 89 6c 24 18  57  48 83 ec 30  0f 29 74 24 20  0f 57 f6
        "CMapFace::InitializeTextureAxes",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x10,
            0x48, 0x89, 0x6C, 0x24, 0x18,
            0x57,
            0x48, 0x83, 0xEC, 0x30,
            0x0F, 0x29, 0x74, 0x24, 0x20,
            0x0F, 0x57, 0xF6
        },
        "xxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMapFaceMethods.InitializeTextureAxes,
    },
#endif
#ifdef USING_CMAPFACE_GETORIENTATION
    {
        // 48 83 ec 58  f3 0f 10 89 90 01 00 00  0f 29 74 24 40
        "CMapFace::GetOrientation",
        (const uint8_t[]){
            0x48, 0x83, 0xEC, 0x58,
            0xF3, 0x0F, 0x10, 0x89, 0x90, 0x01, 0x00, 0x00,
            0x0F, 0x29, 0x74, 0x24, 0x40
        },
        "xxxxxxxxxxxxxxxxx",
        (void **)&CMapFaceMethods.GetOrientation
    },
#endif
#ifdef USING_CHISTORY_KEEPNEW
    {
        // 48 89 5c 24 10  48 89 74 24 18  57  48 81 ec 40 01 00 00  48 83 39 00  41 0f b6 d8
        "CHistory::KeepNew",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x10,
            0x48, 0x89, 0x74, 0x24, 0x18,
            0x57,
            0x48, 0x81, 0xEC, 0x40, 0x01, 0x00, 0x00,
            0x48, 0x83, 0x39, 0x00,
            0x41, 0x0F, 0xB6, 0xD8
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CHistory_KeepNew
    },
#endif
#ifdef USING_CHISTORY_KEEP
    {
        // 48 89 5c 24 10  48 89 74 24 18  57  48 81 ec 40 01 00 00  48 83 39 00  48 8b f2
        "CHistory::Keep",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x10,
            0x48, 0x89, 0x74, 0x24, 0x18,
            0x57,
            0x48, 0x81, 0xEC, 0x40, 0x01, 0x00, 0x00,
            0x48, 0x83, 0x39, 0x00,
            0x48, 0x8B, 0xF2
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CHistory_Keep
    },
#endif
#ifdef USING_CHISTORY_UNDO
    {
        // 48 89 5c 24 08  48 89 74 24 10  57  48 83 ec 20  48 8b fa  49 8b f0  48 8b 11  48 8b d9  4c 8d 42 2c  48 85 d2
        "CHistory::Undo",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x48, 0x89, 0x74, 0x24, 0x10,
            0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x48, 0x8B, 0xFA,
            0x49, 0x8B, 0xF0,
            0x48, 0x8B, 0x11,
            0x48, 0x8B, 0xD9,
            0x4C, 0x8D, 0x42, 0x2C,
            0x48, 0x85, 0xD2
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CHistory_Undo
    },
#endif
#ifdef USING_WCKEYVALUES_GETVALUE
    {
        // 48 89 5c 24 08  57  48 83 ec 20  49 8b d8  48 8b f9  e8 ?? ?? ?? ??  3d ff ff 00 00  75 0d
        "WCKeyValues::GetValue",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x49, 0x8B, 0xD8,
            0x48, 0x8B, 0xF9,
            0xE8, 0x6B, 0xFD, 0xFF, 0xFF,
            0x3D, 0xFF, 0xFF, 0x00, 0x00,
            0x75, 0x0D
        },
        "xxxxxxxxxxxxxxxxx????xxxxxxx",
        (void **)&WCKeyValues_GetValue
    },
#endif
#ifdef USING_CSELECTION_SELECTOBJECTLIST
    {
        // 48 89 5c 24 10  48 89 6c 24 18  48 89 74 24 20  57  48 83 ec 20  41 8b e8 48 8b f2  48 8b f9  41 0f ba e0 0a
        "CSelection::SelectObjectList",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x10,
            0x48, 0x89, 0x6C, 0x24, 0x18,
            0x48, 0x89, 0x74, 0x24, 0x20,
            0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x41, 0x8B, 0xE8,
            0x48, 0x8B, 0xF2,
            0x48, 0x8B, 0xF9,
            0x41, 0x0F, 0xBA, 0xE0, 0x0A
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CSelection_SelectObjectList
    },
#endif

    // offsets
#ifdef USING_OFFSET_CMAINFRAME_FACEEDITSHEET
    {
        // 8b cb  41 bd 03 01 00 00  d1 e9  83 e1 08  48 8b a8 a0 17 00 00  8b d1
        "CMainFrame::m_pFaceEditSheet offset",
        (const uint8_t[]){
            0x8B, 0xCB,
            0x41, 0xBD, 0x03, 0x01, 0x00, 0x00,
            0xD1, 0xE9,
            0x83, 0xE1, 0x08,
            0x48, 0x8B, 0xA8, 0xA0, 0x17, 0x00, 0x00,
            0x8B, 0xD1
        },
        "xxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMainFrame_m_pFaceEditSheet_Offset,
        nullptr,
        16,
        PATTERN_OFFSET
    },
#endif
#ifdef USING_OFFSET_CFACEEDITSHEET_FACES
    {
        // 48 89 5c 24 70  48 89 74 24 78  40 f6 c5 08  0f ?? ?? 00 00 00  33 f6  39 b1 ?? ?? 00 00  7e ??  48 8b 87 ?? ?? 00 00
        "CFaceEditSheet::m_Faces offset",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x70,
            0x48, 0x89, 0x74, 0x24, 0x78,
            0x40, 0xF6, 0xC5, 0x08,
            0x0F, 0x84, 0xC9, 0x00, 0x00, 0x00,
            0x33, 0xF6,
            0x39, 0xB1, 0xA8, 0x2F, 0x00, 0x00,
            0x7E, 0x5C,
            0x48, 0x8B, 0x87, 0x98, 0x2F, 0x00, 0x00
        },
        "xxxxxxxxxxxxxxx??xxxxxxx??xxx?xxx??xx",
        (void **)&CFaceEditSheet_m_Faces_Offset,
        nullptr,
        33,
        PATTERN_OFFSET
    },
#endif

    // hooks
#ifdef USING_HOOK_LOADMENUW
    {
        "LoadMenuW",
        nullptr,
        nullptr,
        (void **)&orig_LoadMenuW,
        hook_LoadMenuW,
    },
#endif
#ifdef USING_HOOK_LOADACCELERATORSA
    {
        "LoadAcceleratorsA",
        nullptr,
        nullptr,
        (void **)&orig_LoadAcceleratorsA,
        hook_LoadAcceleratorsA,
    },
#endif
#ifdef USING_HOOK_ENABLEMENUITEM
    {
        "EnableMenuItem",
        nullptr,
        nullptr,
        (void **)&orig_EnableMenuItem,
        hook_EnableMenuItem,
    },
#endif
#ifdef USING_HOOK_CHISTORY_MARKUNDOPOSITION
    {
        // 48 89 5c 24 10  55  56  57  41 54  41 55  41 56  41 57  48 83 ec 20  49 8b d8  48 8b ea  48 8b f1  48 8b 41 30
        "CHistory::MarkUndoPosition",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x10,
            0x55,
            0x56,
            0x57,
            0x41, 0x54,
            0x41, 0x55,
            0x41, 0x56,
            0x41, 0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x49, 0x8B, 0xD8,
            0x48, 0x8B, 0xEA,
            0x48, 0x8B, 0xF1,
            0x48, 0x8B, 0x41, 0x30
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CHistory_MarkUndoPosition,
        hook_CHistory_MarkUndoPosition
    },
#endif
#ifdef USING_HOOK_CLIPPER3D_DRAWBRUSHEXTENTS
    {
        "Clipper3D::DrawBrushExtents",
        (const uint8_t[]){
            0x48, 0x89, 0x5c, 0x24, 0x10,
            0x48, 0x89, 0x74, 0x24, 0x18,
            0x57,
            0x48, 0x83, 0xec, 0x60,
            0xf3, 0x0f, 0x10, 0x81, 0xfc, 0x00, 0x00, 0x00
        },
        "xxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&orig_Clipper3D_DrawBrushExtents,
        hook_Clipper3D_DrawBrushExtents
    },
#endif
#ifdef USING_HOOK_SELECTION3D_RENDERTOOL2D
    {
        "Selection3D::RenderTool2D",
        (const uint8_t[]){
            0x48, 0x8b, 0xc4,
            0x53,
            0x56,
            0x48, 0x83, 0xec, 0x68,
            0x48, 0x8b, 0xd9,
            0x48, 0x89, 0x78, 0x18
        },
        "xxxxxxxxxxxxxxxx",
        (void **)&orig_Selection3D_RenderTool2D,
        hook_Selection3D_RenderTool2D
    },
#endif
#ifdef USING_HOOK_CFACEEDITSHEET_CLICKFACE
    {
        "CFaceEditSheet::ClickFace",
        (const uint8_t[]){
            0x48, 0x89, 0x6c, 0x24, 0x18,
            0x48, 0x89, 0x7c, 0x24, 0x20,
            0x41, 0x54,
            0x41, 0x56,
            0x41, 0x57,
            0x48, 0x83, 0xec, 0x50
        },
        "xxxxxxxxxxxxxxxxxxxx",
        (void **)&orig_CFaceEditSheet_ClickFace,
        hook_CFaceEditSheet_ClickFace
    },
#endif
#ifdef USING_HOOK_RENDERUTILS_DRAWBOUNDSTEXT
    {
       "RenderUtils::DrawBoundsText",
       (const uint8_t[]){
           0x48, 0x8b, 0xc4,
           0x48, 0x89, 0x58, 0x20,
           0x55,
           0x56,
           0x57,
           0x41, 0x56,
           0x41, 0x57, 0x48, 0x8d, 0x68, 0xb8
       },
       "xxxxxxxxxxxxxxxxxx",
       (void **)&orig_DrawBoundsText,
       hook_DrawBoundsText
    },
#endif
#ifdef USING_HOOK_CSTATUSBAR_SETPANETEXT
    {
        "CStatusBar::SetPaneText",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x10,
            0x48, 0x89, 0x74, 0x24, 0x18,
            0x57,
            0x41, 0x54,
            0x41, 0x55,
            0x41, 0x56,
            0x41, 0x57,
            0x48, 0x83, 0xEC, 0x40,
        },
        "xxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&orig_SetPaneText,
        hook_SetPaneText
    },
#endif
#ifdef USING_HOOK_CWND_SETFOCUS
    {
        "CWnd::SetFocus",
        (const uint8_t[]){
            0x40, 0x53,
            0x48, 0x83, 0xec, 0x20,
            0x48, 0x8b, 0xd9,
            0x48, 0x8b, 0x89, 0xd0, 0x00, 0x00, 0x00,
            0x48, 0x85, 0xc9
        },
        "xxxxxxxxxxxxxxxxxxx",
        (void **)&orig_SetFocus,
        hook_SetFocus,
    },
#endif
#ifdef USING_HOOK_CMAPDOC_SETACTIVEMAPDOC
    {
        "CMapDoc::SetActiveMapDoc",
        (const uint8_t[]){
            0x40, 0x53,
            0x41, 0x56,
            0x48, 0x83, 0xEC, 0x48,
            0x48, 0x8B, 0x1D, 0x39, 0x1D, 0xB7, 0x00,
            0x4C, 0x8B, 0xF1,
        },
        "xxxxxxxxxxx???xxxx",
        (void **)&orig_SetActiveMapDoc,
        hook_SetActiveMapDoc
    },
#endif
#ifdef USING_HOOK_AFXWNDPROC
    {
        "AfxWndProc",
        (const uint8_t[]){
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x48, 0x89, 0x6C, 0x24, 0x10,
            0x48, 0x89, 0x74, 0x24, 0x18,
            0x57,
            0x48, 0x83, 0xEC, 0x30,
            0x49, 0x8B, 0xF1,
            0x49, 0x8B, 0xE8,
            0x8B, 0xFA,
            0x48, 0x8B, 0xD9,
            0x81, 0xFA, 0x60, 0x03, 0x00, 0x00
        },
        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&orig_AfxWndProc,
        hook_AfxWndProc
    },
#endif
#ifdef USING_S_CLASSES
    {
        // 48 8D 0D ?? ?? ?? ?? 4C 89 44 24 ?? 4C 8D 44 24 ?? E8
        // lea     rcx, qword_180CAB528
        "s_Classes",
        (const uint8_t[]) {
            0x48, 0x8D, 0x0D,
            0x00, 0x00, 0x00, 0x00,
            0x4C, 0x89, 0x44, 0x24,
            0x00,
            0x4C, 0x8D, 0x44, 0x24,
            0x00,
            0xE8
        },
        "xxx????xxxx?xxxx?x",
        (void **)&S_Classes,
        nullptr,
        0,
        PATTERN_REL,
    },
#endif
#ifdef USING_CMAPCLASS_GETVISGROUPCOUNT
    {
        // 8B 81 ?? ?? ?? ?? C3 ?? ?? ?? ?? ?? ?? ?? ?? ?? 40 53 48 83 EC ? 48 8B D9
        "CMapClass::GetVisGroupCount",
        (const uint8_t[]) {
            0x8B, 0x81, 0x00, 0x00, 0x00, 0x00, 0xC3, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x53, 0x48, 0x83,
            0xEC, 0x00, 0x48, 0x8B, 0xD9
        },
        "xx????x?????????xxxxx?xxx",
        (void **)&CMapClass_GetVisGroupCount,
    },
#endif
#ifdef USING_CMAPCLASS_GETVISGROUP
    {
        // 48 8B 81 ?? ?? ?? ?? 8B D2 ?? ?? ?? ?? C3 ?? ?? 8B 81 ?? ?? ?? ?? C3 ?? ?? ?? ?? ?? ?? ?? ?? ?? 40 53
        "CMapClass::GetVisGroup",
        (const uint8_t[]) {
            0x48, 0x8B, 0x81, 0x00, 0x00, 0x00, 0x00, 0x8B, 0xD2, 0x00,
            0x00, 0x00, 0x00, 0xC3, 0x00, 0x00, 0x8B, 0x81, 0x00, 0x00,
            0x00, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x40, 0x53
        },
        "xxx????xx????x??xx????x?????????xx",
        (void **)&CMapClass_GetVisGroup,
    },
#endif
#ifdef USING_CVISGROUP_ISAUTOVISGROUP
    {
        // 0F B6 41 ?? C3 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 0F B6 05
        "CVisGroup::IsAutoVisGroup",
        (const uint8_t[]) {
            0x0F, 0xB6, 0x41, 0x00, 0xC3, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x0F, 0xB6, 0x05
        },
        "xxx?x???????????xxx",
        (void **)&CVisGroup_IsAutoVisGroup,
    },
#endif
#ifdef USING_CMAPENTITY_HASSOLIDCHILDREN
    {
        // 48 89 5C 24 08 57 48 83 EC 20 33 DB 48 8B F9 39 99 10 01 00 00 7E 2B
        "CMapEntity::HasSolidChildren",
        (const uint8_t[]) {
            0x48, 0x89, 0x5C, 0x24, 0x08,
            0x57,
            0x48, 0x83, 0xEC, 0x20,
            0x33, 0xDB,
            0x48, 0x8B, 0xF9,
            0x39, 0x99, 0x10, 0x01, 0x00, 0x00,
            0x7E, 0x2B
        },
        "xxxxxxxxxxxxxxxxxxxxxxx",
        (void **)&CMapEntityMethods.HasSolidChildren,
    },
#endif
};

static void *find_pattern(uint8_t *base, size_t size, const uint8_t *pat, const char *mask) {
    size_t len = strlen(mask);

    for (size_t i = 0; i <= size - len; i++) {
        size_t j = 0;
        for (; j < len; j++) {
            if (mask[j] == 'x' && base[i + j] != pat[j])
                break;
        }
        if (j == len)
            return base + i;
    }
    return nullptr;
}

bool scan_all(uint8_t *base, size_t size) {
    bool success = true;

    for (auto i = 0; i < ARRAYSIZE(g_patterns); i++) {
        Pattern_t *p = &g_patterns[i];

        void *addr = nullptr;
        if (p->pattern) {
            addr = find_pattern(
                base,
                size,
                p->pattern,
                p->mask
            );
        } else {
            // only user32 supported rn
            void *user32 = LoadLibraryA("user32.dll");
            ASSERT(user32);
            addr = GetProcAddress(user32, p->name);
        }

        if (!addr) {
            log_msg("[patterns] %s not found\n", p->name);
            if (!(p->flags & PATTERN_OPTIONAL)) {
                success = false;
            }
            continue;
        }

        if (p->offset) {
            addr += p->offset;
        }

        if (p->flags & PATTERN_REL) {
            addr = resolve_rel(addr, addr);
        } else if (p->flags & PATTERN_OFFSET) {
            addr = (void *)(size_t)*(uint32_t*)addr;
        }

        if (p->hook) {
#ifndef TEST_PATTERNS
            if (MH_CreateHook(addr, p->hook, p->out) != MH_OK) {
                log_msg("[patterns] %s MH_CreateHook failed\n", p->name);
                if (!(p->flags & PATTERN_OPTIONAL)) {
                    success = false;
                }
                continue;
            }
#endif
        } else if (p->out) {
            *(p->out) = addr;
        }

#ifndef TEST_PATTERNS
        log_msg("[patterns] %-40s %-6s %c %p\n",
                p->name,
                p->hook ? "hooked" : "found",
                (p->flags & PATTERN_OFFSET ? '=' : '@'),
                addr);
#endif
    }

    return success;
}
