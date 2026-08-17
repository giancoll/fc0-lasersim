Il file dedx.cc fitta la funzione landau, va inserito il file e inidcata se calcolare una media per riga o colonna di pad. Io lo compilo come "root dedx.cc"

Per calcolare la risoluzione, nel file tlsv2.cc è ancora presete a funzione con la somma delle due gaussiane. Vanno specificati 3 parametri per far funzionaer il fit, i valori sono scritti nel codice. Io lo compilo come "root tlsv2.cc"

Di questo file ho scritto una versione c++ pura, che con la versione in funzione tlsv2h , permette di calcolare e salvare i fit di più file cotemporaneamente. 
Dal file main2.cc nella stringa va specificato il nome del file .root, se la traccia è verticale o orrizontale e il paino in cui giace (ad esempio per le tracce a diverse profondità va impostato 1). Il codice stampa a video i risultati. Dal file .h della funzioni tlsv2 andrebbero modificati i path dei file, questa fuzione restituisce il valore del fit a singola gaussiana.
Per compilare: c++ -o exe main2.cc `root-config --cflags --libs`
./exe
E' possibile aggiungere anche il filtraggio prima dei delta togliendo dai commenti il primo ciclo for, la funzione delta.h è impostata a 5mm e 45ns  dei due raggi. 


Dal file delta.cc, ci sono i valori iniziali da impstare, dentro al ciclo alla riga 80 si possono modificare i valori
Io lo compilo come "root delta.cc"


Aggiornamento FC0/HAT per muoni e laser
---------------------------------------

Il file fc0_elena_analysis.cc e' una versione compatibile con la geometria HAT
attuale. Legge il tree "anode" dei file ROOT prodotti con output_level >= 2 e
usa anche il branch "anode_eram" per costruire gli indici globali di pad:

  globalPadZ = ERAM_col * 36 + anode_padZ
  globalPadY = ERAM_row * 32 + anode_padY

Questo evita il problema della vecchia geometria 256 x 76, dove i pad erano
centrati circa in [-128,127] e [-38,37]. La geometria FC0/HAT attuale e':

  144 colonne globali in Z = 4 ERAM x 36 pad
   64 righe globali in Y  = 2 ERAM x 32 pad

Compilazione:

  c++ -std=c++17 -O2 -Wall -Wextra -o Analisi_Elena/fc0_elena_analysis \
    Analisi_Elena/fc0_elena_analysis.cc $(root-config --cflags --libs)

Uso per tracce orizzontali lungo Z, nel piano Y-Z:

  ./Analisi_Elena/fc0_elena_analysis input.root analysis_plots/output_prefix 1 0 0.0801

Parametri:

  medpes = 1  media per colonne globali Z, adatta a tracce orizzontali lungo Z
  plane  = 0  fit y(z)
  vd     = 0.0801 mm/ns, usata solo se plane = 1 per ricostruire x = t * vd

Per studi di densita' su pad, non usare una traccia a y = 0 mm: nella
geometria HAT attuale y = 0 e' il gap meccanico tra le due righe di ERAM.
Usare ad esempio y = -170.5 mm per la riga inferiore, che contiene quattro
ERAM con pad attivi. Inoltre partire da z = -820 mm evita di iniziare fuori
dalla prima area pad, il cui bordo sinistro e' circa z = -834.5 mm.

Output:

  output_prefix_residuals.png       istogramma residui e fit gaussiano
  output_prefix_density_counts.png  conteggi di endpoint per colonna/riga occupata
  output_prefix_avg_pad_map.png     mappa media endpoint driftati per pad/evento

Il programma stampa anche:

  numero medio di endpoint driftati per evento
  endpoint driftati/cm/evento
  RMS residui
  sigma gaussiana dei residui

Per i residui puoi anche fissare la finestra di fit separatamente dalla finestra
del plot:

  ./fc0_elena_analysis input.root out_prefix 1 0 0.0801 3.0 1.5

Qui `3.0` imposta l'asse del grafico a +/-3 mm e `1.5` restringe il fit gaussiano
alla regione centrale +/-1.5 mm.

Per confrontare muoni e laser sulla stessa figura usare:

  python3 analysis/compare_average_pad_maps.py \
    --muon output_muon_horizontal_100_anode.root \
    --laser output_laser_2photon_horizontal_zR50cm_100_anode.root \
    --out analysis_plots/muon_vs_laser_avg_pad_maps.png

