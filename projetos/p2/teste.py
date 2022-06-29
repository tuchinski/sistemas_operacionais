from math import ceil
import os
import re
from FAT import fat
import json

# objeto que representa o imagem FAT
main_fat = fat()

def read_BPB(imagem):
        global main_fat
        # BS_jmpBoot
        jmpBoot = imagem[0:3]
        # instrução de jump para o código de boot
        # aceitos somente o primeiro ou o segundo
        # jmpBoot[0] = 0xeb | 0xe9
        # jmpBoot[1] = 0x?? | 0x??
        # jmpBoot[2] = 0x90 | 0x??
        # print(f"jmpBoot: {hex(jmpBoot[0])} {hex(jmpBoot[1])} {hex(jmpBoot[2])}")
        
        
        # somente um campo com um nome
        # BS_OEMName
        oem_name = imagem[3:11].decode()
        # print(f"OEM Name: {oem_name}")

        # qtde de bytes por setor
        # precisa ter os valores 512, 1024, 2048 or 4096
        # BPB_BytsPerSec
        # bytes_per_sec = imagem[11:13]
        bytes_per_sec = int.from_bytes(imagem[11:13],'little')

        # setores por unidades alocadas
        # tem que ser potência de 2 e maior que 0
        # BPB_SecPerClus
        sector_per_cluster = imagem[13]
        # print(f"Sector per clusters: {sector_per_cluster}")

        # numero de setores reservados
        # BPB_RsvdSecCnt
        # reserved_sectors = imagem[14:16]
        reserved_sectors = int.from_bytes(imagem[14:16],'little')
        # print(f"Reserved Sectors: {reserved_sectors}")

        # numero de estruturas FAT no disco
        # precisa ser 2
        # BPB_NumFATs
        num_fats = imagem[16]
        # print(f"Number of FAT data structures on the volume: {num_fats}")

        # Valor padrao 0xf8
        # BPB_Media
        media = imagem[21]
        # print(f"Media: {hex(media)}")

        # Setores por trilha
        # BPB_SecPerTrk
        sec_per_trk = imagem[24:26]
        # print(f"Sectors per track: {int.from_bytes(sec_per_trk,'little')}")

        # Number of heads for interrupt 0x13
        # BPB_NumHeads
        num_heads = imagem[26:28]
        # print(f"Number of heads: {int.from_bytes(num_heads,'little')}")

        # total de setores de 32 bits
        # BPB_TotSec32
        # tot_sec_32 = imagem[32:36]
        tot_sec_32 = int.from_bytes(imagem[32:36],'little')
        # print(f"total sectors: {tot_sec_32}")

        # Setores por FAT
        # BPB_FATSz32
        # fat_Sz_32 = imagem[36:40]
        fat_Sz_32 = int.from_bytes(imagem[36:40],'little')
        # print(f"FAT32 32-bit count of sectors occupied by ONE FAT: {fat_Sz_32}")
        #aqui!@!@@!!@!@!
        # print(f"Sectors occupied by FAT: {fat_Sz_32}")

        # tamanho da FAT em bytes
        fat_size_bytes = fat_Sz_32 * bytes_per_sec
        # print(f"FAT size: {fat_size_bytes} bytes")

        # flags externas
        # BPB_ExtFlags
        ext_flags = imagem[40:42]
        # print(f"external flags: {ext_flags}")

        # número da versão
        # doc da microsoft é 0:0
        # BPB_FSVer
        fs_ver = imagem[42:44]
        # print(f"version number: {fs_ver}")

        # Cluster raiz
        # Geralmente valor é 2
        # BPB_RootClus
        root_clus = int.from_bytes(imagem[44:48],'little')
        # print(f"root cluster: {root_clus}")

        # número do setor do FSINFO
        # BPB_FSInfo
        fs_info = imagem[48:50]
        # print(f"Sector number of FSINFO: {int.from_bytes(fs_info,'little')}")

        # indicates the sector number in the reserved area of the volume of a copy of the boot record.
        # indica o numero do setor na area reservada do volume da copia do boot record
        # padrao = 6
        # BPB_BkBootSec
        bk_boot_sec = imagem[50:52]
        # print(f"sector number in the reserved area of the volume of a copy of the boot record: {int.from_bytes(bk_boot_sec,'little')}")

        # bytes reservados
        # geralmente é tudo 0
        # BPB_Reserved
        reserved = imagem[52:64]
        # print(f"Reserved: {reserved}")

        # nome do tipo
        # sempre vai ser FAT32
        # BS_FilSysType
        fil_sys_type = imagem[82:90]
        # print(f"FAT type: {fil_sys_type.decode()}")

        # Atribuindo os valores encontrados para o objeto principal
        main_fat.jmp_boot = jmpBoot
        main_fat.oem_name = oem_name
        main_fat.bytes_per_sec = bytes_per_sec
        main_fat.sector_per_cluster = sector_per_cluster
        main_fat.reserved_sectors = reserved_sectors
        main_fat.num_fats = num_fats
        main_fat.media = media
        main_fat.sec_per_trk = sec_per_trk
        main_fat.num_heads = num_heads
        main_fat.tot_sec_32 = tot_sec_32
        main_fat.fat_Sz_32 = fat_Sz_32
        main_fat.fat_size_bytes = fat_size_bytes
        main_fat.ext_flags = ext_flags
        main_fat.fs_ver = fs_ver
        main_fat.root_clus = root_clus
        main_fat.fs_info = fs_info
        main_fat.bk_boot_sec = bk_boot_sec
        main_fat.reserved_bytes = reserved
        main_fat.fil_sys_type = fil_sys_type
        main_fat.start_fat1 = reserved_sectors * bytes_per_sec

        # root_dir_sectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;
        # o BPB_RootEntCnt na FAT32 sempre tem o valor 0
        # por isso o root_dir_sector vai ser 0 tbm na FAT32
        main_fat.root_dir_sectors = int(((0*32) + (bytes_per_sec - 1)) / bytes_per_sec)
        # print(f"root_dir_sectors {main_fat.root_dir_sectors}")

        # FirstDataSector = BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors;
        main_fat.first_data_sector = (reserved_sectors * bytes_per_sec) + (num_fats * fat_size_bytes) + main_fat.root_dir_sectors
        # print(f"first_data_sector {hex(main_fat.first_data_sector)}")

        # DataSec = TotSec – (BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors);
        main_fat.data_sec = tot_sec_32 - (reserved_sectors + (num_fats * fat_Sz_32) + main_fat.root_dir_sectors)
        # print(f"{tot_sec_32} - ({reserved_sectors} + ({num_fats} * {fat_Sz_32}) + {main_fat.root_dir_sectors})")
        # print(f"data_sec {main_fat.data_sec}")

        # CountofClusters = DataSec / BPB_SecPerClus;
        main_fat.count_of_clusters = int(main_fat.data_sec / sector_per_cluster)
        # print(f"count_of_clusters {main_fat.count_of_clusters}")

