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
        print(f"jmpBoot: {hex(jmpBoot[0])} {hex(jmpBoot[1])} {hex(jmpBoot[2])}")
        main_fat.jmp_boot = jmpBoot
        
        # somente um campo com um nome
        # BS_OEMName
        oem_name = imagem[3:11].decode()
        print(f"OEM Name: {oem_name}")

        # qtde de bytes por setor
        # precisa ter os valores 512, 1024, 2048 or 4096
        # BPB_BytsPerSec
        # bytes_per_sec = imagem[11:13]
        bytes_per_sec = int.from_bytes(imagem[11:13],'little')
        print(f"bytes per sector: {bytes_per_sec}")

        # setores por unidades alocadas
        # tem que ser potência de 2 e maior que 0
        # BPB_SecPerClus
        sector_per_cluster = imagem[13]
        print(f"Sector per clusters: {sector_per_cluster}")

        # numero de setores reservados
        # BPB_RsvdSecCnt
        # reserved_sectors = imagem[14:16]
        reserved_sectors = int.from_bytes(imagem[14:16],'little')
        print(f"Reserved Sectors: {reserved_sectors}")

        # numero de estruturas FAT no disco
        # precisa ser 2
        # BPB_NumFATs
        num_fats = imagem[16]
        print(f"Number of FAT data structures on the volume: {num_fats}")

        # Valor padrao 0xf8
        # BPB_Media
        media = imagem[21]
        print(f"Media: {hex(media)}")

        # Setores por trilha
        # BPB_SecPerTrk
        sec_per_trk = imagem[24:26]
        print(f"Sectors per track: {int.from_bytes(sec_per_trk,'little')}")

        # Number of heads for interrupt 0x13
        # BPB_NumHeads
        num_heads = imagem[26:28]
        print(f"Number of heads: {int.from_bytes(num_heads,'little')}")

        # total de setores de 32 bits
        # BPB_TotSec32
        # tot_sec_32 = imagem[32:36]
        tot_sec_32 = int.from_bytes(imagem[32:36],'little')
        print(f"total sectors: {tot_sec_32}")

        # Setores por FAT
        # BPB_FATSz32
        # fat_Sz_32 = imagem[36:40]
        fat_Sz_32 = int.from_bytes(imagem[36:40],'little')
        # print(f"FAT32 32-bit count of sectors occupied by ONE FAT: {fat_Sz_32}")
        print(f"Sectors occupied by FAT: {fat_Sz_32}")

        # tamanho da FAT em bytes
        fat_size_bytes = fat_Sz_32 * bytes_per_sec
        print(f"FAT size: {fat_size_bytes} bytes")

        # flags externas
        # BPB_ExtFlags
        ext_flags = imagem[40:42]
        print(f"external flags: {ext_flags}")

        # número da versão
        # doc da microsoft é 0:0
        # BPB_FSVer
        fs_ver = imagem[42:44]
        print(f"version number: {fs_ver}")

        # Cluster raiz
        # Geralmente valor é 2
        # BPB_RootClus
        root_clus = int.from_bytes(imagem[44:48],'little')
        print(f"root cluster: {root_clus}")

        # número do setor do FSINFO
        # BPB_FSInfo
        fs_info = imagem[48:50]
        print(f"Sector number of FSINFO: {int.from_bytes(fs_info,'little')}")

        # indicates the sector number in the reserved area of the volume of a copy of the boot record.
        # indica o numero do setor na area reservada do volume da copia do boot record
        # padrao = 6
        # BPB_BkBootSec
        bk_boot_sec = imagem[50:52]
        print(f"sector number in the reserved area of the volume of a copy of the boot record: {int.from_bytes(bk_boot_sec,'little')}")

        # bytes reservados
        # geralmente é tudo 0
        # BPB_Reserved
        reserved = imagem[52:64]
        print(f"Reserved: {reserved}")

        # nome do tipo
        # sempre vai ser FAT32
        # BS_FilSysType
        fil_sys_type = imagem[82:90]
        print(f"FAT type: {fil_sys_type.decode()}")

        # validacoes extras
        print(imagem[510]) # tem que ser igual a 0x55 ou 85
        print(imagem[511]) # tem que ser igual a 0xaa ou 170

        # Atribuindo os valores encontrados para o objeto principal
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
        print(f"root_dir_sectors {main_fat.root_dir_sectors}")

        # FirstDataSector = BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors;
        main_fat.first_data_sector = (reserved_sectors * bytes_per_sec) + (num_fats * fat_size_bytes) + main_fat.root_dir_sectors
        print(f"first_data_sector {hex(main_fat.first_data_sector)}")

        # DataSec = TotSec – (BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors);
        main_fat.data_sec = tot_sec_32 - (reserved_sectors + (num_fats * fat_Sz_32) + main_fat.root_dir_sectors)
        print(f"{tot_sec_32} - ({reserved_sectors} + ({num_fats} * {fat_Sz_32}) + {main_fat.root_dir_sectors})")
        print(f"data_sec {main_fat.data_sec}")

        # CountofClusters = DataSec / BPB_SecPerClus;
        main_fat.count_of_clusters = int(main_fat.data_sec / sector_per_cluster)
        print(f"count_of_clusters {main_fat.count_of_clusters}")

