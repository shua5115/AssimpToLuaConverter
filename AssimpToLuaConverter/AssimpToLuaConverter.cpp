// AssimpToLuaConverter.cpp : Defines the entry point for the application.

#include "AssimpToLuaConverter.h"
#include "lua_converter.hpp"
#include <iostream>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <thread>

using namespace std;
using namespace Assimp;
using namespace AssimpToLua;

int main(int argc, char **argv)
{
	ofstream outfile;
	outfile.open("temp.lua");
	auto scene = aiImportFile("animation_with_skeleton.fbx", aiProcessPreset_TargetRealtime_Fast);
	outfile << setprecision(15);
	outfile << "return ";
	convert(outfile, scene);
	outfile << endl;
	outfile.flush();
	outfile.close();
	return 0;
}