def read_fsi(imagem):
    global main_fat
    # print("\n----------------------------------------------------------------\n")
    # print("FAT32 FSInfo Sector Structure and Backup Boot Sector\n")
    
    # os dados da FSInfo começam logo após o BPB, no segundo setor do disco
    start_FSInfo = main_fat.bytes_per_sec

    # dados FAT32 FSInfo Sector Structure and Backup Boot Sector
    # FSI_LeadSig
    # o valor tem que ser 0x41615252
    fsi_lead_sig = int.from_bytes(imagem[start_FSInfo:start_FSInfo+4], 'little')
    if fsi_lead_sig != 1096897106:
        print("ERRO: VALOR FSI_LEAD_SIG DIVERGENTE")
        exit()

    # FSI_Reserved1
    # campo reservado
    # tem que ser inicializado com 0
    fsi_reserved1 = int.from_bytes(imagem[start_FSInfo+4 : start_FSInfo + 484], 'little')
    # print(f'fsi_reserved1 {fsi_reserved1}')

    # FSI_StrucSig
    # o valor deve ser 0x61417272.
    fsi_struc_sig = int.from_bytes(imagem[start_FSInfo + 484: start_FSInfo + 488], 'little')
    if fsi_struc_sig != 1631679090:
        print("ERRO: VALOR FSI_STRUC_SIG DIVERGENTE")
        exit()

    # FSI_Free_Count
    # ultima contagem de clusters livres
    # for 0xFFFFFFFF, então a contagem é desconhecida e precisa ser calculada
    fsi_free_count =  int.from_bytes(imagem[start_FSInfo + 488: start_FSInfo + 492], 'little')
    # print(f'fsi_free_count {fsi_free_count}')

    # FSI_Nxt_Free
    # indica onde o driver pode começar a procurar por clusters livres
    # caso tenha o valor 0xFFFFFFFF vai ter que começar do cluster 2 mesmo
    fsi_nxt_free = int.from_bytes(imagem[start_FSInfo + 492: start_FSInfo + 496], 'little')
    # print(f'fsi_nxt_free {fsi_nxt_free}')

    # FSI_Reserved2
    # campo reservado
    # tem que ser inicializado com 0
    fsi_reserved2 = int.from_bytes(imagem[start_FSInfo+496 : start_FSInfo + 508], 'little')
    # print(f'fsi_reserved2 {fsi_reserved2}')

    # FSI_TrailSig
    # valor tem que ser 0xAA550000
    # Valida que é de fato um FSInfo sector
    fsi_trail_sig = int.from_bytes(imagem[start_FSInfo + 508: start_FSInfo + 512], 'little')
    if fsi_trail_sig != 2857697280:
        print("ERRO: VALOR FSI_TRAIL_SIG DIVERGENTE")
        exit()
    # print(f'fsi_trail_sig {hex(fsi_trail_sig)}')
    
    # atribuindo os valores para o objeto principal
    main_fat.fsi_lead_sig = fsi_lead_sig
    main_fat.fsi_reserved1 = fsi_reserved1
    main_fat.fsi_struc_sig = fsi_struc_sig
    main_fat.fsi_free_count = fsi_free_count
    main_fat.fsi_nxt_free = fsi_nxt_free
    main_fat.fsi_reserved2 = fsi_reserved2
    main_fat.fsi_trail_sig = fsi_trail_sig

# Retorna os dados em formato texto de um cluster
def return_text_from_cluster(imagem, num_cluster) -> str:
    global main_fat
    if num_cluster < main_fat.root_clus:
        # print()
        return f"Os clusters de dados começam em {main_fat.root_clus}"
    
    first_sector_of_cluster = ((num_cluster-2) * main_fat.sector_per_cluster * main_fat.bytes_per_sec) + main_fat.first_data_sector

    cluster_text = imagem[first_sector_of_cluster: first_sector_of_cluster + main_fat.bytes_per_sec * main_fat.sector_per_cluster]
    return cluster_text.decode(errors='backslashreplace')

def print_menu():
    pass
    print('''
info: exibe informações do disco e da FAT.
cluster <num>: exibe o conteúdo do bloco num no formato texto.
pwd: exibe o diretório corrente (caminho absoluto).
attr <file | dir>: exibe os atributos de um arquivo (file) ou diretório (dir).
cd <path>: altera o diretório corrente para o definido como path.
touch <file>: cria o arquivo file com conteúdo vazio.
mkdir <dir>: cria o diretório dir vazio.
rm <file>: remove o arquivo file do sistema.
rmdir <dir>: remove o diretório dir, se estiver vazio.
cp <source_path> <target_path>: copia um arquivo de origem (source_path) para destino (target_path).
mv <source_path> <target_path>: move um arquivo de origem (source_path) para destino (target_path).
rename <file> <newfilename> : renomeia arquivo file para newfilename.
ls: listar os arquivos e diretórios do diretório corrente.
        ''')

# imprime todas as informações coletadas do disco
def print_infos():
    # BPB Info
    print(f"jmpBoot: {hex(main_fat.jmp_boot[0])} {hex(main_fat.jmp_boot[1])} {hex(main_fat.jmp_boot[2])}")
    print(f"OEM Name: {main_fat.oem_name}")
    print(f"Bytes per sector: {main_fat.bytes_per_sec}")
    print(f"Sector per clusters: {main_fat.sector_per_cluster}")
    print(f"Reserved Sectors: {main_fat.reserved_sectors}")
    print(f"Number of FAT data structures: {main_fat.num_fats}")
    print(f"Media: {hex(main_fat.media)}")
    print(f"Sectors per track: {int.from_bytes(main_fat.sec_per_trk,'little')}")
    print(f"Number of heads: {int.from_bytes(main_fat.num_heads,'little')}")
    print(f"Total Sectors: {main_fat.tot_sec_32}")
    print(f"Total Sectors occupied by ONE FAT: {main_fat.fat_Sz_32}")
    print(f"Sectors occupied by FAT: {main_fat.fat_Sz_32}")
    print(f"FAT size: {main_fat.fat_size_bytes} bytes")
    print(f"external flags: {main_fat.ext_flags}")
    print(f"version number: {main_fat.fs_ver}")
    print(f"root cluster: {main_fat.root_clus}")
    print(f"Sector number of FSINFO: {int.from_bytes(main_fat.fs_info,'little')}")
    print(f"Sector number of a copy of the boot record: {int.from_bytes(main_fat.bk_boot_sec,'little')}")
    print(f"Reserved: {main_fat.reserved_bytes}")
    print(f"FAT type: {main_fat.fil_sys_type.decode()}")

    print(f"Root Dir Sectors: {main_fat.root_dir_sectors}")
    print(f"First Data Sector: {hex(main_fat.first_data_sector)}")
    print(f"Data Sector: {main_fat.data_sec}")
    print(f"Count of Clusters: {main_fat.count_of_clusters}")

    for num in range(main_fat.num_fats):
        print(f'Start address of FAT{num+1}: {hex((main_fat.reserved_sectors * main_fat.bytes_per_sec) + (num  * main_fat.fat_size_bytes ))}')

