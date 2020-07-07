/* Project Group: Baiardi Martina, 
                  Giachin Marco, 
                  Lombardini Alessandro */

/* Import delle risorse */
const express = require('express');
const bodyParser = require('body-parser');
const cors = require("cors");
const fs = require('fs');
const XMLHttpRequest = require("xmlhttprequest").XMLHttpRequest;

/* Definizione del server */
const dumpsterService = express();
dumpsterService.use(bodyParser.urlencoded({
    limit: '50mb',
    extended: true
}));
dumpsterService.use(bodyParser.json({
    limit: '50mb',
    extended: true
}));
dumpsterService.use(cors());
dumpsterService.listen(3000);
console.log('\x1b[32m%s\x1b[0m', "*****************************************\n");
console.log('\x1b[31m%s\x1b[0m', "Server running on port 3000 \n");
console.log('\x1b[32m%s\x1b[0m', "*****************************************\n");


/* IP dell'ESP */
const espIP = "http://192.168.1.116:5000/api/"; 

/* Implementazione del servizio (da qui in poi)*/
let isAvailable = true;
let numDeposits = 0;                /* Numero di depositi attualmente nel cestino */
let weightOfDeposits = [];          /* Peso dei rifiuti attualmente nel cestino */
let depositsType = [];              /* Tipologia dei rifiuti attualmente nel cestino */

/*******************************************************************************************************/
/************************ Funzioni per inviare messaggi da parte del SERVICE ***************************/
/*******************************************************************************************************/

/*************************************/
/***** CALLS FROM SERVICE TO ESP *****/
/*************************************/

/* Send POST message: send message to the ESP to get weight of deposit */
function getCurrentWeight(){
    var httpReq = new XMLHttpRequest();
    httpReq.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            try{
                console.log('\x1b[32m%s\x1b[0m', "Asked to ESP the weight of deposit");
            } catch(err) {
                alert(err);
            }                        
        }
    }
    httpReq.open("POST", espIP + "currentWeight");
    httpReq.send();
}

/* Send POST message: send message to the ESP to force not available */
function setNotAvailable(){
    var httpReq = new XMLHttpRequest();
    console.log("Trying to POST set not available");
    httpReq.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            try{
                console.log('\x1b[32m%s\x1b[0m', "Closing the dumpster, now is not available");
                isAvailable = false;
            } catch(err) {
                alert(err);
            }                        
        }
    }
    httpReq.open("POST", espIP + "setNotAvailable");
    httpReq.send();
}

/* Send POST message: send message to the ESP to force available */
function setAvailable(){
    var httpReq = new XMLHttpRequest();
    console.log("Trying to POST set available");
    httpReq.onreadystatechange = function(){
        if(this.readyState == 4 && this.status == 200){
            try{
                console.log('\x1b[32m%s\x1b[0m', "Clearing the dumpster, now is available");
                isAvailable = true;
                weightOfDeposits = [];
                numDeposits = 0;
            } catch(err) {
                alert(err);
            }                        
        }
    }
    httpReq.open("POST", espIP + "setAvailable");
    httpReq.send();
}

/*******************************************************************************************************/
/************************ Funzioni per gestire i messaggi che il SERVICE riceve ************************/
/*******************************************************************************************************/

/*************************************/
/***** CALLS FROM ESP TO SERVICE *****/
/*************************************/

/* Handle POST message: used by the ESP to communicate the value of isAvailable */
dumpsterService.post(
    '/api/isAvailable',
    async function(req,res){
        try{
            isAvailable = req.body.Message;
            if(isAvailable == "true"){
                console.log('\x1b[32m%s\x1b[0m', "Message from ESP: dumpster is available \n");
            }else{
                console.log('\x1b[32m%s\x1b[0m', "Message from ESP: dumpster is not available \n");
            }
            res.sendStatus(200);
        }catch(error){
            res.sendStatus(404);
        }
    }
)

/* Handle POST message: used by the ESP to communicate the value of curren deposit weight */
dumpsterService.post(
    '/api/currentWeight',
    async function(req,res){
        try{
            let t = req.body;
            let w = parseInt(t.Message);
            console.log('\x1b[32m%s\x1b[0m', "Current deposit weight: "+ t.Message + "kg\n");
            weightOfDeposits.push(w);
            numDeposits++;
            if(fs.existsSync('story.json')) {
                fs.readFile('story.json', 'utf8', function (err, data) {
                    let history = JSON.parse(data);
                    let today = new Date();
                    let date = today.getDate()+'-'+(today.getMonth()+1)+'-'+today.getFullYear();  
                    var elemento = {
                        tipo : depositsType[depositsType.length - 1],
                        peso : w
                    }
                    if(history[date] == null){
                        history[date] = [elemento];
                    } else {
                        history[date].push(elemento);
                    }
                    history = JSON.stringify(history);
                    fs.writeFile('story.json', history, function(err, result) {
                        if(err) console.log('error', err);
                    });
                });
            } 
            res.sendStatus(200);
        }catch(error){
            console.log(error);
            res.sendStatus(404);
        }
    }
)

