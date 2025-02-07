#include <vector>
#include <glm/glm.hpp>

template <typename VertexType>
void generateScopeVertices(std::vector<VertexType>& vertices, const glm::vec2& center, float radius, int segmentCount, float Ar) {
    // four screen corners
	std::vector<VertexType> quadVertices = {
        {{-1.0f, -1.0f}, {0.9f, 0.5f}},  
        {{ 1.0f, -1.0f}, {0.9f, 0.5f}},  
        {{ 1.0f,  1.0f}, {0.9f, 0.5f}},  
        {{-1.0f,  1.0f}, {0.9f, 0.5f}}   
    };
    vertices.insert(vertices.end(), quadVertices.begin(), quadVertices.end());
    vertices.push_back({center, {0.9f, 0.5f}}); // centre

    float angleStep = 2.0f * M_PI / segmentCount;
    for (int i = 0; i <= segmentCount; ++i) {  
        float angle = i * angleStep;
		glm::vec2 pos = center + glm::vec2(radius * cos(angle), radius * sin(angle) * Ar );
        vertices.push_back({pos, {0.9f, 0.5f}});
    }
}

void generateScopeIndices(std::vector<unsigned int>& indices, int segmentCount) {
	indices.push_back(1); // top right
	indices.push_back(2); // bottom right
	indices.push_back(5); // 0 degs of circle

	for (int i = 0; i < 16; i++) {
		indices.push_back(2);
		indices.push_back(5 + i);
		indices.push_back(6 + i);
    }

	indices.push_back(2); // bottom right
	indices.push_back(3); //bottom left
	indices.push_back(5 + 16); // bottom of circle 

	for (int i = 0; i < 16; i++) {
		indices.push_back(3);
		indices.push_back(21 + i);
		indices.push_back(22 + i);
    }
	indices.push_back(3);
	indices.push_back(0);
	indices.push_back(5 + 16*2);

	for (int i = 0; i < 16; i++) {
		indices.push_back(0);
		indices.push_back(37 + i);
		indices.push_back(38 + i);
    }

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(5 + 16*3);

	for (int i = 0; i < 16; i++) {
		indices.push_back(1);
		indices.push_back(53 + i);
		indices.push_back(54 + i);
    }
}