# retorna o inicio de um cluster de dados
def return_start_data_cluster(imagem, num_cluster):
    if num_cluster < main_fat.root_clus:
        # print()
        return f"Os clusters de dados começam em {main_fat.root_clus}"
    
    return ((num_cluster-2) * main_fat.sector_per_cluster * main_fat.bytes_per_sec) + main_fat.first_data_sector

# extrai as informações dos bytes passados
def extract_infos(bytes):
    inicio_dados = 0 
    
    # caso o bit 11 tenha o valor 0x0f, é uma entrada de nome longa 
    # e os dados do arquivo vai ficar nos proximos 32 bytes
    if (bytes[inicio_dados + 11] == 15): 
        ###print('entrada de arquivos com nome longo, lendo os 32 proximos bytes')
        inicio_dados = inicio_dados + 32
        return {
            'tipo': 'long name',
        }
    elif bytes[inicio_dados + 11] == 0:
        ### print("não existe nada armazenado nessa posição")
        return {
            'tipo': 'vazio',
        }
    elif bytes[0] == 229:
        # nesse caso a entrada foi deletada e não deve ser mostrada no ls
        return {
            'tipo': 'deletado',
        }
    
    # DIR_Name e DIR_extension
    # o nome curto tem 8 bytes, e a extensao 3 bytes
    dir_name = bytes[inicio_dados: inicio_dados + 8].decode().strip()
    dir_extension = bytes[inicio_dados+8: inicio_dados + 11].decode().strip()
    
    ###print(f'dir_name + dir extension: {dir_name}.{dir_extension}')
    
    # DIR_Attr 
    # READ_ONLY=0x01 
    # HIDDEN=0x02 
    # SYSTEM=0x04 
    # VOLUME_ID=0x08 
    # DIRECTORY=0x10 
    # ARCHIVE=0x20 
    # LFN=READ_ONLY|HIDDEN|SYSTEM|VOLUME_ID
    dir_attr_cod = bytes[inicio_dados + 11]
    dir_attr_name = ''
    if dir_attr_cod == 1:
        ###print(f'dir_attr: READ_ONLY')
        dir_attr_name = 'READ_ONLY'
    elif dir_attr_cod == 2:
        ###print(f'dir_attr: HIDDEN')
        dir_attr_name = 'HIDDEN'
    elif dir_attr_cod == 4:
        ###print(f'dir_attr: SYSTEM')
        dir_attr_name = 'SYSTEM'
    elif dir_attr_cod == 8:
        ###print(f'dir_attr: VOLUME_ID')
        dir_attr_name = 'VOLUME_ID'
    elif dir_attr_cod == 16:
        ###print(f'dir_attr: DIRECTORY')
        dir_attr_name = 'DIRECTORY'
    elif dir_attr_cod == 32:
        ###print(f'dir_attr: ARCHIVE')
        dir_attr_name = 'ARCHIVE'

    # DIR_CrtTimeTenth
    dir_crt_time_tenth = bytes[inicio_dados + 13]
    ###print(f'dir_crt_time_tenth: {dir_crt_time_tenth}')

    # DIR_CrtTime
    # TODO verificar como faz pra contar as horas
    # dir_crt_time = bin(int.from_bytes(bytes[inicio_dados + 14: inicio_dados + 16], 'little'))[2:]
    dir_crt_time = bytes[inicio_dados + 14: inicio_dados + 16]
    # print(f'dir_crt_time: {hex(dir_crt_time)}')

    # DIR_CrtDate
    # TODO verificar como faz pra contar as horas
    dir_crt_date = bytes[inicio_dados + 16: inicio_dados + 18]
    # print(f'dir_crt_time: {hex(dir_crt_time)}')

    # DIR_FstClusHI
    dir_fst_clus_HI = bytes[inicio_dados + 20: inicio_dados + 22]
    ###print(f'dir_fst_clus_HI: {dir_fst_clus_HI}')
    segunda_parte_hi = bytes[inicio_dados + 21]
    primeira_parte_hi = bytes[inicio_dados + 20]
    concat_hi  = hex(segunda_parte_hi) + hex(primeira_parte_hi)[2:] # concatenando os hex, a segunda parte vem primeiro pq é little endian
    ###print(concat_hi)
    

    # DIR_WrtTime
    # TODO verificar como faz pra contar as horas
    dir_wrt_time = bytes[inicio_dados + 22: inicio_dados + 24]
    # print(f'dir_crt_time: {hex(dir_crt_time)}')

    # DIR_WrtDate
    # TODO verificar como faz pra contar as horas
    dir_wrt_date = bytes[inicio_dados + 24: inicio_dados + 26]

    # DIR_FstClusLO
    # dir_fst_clus_LO = bytes[inicio_dados + 26: inicio_dados + 28]
    segunda_parte_lo = bytes[inicio_dados + 27]
    primeira_parte_lo = bytes[inicio_dados + 26]
    concat_lo  = hex(segunda_parte_lo) + hex(primeira_parte_lo)[2:] # concatenando os hex, a segunda parte vem primeiro pq é little endian
    ###print(concat_lo)

    first_cluster_dir = int(concat_hi + concat_lo[2:], 16)
    ###print(f'first_cluster_dir: {first_cluster_dir} -> {hex(first_cluster_dir)}')   

    # DIR_FileSize
    dir_file_size = int.from_bytes(bytes[inicio_dados + 28 : inicio_dados + 32], 'little')
    ###print(f'dir_file_size: {dir_file_size}')

    # retorna os dados encontrados do arquivo
    # ! importante: esta faltando as datas
    # todo: datas
    return {
        "tipo": dir_attr_cod,
        "dir_name": dir_name,
        "dir_extension": dir_extension,
        "dir_attr_cod": dir_attr_cod,
        "dir_attr_name": dir_attr_name,
        'dir_crt_time_tenth': dir_crt_time_tenth,
        'first_cluster_dir': first_cluster_dir,
        'dir_file_size': dir_file_size,
    }

