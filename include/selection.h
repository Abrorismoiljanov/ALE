#include "mesh.h"

struct SelectedMeshInfo {
    Mesh* mesh;          // pointer to the mesh
    int id;              // optional unique ID
    bool hovered;        // true if mouse is over it
    bool selected;       // true if actively selected
};
