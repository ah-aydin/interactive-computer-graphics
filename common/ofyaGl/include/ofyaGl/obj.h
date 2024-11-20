#pragma once

#include <optional>
#include <ostream>
#include <vector>

namespace ofyaGl {
#pragma pack(push, 1)
struct VertPos {
  float x;
  float y;
  float z;

  friend std::ostream &operator<<(std::ostream &os, const VertPos &vert) {
    os << "Vert(" << vert.x << " " << vert.y << " " << vert.z << ")";
    return os;
  }

  bool operator==(const VertPos &other) const {
    return x == other.x && y == other.y && z == other.z;
  }
};

struct TexCoord {
  float u;
  float v;
  float t;

  friend std::ostream &operator<<(std::ostream &os, const TexCoord &textCoord) {
    os << "TextCoord(" << textCoord.u << " " << textCoord.v << " "
       << textCoord.t << ")";
    return os;
  }

  bool operator==(const TexCoord &other) const {
    return u == other.u && v == other.v && t == other.t;
  }
};

struct VertNormal {
  float x;
  float y;
  float z;

  friend std::ostream &operator<<(std::ostream &os,
                                  const VertNormal &vertNormal) {
    os << "VertNormal(" << vertNormal.x << " " << vertNormal.y << " "
       << vertNormal.z << ")";
    return os;
  }

  bool operator==(const VertNormal &other) const {
    return x == other.x && y == other.y && z == other.z;
  }
};

struct Vertex {
  VertPos pos;
  TexCoord texCoord;
  VertNormal normal;

  friend std::ostream &operator<<(std::ostream &os, const Vertex &vertex) {
    os << "Vertex(" << vertex.pos << ", " << vertex.texCoord << ", "
       << vertex.normal << ")";
    return os;
  }

  bool operator==(const Vertex &other) const {
    return pos == other.pos && texCoord == other.texCoord &&
           normal == other.normal;
  }
};

struct VertexHash {
  size_t operator()(const Vertex &v) const {
    return std::hash<float>()(v.pos.x) ^ std::hash<float>()(v.pos.y) ^
           std::hash<float>()(v.pos.z) ^ std::hash<float>()(v.texCoord.u) ^
           std::hash<float>()(v.texCoord.v) ^ std::hash<float>()(v.texCoord.t) ^
           std::hash<float>()(v.normal.x) ^ std::hash<float>()(v.normal.y) ^
           std::hash<float>()(v.normal.z);
  }
};

struct ObjData {
  std::vector<Vertex> verts;
  std::vector<unsigned int> indicies;
};

std::optional<ObjData> loadObjDataFromFile(const char *fileName);
#pragma pack(pop)
} // namespace ofyaGl
