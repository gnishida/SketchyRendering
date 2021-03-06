#include "OBJLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QFile>

/**
 * Load vertices data from a OBJ file.
 */
void OBJLoader::load(const char* filename, std::vector<Vertex>& vertices) {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	QTextStream in(&file);
 
	std::vector<float> raw_vertices;
	std::vector<float> raw_normals;
	std::vector<float> raw_texCoords;
	std::vector<unsigned int> v_elements;
	std::vector<unsigned int> t_elements;
	std::vector<unsigned int> n_elements;

	while (!in.atEnd()) {
		QString line = in.readLine();
		if (line.startsWith("v ")) {
			int index1 = line.indexOf(QRegExp("[0-9\\.\\-]"), 2);
			int index2 = line.lastIndexOf(QRegExp("[0-9]"));
			QStringList values = line.mid(index1, index2 - index1 + 1).split(" ");
			raw_vertices.push_back(values[0].toFloat());
			raw_vertices.push_back(values[1].toFloat());
			raw_vertices.push_back(values[2].toFloat());
		} else if (line.startsWith("vn ")) {
			int index1 = line.indexOf(QRegExp("[0-9\\.\\-]"), 2);
			int index2 = line.lastIndexOf(QRegExp("[0-9]"));
			QStringList values = line.mid(index1, index2 - index1 + 1).split(" ");
			raw_normals.push_back(values[0].toFloat());
			raw_normals.push_back(values[1].toFloat());
			raw_normals.push_back(values[2].toFloat());
		} else if (line.startsWith("vt ")) {
			int index1 = line.indexOf(QRegExp("[0-9\\.\\-]"), 2);
			int index2 = line.lastIndexOf(QRegExp("[0-9]"));
			QStringList values = line.mid(index1, index2 - index1 + 1).split(" ");
			raw_texCoords.push_back(values[0].toFloat());
			raw_texCoords.push_back(values[1].toFloat());
		} else if (line.startsWith("f ")) {
			int index1 = line.indexOf(QRegExp("[0-9]"), 2);
			int index2 = line.lastIndexOf(QRegExp("[0-9]"));
			QStringList values = line.mid(index1, index2 - index1 + 1).split(" ");
			for (int i = 0; i < values.size() - 2; ++i) {
				unsigned int a,b,c;
				a = values[0].split("/")[0].toUInt() - 1;
				b = values[i+1].split("/")[0].toUInt() - 1;
				c = values[i+2].split("/")[0].toUInt() - 1;
				v_elements.push_back(a); v_elements.push_back(b); v_elements.push_back(c);

				if (values[0].split("/").size() >= 2 && values[0].split("/")[1].size() > 0) {
					a = values[0].split("/")[1].toUInt() - 1;
					b = values[i+1].split("/")[1].toUInt() - 1;
					c = values[i+2].split("/")[1].toUInt() - 1;
					t_elements.push_back(a); t_elements.push_back(b); t_elements.push_back(c);
				}

				if (values[0].split("/").size() >= 3 && values[0].split("/")[2].size() > 0) {
					a = values[0].split("/")[2].toUInt() - 1;
					b = values[i+1].split("/")[2].toUInt() - 1;
					c = values[i+2].split("/")[2].toUInt() - 1;
					n_elements.push_back(a); n_elements.push_back(b); n_elements.push_back(c);
				}
			}
		} else if (line.size() == 0) {
			/* ignore empty line */
		} else if (line.startsWith("#")) {
			/* ignore comment line */
		} else {
			/* ignore this line */
		}
	}

	vertices.resize(v_elements.size());
	for (int i = 0; i < v_elements.size(); i+=3) {
		unsigned int ia = v_elements[i];
		unsigned int ib = v_elements[i+1];
		unsigned int ic = v_elements[i+2];

		for (int j = 0; j < 3; ++j) {
			vertices[i].position[j] = raw_vertices[ia*3 + j];
			vertices[i+1].position[j] = raw_vertices[ib*3 + j];
			vertices[i+2].position[j] = raw_vertices[ic*3 + j];
		}

		if (n_elements.size() > 0) {
			for (int j = 0; j < 3; ++j) {
				vertices[i].normal[j] = raw_normals[n_elements[i]*3 + j];
				vertices[i+1].normal[j] = raw_normals[n_elements[i+1]*3 + j];
				vertices[i+2].normal[j] = raw_normals[n_elements[i+2]*3 + j];
			}
		} else {
			glm::vec3 normal = glm::cross(
				glm::vec3(raw_vertices[ib*3], raw_vertices[ib*3+1], raw_vertices[ib*3+2]) - glm::vec3(raw_vertices[ia*3], raw_vertices[ia*3+1], raw_vertices[ia*3+2]),
				glm::vec3(raw_vertices[ic*3], raw_vertices[ic*3+1], raw_vertices[ic*3+2]) - glm::vec3(raw_vertices[ia*3], raw_vertices[ia*3+1], raw_vertices[ia*3+2]));
			normal = glm::normalize(normal);

			for (int j = 0; j < 3; ++j) {
				vertices[i].normal[j] = j == 0 ? normal.x : (j == 1 ? normal.y : normal.z);
				vertices[i+1].normal[j] = j == 0 ? normal.x : (j == 1 ? normal.y : normal.z);
				vertices[i+2].normal[j] = j == 0 ? normal.x : (j == 1 ? normal.y : normal.z);
			}
		}

		/*if (t_elements.size() > 0) {
			for (int j = 0; j < 2; ++j) {
				vertices[i].texCoord[j] = raw_texCoords[t_elements[i]*2 + j];
				vertices[i+1].texCoord[j] = raw_texCoords[t_elements[i+1]*2 + j];
				vertices[i+2].texCoord[j] = raw_texCoords[t_elements[i+2]*2 + j];
			}
		} else {*/
			for (int j = 0; j < 2; ++j) {
				vertices[i].texCoord[j] = 0.0f;
				vertices[i+1].texCoord[j] = 0.0f;
				vertices[i+2].texCoord[j] = 0.0f;
			}
		//}

		// assign some colors
		for (int j = 0; j < 3; ++j) {
			vertices[i].color[j] = 1.0f;
			vertices[i+1].color[j] = 1.0f;
			vertices[i+2].color[j] = 1.0f;
		}
	}
}

