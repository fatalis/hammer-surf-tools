#include <MinHook.h>

#include "common.h"
#include "patterns.h"
#include "wndproc.h"
#include "hooks.h"
#include "contextmenu.h"
#include "scriptfuncs.h"
#include "hotkeys.h"
#include "hammerfuncs.h"

static void *resolve_rel(void *call_addr, const uint8_t *insn) {
    if (insn[0] == 0xE8) { // call
        int32_t rel = *(int32_t *)(insn + 1);
        return call_addr + 5 + rel;
    } else if (insn[0] == 0x48) { // lea
        int32_t rel = *(int32_t *)(insn + 3);
        return call_addr + 7 + rel;
    }
    assert(false);
}

static Pattern_t g_patterns[] = {
    // { 
    //     // find "You are about to export over your"
    //     "GetMainWnd",
    //     (const uint8_t[]){
    //         0x48, 0x8B, 0x0A,
    //         0x48, 0x8B, 0x01,
    //         0xFF, 0x50, 0x08,
    //         0x85, 0xFF,
    //         0x75, 0x2A,
    //         0xE8, 0x8F, 0x55, 0xFE, 0xFF,
    //         0x41, 0xB9, 0x34, 0x00, 0x00, 0x00
    //     },
    //     "xxxxxxxxxxxxxx????xxxxxx",
    //     (void **)&GetMainWnd,
    //     nullptr,
    //     13,
    //     true
    // },
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
    // { 
    //     "AfxGetApp",
    //     (const uint8_t[]){
    //         0x48, 0x83, 0xEC, 0x28,
    //         0x48, 0x8D, 0x15, 0x00, 0x00, 0x00, 0x00,
    //         0x48, 0x8D, 0x0D, 0x00, 0x00, 0x00, 0x00,
    //         0xE8, 0x00, 0x00, 0x00, 0x00,
    //         0x48, 0x85, 0xC0,
    //         0x74, 0x26
    //     },
    //     "xxxxxxx????xxx????x????xxxxx",
    //     (void **)&AfxGetApp,
    // },
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
        (void **)&EnumChildren
    },
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
        (void **)&CMapDoc_UpdateAllViews
    },
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
    // { 
    //     "CToolMaterial::OnMouseMove3D",
    //     (const uint8_t[]){
    //         0x48, 0x89, 0x5C, 0x24, 0x08,
    //         0x57,
    //         0x48, 0x83, 0xEC, 0x20,
    //         0x8D, 0x42, 0x01,
    //         0x8B, 0xFA,
    //         0x48, 0x8B, 0xD9,
    //         0x83, 0xF8, 0x02,
    //         0x77, 0x71
    //     },
    //     "xxxxxxxxxxxxxxxxxxxxxxx",
    //     (void **)&orig_CToolMaterial_OnMouseMove3D,
    //     hook_CToolMaterial_OnMouseMove3D
    // },
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
    //{ 
    //    "RenderUtils::DrawBoundsText",
    //    (const uint8_t[]){
    //        0x48, 0x8b, 0xc4,
    //        0x48, 0x89, 0x58, 0x20,
    //        0x55,
    //        0x56,
    //        0x57,
    //        0x41, 0x56,
    //        0x41, 0x57, 0x48, 0x8d, 0x68, 0xb8
    //    },
    //    "xxxxxxxxxxxxxxxxxx",
    //    (void **)&orig_DrawBoundsText,
    //    hook_DrawBoundsText
    //},
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
        (void **)&SetModifiedFlag
    },
    // { 
    //     "CStatusBar::SetPaneText",
    //     (const uint8_t[]){
    //         0x48, 0x89, 0x5C, 0x24, 0x10,
    //         0x48, 0x89, 0x74, 0x24, 0x18,
    //         0x57,
    //         0x41, 0x54,
    //         0x41, 0x55,
    //         0x41, 0x56,
    //         0x41, 0x57,
    //         0x48, 0x83, 0xEC, 0x40,
    //     },
    //     "xxxxxxxxxxxxxxxxxxxxxxx",
    //     (void **)&orig_SetPaneText,
    //     hook_SetPaneText
    // },
    // { 
    //     "CWnd::SetFocus",
    //     (const uint8_t[]){
    //         0x40, 0x53,
    //         0x48, 0x83, 0xec, 0x20,
    //         0x48, 0x8b, 0xd9,
    //         0x48, 0x8b, 0x89, 0xd0, 0x00, 0x00, 0x00,
    //         0x48, 0x85, 0xc9
    //     },
    //     "xxxxxxxxxxxxxxxxxxx",
    //     (void **)&orig_SetFocus,
    //     hook_SetFocus,
    // },
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
        (void **)&CMapFace_SetTexture
    },
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
        true
    },
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
        true
    },
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
        (void **)&CMapEntity_CMapEntity,
    },
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
    // { 
    //     // 48 89 5c 24 08  57  48 83 ec 20  48 8b f9  e8 ?? ?? ?? ??  48 8b d8  48 85 c0  75 2b 66 0f 1f 44 00 00
    //     "CMapSolid::ClipByFace",
    //     (const uint8_t[]){
    //         0x48, 0x89, 0x5C, 0x24, 0x08,
    //         0x48, 0x89, 0x6C, 0x24, 0x10,
    //         0x48, 0x89, 0x74, 0x24, 0x20,
    //         0x57,
    //         0x41, 0x56,
    //         0x41, 0x57,
    //         0x48, 0x81, 0xEC, 0x90, 0x03, 0x00, 0x00
    //     },
    //     "xxxxxxxxxxxxxxxxxxxxxxxxxxx",
    //     (void **)&CMapSolid_ClipByFace,
    // },
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
        (void **)&CMapSolid_CMapSolid,
    },
    // { 
    //     // 48 89 5c 24 08  48 89 6c 24 10  48 89 74 24 18  57  41 56  41 57  48 81 ec 80 03 00 00  48 8b da
    //     "CMapSolid:AddPlane",
    //     (const uint8_t[]){
    //         0x48, 0x89, 0x5C, 0x24, 0x08,
    //         0x48, 0x89, 0x6C, 0x24, 0x10,
    //         0x48, 0x89, 0x74, 0x24, 0x18,
    //         0x57,
    //         0x41, 0x56,
    //         0x41, 0x57,
    //         0x48, 0x81, 0xEC, 0x80, 0x03, 0x00, 0x00,
    //         0x48, 0x8B, 0xDA
    //     },
    //     "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    //     (void **)&CMapSolid_AddPlane,
    // },
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
    // { 
    //     // 48 89 5c 24 08  48 89 74 24 10  57  48 83 ec 20  8b 71 10  ba 01 00 00 00  48 8b d9  e8 ?? ?? ?? ??  8b 43 10 8b ce  2b c6  ff c8  48 69 f9 60 03 00 00
    //     "CMapFace::CopyFrom",
    //     (const uint8_t[]){
    //         0x48, 0x89, 0x6C, 0x24, 0x18,
    //         0x56,
    //         0x57,
    //         0x41, 0x56,
    //         0x48, 0x83, 0xEC, 0x20,
    //         0x45, 0x0F, 0xB6, 0xF1,
    //         0x41, 0x8B, 0xE8,
    //         0x48, 0x8B, 0xF2,
    //         0x48, 0x8B, 0xF9,
    //         0x48, 0x85, 0xD2
    //     },
    //     "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    //     (void **)&CMapFace_CopyFrom,
    // },
    // { 
    //     // 48 8b c4  4c 89 40 18  89 50 10  55  53  56  57  41 54  41 55  41 56  41 57  48 8d a8 a8 fe ff ff
    //     "CMapSolid::CreateFromPlanes",
    //     (const uint8_t[]){
    //         0x48, 0x8B, 0xC4,
    //         0x4C, 0x89, 0x40, 0x18,
    //         0x89, 0x50, 0x10,
    //         0x55,
    //         0x53,
    //         0x56,
    //         0x57,
    //         0x41, 0x54,
    //         0x41, 0x55,
    //         0x41, 0x56,
    //         0x41, 0x57,
    //         0x48, 0x8D, 0xA8, 0xA8, 0xFE, 0xFF, 0xFF
    //     },
    //     "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    //     (void **)&CMapSolid_CreateFromPlanes,
    // },
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
        (void **)&CMapFace_CreateFace,
    },
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
        (void **)&CMapFace_InitializeTextureAxes,
    },
    { 
        // 48 83 ec 58  f3 0f 10 89 90 01 00 00  0f 29 74 24 40
        "CMapFace::GetOrientation",
        (const uint8_t[]){
            0x48, 0x83, 0xEC, 0x58,
            0xF3, 0x0F, 0x10, 0x89, 0x90, 0x01, 0x00, 0x00,
            0x0F, 0x29, 0x74, 0x24, 0x40
        },
        "xxxxxxxxxxxxxxxxx",
        (void **)&CMapFace_GetOrientation
    },
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
        (void **)&CHistory_MarkUndoPosition
    },
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
    { 
        "LoadMenuW",
        nullptr,
        nullptr,
        (void **)&orig_LoadMenuW,
        hook_LoadMenuW,
    },
    { 
        "LoadAcceleratorsA",
        nullptr,
        nullptr,
        (void **)&orig_LoadAcceleratorsA,
        hook_LoadAcceleratorsA,
    },
    { 
        "EnableMenuItem",
        nullptr,
        nullptr,
        (void **)&orig_EnableMenuItem,
        hook_EnableMenuItem,
    },
    // {
    //     "<SetupHotkeys>",
    //     (const uint8_t[]){
    //         0x48, 0x89, 0x5C, 0x24, 0x10,
    //         0x44, 0x89, 0x44, 0x24, 0x18,
    //         0x48, 0x89, 0x4C, 0x24, 0x08,
    //         0x55,
    //         0x56,
    //         0x57,
    //         0x41, 0x54,
    //         0x41, 0x55,
    //         0x41, 0x56,
    //         0x41, 0x57,
    //         0x48, 0x83, 0xEC, 0x30,
    //         0x48, 0x8B, 0xFA
    //     },
    //     "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    //     (void **)&orig_SetupHotkeys,
    //     hook_SetupHotkeys,
    // },
    // {
    //     "<RegisterHotkey>",
    //     (const uint8_t[]){
    //         0x4C, 0x8D, 0x84, 0x24, 0x88, 0x00, 0x00, 0x00,
    //         0x8B, 0x53, 0x18,
    //         0x48, 0x8D, 0x4B, 0x08,
    //         0xE8
    //     },
    //     "xxxxxxxxxxxxxxxx",
    //     (void **)&orig_RegisterHotkey,
    //     hook_RegisterHotkey,
    //     15,
    //     true
    // }
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
            assert(user32);
            addr = GetProcAddress(user32, p->name);
        }

        if (!addr) {
            log_msg("[patterns] %s not found\n", p->name);
            success = false;
            continue;
        }

        if (p->offset) {
            addr += p->offset;
        }

        if (p->rel) {
            addr = resolve_rel(addr, addr);
        }

        if (p->hook) {
#ifndef TEST_PATTERNS
            if (MH_CreateHook(addr, p->hook, p->out) != MH_OK) {
                log_msg("[patterns] %s MH_CreateHook failed\n", p->name);
                success = false;
                continue;
            }
#endif
        } else if (p->out) {
            *(p->out) = addr;
        }

#ifndef TEST_PATTERNS
        log_msg("[patterns] %-25s\tfound @ %p\n", p->name, addr);
#endif
    }

    return success;
}
