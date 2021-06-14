const express = require('express');

/* irrelevant
const dotenv = require('dotenv');
dotenv.config();

function getNLUInstance() {
    let api_key = process.env.API_KEY;
    let api_url = process.env.API_URL;

    const NaturalLanguageUnderstandingV1 = require('ibm-watson/natural-language-understanding/v1');
    const { IamAuthenticator } = require('ibm-watson/auth');

    const naturalLanguageUnderstanding = new NaturalLanguageUnderstandingV1({
        version: '2020-08-01',
        authenticator: new IamAuthenticator({
            apikey: api_key,
        }),
        serviceUrl: api_url,
    });
    return naturalLanguageUnderstanding;
}
*/

const app = new express();

app.use(express.static('client'))

const cors_app = require('cors');
app.use(cors_app());

// Routing
app.get("/",(req,res)=>{
    res.render('index.html');
  });

// const NLU = getNLUInstance(); irrelevant

app.get("/url/emotion", (req,res) => { // This gets called by the get request from React
    /*
    let analyzeParams = {
        'url': req.query.url,
        'features': {
            'entities': {
            'emotion': true,
            'limit': 2, // Maybe change this
            },
            'keywords': {
            'emotion': true,
            'limit': 2, // Maybe change this
            },
        },
    };
    */
    NLU.analyze(analyzeParams) // returns Promise
        .then(analysisResults => {
            return res.send(JSON.stringify(analysisResults, null, 2)); // Sends response
        })
        .catch(err => {
            console.log('error:', err);
            return res.send(`error: ${err}`);
        })
});

app.get("/url/sentiment", (req,res) => {
    let analyzeParams = {
        'url': req.query.url,
        'features': {
            'entities': {
            'sentiment': true,
            'limit': 2, // Maybe change this
            },
            'keywords': {
            'sentiment': true,
            'limit': 2, // Maybe change this
            },
        },
    };
    NLU.analyze(analyzeParams)
        .then(analysisResults => {
            return res.send(JSON.stringify(analysisResults, null, 2));
        })
        .catch(err => {
            console.log('error:', err);
            return res.send(`error: ${err}`);
        })
});

app.get("/text/emotion", (req,res) => {
    let analyzeParams = {
        'test': req.query.text,
        'features': {
            'entities': {
            'emotion': true,
            'limit': 2, // Maybe change this
            },
            'keywords': {
            'emotion': true,
            'limit': 2, // Maybe change this
            },
        },
    };
    NLU.analyze(analyzeParams)
        .then(analysisResults => {
            return res.send(JSON.stringify(analysisResults, null, 2));
        })
        .catch(err => {
            console.log('error:', err);
            return res.send(`error: ${err}`);
        })
});

app.get("/text/sentiment", (req,res) => {
    console.log(req.query.text);
    let analyzeParams = {
        'test': req.query.text,
        'features': {
            'entities': {
            'sentiment': true,
            'limit': 2, // Maybe change this
            },
            'keywords': {
            'sentiment': true,
            'limit': 2, // Maybe change this
            },
        },
    };
    NLU.analyze(analyzeParams)
        .then(analysisResults => {
            return res.send(JSON.stringify(analysisResults, null, 2));
        })
        .catch(err => {
            console.log('error:', err);
            return res.send(`error: ${err}`);
        })
});

// Sets up server on port 8080
let server = app.listen(8080, () => {
    console.log('Listening', server.address().port)
})

