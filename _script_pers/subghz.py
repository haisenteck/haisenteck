import os
import re
import chardet

# Cartella dei file .c
cartella = r'D:\xxx_flipper_code\firmware\flipperzero-Haisenteck\lib\subghz\protocols'

# Lista dei file nella cartella
files = [f for f in os.listdir(cartella) if f.endswith('.c') and f not in ["base.c", "protocol_items.c", "keeloq_common.c"]]

# Nome del file di output
output_file = "6_Subghz_protocolli.txt"

# Parole da escludere
exclude_words = ["SubGhzProtocol", "SubGhz_protocol", "subghz_protocol_", "SubGhzProtocolFlag_", "SubGhzProtoco", "subghz_protocol", "SubGhzProtoco"]

# Apri il file di output in modalità scrittura
with open(output_file, 'w', encoding='utf-8') as output:
    # Scrive l'intestazione della tabella HTML
    output.write("<table>\n")
    output.write("<tr><th>Tag</th><th>Type</th><th>Flag</th></tr>\n")

    # Itera attraverso i file nella cartella
    for file in files:
        # Percorso completo del file corrente
        file_path = os.path.join(cartella, file)

        try:
            # Utilizza chardet per rilevare automaticamente l'encoding del file
            with open(file_path, 'rb') as raw_file:
                result = chardet.detect(raw_file.read())
                file_encoding = result['encoding']

            # Apre il file corrente in modalità lettura con l'encoding rilevato
            with open(file_path, 'r', encoding=file_encoding) as current_file:
                # Legge il contenuto del file
                content = current_file.read()

                # Utilizza espressioni regolari per trovare i parametri richiesti
                tags = re.findall(r'#define TAG\s*([^\n]+)', content)
                types = re.findall(r'\.type = ([^\n]+)', content)
                flags = re.findall(r'flag = ([^\n]+)', content)

                # Scrive i risultati nel file di output
                for tag, type_, flag in zip(tags, types, flags):
                    # Pulisce il tag rimuovendo le parti specificate e le virgolette
                    cleaned_tag = re.sub('|'.join(map(re.escape, exclude_words)), '', tag.strip()).strip('\"')

                    # Traduci i tipi desiderati
                    if "SubGhzProtocolWeatherStation" in type_:
                        type_description = "WeatherStation"
                    elif "SubGhzProtocolTypeStatic" in type_:
                        type_description = "Statico"
                    elif "SubGhzProtocolTypeDynamic" in type_:
                        type_description = "Dinamico\Rolling Code"
                    else:
                        type_description = type_.strip()  # Mantieni il valore originale se non corrisponde a nessuno dei casi sopra

                    # Traduci i flag desiderati
                    flag_descriptions = []
                    if "SubGhzProtocolFlag_433" in flag:
                        flag_descriptions.append("433 Enable")
                    if "SubGhzProtocolFlag_315" in flag:
                        flag_descriptions.append("315 Enable")
                    if "SubGhzProtocolFlag_868" in flag:
                        flag_descriptions.append("868 Enable")
                    if "SubGhzProtocolFlag_AM" in flag:
                        flag_descriptions.append("AM")
                    if "SubGhzProtocolFlag_FM" in flag:
                        flag_descriptions.append("FM")
                    if "SubGhzProtocolFlag_Save" in flag:
                        flag_descriptions.append("Save Enable")
                    if "SubGhzProtocolFlag_Send" in flag:
                        flag_descriptions.append("Send Enable")

                    # Combina le descrizioni dei flag in una stringa separata da virgole
                    flag_description = ', '.join(flag_descriptions)

                    # Scrive i dati nel file di output solo se il tag pulito non è vuoto
                    if cleaned_tag:
                        output.write(f"<tr><td>{cleaned_tag}</td><td>{type_description}</td><td>{flag_description}</td></tr>\n")

        except Exception as e:
            # Gestisce eventuali eccezioni e fornisce informazioni di debug
            print(f"Errore durante l'elaborazione del file {file}: {e}")

    # Scrive la chiusura della tabella HTML
    output.write("</table>\n")

print(f"I dati sono stati scritti nel file: {output_file}")
