#ifndef TILED_HPP
#define TILED_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>


class Tile
{
public:
	int gid;
	int width;
	int height;
	std::string filename;
};

class TileMap
{
public:
	std::vector<Tile> tiles;
	std::vector<int> map;
	std::unordered_map<int, Tile> guids;

	std::size_t N() {
		// !!! MAPA MUSI BYT CTVERCOVA !!!
		int N = sqrt(map.size());
		assert(N*N == map.size());

		return N;
	}

	int& gid(std::size_t i, std::size_t j) {		
		return map[i * N() + j];
	}
};

TileMap load_tiles(const std::string& filename) {
	namespace pt = boost::property_tree;

	pt::ptree tree;
	pt::read_xml(filename, tree);
	std::unordered_map<int, Tile> guids;

	std::vector<Tile> tiles;

	for (auto& x: tree.get_child("map.tileset")) {
		if (x.first == "tile") {
			auto& tile = x.second;
			int gid = tile.get<int>("<xmlattr>.id");

			auto& image = tile.get_child("image");
			int width = image.get<int>("<xmlattr>.width");
			int height = image.get<int>("<xmlattr>.height");
			std::string source = image.get<std::string>("<xmlattr>.source");

			tiles.push_back({
				gid,
				width,
				height,
				source
			});

			guids[gid] = tiles.back();

		}
	}

	std::vector<int> map;


	for (auto& tile : tree.get_child("map.layer.data")) {
		if (tile.first == "tile") {
			map.push_back(tile.second.get<int>("<xmlattr>.gid"));
		}
	}

	return {tiles, map, guids};
}


#endif /* TILED_HPP */

