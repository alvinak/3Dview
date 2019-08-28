#ifndef MESHOPERATIONS_H
#define MESHOPERATIONS_H

#include "Mesh.h"
#include <QVector>

class MeshOperations
{
public:
    static void decimation(Mesh::MeshPtr mesh, double reduction);
    static void subdivision(Mesh::MeshPtr mesh, int numOfSubdivisions);
    static void fillHoles(Mesh::MeshPtr mesh);
    static Mesh::MeshPtr booleanUnion(const QVector<Mesh::MeshPtr>& meshes);
};

#endif