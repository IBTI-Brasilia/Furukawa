import sys, os

def main():
    # Arquivos .txt no path
    print('sys.argv[0] =', sys.argv[0])             
    pathname = os.path.dirname(sys.argv[0])        
    raiz = os.path.abspath(pathname)
    print('full path =', raiz)
    raiz = raiz.replace("\\" ,"/")        
    path = raiz + "/Configuração"
    print('path =', path)
    text_files = [fileX for fileX in os.listdir(path) if fileX.endswith('.txt')]
    print(text_files)
    for base_name in text_files:
        # Nome do arquivo de configuracao
        print("+ Config file " + base_name)
        # Abertura do arquivo base que sera alterado 
        #  com as novas chaves do arquivo de chaves
        base_file = open("furukawa_sce_base.hex", "r")
        # Criacao do arquivo bat para gravacao do fw
        bat_file = open(raiz +  "/Gravação/" + base_name[:-4] + ".bat", "w+")
        # Arquivo que contem as chaves
        keys_file = open(path + "/" + base_name, "r")
        # Criacao de um novo arquivo de FW com as novas chaves
        new_file_name = raiz + "/Hexa/" + base_name[:-4] + "_fwk_sce.hex"
        new_file = open(new_file_name, "w+")

        # Criacao do arquivo bat
        load = "ST-LINK_CLI.exe -P "
        bat_file.write(load + new_file_name + "\npause")

        # Leitura do arquivo de chaves linha a linha
        lines = keys_file.readlines()
        for line in lines:
            split_line = line.split(":")
            # Obtencao do novo DevEUI
            if split_line[0] == "DevEUI":
                new_deveui = split_line[1][:-1] # Retirada do \n
                print("+ NEW DevEUI:" + new_deveui)
            # Obtencao da nova AppKey
            elif split_line[0] == "AppKey":
                new_appkey = split_line[1][:-1]
                print("+ NEW AppKey:" + new_appkey)
            # Obtencao da nova AppEUI
            elif split_line[0] == "AppEUI":
                new_appeui = split_line[1][:-1]
                print("+ NEW AppEUI:" + new_appeui)

        new_devaddr = "6220E28E"
        print("+ NEW dAddr " + new_devaddr)        
        # Leitura do arquivo base linha a linha
        lines = base_file.readlines()
        for line in lines:
            # Filtragem das linhas que possuem as chaves
            if line.startswith(":108"):
                split_line = line.split(":108")
                # Retirada do antigo checksum das linhas que possuem as chaves
                checksumless_line = split_line[1][:-3]
                # Obtencao do endereco das linhas que possuem as chaves
                address = checksumless_line[:3]
                # Obtencao dos dados das linhas que possuem as chaves
                data = checksumless_line[5:]

                if address == '2DC':
                    print("- BASE line 2DC \n" + line[:-1])
                    # Primeira parte do DevEUI
                    deveui1 = data[-4:]
                    # Nova linha sem checksum
                    new_line = line[:-7] + new_deveui[:4]
                    print("- NEW line 2DC \n" + new_line)
                    # Calculo do checksum
                    n = checksum(new_line[1:]) # Retirada do :
                    #print(n)
                    # Nova linha com checksum
                    final_line = new_line + n.upper() + '\n'
                    print("= Checksum NEW line 2DC \n" + final_line)
                    # Escrita da nova linha
                    new_file.write(final_line)

                elif address == '2EC':
                    print("- BASE line 2EC \n" + line[:-1])
                    # Segunda parte do DevEUI
                    deveui2 = data[:12]
                    deveui = deveui1 + deveui2
                    #print("+ DevEUI:" + deveui)
                    # Primeira parte do AppEUI
                    appeui1 = data[-8:]
                    # nova linha sem checksum
                    new_line = line[:9] + new_deveui[4:] + "0000" + new_devaddr[::-1] + new_appeui[:8]
                    print("- NEW line 2EC \n" + new_line)
                    # Calculo do checksum
                    n = checksum(new_line[1:]) # Retirada do :
                    #print(n)
                    # Nova linha com checksum
                    final_line = new_line + n.upper() + '\n'
                    print("= Checksum NEW line 2EC \n" + final_line)
                    # Escrita da nova linha
                    new_file.write(final_line)

                elif address == '2FC':
                    print("- BASE line 2FC \n" + line[:-1])
                    # Segunda parte do AppEUI
                    appeui2 = data[:8]
                    appeui = appeui1 + appeui2
                    #print("+ AppEUI:" + appeui)
                    # Primeira parte do AppKey
                    appkey1 = data[8:]
                    # nova linha sem checksum
                    new_line = line[:9] + new_appeui[8:] + new_appkey[:24]
                    print("- NEW line 2FC \n" + new_line)
                    # Calculo do checksum
                    n = checksum(new_line[1:]) # Retirada do :
                    #print(n)
                    # Nova linha com checksum
                    final_line = new_line + n.upper() + '\n'
                    print("= Checksum NEW line 2FC \n" + final_line)
                    # Escrita da nova linha
                    new_file.write(final_line)

                elif address == '30C':
                    print("- BASE line 30C \n" + line[:-1])
                    # Segunda parte do AppKey
                    appkey2 = data[:8]
                    appkey = appkey1 + appkey2
                    #print("+ AppKey:" + appkey)
                    # nova linha sem checksum
                    new_line = line[:9] + new_appkey[24:] + line[17:-3]
                    print("- NEW line 30C \n" + new_line)
                    # Calculo do checksum
                    n = checksum(new_line[1:]) # Retirada do :
                    #print(n)
                    # Nova linha com checksum
                    final_line = new_line + n.upper() + '\n'
                    print("= Checksum NEW line 30C \n" + final_line)
                    # Escrita da nova linha
                    new_file.write(final_line)

                # Se a linha nao possui chaves
                else:
                    new_file.write(line)

            # Se a linha nao commeca com :108
            else:
                new_file.write(line)

        base_file.close()
        bat_file.close()
        keys_file.close()
        new_file.close()

def checksum(line):
    #print(line)
    # Separa a linha byte a byte
    byte = [int(line[i:i+2], 16) for i in range(0, len(line), 2)]
    #print(byte)
    # Soma os bytes da linha como hexadecimal
    n = sum(byte)
    #print(n)
    # Calculo do XOR e retirada do overflow
    xor = (n ^ 0xFF) % 0x100
    #print(xor)
    # Calculo do complemento de 2
    n_xor = str("%02x" % (xor + 1))
    #print(n_xor)
    return n_xor


if __name__ == "__main__":
    main()