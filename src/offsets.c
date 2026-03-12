#include "offsets.h"

#ifdef USING_OFFSET_CMAINFRAME_FACEEDITSHEET
size_t CMainFrame_m_pFaceEditSheet_Offset;
#endif

#ifdef USING_OFFSET_CFACEEDITSHEET_FACES
size_t CFaceEditSheet_m_Faces_Offset;
#endif

void dump_offsets() {
    // log_msg("RefVector size %zx\n", sizeof(RefVector));
    // log_msg("RefVector::allocation_count %zx\n", offsetof(RefVector, allocation_count));
    // log_msg("RefVector::grow_size %zx\n", offsetof(RefVector, grow_size));
    // log_msg("RefVector::padding %zx\n", offsetof(RefVector, padding));
    // log_msg("RefVector::length %zx\n", offsetof(RefVector, length));
    //
    // log_msg("CMapFace size %zx\n", sizeof(CMapFace));
    // log_msg("CMapFace::Points %zx\n", offsetof(CMapFace, Points));
    //
    // log_msg("CMapClass::m_Origin %zx\n", offsetof(CMapClass, m_Origin));
}
