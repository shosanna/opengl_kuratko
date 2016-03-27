#ifndef TILED_HPP
#define TILED_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <iostream>
#include <vector>
#include <string>

class TileMap {
  public:
    std::vector<Tile> tiles;
    std::vector<int> map;
  Tile
};

std::vector<int> tile_ids(const std::string& filename) {
  // using pt = boost::property_tree;

  boost::property_tree::ptree tree;
  boost::property_tree::read_xml(filename, tree);

  std::cout << tree.get<std::string>("map.<xmlattr>.orientation") << std::endl;

  for (auto& attr : tree.get_child("map.layer.data")) {
    std::cout << attr.second.get<int>("<xmlattr>.gid") << std::endl;
    std::cout << attr.first << std::endl; // << " " << attr.second.data() << std::endl;
  }
  return {};
}



#endif /* TILED_HPP */
