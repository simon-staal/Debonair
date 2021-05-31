import React from 'react';
import './App.css';
import TabNav from './components/TabNav';
import Home from './components/Home';
import Map from './components/Map';
import Status from './components/Status';
import {BrowserRouter as Router, Route, Switch} from 'react-router-dom';

function App(){
    return (
      <> 
        <Router>
        <TabNav />
          <Switch>
            <Route exact path="/" component={Home}/>
            <Route exact path="/map" component={Map}/>
            <Route exact path="/status" component={Status}/>
          </Switch>
        </Router>
      </>
  );
}

export default App;
