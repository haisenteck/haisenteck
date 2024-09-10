import os
testo_iniziale = "</a>\n<h3>Haisenteck - Flipper zero firmware Mod</h3><br>\nLink all'installazione dell'ultima release tramite Web Updater:<a href='https://lab.flipper.net/?url="
link_produzione = input("Inserisci il link di produzione da https://raw.githack.com/:\n")
testo_completo = testo_iniziale + link_produzione
testo_completo += "&channel=Haisenteck_"
with open("D:\\xxx_flipper_code\\firmware\\flipperzero-Haisenteck\\versione.txt", "r") as file_versione:
    versione = file_versione.read().strip()
testo_completo += versione
testo_completo += "&version="
testo_completo += versione
testo_completo += "' target='_blank'>WEB UPDATER - RELEASE</a><br>"
with open("1_Link.txt", "w") as f:
    f.write(testo_completo)
    f.write("\n")
    f.write("link al file di installazione stabile: <a href='https://github.com/haisenteck/flipperzero-Haisenteck/releases/tag/")
    f.write(versione)
    f.write("' target='_blank'>Versione STABILE</a><br>\n")
    f.write("Link al file di installazione DEV build: <a href='https://github.com/haisenteck/flipperzero-Haisenteck/blob/dev/dist/f7-C/flipper-z-f7-update-Haisenteck_")
    f.write(versione)
    f.write(".tgz' target='_blank'>Versione DEV Build</a><br>\n")
    f.write("Link al repository dei contenuti extra: <a href='https://github.com/haisenteck/Flipper_MicroSD' target='_blank'>Contenuti Extra per Micro SD</a><br>\n")
    f.write("<br>\n")
    f.write("# Novità<br>\n")
print(f"1_Link.txt creato")