def print_ls(lista_itens):
    for item in lista_itens:
        if item["dir_attr_cod"] == 32:
            print(f'(file){item["dir_name"]}.{item["dir_extension"]}', end="   ")
        elif item["dir_attr_cod"] == 16:
            print(f'(directory){item["dir_name"]}', end="   ")
            
# lista os arquivos do diretorio atual tendo como padrão o diretorio raiz
# o parametro cluster diz qual cluster deve ser analisado
# o paramentro define_dir_atual, define se o diretorio que está sendo analisado deve ser
# o diretorio corrente
def list_files(imagem, cluster, define_dir_corrente = False):
    # global main_fat
    num_cluster_diretorio = cluster
    first_sector_of_cluster = ((num_cluster_diretorio-2) * main_fat.sector_per_cluster * main_fat.bytes_per_sec) + main_fat.first_data_sector


    dados_cluster_atual = imagem[first_sector_of_cluster:first_sector_of_cluster+main_fat.bytes_per_sec]
    next_cluster = find_next_cluster(imagem, num_cluster_diretorio)

    # Verifica se o cluster atual é o final, se não for concatena os dados
    # se for só passa pra listagem dos dados
    #todo tem que resolver esse bo aqui
    while next_cluster != -1:
        pass
    
    # tamanho da entrada de um arquivo dentro do cluster de diretorio
    count = 32
    infos_diretorio = {'diretorios': []} # dados extraidos do diretorio
    for x in range(int(len(dados_cluster_atual) / 32)):
        info_extraida = extract_infos(imagem[ first_sector_of_cluster + (count*x) : first_sector_of_cluster + (count*(x+1)) ])
        info_extraida['inicio_endereco'] = first_sector_of_cluster + (count*x)
        if info_extraida['tipo'] == 'vazio':
            infos_diretorio['inicio_proxima_entrada_dir'] = (first_sector_of_cluster + (count*x))
            break
        elif info_extraida['tipo'] == 'deletado': 
            continue
        # ! ignorando por enquanto nomes longos dos arquivos
        elif info_extraida['tipo'] != 'long name':
            infos_diretorio['diretorios'].append(info_extraida)
        
    # print(f'info extraida: {json.dumps(infos_diretorio, indent=4)}')
    if define_dir_corrente:
        main_fat.dados_diretorio_atual = infos_diretorio
    return infos_diretorio

# verifica na tabela FAT, se o cluster passado por parâmetro tem sequência ou acabou por ali
# caso tenha acabado retorna -1 
# se tiver sequencia, retorna o valor do proximo cluster no formato int
def find_next_cluster(imagem, start_cluster) -> int:
    # print("verificando na fat se o cluster termina ali ou tem que ver mais coisa ainda")
    # print(f'start_fat1 {main_fat.start_fat1}')

    start_fat = main_fat.start_fat1
    offset_tabela_fat  = start_cluster * 4

    proxima_entrada_fat = imagem[start_fat + offset_tabela_fat: start_fat + offset_tabela_fat + 4]
    proxima_entrada_fat = int.from_bytes(proxima_entrada_fat, 'little')
    # 268435448 = 0xffffff8
    # 268435455 = 0xfffffff
    # caso a entrada seja maior ou igual a 268435448, o arquivo ou diretório acaba ali mesmo
    if proxima_entrada_fat >= 268435448:
        ###print('não existem mais entradas')
        return -1
    else:
        ###print(f'proxima_entrada_FAT {(proxima_entrada_fat)}')
        return proxima_entrada_fat

# mostra os atributos do arquivo ou diretorio selecionado
def show_attributes(name):
    # print(f"name: {name}")
    # print(json.dumps(main_fat.dados_diretorio_atual, indent=4))
    nome_e_extensao = name.split('.')
    for item in main_fat.dados_diretorio_atual['diretorios']:
        if item['dir_name'] == nome_e_extensao[0].upper():
            if (len(nome_e_extensao) == 1 and item['dir_extension'] == '') or (nome_e_extensao[1].upper() == item['dir_extension']):
                # se entrar aqui é o arquivo ou diretorio que estamos procurando
                print(f"Type: {item['dir_attr_name']}")
                print(f"Name: {item['dir_name']}", end="")
                if(item['dir_attr_cod'] != 16): # se entrar no if, é um diretório 
                    print(f".{item['dir_extension']}", end='')
                    print(f"\nSize: {item['dir_file_size']} bytes")
                # todo: precisa incluir as datas

# faz o processo de entrar em um diretorio 
def enter_directory(imagem, nome_dir):
    for item in main_fat.dados_diretorio_atual['diretorios']:
        if (item['dir_name'] == nome_dir.upper()) and (not item['dir_extension']) and item['dir_attr_cod'] == 16:
            # confirmando que é tem o mesmo nome e é um diretório
            if item['dir_name'] == '..':
                main_fat.nome_diretorio_atual = "/".join(main_fat.nome_diretorio_atual.split('/')[:-1])
                if item['first_cluster_dir'] == 0:
                    item['first_cluster_dir'] = main_fat.root_clus
                    main_fat.nome_diretorio_atual = '/'
            elif item['dir_name'] == '.':
                pass                            
            else:
                if main_fat.nome_diretorio_atual == '/':
                    main_fat.nome_diretorio_atual =  main_fat.nome_diretorio_atual + item['dir_name'] 
                else: 
                    main_fat.nome_diretorio_atual =  main_fat.nome_diretorio_atual + "/" + item['dir_name'] 
    
            main_fat.cluster_inicial_diretorio_atual = item['first_cluster_dir']
            list_files(imagem, item['first_cluster_dir'], True) # atualiza os dados do diretorio atual
            return
    print("Diretorio invalido")

# encontra e retorna o cluster x na fat
def find_cluster_fat(imagem, cluster):
    start_fat = main_fat.start_fat1
    start_cluster = start_fat + (4*cluster)
    return start_cluster

