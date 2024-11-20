#include <ofyaGl/obj.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace ofyaGl {

struct FaceVertexData {
  unsigned int v;
  unsigned int vt;
  unsigned int vn;

  friend std::ostream &operator<<(std::ostream &os, const FaceVertexData &fvd) {
    os << fvd.v << "/" << fvd.vt << "/" << fvd.vn;
    return os;
  }
};

struct FaceData {
  FaceVertexData v1;
  FaceVertexData v2;
  FaceVertexData v3;

  friend std::ostream &operator<<(std::ostream &os, const FaceData &fd) {
    os << "FaceData(" << fd.v1 << " " << fd.v2 << " " << fd.v3 << ")";
    return os;
  }
};

std::optional<VertPos> parseVertPos(const std::string &input) {
  VertPos vertPos{};
  std::stringstream ss(input);

  // Skip 'v'
  std::string prefix;
  ss >> prefix;

  float x, y, z;

  if (!(ss >> x >> y >> z)) {
    std::cout << "Expected 3 numbers, but parsed less\n";
    return {};
  }

  std::string leftover;
  if (ss >> leftover) {
    if (leftover[0] != '#') {
      std::cout << "Expected 3 numbers, but parsed more\n";
      return {};
    }
  }

  vertPos.x = x;
  vertPos.y = y;
  vertPos.z = z;
  return vertPos;
}

std::optional<TexCoord> parseTexCoord(const std::string &input) {
  TexCoord texCoord{};
  std::stringstream ss(input);

  // Skip 'vt'
  std::string prefix;
  ss >> prefix;

  // TODO handle the case when 3rd value is present
  float u, v, t = 0;
  if (!(ss >> u >> v)) {
    std::cout << "Got less then 2 numbers\n";
    return {};
  }

  texCoord.u = u;
  texCoord.v = v;
  texCoord.t = t;
  return texCoord;
}

std::optional<VertNormal> parseVertNormal(const std::string &input) {
  VertNormal vert{};
  std::stringstream ss(input);

  // Skip 'vn'
  std::string prefix;
  ss >> prefix;

  float x, y, z;

  if (!(ss >> x >> y >> z)) {
    std::cout << "Expected 3 numbers, but parsed less\n";
    return {};
  }

  std::string leftover;
  if (ss >> leftover) {
    if (leftover[0] != '#') {
      std::cout << "Expected 3 numbers, but parsed more\n";
      return {};
    }
  }

  vert.x = x;
  vert.y = y;
  vert.z = z;
  return vert;
}

int extractNumbers(const std::string &input, float &x, float &y, float &z) {
  std::istringstream iss(input);

  std::string prefix;
  iss >> prefix;

  if (!(iss >> x >> y >> z)) {
    std::cout << "Expected 3 numbers, but parsed less\n";
    return 1;
  }

  std::string leftover;
  if (iss >> leftover) {
    if (leftover[0] == '#') {
      return 0;
    }
    std::cout << "Expected 3 numbers, but parsed more\n";
    return 1;
  }

  return 0;
}

FaceVertexData parseFaceVertex(const std::string &vertex) {
  FaceVertexData data{};

  std::stringstream ss(vertex);
  std::string token;
  int idx = 0;

  while (std::getline(ss, token, '/')) {
    if (!token.empty()) {
      unsigned int value = std::stoi(token);
      if (idx % 3 == 0) {
        data.v = value;
      } else if (idx % 3 == 1) {
        data.vt = value;
      } else if (idx % 3 == 2) {
        data.vn = value;
      }
    }
    idx++;
  }

  return data;
}

std::vector<FaceData> parseFace(const std::string &line) {
  std::vector<FaceData> result = {};
  std::stringstream ss(line);
  std::string token;

  // Skip 'f'
  ss >> token;

  std::vector<FaceVertexData> verticies;
  while (ss >> token) {
    verticies.push_back(parseFaceVertex(token));
  }

  if (verticies.size() < 3) {
    std::cerr << "Expected at least 3 verticies for the face\n";
    return {};
  }

  for (size_t i = 1; i < verticies.size() - 1; i++) {
    result.push_back({verticies[0], verticies[i], verticies[i + 1]});
  }

  return result;
}
std::optional<ObjData> loadObjDataFromFile(const char *fileName) {
  std::string objDir = std::getenv("ICG_OBJ_DIR");
  std::string s_fileName = fileName;
  std::filesystem::path fullFilePath = objDir + "/" + fileName;

  auto extention = fullFilePath.extension();
  if (fullFilePath.extension() != ".obj") {
    std::cerr << "There is no support for '" << extention << "' file formats\n";
    return {};
  }

  std::cout << "Loading obj data from file '" << fullFilePath << "'\n";

  std::ifstream file(fullFilePath);
  if (!file.is_open()) {
    std::cerr << "Failed to open file '" << fullFilePath << "'\n";
    return {};
  }

  std::vector<VertPos> vertPoses;
  std::vector<TexCoord> texCoords;
  std::vector<VertNormal> vertNormals;
  std::vector<FaceData> faceDatas;

  unsigned int lineNumber = 0;
  std::string line;
  while (getline(file, line)) {
    lineNumber++;
    if (line.length() == 0) {
      continue;
    }

    // Comment
    if (line[0] == '#') {
      continue;
    }

    if (line[0] == 'v') {
      if (line[1] == ' ') { // Vertex
        auto vertPos = parseVertPos(line);
        if (!vertPos.has_value()) {
          std::cerr << "Error at line: " << lineNumber << std::endl;
          return {};
        }
        vertPoses.push_back(vertPos.value());
      } else if (line[1] == 't') { // Texture coordinate
        auto texCoord = parseTexCoord(line);
        if (!texCoord.has_value()) {
          std::cerr << "Error at line: " << lineNumber << std::endl;
          return {};
        }
        texCoords.push_back(texCoord.value());
      } else if (line[1] == 'n') { // Vertex normal
        auto vertNormal = parseVertNormal(line);
        if (!vertNormal.has_value()) {
          std::cerr << "Error at line: " << lineNumber << std::endl;
          return {};
        }
        vertNormals.push_back(vertNormal.value());
      }
    } else if (line[0] == 'f') { // face
      auto parsedDatas = parseFace(line);
      if (parsedDatas.size() == 0) {
        std::cerr << "Error at line: " << lineNumber << std::endl;
        return {};
      }
      for (auto parsedData : parsedDatas) {
        faceDatas.push_back(parsedData);
      }
    }

    // Ignore anything else
  }

  file.close();

  std::vector<Vertex> verts;
  std::vector<unsigned int> indicies;

  std::unordered_map<Vertex, unsigned int, VertexHash> vertexToIndex;

  for (const auto &faceData : faceDatas) {
    FaceVertexData faceVertexData[3] = {faceData.v1, faceData.v2, faceData.v3};
    for (const auto &fvd : faceVertexData) {
      Vertex vertex = {vertPoses[fvd.v - 1], texCoords[fvd.vt - 1],
                       vertNormals[fvd.vn - 1]};

      auto it = vertexToIndex.find(vertex);
      if (it != vertexToIndex.end()) {
        indicies.push_back(it->second);
      } else {
        unsigned int newIndex = verts.size();
        vertexToIndex[vertex] = newIndex;
        verts.push_back(vertex);
        indicies.push_back(newIndex);
      }
    }
  }

  std::cout << "Loaded obj\n";

  return ObjData{verts, indicies};
}

} // namespace ofyaGl