/**
 * Load vertices data from a OBJ file.
 */
void OBJLoader::load(const char* filename, std::vector<glm::vec3>& points, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& texCoords) {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	QTextStream in(&file);
 
	std::vector<float> raw_vertices;
	std::vector<float> raw_normals;
	std::vector<float> raw_texCoords;
	std::vector<unsigned int> v_elements;
	std::vector<unsigned int> t_elements;
	std::vector<unsigned int> n_elements;

	while (!in.atEnd()) {
		QString line = in.readLine();
		if (line.startsWith("v ")) {
			int index1 = line.indexOf(QRegExp("[0-9\\.\\-]"), 2);
			int index2 = line.lastIndexOf(QRegExp("[0-9]"));
			QStringList values = line.mid(index1, index2 - index1 + 1).split(" ");
			raw_vertices.push_back(values[0].toFloat());
			raw_vertices.push_back(values[1].toFloat());
			raw_vertices.push_back(values[2].toFloat());
		} else if (line.startsWith("vn ")) {
			int index1 = line.indexOf(QRegExp("[0-9\\.\\-]"), 2);
			int index2 = line.lastIndexOf(QRegExp("[0-9]"));
			QStringList values = line.mid(index1, index2 - index1 + 1).split(" ");
			raw_normals.push_back(values[0].toFloat());
			raw_normals.push_back(values[1].toFloat());
			raw_normals.push_back(values[2].toFloat());
		} else if (line.startsWith("vt ")) {
			int index1 = line.indexOf(QRegExp("[0-9\\.\\-]"), 2);
			int index2 = line.lastIndexOf(QRegExp("[0-9]"));
			QStringList values = line.mid(index1, index2 - index1 + 1).split(" ");
			raw_texCoords.push_back(values[0].toFloat());
			raw_texCoords.push_back(values[1].toFloat());
		} else if (line.startsWith("f ")) {
			int index1 = line.indexOf(QRegExp("[0-9]"), 2);
			int index2 = line.lastIndexOf(QRegExp("[0-9]"));
			QStringList values = line.mid(index1, index2 - index1 + 1).split(" ");
			for (int i = 0; i < values.size() - 2; ++i) {
				unsigned int a,b,c;
				a = values[0].split("/")[0].toUInt() - 1;
				b = values[i+1].split("/")[0].toUInt() - 1;
				c = values[i+2].split("/")[0].toUInt() - 1;
				v_elements.push_back(a); v_elements.push_back(b); v_elements.push_back(c);

				if (values[0].split("/").size() >= 2 && values[0].split("/")[1].size() > 0) {
					a = values[0].split("/")[1].toUInt() - 1;
					b = values[i+1].split("/")[1].toUInt() - 1;
					c = values[i+2].split("/")[1].toUInt() - 1;
					t_elements.push_back(a); t_elements.push_back(b); t_elements.push_back(c);
				}

				if (values[0].split("/").size() >= 3 && values[0].split("/")[2].size() > 0) {
					a = values[0].split("/")[2].toUInt() - 1;
					b = values[i+1].split("/")[2].toUInt() - 1;
					c = values[i+2].split("/")[2].toUInt() - 1;
					n_elements.push_back(a); n_elements.push_back(b); n_elements.push_back(c);
				}
			}
		} else if (line.size() == 0) {
			/* ignore empty line */
		} else if (line.startsWith("#")) {
			/* ignore comment line */
		} else {
			/* ignore this line */
		}
	}

	points.resize(v_elements.size());
	normals.resize(v_elements.size());
	texCoords.resize(v_elements.size());
	for (int i = 0; i < v_elements.size(); i+=3) {
		unsigned int ia = v_elements[i];
		unsigned int ib = v_elements[i+1];
		unsigned int ic = v_elements[i+2];

		for (int j = 0; j < 3; ++j) {
			points[i][j] = raw_vertices[ia*3 + j];
			points[i+1][j] = raw_vertices[ib*3 + j];
			points[i+2][j] = raw_vertices[ic*3 + j];
			/*vertices[i].position[j] = raw_vertices[ia*3 + j];
			vertices[i+1].position[j] = raw_vertices[ib*3 + j];
			vertices[i+2].position[j] = raw_vertices[ic*3 + j];*/
		}

		if (n_elements.size() > 0) {
			for (int j = 0; j < 3; ++j) {
				normals[i][j] = raw_normals[n_elements[i]*3 + j];
				normals[i+1][j] = raw_normals[n_elements[i+1]*3 + j];
				normals[i+2][j] = raw_normals[n_elements[i+2]*3 + j];
				/*
				vertices[i].normal[j] = raw_normals[n_elements[i]*3 + j];
				vertices[i+1].normal[j] = raw_normals[n_elements[i+1]*3 + j];
				vertices[i+2].normal[j] = raw_normals[n_elements[i+2]*3 + j];*/
			}
		} else {
			glm::vec3 normal = glm::cross(
				glm::vec3(raw_vertices[ib*3], raw_vertices[ib*3+1], raw_vertices[ib*3+2]) - glm::vec3(raw_vertices[ia*3], raw_vertices[ia*3+1], raw_vertices[ia*3+2]),
				glm::vec3(raw_vertices[ic*3], raw_vertices[ic*3+1], raw_vertices[ic*3+2]) - glm::vec3(raw_vertices[ia*3], raw_vertices[ia*3+1], raw_vertices[ia*3+2]));
			normal = glm::normalize(normal);

			for (int j = 0; j < 3; ++j) {
				normals[i][j] = j == 0 ? normal.x : (j == 1 ? normal.y : normal.z);
				normals[i+1][j] = j == 0 ? normal.x : (j == 1 ? normal.y : normal.z);
				normals[i+2][j] = j == 0 ? normal.x : (j == 1 ? normal.y : normal.z);
				/*
				vertices[i].normal[j] = j == 0 ? normal.x : (j == 1 ? normal.y : normal.z);
				vertices[i+1].normal[j] = j == 0 ? normal.x : (j == 1 ? normal.y : normal.z);
				vertices[i+2].normal[j] = j == 0 ? normal.x : (j == 1 ? normal.y : normal.z);*/
			}
		}

		if (t_elements.size() > 0) {
			for (int j = 0; j < 2; ++j) {
				texCoords[i][j] = raw_texCoords[t_elements[i]*2 + j];
				texCoords[i+1][j] = raw_texCoords[t_elements[i+1]*2 + j];
				texCoords[i+2][j] = raw_texCoords[t_elements[i+2]*2 + j];
				/*
				vertices[i].texCoord[j] = raw_texCoords[t_elements[i]*2 + j];
				vertices[i+1].texCoord[j] = raw_texCoords[t_elements[i+1]*2 + j];
				vertices[i+2].texCoord[j] = raw_texCoords[t_elements[i+2]*2 + j];*/
			}
		} else {
			for (int j = 0; j < 3; ++j) {
				texCoords[i][j] = 0.0f;
				texCoords[i+1][j] = j == 0 ? 1.0f : 0.0f;
				texCoords[i+2][j] = j == 0 ? 0.0f : 1.0f;
				/*
				vertices[i].texCoord[j] = 0.0f;
				vertices[i+1].texCoord[j] = j == 0 ? 1.0f : 0.0f;
				vertices[i+2].texCoord[j] = j == 0 ? 0.0f : 1.0f;*/
			}
		}
	}
}
