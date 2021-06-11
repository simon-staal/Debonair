#include "functionexample.h"
#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
#include <string>

std::pair<int,int> genIntermed(std::vector<std::pair<int,int>> &obstacles, std::pair<int, int> dest, std::pair<int, int> pos);
std::vector<int> inTheWay(std::pair<int,int> x_range, std::pair<int,int> y_range, std::vector<std::pair<int,int>> &obstacles, int clearance);
std::pair<int,int> avoid(std::pair<int,int> obstacle, std::pair<int,int> x_range, std::pair<int,int> y_range, int clearance);

std::string pathfinding::genPath(std::string pos, std::string dest, std::string obs) {
  // Processing inputs
  int comma = pos.find(',');
  std::string posX = pos.substr(0,comma); // extract x position
  std::string posY = pos.substr(comma+1); // extract y position
  std::pair<int,int> position = {std::stoi(posX), std::stoi(posY)}; // generates pair position
  comma = dest.find(',');
  std::string destX = dest.substr(0,comma);
  std::string destY = dest.substr(comma+1);
  std::pair<int,int> destination = {std::stoi(destX), std::stoi(destY)}; // generates pair destination

  std::vector<std::pair<int,int>> obstacles;
  bool newPair = 0;
  bool second = 0;
  std::string x, y;
  for(int i = 0; i < obs.size(); i++) { // Extracts obstacles
    if(obs[i] == '{') newPair = 1;
    else if(obs[i] == ',') second = 1;
    else if (obs[i] == '}') {
      obstacles.push_back({std::stoi(x),std::stoi(y)});
      newPair = 0;
      second = 0;
      x.clear();
      y.clear();
    }
    else if(newPair && !second) x.push_back(obs[i]);
    else if(newPair && second) y.push_back(obs[i]);
  }

  // Generate path
  std::vector<std::pair<int,int>> path; //coordinates for the path the rover should take to avoid obstacles
  std::pair<int,int> intermed = genIntermed(obstacles, destination, position);
  path.push_back(intermed);
  while(intermed != destination){
    intermed = genIntermed(obstacles,destination,intermed);
    path.push_back(intermed);
  }

  // Processing output
  std::string processed_path;
  for (int i = 0; i < path.size(); i++) {
    processed_path += ("Destination "+std::to_string(i+1)+": "+std::to_string(path[i].first)+","+std::to_string(path[i].second)+"\n");
  }
  return ("Generated path: "+processed_path);
}

Napi::String pathfinding::GenPathWrapped(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  if (info.Length() < 3) {
    Napi::Error::New(env, "Insufficient arguments (expected 3)").ThrowAsJavaScriptException();
  }

  std::string pos = (std::string) info[0].ToString();
  std::string dest = (std::string) info[1].ToString();
  std::string obs = (std::string) info[2].ToString();

  std::string path = pathfinding::genPath(pos, dest, obs);

  return Napi::String::New(env, path);
}

Napi::Object pathfinding::Init(Napi::Env env, Napi::Object exports)
{
  exports.Set("genPath", Napi::Function::New(env, pathfinding::GenPathWrapped));

  return exports;
}

std::pair<int,int> genIntermed(std::vector<std::pair<int,int>> &obstacles, std::pair<int, int> dest, std::pair<int, int> pos)
{
  int clearance = 100;
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

  std::vector<int> obsRef = inTheWay(x_range, y_range, obstacles, clearance);

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
  std::pair<int,int> intermed = (closest == -1) ? dest : avoid(obstacles[closest], x_range, y_range, clearance);
  return intermed;
}

std::vector<int> inTheWay(std::pair<int,int> x_range, std::pair<int,int> y_range, std::vector<std::pair<int,int>> &obstacles, int clearance)
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

std::pair<int,int> avoid(std::pair<int,int> obstacle, std::pair<int,int> x_range, std::pair<int,int> y_range, int clearance)
{
  float ang = atan2((y_range.second - y_range.first), (x_range.second - x_range.first));
  std::pair<int,int> newDest;
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


