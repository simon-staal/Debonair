#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <bits/stdc++.h>

using namespace std;

pair<int,int> genIntermed(vector<pair<int,int>> &obstacles, pair<int, int> dest, pair<int, int> pos = {0,0});
vector<int> inTheWay(pair<int,int> x_range, pair<int,int> y_range, vector<pair<int,int>> &obstacles);
pair<int,int> avoid(pair<int,int> obstacle, pair<int,int> x_range, pair<int,int> y_range);
int clearance = 100;

int main() {


  pair<int, int> pos = {0,0};
  // cout << "Enter position" << endl;
  // cin >> pos.first >> pos.second;
  pair<int, int> dest;
  cout << "Enter dest" << endl;
  cin >> dest.first >> dest.second;
  vector<pair<int,int>> obstacles = {{200,200}, {-200,200}, {300,-200}}; //assumption that this has already been filled with pairs corresponding to object coordinates
  vector<pair<int,int>> path; //coordinates for the path the rover should take to avoid obstacles
  pair<int,int> intermed = genIntermed(obstacles, dest);
  path.push_back(intermed);
  while(intermed != dest){
    intermed = genIntermed(obstacles,dest,intermed);
    path.push_back(intermed);
  }
  cout << "New path" << endl;
  for(int i = 0; i < path.size(); i++){
    cout << path[i].first << "," << path[i].second << endl;
  }

}

pair<int,int> genIntermed(vector<pair<int,int>> &obstacles, pair<int, int> dest, pair<int, int> pos)
{
  pair<int,int> y_range;
  pair<int,int> x_range;

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

  vector<int> obsRef = inTheWay(x_range, y_range, obstacles);

  int closest = -1;
  int minDistance = INT_MAX;
  for(int i = 0; i < obsRef.size(); i++){
    int dist = sqrt(pow(obstacles[obsRef[i]].first - pos.first,2) + pow(obstacles[obsRef[i]].second - pos.second, 2));
    if(dist < minDistance){
      closest = i;
      minDistance = dist;
    }
  }
  pair<int,int> intermed = (closest == -1) ? dest : avoid(obstacles[closest], x_range, y_range);
  return intermed;
}

vector<int> inTheWay(pair<int,int> x_range, pair<int,int> y_range, vector<pair<int,int>> &obstacles)
{
  vector<int> obsInTheWay;
  float ang = atan2((y_range.second - y_range.first), (x_range.second - x_range.first));
  for(int i = 0; i < obstacles.size(); i++){
    if(obstacles[i].first > x_range.first-clearance && obstacles[i].first < x_range.second+clearance){
      int y_val = obstacles[i].first*tan(ang);
      int y_clear = (clearance / sin(90-ang))/2;
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
  // Change x value
  if(abs(ang) < M_PI/4 || abs(ang) > M_PI*3/4){
    newDest.second = obstacle.second;
    int x_pos = obstacle.second/tan(ang);
    if(obstacle.first < x_pos){
      newDest.first = obstacle.first + ((clearance/2)/sin(ang) + 10);
    }
    else{
      newDest.first = obstacle.first - ((clearance/2)/sin(ang) + 10);
    }
  }
  // Change y value
  else{
    newDest.first = obstacle.first;
    int y_pos = obstacle.first*tan(ang);
    if(obstacle.second < y_pos){
      newDest.second = obstacle.second + ((clearance/2)/sin(90 - ang) + 10);
    }
    else{
      newDest.second = obstacle.second - ((clearance/2)/sin(90 - ang) + 10);
    }
  }
  return newDest;
}