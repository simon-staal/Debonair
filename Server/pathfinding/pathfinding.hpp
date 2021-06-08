#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <bits/stdc++.h>

std::vector<std::pair<int,int>> genPath(std::pair<int,int> pos, std::pair<int,int> dest, std::vector<std::pair<int,int>> obstacles);
std::pair<int,int> genIntermed(std::vector<std::pair<int,int>> &obstacles, std::pair<int, int> dest, std::pair<int, int> pos = {0,0});
std::vector<int> inTheWay(std::pair<int,int> x_range, std::pair<int,int> y_range, std::vector<std::pair<int,int>> &obstacles);
std::pair<int,int> avoid(std::pair<int,int> obstacle, std::pair<int,int> x_range, std::pair<int,int> y_range);