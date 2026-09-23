#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <err.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

struct Printer
{
	int indent = 0;

	void print_indent()
	{
		for (auto i = 0; i < indent; i++)
			printf("\t");
	}

	void print(const char *fmt, ...)
	{
		va_list ap;

		print_indent();
		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);
	}

	void println(const char *fmt, ...)
	{
		va_list ap;

		print_indent();
		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);
		printf("\n");
	}
};

void usage()
{
	printf("usage: <model_file>\n");
	exit(2);
}

void dump_scene(const aiScene *scene)
{
	Printer p;

	p.println("Number of Meshes: %u", scene->mNumMeshes);
	p.println("Number of Materials: %u", scene->mNumMaterials);
	p.println("");
	for (auto i = 0u; i < scene->mNumMeshes; i++)
	{
		auto mesh = scene->mMeshes[i];

		p.println("Mesh %u {", i + 1);
		p.indent++;

		p.println("Primitive Types: %#x", mesh->mPrimitiveTypes);
		p.println("Material Index: %d", mesh->mMaterialIndex);
		p.println("Number of Vertices: %u", mesh->mNumVertices);
		p.println("Number of Faces: %u", mesh->mNumFaces);
		p.println("Number of Bones: %u", mesh->mNumBones);
		p.println("Number of Animation Meshes: %u", mesh->mAnimMeshes);

		for (auto j = 0u; j < mesh->mNumFaces; j++)
		{
			p.print("Faces %u: { ", j);
			auto &face = mesh->mFaces[j];
			for (auto k = 0u; k < face.mNumIndices; k++)
				printf("%d ", face.mIndices[k]);
			printf("}\n");
		}

		p.println("Vertices: {");
		p.indent++;
		for (auto j = 0u; j < mesh->mNumVertices; j++)
		{
			auto vert = mesh->mVertices[j];
			p.println("%.3f %.3f %.3f", vert.x, vert.y, vert.z);
		}
		p.indent--;
		p.println("}");

		p.println("Normals: {");
		p.indent++;
		for (auto j = 0u; j < mesh->mNumVertices; j++)
		{
			auto norm = mesh->mNormals[j];
			p.println("%.3f %.3f %.3f", norm.x, norm.y, norm.z);
		}
		p.indent--;
		p.println("}");

		p.println("Tangents: {");
		p.indent++;
		for (auto j = 0u; j < mesh->mNumVertices; j++)
		{
			auto tangent = mesh->mTangents[j];
			p.println("%.3f %.3f %.3f", tangent.x, tangent.y, tangent.z);
		}
		p.indent--;
		p.println("}");

		p.println("Bitangents: {");
		p.indent++;
		for (auto j = 0u; j < mesh->mNumVertices; j++)
		{
			auto bitangent = mesh->mBitangents[j];
			p.println("%.3f %.3f %.3f", bitangent.x, bitangent.y, bitangent.z);
		}
		p.indent--;
		p.println("}");

		for (auto j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++)
		{
			if (!mesh->mTextureCoords[j])
				continue;

			p.println("Texcoords %u: {", j);
			p.indent++;
			for (auto k = 0u; k < mesh->mNumVertices; k++)
			{
				auto texcoord = mesh->mTextureCoords[j][k];
				p.println("%.3f %.3f %.3f", texcoord.x, texcoord.y, texcoord.z);
			}
			p.indent--;
			p.println("}");
		}

		for (auto j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++)
		{
			if (!mesh->mColors[j])
				continue;

			p.println("Colors %u: {", j);
			p.indent++;
			for (auto k = 0u; k < mesh->mNumVertices; k++)
			{
				auto color = mesh->mColors[j][k];
				p.println("%.3f %.3f %.3f %.3f", color.r, color.g, color.b, color.a);
			}
			p.indent--;
			p.println("}");
		}

		p.indent--;
		p.println("}\n");
	}

	p.println("Materials {");
	p.indent++;
	for (auto i = 0u; i < scene->mNumMaterials; i++)
	{
		aiTextureType types[] = { aiTextureType_DIFFUSE };
		const char *typestr[] = { "Diffuse" };

		auto material = scene->mMaterials[i];
		for (auto j = 0u; j < nelem(types); j++)
		{
			p.println("%s {", typestr[j]);
			p.indent++;
			for (auto k = 0u; k < material->GetTextureCount(types[j]); k++)
			{
				aiString texture_path;
				material->GetTexture(types[j], k, &texture_path);
				p.println("Texture Path \"%s\"", texture_path.C_Str());
			}
			p.indent--;
			p.println("}");
		}
	}
	p.indent--;
	p.println("}");
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		usage();

	auto flags = aiProcess_CalcTangentSpace |
				 aiProcess_GenSmoothNormals |
				 aiProcess_Triangulate |
				 aiProcess_JoinIdenticalVertices |
				 aiProcess_SortByPType;
	Assimp::Importer importer;
	auto scene = importer.ReadFile(argv[1], flags);
	if (!scene)
		errx(1, "Failed to load model");

	dump_scene(scene);
	return 0;
}
