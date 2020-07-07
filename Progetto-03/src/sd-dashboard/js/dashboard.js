/* Project Group: Baiardi Martina, 
                  Giachin Marco, 
                  Lombardini Alessandro */

/* IP del server | La porta è la 3000 */
const serverInterface = "http://127.0.0.1:3000/api/"; 

let precL = -1;
let index = 0;
let n_days = 0;
let data = [];
let day = 0;

$(document).ready(function() {
    update();
    setInterval(() => { update() }, 100);  /* Imposta intervallo di aggiornamento del sito */

    /* Gestione del pulsante per variare lo stato del Dumpster */
    $("#cambiaStato").click(() => {
        if($("#cambiaStato").text() == "Disabilita servizio"){
            $.ajax({
                url: serverInterface + "setNotAvailable",
                type: "POST"
            });
        } else {
            $.ajax({
                url: serverInterface + "setAvailable",
                type: "POST"
            });
        }
    })

    /* Imposto i possibili valori di selezione del SELECT */
    for(let i = 1; i < 31; i++){
        if(i != 30){
            $("select").append(`<option value=${i}>${i}</option>`);
        } else {
            $("select").append(`<option value=${i} selected>${i}</option>`);
        }
    }
    $("select").val("7");
})

/* Richiede le informazioni aggiornate al server e le mostra all'utente */
function update(){
    const numeroDiGiorni = $("#numeroGiorni").val();
    $.ajax({
        url: serverInterface + "update",
        data: { 'numeroGiorni' : numeroDiGiorni },
        type: "POST",
        success: function(response, status, http) {
            if (response) {
                if(response.isAvailable==true){
                    $("#available").text('ATTIVO');
                    $("#cambiaStato").text('Disabilita servizio');
                }else{
                    $("#available").text('NON ATTIVO');
                    $("#cambiaStato").text('Abilita servizio');
                }
                $("#numeroDepositi").text(response.numDeposits);
                $("#quantitaCorrente").text(response.weightOfDeposits.reduce((a,b) => { return a + b }, 0));
                printGraphic(response.date, response.numeroDepositiPerGiorno, response.pesoPerGiorno, response.riufitiPerPeriodo, response.tipoDiRifiutiPerPeriodoConPeso);
            }
        }
    });
}

/* Pubblica i grafici di andamento */
function printGraphic(labels, numeroDepositiPerGiorno, pesoPerGiorno, riufitiPerPeriodo, tipoDiRifiutiPerPeriodoConPeso){
    /* Grafico relativo al numero di depositi */
    var ctx = document.getElementById('numeroDepositiPerGiorno').getContext('2d');
    var myChart = new Chart(ctx, {
        type: 'line',	
        data: {
            xLabels: labels,
            datasets: [{
                    data: numeroDepositiPerGiorno
            }]
        },
        options: {
            maintainAspectRatio: true,
            animation: false,
            legend: {
                display: false
            },
            scales: {
                yAxes: [{
                    ticks: {
                        display: true,
                        beginAtZero: true,                           
                    },
                    scaleLabel: {
                        display: true,
                        labelString: "Numero di depositi"
                    }
                }],
                xAxes: [{
                    ticks: {
                        display : true
                    },
                    scaleLabel: {
                        display: true,
                        labelString: 'Data'
                    },
                }]
            }, 
            tooltips: {
                enabled: false
            }
        }
    });
    /* Grafico relativo alla quantità depositata */
    var ctx = document.getElementById('pesoDepositatoPerGiorno').getContext('2d');
    var myChart = new Chart(ctx, {
        type: 'line',	
        data: {
            xLabels: labels,
            datasets: [{
                    data: pesoPerGiorno
            }]
        },
        options: {
            maintainAspectRatio: true,
            animation: false,
            legend: {
                display: false
            },
            scales: {
                yAxes: [{
                    ticks: {
                        display: true,
                        beginAtZero: true,                           
                    },
                    scaleLabel: {
                        display: true,
                        labelString: "Quantità depositata"
                    }
                }],
                xAxes: [{
                    ticks: {
                        display : true
                    },
                    scaleLabel: {
                        display: true,
                        labelString: 'Data'
                    },
                }]
            }, 
            tooltips: {
                enabled: false
            }
        }
    });
    /* Grafico relativo alla quantità depositata */
    var ctx = document.getElementById('tipoDiRifiutiPerPeriodo').getContext('2d');
    var myChart = new Chart(ctx, {
        type: 'bar',	
        data: {
            labels: [
                "Tipo A",
                "Tipo B",
                "Tipo C"
            ],
            datasets: [
                {
                    data: riufitiPerPeriodo,
                    backgroundColor: [
                        "grey",
                        "grey",
                        "grey"
                    ]
                }]
        },
        options: {
            scales: {
                yAxes: [{
                    ticks: {
                        beginAtZero: true
                    }
                }]
            },
            animation: false, 
            tooltips: {
                enabled: false,
            }, 
            legend: {
                display: false
            },
            scales: {
                yAxes: [{
                    ticks: {
                        display: true,
                        beginAtZero: true,                           
                    },
                    scaleLabel: {
                        display: true,
                        labelString: "Numero di depositi"
                    }
                }],
                xAxes: [{
                    ticks: {
                        display: true,
                        beginAtZero: true,                           
                    },
                    scaleLabel: {
                        display: true,
                        labelString: "Tipo di rifiuto"
                    }
                }],
            }
        }
    });/* Grafico relativo alla quantità depositata */
    var ctx = document.getElementById('tipoDiRifiutiPerPeriodoConPeso').getContext('2d');
    var myChart = new Chart(ctx, {
        type: 'bar',	
        data: {
            labels: [
                "Tipo A",
                "Tipo B",
                "Tipo C"
            ],
            datasets: [
                {
                    data: tipoDiRifiutiPerPeriodoConPeso,
                    backgroundColor: [
                        "grey",
                        "grey",
                        "grey"
                    ]
                }]
        },
        options: {
            scales: {
                yAxes: [{
                    ticks: {
                        beginAtZero: true
                    }
                }]
            },
            animation: false, 
            tooltips: {
                enabled: false,
            }, 
            legend: {
                display: false
            },
            scales: {
                yAxes: [{
                    ticks: {
                        display: true,
                        beginAtZero: true,                           
                    },
                    scaleLabel: {
                        display: true,
                        labelString: "Quantità depositata"
                    }
                }],
                xAxes: [{
                    ticks: {
                        display: true,
                        beginAtZero: true,                           
                    },
                    scaleLabel: {
                        display: true,
                        labelString: "Tipo di rifiuto"
                    }
                }],
            }
        }
    });
}

