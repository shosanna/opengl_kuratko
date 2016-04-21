#ifndef TILED_HPP
#define TILED_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <iostream>
#include <vector>
#include <string>


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
};

TileMap load_tiles(const std::string& filename) {
	namespace pt = boost::property_tree;

	pt::ptree tree;
	pt::read_xml(filename, tree);

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

		}
	}

	std::vector<int> map;


	for (auto& tile : tree.get_child("map.layer.data")) {
		if (tile.first == "tile") {
			map.push_back(tile.second.get<int>("<xmlattr>.gid"));
		}
	}

	return {tiles, map};
}


#endif /* TILED_HPP */

