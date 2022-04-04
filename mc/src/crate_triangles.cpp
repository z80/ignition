// no triangles if it is surrounded by air or surrounded by blocks
                    if (Table.EDGES[cubeindex] == 0 || Table.EDGES[cubeindex] == 255) continue;
 
                    // Find the vertices where the surface intersects the cube
                    if ((Table.EDGES[cubeindex] & 1) == 1)
                        vertList[0] = vertexInterpolation(ISO_LEVEL, corners[0], corners[1], densities[0], densities[1]);
                    if ((Table.EDGES[cubeindex] & 2) == 2)
                        vertList[1] = vertexInterpolation(ISO_LEVEL, corners[1], corners[2], densities[1], densities[2]);
                    if ((Table.EDGES[cubeindex] & 4) == 4)
                        vertList[2] = vertexInterpolation(ISO_LEVEL, corners[2], corners[3], densities[2], densities[3]);
                    if ((Table.EDGES[cubeindex] & 8) == 8)
                        vertList[3] = vertexInterpolation(ISO_LEVEL, corners[3], corners[0], densities[3], densities[0]);
                    if ((Table.EDGES[cubeindex] & 16) == 16)
                        vertList[4] = vertexInterpolation(ISO_LEVEL, corners[4], corners[5], densities[4], densities[5]);
                    if ((Table.EDGES[cubeindex] & 32) == 32)
                        vertList[5] = vertexInterpolation(ISO_LEVEL, corners[5], corners[6], densities[5], densities[6]);
                    if ((Table.EDGES[cubeindex] & 64) == 64)
                        vertList[6] = vertexInterpolation(ISO_LEVEL, corners[6], corners[7], densities[6], densities[7]);
                    if ((Table.EDGES[cubeindex] & 128) == 128)
                        vertList[7] = vertexInterpolation(ISO_LEVEL, corners[7], corners[4], densities[7], densities[4]);
                    if ((Table.EDGES[cubeindex] & 256) == 256)
                        vertList[8] = vertexInterpolation(ISO_LEVEL, corners[0], corners[4], densities[0], densities[4]);
                    if ((Table.EDGES[cubeindex] & 512) == 512)
                        vertList[9] = vertexInterpolation(ISO_LEVEL, corners[1], corners[5], densities[1], densities[5]);
                    if ((Table.EDGES[cubeindex] & 1024) == 1024)
                        vertList[10] = vertexInterpolation(ISO_LEVEL, corners[2], corners[6], densities[2], densities[6]);
                    if ((Table.EDGES[cubeindex] & 2048) == 2048)
                        vertList[11] = vertexInterpolation(ISO_LEVEL, corners[3], corners[7], densities[3], densities[7]);
                    
                    for (int i = 0; Table.TRIANGLES[cubeindex][i] != -1; i += 3) {
                        vertices.add(createVertex(chunk, mul, liquid, vertList[Table.TRIANGLES[cubeindex][i + 2]]));
                        vertices.add(createVertex(chunk, mul, liquid, vertList[Table.TRIANGLES[cubeindex][i + 1]]));
                        vertices.add(createVertex(chunk, mul, liquid, vertList[Table.TRIANGLES[cubeindex][i]]));
                    }
                }
            }
        }
    }
    
    private static Vertex createVertex(ChunkThread chunk, float mul, boolean liquid, Vector3d position) {
        Vertex vertex = new Vertex();
        vertex.position.set(position.x + chunk.x * ChunkThread.CHUNK_SIZE, position.y + chunk.y * ChunkThread.CHUNK_SIZE, position.z + chunk.z * ChunkThread.CHUNK_SIZE);
        vertex.normal = chunk.calculateNormal((float) position.x, (float) position.y, (float) position.z, NORMAL_SMOOTHNESS * mul, liquid);
        vertex.material = chunk.getMaterial((int) position.x, (int) position.y, (int) position.z);
        
        return vertex;
    }
 
    private static Vector3d vertexInterpolation(float isolevel, Vector3i p1, Vector3i p2, float valp1, float valp2) {
        float mu;
        Vector3d p = new Vector3d();
 
        mu = (isolevel - valp1) / (valp2 - valp1);
        p.x = p1.x + mu * (float) (p2.x - p1.x);
        p.y = p1.y + mu * (float) (p2.y - p1.y);
        p.z = p1.z + mu * (float) (p2.z - p1.z);
 
        return p;
    }
 
}