# verifica se o cluster está vazio na FAT
def verify_empty_fat(imagem, cluster) -> bool:
    start_cluster = find_cluster_fat(imagem, cluster)

    data_cluster = int.from_bytes(imagem[start_cluster : start_cluster + 4], 'little')
    # print(f'data_cluster {data_cluster}')
    if data_cluster == 0:
        return True
    else:
        return False

# define um cluster na fat como utilizado
# o parametro valor, representa o valor a ser inserido na fat
# se não passar nada, vai ser preenchido como EOC
def get_cluster_fat(imagem, cluster, valor = 268435455 ):
    # alterando na FAT
    start_cluster = find_cluster_fat(imagem, cluster)
    
    # valor inteiro que representa que a fat ta sendo utilizada
    # e não tem mais nenhum node
    valor_fat_utilizado = valor
    bytes_fat = int.to_bytes(valor_fat_utilizado, 4, 'little')
    
    # alterando os valores nas FATs
    for x in range(main_fat.num_fats):
        inicio_cluster =  start_cluster + (main_fat.fat_size_bytes * x)
        fim_cluster = start_cluster + (main_fat.fat_size_bytes * x) + 4
        imagem[inicio_cluster :fim_cluster] = bytes_fat

    # alterar os dados na FSInfo
    # os dados da FSInfo começam logo após o BPB, no segundo setor do disco
    start_FSInfo = main_fat.bytes_per_sec
    main_fat.fsi_free_count = main_fat.fsi_free_count - 1
    main_fat.fsi_nxt_free = main_fat.fsi_nxt_free + 1
    imagem[start_FSInfo + 488: start_FSInfo + 492] = int.to_bytes(main_fat.fsi_free_count, 4, 'little')
    imagem[start_FSInfo + 492: start_FSInfo + 496] = int.to_bytes(main_fat.fsi_nxt_free, 4, 'little')


# metodo que persiste as alterações no disco
def persist_in_disk(imagem):
    # tot_sec_32 * bytes_per_sec
    with open('myimagefat32.img', 'wb') as imagem_iso:
        imagem_iso.write(imagem)
    

# cria um arquivo ou diretório vazio
def create_file_directory(imagem, file_name, file_type):
    
    # validando nome
    # vendo se tem nome repetido
    for item in main_fat.dados_diretorio_atual['diretorios']:
        if file_name.split('.')[0].upper() == item['dir_name']:
            if item['dir_attr_cod'] == 16:
                print("ERRO: já existe um diretório com o mesmo nome")
            elif item['dir_attr_cod'] == 32 and file_name.split('.')[1].upper() == item['dir_extension'].upper():
                print('ERRO: já existe um arquivo com o mesmo nome')
                return 

    # criar o arquivo de entrada dos dados
    filename_split = file_name.split('.')
    if len(filename_split[0]) > 8:
        print("ERRO: ainda sem suporte para nomes com mais de 8 caracteres")
    
    # todo: verificar se os caracteres sao validos


    dir_name = filename_split[0].upper() # colocando em maiusculo os nomes
    dir_extension = ''
    if(len(filename_split) == 2):
        dir_extension = filename_split[1].upper() # colocando em maiusculo os nomes
        for _ in range(len(dir_extension),3):
            dir_extension = dir_extension + " " 
    else:
        dir_extension = "   "
    
    for _ in range(len(dir_name), 8):
            dir_name = dir_name + ' '
    
    dir_name = dir_name.encode()
    dir_extension = dir_extension.encode()

    dir_attr = int.to_bytes(file_type, 1, 'little')
    dir_NT_res = int.to_bytes(0, 1, 'little')
    
    prox_espaco_livre = main_fat.fsi_nxt_free + 1
    
    
    # todo: arrumar a hora e data
    dir_crt_time_tenth = int.to_bytes(0, 1, 'little')
    dir_crt_time = int.to_bytes(0, 2, 'little')
    dir_crt_date = int.to_bytes(0, 2, 'little')
    dir_lst_acc_date = int.to_bytes(0, 2, 'little')
    

    dir_wrt_time = int.to_bytes(0, 2, 'little')
    dir_wrt_date = int.to_bytes(0, 2, 'little')
    dir_File_size  = int.to_bytes(0, 4, 'little')

    bytes_prox_espaco_livre = int.to_bytes(prox_espaco_livre, 4, 'little')
    dir_Fst_clusLO = bytes_prox_espaco_livre[0:2]
    dir_fst_clus_HI = bytes_prox_espaco_livre[2:]


    entrada_diretorio_bytes = dir_name + dir_extension + dir_attr + dir_NT_res + dir_crt_time_tenth + dir_crt_time + dir_crt_date + dir_lst_acc_date + dir_fst_clus_HI + dir_wrt_time + dir_wrt_date + dir_Fst_clusLO + dir_File_size

    if len(entrada_diretorio_bytes) != 32:
        print("ERRO: a entrada criada para o diretório está incorreta")
    if verify_empty_fat(imagem, prox_espaco_livre) == False:
        print("erro ao encontrar espaço livre!")
        exit()

    # reservando o valor nas fats e atualizando a FSInfo
    get_cluster_fat(imagem, prox_espaco_livre)

    # colocar a entrada no diretório corrente
    imagem[main_fat.inicio_proxima_entrada_dir_atual: main_fat.inicio_proxima_entrada_dir_atual+32] = entrada_diretorio_bytes

    # # criando um arquivo
    # if file_type == 32:


    
    # criando um diretorio
    if file_type == 16:
        # precisa criar os arquivos . e .. no novo diretorio
        
        bytes_arquivo_ponto = b'.          '
        bytes_arquivo_pontoPonto = b'..         '

        # criando apenas o enderecamento do cluster do dir ..
        # porque do dir . é o mesmo endereço em que a pasta vai ser criada
        cluster_dir_atual = main_fat.cluster_inicial_diretorio_atual
        if cluster_dir_atual == main_fat.root_clus:
            cluster_dir_atual = 0

        bytes_cluster_dir_atual = int.to_bytes(cluster_dir_atual, 4, 'little')

        dir_pontoPonto_Fst_clusLO = bytes_cluster_dir_atual[0:2]
        dir_pontoPonto_fst_clus_HI = bytes_cluster_dir_atual[2:]

        entrada_diretorio_bytes_ponto = bytes_arquivo_ponto + dir_attr + dir_NT_res + dir_crt_time_tenth + dir_crt_time + dir_crt_date + dir_lst_acc_date + dir_fst_clus_HI + dir_wrt_time + dir_wrt_date + dir_Fst_clusLO + dir_File_size
        entrada_diretorio_bytes_pontoPonto = bytes_arquivo_pontoPonto + dir_attr + dir_NT_res + dir_crt_time_tenth + dir_crt_time + dir_crt_date + dir_lst_acc_date + dir_pontoPonto_fst_clus_HI + dir_wrt_time + dir_wrt_date + dir_pontoPonto_Fst_clusLO + dir_File_size

        if len(entrada_diretorio_bytes_ponto) != 32 and len(entrada_diretorio_bytes_pontoPonto)!=32:
            print("ERRO: a entrada criada para os arquivos . e .. do diretório estao incorretas")

        start_cluster_dir = return_start_data_cluster(imagem, prox_espaco_livre)
        imagem[start_cluster_dir: start_cluster_dir+32] = entrada_diretorio_bytes_ponto
        imagem[start_cluster_dir+32: start_cluster_dir+64] = entrada_diretorio_bytes_pontoPonto

    persist_in_disk(imagem)