/*******************************************/
/***** CALLS FROM TELEPHONE TO SERVICE *****/
/*******************************************/

/* Handle POST message: used by the ESP to communicate the value of isAvailable */
dumpsterService.post(
    '/api/getToken',
    async function(req,res){
        try{
            let obj;
            console.log('\x1b[32m%s\x1b[0m', "Message from Telephone \n");
            console.log("Dumpster available: " + isAvailable);
            if(isAvailable==true){
                obj = {
                    "available" : "true",
                    "token" : new Date().getTime(),
                    "time" : 30
                }
            }else{
                obj = {"available" : "false"}
            }
            console.log(obj);
            res.send(obj);
        }catch(error){
            res.sendStatus(404);
        }
    }
)

/* Handle POST message: used by mobile app to communicate deposit has finished */
dumpsterService.post(
    '/api/finishDeposit',
    async function(req,res){
        try{
            for (x in req.body) {
                let t = JSON.parse(x);
                depositsType.push(t.type);
                console.log(depositsType);
            }
            getCurrentWeight();
            res.sendStatus(200);
        }catch{
            res.sendStatus(404);
        }
    } 
);

/*******************************************/
/***** CALLS FROM DASHBOARD TO SERVICE *****/
/*******************************************/

/* Handle POST message: used by the dashboard to force not available */
dumpsterService.post(
    '/api/setNotAvailable',
    async function(req,res){
        try{
            setNotAvailable();
            res.sendStatus(200);
        }catch(error){
            res.sendStatus(404);
        }
    }
)

/* Handle POST message: used by the dashboard to force available */
dumpsterService.post(
    '/api/setAvailable',
    async function(req,res){
        try{
            setAvailable();
            res.sendStatus(200);
        }catch(error){
            res.sendStatus(404);
        }
    }
)

/* Handle POST message: used by the dashboard to get che current new values of variables */
dumpsterService.post(
    '/api/update',
    async function(req,res){
        var vect_data = [];
        var vect_pesoPerGiorno = [];
        var vect_numeroDepositiPerGiorno = [];
        var vect_quanitaTipoRifiutiPerPeriodo = [0, 0, 0];
        var vect_quanitaTipoRifiutiPerPeriodoConPeso = [0, 0, 0];
        fs.readFile('story.json', 'utf8', function (err, data) {
            let history = JSON.parse(data);
            for(let i = 0; i < req.body.numeroGiorni; i++){
                let today = new Date();
                today.setDate(today.getDate() - i);
                date = today.getDate()+'-'+(today.getMonth()+1)+'-'+today.getFullYear();  
                vect_data[i] = date;
                if(history[date] == null){
                    vect_pesoPerGiorno[i] = 0;
                    vect_numeroDepositiPerGiorno[i] = 0;
                } else {
                    listaPesiInGiorno = [];
                    history[date].forEach(element => listaPesiInGiorno.push(element["peso"]));
                    history[date].forEach(element => {
                        if(element["tipo"] == "A"){
                            vect_quanitaTipoRifiutiPerPeriodoConPeso[0]+=element["peso"];
                            vect_quanitaTipoRifiutiPerPeriodo[0]++;
                        } else if(element["tipo"] == "B"){
                            vect_quanitaTipoRifiutiPerPeriodoConPeso[1]+=element["peso"];
                            vect_quanitaTipoRifiutiPerPeriodo[1]++;
                        } else if(element["tipo"] == "C"){
                            vect_quanitaTipoRifiutiPerPeriodoConPeso[2]+=element["peso"];
                            vect_quanitaTipoRifiutiPerPeriodo[2]++;
                        }
                    });
                    vect_pesoPerGiorno[i] = listaPesiInGiorno.reduce((a,b) => { return a + b }, 0);
                    vect_numeroDepositiPerGiorno[i] = history[date].length;
                }
            }
            try{
                vect_pesoPerGiorno.reverse();
                vect_numeroDepositiPerGiorno.reverse();
                vect_data.reverse();
                res.send({
                    "isAvailable" : isAvailable,
                    "numDeposits" : numDeposits,
                    "depositsType" : depositsType,
                    "weightOfDeposits" : weightOfDeposits,
                    "date" : vect_data,
                    "pesoPerGiorno" : vect_pesoPerGiorno,
                    "numeroDepositiPerGiorno": vect_numeroDepositiPerGiorno,
                    "riufitiPerPeriodo" : vect_quanitaTipoRifiutiPerPeriodo,
                    "tipoDiRifiutiPerPeriodoConPeso": vect_quanitaTipoRifiutiPerPeriodoConPeso
                });
            }catch(error){
                res.sendStatus(404);
            }
        }); 
        
    }
)


/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/

