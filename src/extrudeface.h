#ifndef EXTRUDE_FACE_H
#define EXTRUDE_FACE_H

void do_extrude_face();
void extrude_face_close();
void extrude_face_mouse_move_3d(CMapView3D *view, const Vec2 *point);
void extrude_face_mouse_move_2d(CMapView2D *view, const Vec2 *point);

#endif // EXTRUDE_FACE_H