# remove um arquivo                            
def remove_file(imagem, filename): 
    nome_e_extensao = filename.split('.')
    for item in main_fat.dados_diretorio_atual['diretorios']:
        if item['dir_attr_cod'] == 32:
            if item['dir_name'] == nome_e_extensao[0].upper():
                if len(nome_e_extensao) == 1:
                    nome_e_extensao[1] = ''
                if (nome_e_extensao[1].upper() == item['dir_extension']):
                    # file_first_cluster = item['first_cluster_dir']
                    next_cluster = item['first_cluster_dir']
                    while next_cluster != -1:
                        # liberando o espaço na FAT
                        start_cluster_fat = find_cluster_fat(imagem, next_cluster)
                        next_cluster = find_next_cluster(imagem, next_cluster)
                        for x in range(0,main_fat.num_fats):
                            fat_offset = (x * main_fat.fat_size_bytes)
                            start_data_fats = start_cluster_fat + fat_offset
                            end_data_fats = start_cluster_fat + 4 + fat_offset
                            imagem[start_data_fats: end_data_fats] = int.to_bytes(0, 4, 'little')

                    # atribuindo o valor 0xE5 no inicio da entrada do arquivo
                    # deixando assim, ele excluido                    
                    imagem[item['inicio_endereco']] = 229
            
                    # alterando os dados da FSinfo
                    start_FSInfo = main_fat.bytes_per_sec
                    main_fat.fsi_free_count = main_fat.fsi_free_count + 1
                    imagem[start_FSInfo + 488: start_FSInfo + 492] = int.to_bytes(main_fat.fsi_free_count, 4, 'little')

                    persist_in_disk(imagem)
                    return
    print("ERRO: arquivo não encontrado")

def remove_dir(imagem, dirname):
    for item in main_fat.dados_diretorio_atual['diretorios']:
        if dirname.upper() == item['dir_name'] and item['dir_attr_cod'] == 16:
            # achou o diretorio
            arquivos = list_files(imagem, item['first_cluster_dir'], False)
            if len(arquivos) > 2:
                print("ERRO: Diretório não está vazio")
                return 
            next_cluster = item['first_cluster_dir']

            while next_cluster != -1:
                # liberando o espaço na FAT
                start_cluster_fat = find_cluster_fat(imagem, next_cluster)
                next_cluster = find_next_cluster(imagem, next_cluster)
                for x in range(0,main_fat.num_fats):
                    fat_offset = (x * main_fat.fat_size_bytes)
                    start_data_fats = start_cluster_fat + fat_offset
                    end_data_fats = start_cluster_fat + 4 + fat_offset
                    imagem[start_data_fats: end_data_fats] = int.to_bytes(0, 4, 'little')

            # atribuindo o valor 0xE5 no inicio da entrada do arquivo
            # deixando assim, ele excluido                    
            imagem[item['inicio_endereco']] = 229
    
            # alterando os dados da FSinfo
            start_FSInfo = main_fat.bytes_per_sec
            main_fat.fsi_free_count = main_fat.fsi_free_count + 1
            imagem[start_FSInfo + 488: start_FSInfo + 492] = int.to_bytes(main_fat.fsi_free_count, 4, 'little')
            
            persist_in_disk(imagem)
            return 
    print("ERRO: diretório não encontrado")
    
# retorna todos os bytes de um arquivo dado o cluster inicial
def get_file_content(imagem, start_cluster):
    dados = b''
    next_cluster = start_cluster
    while next_cluster != -1:
        posicao_inicial_cluster =  return_start_data_cluster(imagem, next_cluster)
        dados = dados + imagem[posicao_inicial_cluster : posicao_inicial_cluster + main_fat.bytes_per_sec]

        next_cluster = find_next_cluster(imagem, next_cluster)
    
    return dados

# encontra as informacoes de um arquivo ou diretorio
# caso seja informado o tipo do arquivo, o metodo irá fazer a distinção
# senão acha o primeiro que tiver o mesmo nome
# ps: o arquivo tem que estar dentro do diretorio
def find_file_info(imagem, filename, dados_diretorio,filetype = 0):
    nome_e_extensao = filename.split('.')
    if len(nome_e_extensao) == 1:
        nome_e_extensao.append("")
    for item in dados_diretorio :
        if item['dir_name'] == nome_e_extensao[0].upper() and item['dir_extension'] == nome_e_extensao[1].upper():
            if filetype != 0:
                if item['dir_attr_cod'] == filetype:
                    return item
            else:
                    return item

    return -1

def find_dir_info(imagem, directory):
    nome_e_extensao = directory.split('.')
    if len(nome_e_extensao) == 1:
        nome_e_extensao.append("")
    for item in main_fat.dados_diretorio_atual['diretorios']:
        if item['dir_name'] == nome_e_extensao[0].upper() and item['dir_attr_cod'] == 16 and item['dir_extension'] == nome_e_extensao[1].upper():
            return item
    return {}

def valid_file_name(name):
    if len(name) > 8:
        return False 
    name_extension = name.split('.',1)
    regex = r"[\"\*\+,\./:;<=>\?\[\\\]\|]+" # caracteres inválidos
    
    for item in name_extension:    
        if len(re.findall(regex, item)) != 0:
            return False
        
    return True

