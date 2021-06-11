#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <bits/stdc++.h>
#include "pathfinding.hpp"

int clearance = 100;
/*
int main() {


  std::pair<int, int> pos = {0,0};
  // cout << "Enter position" << endl;
  // cin >> pos.first >> pos.second;
  std::pair<int, int> dest;
  std::cout << "Enter dest" << std::endl;
  std::cin >> dest.first >> dest.second;
  std::vector<std::pair<int,int>> obstacles = {{1000,1450}, {2230,3100}, {2700,3600}, {3100,4450}, {3320,4550}}; //assumption that this has already been filled with pairs corresponding to object coordinates
  std::vector<std::pair<int,int>> path = genPath(pos, dest, obstacles); //coordinates for the path the rover should take to avoid obstacles
  std::cout << "New path" << std::endl;
  for(int i = 0; i < path.size(); i++){
    std::cout << path[i].first << "," << path[i].second << std::endl;
  }

}
*/
std::vector<std::pair<int,int>> genPath(std::pair<int,int> pos, std::pair<int,int> dest, std::vector<std::pair<int,int>> obstacles)
{
  std::vector<std::pair<int,int>> path; //coordinates for the path the rover should take to avoid obstacles
  std::pair<int,int> intermed = genIntermed(obstacles, dest);
  path.push_back(intermed);
  while(intermed != dest){
    intermed = genIntermed(obstacles,dest,intermed);
    path.push_back(intermed);
  }
return path;
}

std::pair<int,int> genIntermed(std::vector<std::pair<int,int>> &obstacles, std::pair<int, int> dest, std::pair<int, int> pos)
{
  std::pair<int,int> y_range;
  std::pair<int,int> x_range;

  if(pos.first < dest.first){
    x_range.first = pos.first;
    x_range.second = dest.first;
  }
  else{
    x_range.first = dest.first;
    x_range.second = pos.first;
  }
  if(pos.second < dest.second){
    y_range.first = pos.second;
    y_range.second = dest.second;
  }
  else{
    y_range.first = dest.second;
    y_range.second = pos.second;
  }

  std::vector<int> obsRef = inTheWay(x_range, y_range, obstacles);

  int closest = -1;
  int minDistance = INT_MAX;
  for(int i = 0; i < obsRef.size(); i++){
    int dist = sqrt(pow(obstacles[obsRef[i]].first - pos.first,2) + pow(obstacles[obsRef[i]].second - pos.second, 2));
    if(dist < minDistance){
      closest = obsRef[i];
      //cout << closest << endl;
      minDistance = dist;

    }
  }
  std::pair<int,int> intermed = (closest == -1) ? dest : avoid(obstacles[closest], x_range, y_range);
  return intermed;
}

std::vector<int> inTheWay(std::pair<int,int> x_range, std::pair<int,int> y_range, std::vector<std::pair<int,int>> &obstacles)
{
  std::vector<int> obsInTheWay;
  float ang = atan2((y_range.second - y_range.first), (x_range.second - x_range.first));
  //cout << ang << endl;
  for(int i = 0; i < obstacles.size(); i++){
    int y_val;
    if(obstacles[i].first > x_range.first-clearance && obstacles[i].first < x_range.second+clearance){
      if(x_range.second > 0){ //differentiates between destination on right or left of origin (assumption rover starts at origin)
        if(y_range.second > 0){ // differentiates between destination above or below origin (same assumption ^^)
          y_val = y_range.first + (obstacles[i].first - x_range.first)*tan(ang); //top right quadrant
        }
        else{ //bottom right quadrant
          y_val = y_range.second - (obstacles[i].first - x_range.first)*tan(ang);
        }
      }
      else{ // addresses destinations on left of origin
        if(y_range.second > 0){ //top left quadrant
          y_val = y_range.first + (abs(obstacles[i].first - x_range.second))*tan(ang);
        }
        else{ //bottom left quadrant
          y_val = y_range.second - (abs(obstacles[i].first - x_range.second))*tan(ang);
        }
      }
      //cout << y_val << "," << i << endl;
      int y_clear = (clearance/cos(ang));
      //cout << y_clear << endl;
      if(obstacles[i].second > y_val-y_clear && obstacles[i].second < y_val+y_clear){
        obsInTheWay.push_back(i);
      }
    }
  }
  return obsInTheWay;
}