def read_fsi(imagem):
    global main_fat
    print("\n----------------------------------------------------------------\n")
    print("FAT32 FSInfo Sector Structure and Backup Boot Sector\n")
    
    # dados FAT32 FSInfo Sector Structure and Backup Boot Sector
    # FSI_LeadSig
    # o valor tem que ser 0x41615252
    start_FSInfo = main_fat.bytes_per_sec
    fsi_lead_sig = int.from_bytes(imagem[start_FSInfo:start_FSInfo+4], 'little')
    print(f'fsi_lead_sig {hex(fsi_lead_sig)}')

    # FSI_Reserved1
    # campo reservado
    # tem que ser inicializado com 0
    fsi_reserved1 = int.from_bytes(imagem[start_FSInfo+4 : start_FSInfo + 484], 'little')
    print(f'fsi_reserved1 {fsi_reserved1}')

    # FSI_StrucSig
    # o valor deve ser 0x61417272.
    fsi_struc_sig = int.from_bytes(imagem[start_FSInfo + 484: start_FSInfo + 488], 'little')
    print(f'fsi_struc_sig {hex(fsi_struc_sig)}')

    # FSI_Free_Count
    # ultima contagem de clusters livres
    # for 0xFFFFFFFF, então a contagem é desconhecida e precisa ser calculada
    fsi_free_count =  int.from_bytes(imagem[start_FSInfo + 488: start_FSInfo + 492], 'little')
    print(f'fsi_free_count {fsi_free_count}')

    # FSI_Nxt_Free
    # indica onde o driver pode começar a procurar por clusters livres
    # caso tenha o valor 0xFFFFFFFF vai ter que começar do cluster 2 mesmo
    fsi_nxt_free = int.from_bytes(imagem[start_FSInfo + 492: start_FSInfo + 496], 'little')
    print(f'fsi_nxt_free {fsi_nxt_free}')

    # FSI_Reserved2
    # campo reservado
    # tem que ser inicializado com 0
    fsi_reserved2 = int.from_bytes(imagem[start_FSInfo+496 : start_FSInfo + 508], 'little')
    print(f'fsi_reserved2 {fsi_reserved2}')

    # FSI_TrailSig
    # valor tem que ser 0xAA550000
    # Valida que é de fato um FSInfo sector
    fsi_trail_sig = int.from_bytes(imagem[start_FSInfo + 508: start_FSInfo + 512], 'little')
    print(f'fsi_trail_sig {hex(fsi_trail_sig)}')
    
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