# copia um arquivo para o disco
# pode receber arquivos da maquina ou do disco
# /img
# retorna o local do arquivo_origem(vai facilitar o processo de mover)
def copy_file(imagem, arquivo_origem, arquivo_destino):
    # guarda os arquivos do disco

    bytes_arquivo_origem = b'' # representa os bytes do arquivo
    estrutura_arquivo_destino = b'' # representa a estrutura que fica no diretorio
    prox_espaco_livre = main_fat.fsi_nxt_free + 1 # proximo cluster livre para escrita do arquivo
    local_arquivo_origem = 'imagem' # variavel que verifica o local do arquivo de origem. Pode ser imagem ou disco

    # pegando os bytes do arquivo de origem, se o arquivo for da imagem
    
    if arquivo_origem[0:4] == 'img/':
        nome_arquivo_origem = arquivo_origem[4:]
        infos_arquivo_origem = find_file_info(imagem, nome_arquivo_origem, main_fat.dados_diretorio_atual['diretorios'],32)
        if infos_arquivo_origem == -1: 
            print("ERRO: arquivo de origem não encontrado na imagem")
            return

        bytes_arquivo_origem = get_file_content(imagem, infos_arquivo_origem['first_cluster_dir'])[0:infos_arquivo_origem['dir_file_size']]
        estrutura_arquivo_destino = imagem[infos_arquivo_origem['inicio_endereco'] : infos_arquivo_origem['inicio_endereco']+32]

    else:
        # pegando os bytes do arquivo de origem, caso ele seja do disco
        local_arquivo_origem = 'disco'
        try:
            with open(arquivo_origem, 'rb') as file:
                bytes_arquivo_origem = file.read()
        except FileNotFoundError:
            print("ERRO: arquivo de origem não encontrado no disco")
            return
        

        # tem que criar a entrada do arquivo que vai ficar no diretorio
        # criar a estrutura do arquivo que vai ficar no diretorio de destino    

        nome_extensao_arquivo_origem = arquivo_origem.split('.')
        nome_extensao_arquivo_origem[0] = nome_extensao_arquivo_origem[0][0:8]
        
        dir_name = nome_extensao_arquivo_origem[0].upper()
        dir_extension = ''

        if(len(nome_extensao_arquivo_origem) == 2):
            dir_extension = nome_extensao_arquivo_origem[1].upper() # colocando em maiusculo os nomes
            for _ in range(len(dir_extension),3):
                dir_extension = dir_extension + " " 
        else:
            dir_extension = "   "

        for _ in range(len(dir_name), 8):
                dir_name = dir_name + ' '
        
        dir_name = dir_name.encode()
        dir_extension = dir_extension.encode()

        dir_attr = int.to_bytes(32, 1, 'little')
        dir_NT_res = int.to_bytes(0, 1, 'little')    
        
        # todo: arrumar a hora e data
        dir_crt_time_tenth = int.to_bytes(0, 1, 'little')
        dir_crt_time = int.to_bytes(0, 2, 'little')
        dir_crt_date = int.to_bytes(0, 2, 'little')
        dir_lst_acc_date = int.to_bytes(0, 2, 'little')
        

        dir_wrt_time = int.to_bytes(0, 2, 'little')
        dir_wrt_date = int.to_bytes(0, 2, 'little')
        dir_File_size  = int.to_bytes(len(bytes_arquivo_origem), 4, 'little')

        # prox_espaco_livre = main_fat.fsi_nxt_free + 1
        bytes_prox_espaco_livre = int.to_bytes(prox_espaco_livre, 4, 'little')
        dir_Fst_clusLO = bytes_prox_espaco_livre[0:2]
        dir_fst_clus_HI = bytes_prox_espaco_livre[2:]


        entrada_diretorio_bytes = dir_name + dir_extension + dir_attr + dir_NT_res + dir_crt_time_tenth + dir_crt_time + dir_crt_date + dir_lst_acc_date + dir_fst_clus_HI + dir_wrt_time + dir_wrt_date + dir_Fst_clusLO + dir_File_size

        if len(entrada_diretorio_bytes) != 32:
            print("ERRO: a entrada criada para o diretório está incorreta")
            return 
        else:
            estrutura_arquivo_destino = bytearray(entrada_diretorio_bytes)

        infos_arquivo_origem = {
            'dir_file_size': len(bytes_arquivo_origem)
        }

    
    if arquivo_destino[0:4] == 'img/':
        # caso o arquivo de destino esteja na imagem
        nome_arquivo_destino = arquivo_destino[4:]
        
        # verifica se tem algum arquivo ou diretorio com o nome de destino
        infos_arquivo_destino = find_file_info(imagem, nome_arquivo_destino, main_fat.dados_diretorio_atual['diretorios'])
        if infos_arquivo_destino == -1: 
            # se não um arquivo com esse nome, temos que ver se é um nome valido para criar a copia do arquivo
            if not valid_file_name(nome_arquivo_destino):
                print("ERRO: nome de destino inválido")
                return
        
            # alterando o nome do arquivo de destino
            nome_extensao_arquivo_destino = nome_arquivo_destino.split('.') 
            
            dir_name = nome_extensao_arquivo_destino[0].upper()
            dir_extension = ''

            if(len(nome_extensao_arquivo_destino) == 2):
                dir_extension = nome_extensao_arquivo_destino[1].upper() # colocando em maiusculo os nomes
                for _ in range(len(dir_extension),3):
                    dir_extension = dir_extension + " " 
            else:
                dir_extension = "   "

            for _ in range(len(dir_name), 8):
                    dir_name = dir_name + ' '
            
            dir_name_destino = dir_name.encode()
            dir_extension_destino = dir_extension.encode()

            # colocando o nome do arquivo de destino
            estrutura_arquivo_destino[0:11] = dir_name_destino + dir_extension_destino
            infos_diretorio_destino = main_fat.dados_diretorio_atual

        # se for um diretorio, precisa ver se existe um arquivo 
        # com o mesmo nome no diretorio de destino
        elif infos_arquivo_destino['dir_attr_cod'] == 16:
            infos_diretorio_destino = list_files(imagem, infos_arquivo_destino['first_cluster_dir'], False)
            if find_file_info(imagem, nome_arquivo_origem, infos_diretorio_destino['diretorios']) != -1:
               print("ERRO: arquivo já existe no diretório de destino")
               return 
            nome_arquivo_destino = nome_arquivo_origem
        # precisa calcular quantos clusters são necessarios para aquele arquivo
        
        
        # verificando quantos clusters são necessários para o arquivo
        qtde_cluster =  ceil(infos_arquivo_origem['dir_file_size']/main_fat.bytes_per_sec)
        espacos_alocados = 0 

        # alocando os espacos na fat
        # aloca até a qtde de clusters necessarios-1, porque o ultimo cluster
        # tem que ter o valor 0xfffffff
        for x in range(qtde_cluster-1):
            if verify_empty_fat(imagem, prox_espaco_livre + x):
                get_cluster_fat(imagem, prox_espaco_livre + x, prox_espaco_livre + x+1)
                espacos_alocados = espacos_alocados + 1
                
                byte_inicio_cluster_atual = return_start_data_cluster(imagem, prox_espaco_livre + x)
                # imagem[byte_inicio_cluster_atual: byte_inicio_cluster_atual+main_fat.]
        # aloca o ultimo cluster, e 'informa' que é o ultimo mesmo
        get_cluster_fat(imagem, prox_espaco_livre + espacos_alocados)

        # valida se foram alocados a qtde necessaria de clusters
        espacos_alocados = espacos_alocados + 1
        if espacos_alocados != qtde_cluster:
            print('ERRO: não foi alocada a qtde necessária de clusters')
            exit()
        # como copiamos a entrada no diretorio do arquivo, temos que trocar a entrada
        # do primeiro cluster do arquivo
        bytes_prox_espaco_livre = int.to_bytes(prox_espaco_livre, 4, 'little')
        dir_Fst_clusLO = bytes_prox_espaco_livre[0:2]
        dir_fst_clus_HI = bytes_prox_espaco_livre[2:]

        estrutura_arquivo_destino[26:28] = dir_Fst_clusLO
        estrutura_arquivo_destino[20:22] = dir_fst_clus_HI

        # gravando a referencia do arquivo no diretorio
        imagem[infos_diretorio_destino['inicio_proxima_entrada_dir']: infos_diretorio_destino['inicio_proxima_entrada_dir']+32] = estrutura_arquivo_destino
        
        byte_inicial_dados = return_start_data_cluster(imagem, prox_espaco_livre)
        

        # !salvando os dados contiguamente
        imagem[byte_inicial_dados: byte_inicial_dados + infos_arquivo_origem['dir_file_size']] = bytes_arquivo_origem

        persist_in_disk(imagem)
    else:
        # procedimento quando o destino é o disco(pc), e não a imagem
        if local_arquivo_origem == 'disco':
            print("Erro: arquivo de origem e destino não estão na imagem")
            return 
        
        # verifica se o local escolhido é um diretório
        # se for, coloca o nome do arquivo original para o destino
        if os.path.isdir(arquivo_destino) and arquivo_destino[-1] != '/':
            arquivo_destino = arquivo_destino + "/" + nome_arquivo_origem
        else:
            arquivo_destino = arquivo_destino + nome_arquivo_origem

        with open(arquivo_destino, 'wb') as file_destino:
            file_destino.write(bytes_arquivo_origem)

    return local_arquivo_origem