pair<int,int> avoid(pair<int,int> obstacle, pair<int,int> x_range, pair<int,int> y_range)
{
  float ang = atan2((y_range.second - y_range.first), (x_range.second - x_range.first));
  pair<int,int> newDest;
  if(y_range.second > 0){ //differentiates between destinations above and below origin
    if(x_range.second > 0){ // differentiates between destinations left or right of origin (top right quadrant here)
      int x_pos = x_range.first + (obstacle.second - y_range.first)/tan(ang);
      if(obstacle.first < x_pos){ //if obstacle is left of path
        newDest.first = x_pos + clearance*(ang/(M_PI/2));
        if(newDest.first - obstacle.first > clearance){ // if distance in x coordinates is more than 100 just adjust y coordinate by 100
          newDest.second = obstacle.second - clearance;
        }
        else{ // if x clearance is less than 100, ensure y distance is enough so that closest rover travels is 100 using pythagoras
          int y = sqrt(pow(clearance,2) - pow((x_pos - obstacle.first),2));
          newDest.second = obstacle.second - y;
        }
      }
      else{ //if obstacle is right of path
        newDest.first = x_pos - clearance*(ang/(M_PI/2));
        if(obstacle.first - newDest.first > clearance){
          newDest.second = obstacle.second + clearance;
        }
        else{
          int y = sqrt(pow(clearance,2) - pow((obstacle.first - x_pos),2));
          newDest.second = obstacle.second + y;
        }
      }
    }
      else{ //top left quadrant here
        int x_pos = x_range.second - (obstacle.second - y_range.first)/tan(ang);
        if(obstacle.first < x_pos){ //obstacle on left of path
          newDest.first = x_pos + clearance*(ang/(M_PI/2));
          if(newDest.first - obstacle.first > clearance){
          newDest.second = obstacle.second + clearance;
          }
          else{
            int y = sqrt(pow(clearance,2) - pow((x_pos - obstacle.first),2));
            newDest.second = obstacle.second + y;
          }
        }
        else{ //obstacle on right of path
          newDest.first = x_pos - clearance*(ang/(M_PI/2));
          if(obstacle.first - newDest.first > clearance){
            newDest.second = obstacle.second - clearance;
          }
          else{
            int y = sqrt(pow(clearance,2) - pow((obstacle.first - x_pos),2));
            newDest.second = obstacle.second - y;
          }
        }
      }
    }
    else{ // destinations below origin addressed here
      if(x_range.second > 0){ // bottom right quadrant here
        int x_pos = x_range.first + abs((obstacle.second - y_range.second))/tan(ang);
        if(obstacle.first < x_pos){ //obstacle on right of path
          newDest.first = x_pos + clearance*(ang/(M_PI/2));
          if(newDest.first - obstacle.first > clearance){
            newDest.second = obstacle.second + clearance;
          }
          else{
            int y = sqrt(pow(clearance,2) - pow((x_pos - obstacle.first),2));
            newDest.second = obstacle.second + y;
          }
        }
        else{ //obstacle on left of path
          newDest.first = x_pos - clearance*(ang/(M_PI/2));
          if(obstacle.first - newDest.first > clearance){
            newDest.second = obstacle.second - clearance;
          }
          else{
            int y = sqrt(pow(clearance,2) - pow((obstacle.first - x_pos),2));
            newDest.second = obstacle.second - y;
          }
        }
      }
      else{ //bottom left quadrant here
        int x_pos = x_range.second - abs((obstacle.second - y_range.second))/tan(ang);
          if(obstacle.first < x_pos){ //obstacle on right of path
            newDest.first = x_pos + clearance*(ang/(M_PI/2));
            if(newDest.first - obstacle.first > clearance){
              newDest.second = obstacle.second - clearance;
            }
            else{
              int y = sqrt(pow(clearance,2) - pow((x_pos - obstacle.first),2));
              newDest.second = obstacle.second - y;
            }
          }
          else{ //obstacle on right of path
            newDest.first = x_pos - clearance*(ang/(M_PI/2));
            if(obstacle.first - newDest.first > clearance){
              newDest.second = obstacle.second + clearance;
            }
            else{
              int y = sqrt(pow(clearance,2) - pow((obstacle.first - x_pos),2));
              newDest.second = obstacle.second + y;
            }
          }
        }
    }
    return newDest;
}