Per produrre la distribuzione di endpoint driftati per unita' di lunghezza
di traccia, in bin fisici da 1 cm lungo z:

  python3 analysis/track_unit_density.py \
    --muon output_muon_horizontal_100_anode.root \
    --laser output_laser_2photon_horizontal_zR50cm_100_anode.root \
    --out-prefix analysis_plots/track_unit_density \
    --bin-width-cm 1.0 \
    --z-min -250 \
    --z-max 250

Il fit Landau viene applicato alla distribuzione dei muoni. Per il laser,
`--z-min` e `--z-max` selezionano la finestra in coordinata z usata per la
distribuzione, mentre il profilo medio in funzione di z permette di scegliere
una regione vicina al waist o una regione piu' uniforme.

Il tipo di fit e la finestra in z usata per riempire l'istogramma del fit si
possono impostare separatamente:

  python3 analysis/track_unit_density.py \
    --muon output_muon_horizontal_100_anode.root \
    --laser output_laser_2photon_horizontal_zR50cm_100_anode.root \
    --out-prefix analysis_plots/track_unit_density \
    --muon-fit-type landau \
    --laser-fit-type gaussian \
    --muon-fit-z-min -250 \
    --muon-fit-z-max 250 \
    --laser-fit-z-min -150 \
    --laser-fit-z-max 150

Output principali:

  analysis_plots/track_unit_density_density_overlay.png
  analysis_plots/track_unit_density_z_profile.png
  analysis_plots/track_unit_density_muon_landau.png
  analysis_plots/track_unit_density_laser_gaussian.png
  analysis_plots/track_unit_density_muon_profile.csv
  analysis_plots/track_unit_density_laser_profile.csv

Per analizzare le quantita' al punto di generazione, quindi prima del drift,
usare il tree "clusters". Il comando seguente produce:

  profilo longitudinale in z della fluence laser e dei primari laser/muone
  profilo trasverso radiale nel piano x-y della fluence laser e dei primari

Comando:

  python3 analysis/source_track_profiles.py \
    --muon output_muon_horizontal_100_anode.root \
    --laser output_laser_2photon_horizontal_zR50cm_100_anode.root \
    --config config/laser_2photon_horizontal_zR50cm_100_anode.json \
    --out-prefix analysis_plots/source_track_profiles \
    --z-min -820 \
    --z-max 834 \
    --z-bin-cm 1.0 \
    --r-max-mm 8 \
    --r-bin-mm 0.1

Output:

  analysis_plots/source_track_profiles_longitudinal.png
  analysis_plots/source_track_profiles_transverse.png
  analysis_plots/source_track_profiles_muon_primaries.csv
  analysis_plots/source_track_profiles_laser_primaries.csv
  analysis_plots/source_track_profiles_laser_energy.csv

Nel grafico longitudinale la curva laser e' la fluence on-axis [J/cm2] lungo z.
Nel grafico trasverso la curva laser e' la fluence radiale mediata sulla
finestra z selezionata. Le densita' dei primari sono normalizzate per evento e
per cm di traccia; il profilo radiale e' normalizzato anche per area anulare
[mm2].

I file `config/muon_horizontal_100_anode.json` e
`config/laser_2photon_horizontal_zR50cm_100_anode.json` ora estendono un
template condiviso:

  config/horizontal_active_row_shared.json

Cosı' muoni e laser restano sullo stesso asse geometrico e differiscono solo
per la sorgente primaria.

Nota per i muoni TrackHeed:

  Se compare il messaggio

    Cannot retrieve database path (none of the environment variables
    HEED_DATABASE, GARFIELD_INSTALL, GARFIELD_HOME is defined)

  allora Heed non trova le sue tabelle e la traccia produce 0 elettroni
  primari. Prima di generare muoni eseguire:

    source scripts/setup_garfield_env.sh

  oppure esportare esplicitamente:

    export GARFIELD_INSTALL=/local/simulazioni/garfieldpp/install
    export GARFIELD_HOME=/local/simulazioni/garfieldpp
    export HEED_DATABASE=/local/simulazioni/garfieldpp/install/share/Heed/database
    export LD_LIBRARY_PATH=/local/simulazioni/garfieldpp/install/lib64:$LD_LIBRARY_PATH
