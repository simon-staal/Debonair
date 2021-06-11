import React from 'react';
import './App.css';
import TabNav from './components/TabNav';
import Home from './components/Home';
import Map from './components/Map';
import About from './components/About';
import Status from './components/Status';
import {BrowserRouter as Router, Route, Switch} from 'react-router-dom';

function App(){
    return (
      <> 
        <Router>
        <TabNav />
          <Switch>
            <Route exact path="/" component={About}/>
            <Route exact path="/coords" component={Home}/>
            <Route exact path="/controller" component={Map}/>
            <Route exact path="/status" component={Status}/>
          </Switch>
        </Router>
      </>
  );
}

export default App;