# extrai as informações dos bytes passados
def extract_infos(bytes):
    inicio_dados = 0 
    
    # caso o bit 11 tenha o valor 0x0f, é uma entrada de nome longa 
    # e os dados do arquivo vai ficar nos proximos 32 bytes
    if (bytes[inicio_dados + 11] == 15): 
        ###print('entrada de arquivos com nome longo, lendo os 32 proximos bytes')
        inicio_dados = inicio_dados + 32
        return {
            'tipo': 'long name'
        }
    elif bytes[inicio_dados + 11] == 0:
        ### print("não existe nada armazenado nessa posição")
        return {
            'tipo': 'vazio'
        }
    
    # DIR_Name e DIR_extension
    # o nome curto tem 8 bytes, e a extensao 3 bytes
    dir_name = bytes[inicio_dados: inicio_dados + 8].decode().strip()
    dir_extension = bytes[inicio_dados+8: inicio_dados + 11].decode()
    
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
        'concat_hi': concat_hi,
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
def list_files(imagem):
    global main_fat
    num_cluster_diretorio = main_fat.cluster_inicial_diretorio_atual
    first_sector_of_cluster = ((num_cluster_diretorio-2) * main_fat.sector_per_cluster * main_fat.bytes_per_sec) + main_fat.first_data_sector


    dados_cluster_atual = imagem[first_sector_of_cluster:first_sector_of_cluster+main_fat.bytes_per_sec]
    next_cluster = find_next_cluster(imagem, num_cluster_diretorio)

    # Verifica se o cluster atual é o final, se não for concatena os dados
    # se for só passa pra listagem dos dados
    while next_cluster != -1:
        pass
    
    count = 32
    infos_diretorio = [] # dados extraidos do diretorio
    for x in range(int(len(dados_cluster_atual) / 32)):
        info_extraida = extract_infos(imagem[ first_sector_of_cluster + (count*x) : first_sector_of_cluster + (count*(x+1)) ])
        
        if info_extraida['tipo'] == 'vazio':
            break
        # ! ignorando por enquanto nomes longos dos arquivos
        if info_extraida['tipo'] != 'long name':
            infos_diretorio.append(info_extraida)
        
    # print(f'info extraida: {json.dumps(infos_diretorio, indent=4)}')
    main_fat.dados_diretorio_atual = infos_diretorio
    print_ls(infos_diretorio)

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

def main():
    with open('myimagefat32.img', 'rb') as imagem_iso:
        global main_fat
        a = imagem_iso.read()
        # print_menu()



        read_BPB(a)
        read_fsi(a)
        # print('\n\n Printando o obj fat')
        # print(main_fat)

        # # FirstSectorofCluster = ((N – 2) * BPB_SecPerClus) + FirstDataSector;
        # # como achar o primeiro setor de um cluster N
        # byte_ret = return_text_from_cluster(a, 2)
        # # print(byte_ret)
       
        # list_files(a)
        exit = 0
        while exit != 1:
            print(f"\nfatshell: [img{main_fat.nome_diretorio_atual}]$ ", end="")
            comando = input().split(" ")
            
            if comando[0].lower() == 'exit':
                exit = 1
            elif comando[0] == 'info':
                print('entrando no comando info')
            elif comando[0] == 'cluster':
                if len(comando) != 2 or comando[1].isnumeric() == False:
                    print('cluster <num>: exibe o conteúdo do bloco num no formato texto.')
                    continue
                 
                print(return_text_from_cluster(a, int(comando[1])), end="")
            elif comando[0] == 'pwd':
                print(main_fat.nome_diretorio_atual, end='')
            elif comando[0] == 'attr':
                print("entrando no comando attr")
            elif comando[0] == 'cd':
                print("entrando no comando cd")
            elif comando[0] == 'touch':
                print("entrando no comando touch")
            elif comando[0] == 'mkdir':
                print("entrando no comando mkdir")
            elif comando[0] == 'rm':
                print("entrando no comando rm")
            elif comando[0] == 'rmdir':
                print("entrando no comando rmdir")
            elif comando[0] == 'cp':
                print("entrando no comando cp")
            elif comando[0] == 'mv':
                print("entrando no comando mv")
            elif comando[0] == 'rename':
                print("entrando no comando rename")
            elif comando[0] == 'ls':
                list_files(a)
            else:
                print_menu()

        # print(find_next_cluster(a, 6))

       


       

    # print("\n----------------------------------------------------------------\n")

    





if __name__ == '__main__':
    main()