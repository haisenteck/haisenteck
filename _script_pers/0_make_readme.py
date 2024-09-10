import os

# Esegui lo script "elenco_fap.py" nella stessa cartella
os.system("py elenco_fap.py")
os.system("py link.py")
os.system("py subghz.py")

# Trova tutti i file txt nella cartella corrente
file_list = [file for file in os.listdir() if file.endswith(".txt")]

# Ordina i file in base alla loro numerazione crescente
file_list.sort(key=lambda x: int(x.split('_')[0]))

# Unisci i file in uno solo
output_file = r"D:\xxx_flipper_code\firmware\flipperzero-Haisenteck\ReadMe.md"
with open(output_file, "w") as outfile:
    for filename in file_list:
        with open(filename, "r") as infile:
            outfile.write(infile.read())

print("Unione completata. Il risultato è stato scritto in", output_file)