Matteo buonasera,
allora il codice che c'è ora su github è quello che ho io sul pc in 
questo momento. Per creare il simulatore basta fare un JSON come quello 
di esempio nella tesi che ti ricopio qui.
```JSON
{
     "sim_step" : 1,
     "sim_len": 7736400,
     "vref_bus" : 3.3,
     "soc_init" : 1.0,
     "selfdisch_factor" : 0.0,
     "battery" : "circuit",
     "sensors" : [
         {
             "power"  : true,
             "name"   : "air_quality_sensor",
             "voltage": 3.3,
             "reg" : 50,
             "states" : [
                 {
                     "name"    : "ON_READ",
                     "current" : "48.2",
                     "time_on" : "30"
                 },
                 {
                     "name"    : "ON_WRITE",
                     "current" : "49.2",
                     "time_on" : "30"
                 },
                 {
                     "name"    : "IDLE",
                     "current" : "0.002"
                 }
             ]
         }]
}
```

* per ogni sensore vanno date le informazioni principali e gli stati di 
power (Unica eccezione è IDLE che non deve avere "time_on" come 
parametro).

* Per la batteria puoi scrivere 'circuit' o 'peukert' nel campo 
'battery' per selezionare quella che vuoi.

* Fatto ciò avvii il file python codgen.py con argomento -f 
"nome_file".json che trovi nella cartella codgen

* da li prendi le cartelle 'inc' e 'src' e il file make e puoi buildare 
il simulatore. (In automatico nel file config.h trovi il BASE ADDRESS di 
ogni componente e tutti i vari parametri)

* Per il programma Helloworld di GvSoC va solo fatta attenzione agli 
indirizzi che devono essere multipli di 4 altrimenti va in errore.

* Il simulatore che crei da python ha come unità di tempo dei componenti 
sc_core::SC_SEC quindi andrà un pò lenta, se vuoi nel ramo GvSoC ç'è il 
simulatore più veloce con sc_core::SC_MS

In qualsiasi caso da settimana prossima se vuoi possiamo fare una 
chiamata per spiegarmi meglio.

Grazie mille e buona serata.

N.B. Va esposta la variabile GAP_HOME in .bashrc che contiene il 
percorso dell' SDK di gvsoc, inoltre prima di avviare il simulatore va 
impostata la board nella shell dalla quale si avvia il simulatore


{ BasedOnStyle: LLVM, IndentWidth: 4, ColumnLimit: 0}