# Move o arquivo de origem para o arquivo destino
def move_file(imagem, arquivo_origem, arquivo_destino):
    # mover o arquivo é basicamente copia-lo para o destino
    # e depois apagar da origem
    local_arquivo_origem = copy_file(imagem, arquivo_origem, arquivo_destino)

    if local_arquivo_origem == 'imagem':
        remove_file(imagem, arquivo_origem[4:])
    else:
        os.remove(arquivo_origem)
        
    


def main():
    with open('myimagefat32.img', 'rb') as imagem_iso:
        global main_fat
        a = imagem_iso.read()
        a = bytearray(a)

    read_BPB(a)
    read_fsi(a)
    # print('\n\n Printando o obj fat')
    # print(main_fat)
    

    # # FirstSectorofCluster = ((N – 2) * BPB_SecPerClus) + FirstDataSector;
    # # como achar o primeiro setor de um cluster N
    # byte_ret = return_text_from_cluster(a, 2)
    # # print(byte_ret)
    
    list_files(a, main_fat.cluster_inicial_diretorio_atual, True)
    sair = 0
    while sair != 1:
        print(f"\nfatshell: [img{main_fat.nome_diretorio_atual}]$ ", end="")
        comando = input().split(" ")
        
        if comando[0].lower() == 'exit':
            sair = 1
        elif comando[0] == 'info':
            print_infos()
        elif comando[0] == 'cluster':
            if len(comando) != 2 or comando[1].isnumeric() == False:
                print('cluster <num>: exibe o conteúdo do bloco num no formato texto.')
                continue
            print(return_text_from_cluster(a, int(comando[1])), end="")
        elif comando[0] == 'ls':
            print_ls(list_files(a, main_fat.cluster_inicial_diretorio_atual, True)['diretorios'])
        elif comando[0] == 'pwd':
            print(main_fat.nome_diretorio_atual, end='')
        elif comando[0] == 'attr':
            if len(comando) != 2 or comando[1].isspace():
                print('attr <file | dir>: exibe os atributos de um arquivo (file) ou diretório (dir)')
                continue
            show_attributes(comando[1])

        elif comando[0] == 'cd':
            if len(comando) != 2 or comando[1].isspace():
                print('cd <path>: altera o diretório corrente para o definido como path.')
                continue
            enter_directory(a, comando[1])
        elif comando[0] == 'touch':
            if len(comando) != 2 or comando[1].isspace():
                print('touch <file>: cria o arquivo file com conteúdo vazio.')
                continue
            # cria um arquivo, por isso esta sendo passado o valor 32 
            create_file_directory(a, comando[1], 32) 
        elif comando[0] == 'mkdir':
            if len(comando) != 2 or comando[1].isspace():
                print('mkdir <dir>: cria o diretório dir vazio')
                continue
            # cria um diretorio, por isso esta sendo passado o valor 16
            create_file_directory(a, comando[1], 16) 
        elif comando[0] == 'rm':
            if len(comando) != 2 or comando[1].isspace():
                print('rm <file>: remove o arquivo file do sistema.')
                continue
            remove_file(a, comando[1])
        elif comando[0] == 'rmdir':
            if len(comando) != 2 or comando[1].isspace():
                print('rmdir <dir>: remove o diretório dir, se estiver vazio.')
                continue
            remove_dir(a, comando[1])
        elif comando[0] == 'cp':
            if len(comando) != 3 or comando[1].isspace() or comando[2].isspace():
                print("cp <source_path> <target_path>: copia um arquivo de origem (source_path) para destino (target_path).")
                continue
            copy_file(a, comando[1], comando[2])
        elif comando[0] == 'mv':
            if len(comando) != 3 or comando[1].isspace() or comando[2].isspace():
                print("mv <source_path> <target_path>: move um arquivo de origem (source_path) para destino (target_path).")
                continue
            move_file(a, comando[1], comando[2])
        elif comando[0] == 'rename':
            print("entrando no comando rename")
        else:
            print_menu()

if __name__ == '__main__':
    main()